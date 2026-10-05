#!/bin/bash
# Tests webserv calques sur la fiche d'evaluation 42.
#
# Depuis la racine du repo :
#   ./webserv eval/eval.config        (terminal 1)
#   bash eval/run_eval.sh             (terminal 2)
#
# Options :
#   LONG=1 bash eval/run_eval.sh      ajoute les tests lents (timeouts, ~45 s)
#   (siege est utilise automatiquement s'il est installe)

A=http://127.0.0.1:8080
B=http://127.0.0.2:8080
C=http://127.0.0.1:8081
UP=eval/uploads
G="\033[32m"; R="\033[31m"; Y="\033[33m"; N="\033[0m"
PASS=0; FAIL=0

section() { echo -e "\n${Y}== $1 ==${N}"; }

ok()   { PASS=$((PASS+1)); echo -e "[${G}OK${N}  ] $1"; }
ko()   { FAIL=$((FAIL+1)); echo -e "[${R}FAIL${N}] $1  ${R}-> $2${N}"; }

# code HTTP d'une requete curl (000 = pas de reponse)
code() { : > /tmp/ws_body; curl -s -m "${T:-5}" -o /tmp/ws_body -w "%{http_code}" "$@"; }
body() { tr -c '[:print:]\n' '.' < /tmp/ws_body | head -c 120; }

# expect "description" code_attendu  [arguments curl...]
expect() {
	local name="$1" want="$2"; shift 2
	local got; got=$(code "$@")
	if [ "$got" = "$want" ]; then ok "$name"; else ko "$name" "attendu $want, recu $got"; fi
}

# expect_body "description" "texte attendu dans le body" [arguments curl...]
expect_body() {
	local name="$1" want="$2"; shift 2
	local got; got=$(code "$@")
	if grep -q -- "$want" /tmp/ws_body; then ok "$name"
	elif [ "$got" = 000 ]; then ko "$name" "pas de reponse (requete bloquee)"
	else ko "$name" "code $got, body: $(body)"; fi
}

# requete brute (pour ce que curl ne sait pas envoyer)
raw() { printf "$1" | timeout 3 nc -q 2 127.0.0.1 8080 2>/dev/null | head -1 | tr -d '\r'; }

alive() { curl -s -m 3 -o /dev/null "$A/"; }

server_pid() { pgrep -x webserv | head -1; }

if ! alive; then
	echo -e "${R}Le serveur ne repond pas. Lance d'abord : ./webserv eval/eval.config${N}"
	exit 1
fi
rm -f "$UP"/*.txt "$UP"/*.bin
PID=$(server_pid)
FD_START=$(ls /proc/$PID/fd 2>/dev/null | wc -l)

# ---------------------------------------------------------------------------
section "CONFIGURATION : plusieurs sites, interfaces et ports"
expect_body "127.0.0.1:8080 -> site A" "SITE A" "$A/"
expect_body "127.0.0.2:8080 -> site B" "SITE B" "$B/"
expect_body "127.0.0.1:8081 -> site C" "SITE C" "$C/"
expect_body "Host: localhost -> site A" "SITE A" "http://localhost:8080/"

section "CONFIGURATION : pages d'erreur"
expect      "page inexistante -> 404" 404 "$A/existepas"
expect_body "404 = page perso" "MA PAGE 404" "$A/existepas"
expect      "DELETE interdit -> 405 (et pas 404)" 405 -X DELETE "$A/nodelete/x"
expect_body "405 = page perso" "MA PAGE 405" -X DELETE "$A/nodelete/x"

section "CONFIGURATION : limite du body (commande de la fiche, limite 20 octets)"
expect "body court -> 201" 201 -X POST -H "Content-Type: plain/text" --data "BODY" "$A/upload/court.txt"
expect "body long -> 413" 413 -X POST -H "Content-Type: plain/text" \
	--data "BODY IS HERE write something shorter or longer than body limit" "$A/upload/long.txt"
if [ -f "$UP/long.txt" ]; then ko "body long : aucun fichier cree" "long.txt a ete cree"; else ok "body long : aucun fichier cree"; fi

section "CONFIGURATION : routes, fichier par defaut, methodes"
expect "route /static -> autre dossier" 200 "$A/static/css/style.css"
expect_body "dossier -> fichier par defaut" "default file" "$A/dir_index/"
curl -s -m 5 -o /dev/null --data "abc" "$A/up/a_supprimer.txt"
expect "DELETE autorise -> 204" 204 -X DELETE "$A/up/a_supprimer.txt"
echo "garde" > "$UP/garde.txt"
expect "DELETE interdit -> 405" 405 -X DELETE "$A/nodelete/garde.txt"
if [ -f "$UP/garde.txt" ]; then ok "DELETE interdit : fichier toujours la"; else ko "DELETE interdit : fichier toujours la" "supprime !"; fi

# ---------------------------------------------------------------------------
section "BASIC CHECKS : GET / POST / DELETE"
expect "GET /" 200 "$A/"
expect "GET image" 200 "$A/static/images/marc.png"
expect "GET PDF 5 Mo" 200 "$A/static/documents/planning-adultes.pdf"
expect "GET fichier vide -> 200" 200 "$A/empty.html"
expect "POST -> 201" 201 --data "hello" "$A/up/hello.txt"
expect_body "GET du fichier poste" "hello" "$A/up/hello.txt"
expect "DELETE -> 204" 204 -X DELETE "$A/up/hello.txt"
expect "DELETE a nouveau -> 404" 404 -X DELETE "$A/up/hello.txt"

section "BASIC CHECKS : requetes inconnues (ne doivent pas crasher)"
r=$(raw 'BREW / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n')
case "$r" in *" 405 "*|*" 501 "*) ok "methode inconnue -> 405/501";; *) ko "methode inconnue -> 405/501" "$r";; esac
r=$(raw 'n importe quoi\r\n\r\n')
case "$r" in *" 400 "*) ok "requete invalide -> 400";; *) ko "requete invalide -> 400" "$r";; esac
r=$(raw 'GET / HTTP/1.1\r\n\r\n')
case "$r" in *" 400 "*) ok "sans Host -> 400";; *) ko "sans Host -> 400" "$r";; esac
r=$(raw 'GET /../../../etc/passwd HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n')
case "$r" in *" 400 "*|*" 403 "*|*" 404 "*) ok "path traversal refuse";; *) ko "path traversal refuse" "$r";; esac
if alive; then ok "serveur vivant apres les requetes bizarres"; else ko "serveur vivant" "MORT"; fi

section "BASIC CHECKS : upload et recuperation"
echo "contenu upload" > /tmp/ws_doc.txt
expect "curl -F sur /up/ (comme un formulaire)" 201 -F "file=@/tmp/ws_doc.txt" "$A/up/"
expect_body "fichier uploade recuperable" "contenu upload" "$A/up/ws_doc.txt"
head -c 2000000 /dev/urandom > /tmp/ws_big.bin
expect "upload 2 Mo (limite serveur 10 Mo)" 201 --data-binary @/tmp/ws_big.bin "$A/up/big.bin"
if cmp -s /tmp/ws_big.bin "$UP/big.bin"; then ok "fichier 2 Mo identique"; else ko "fichier 2 Mo identique" "different ou absent"; fi

# ---------------------------------------------------------------------------
section "CGI"
expect_body "CGI GET + query string" "QUERY=nom=bob" "$A/cgi/env.py?nom=bob"
expect_body "CGI POST + body" "BODY=a=1&b=2" -d "a=1&b=2" "$A/cgi/env.py"
expect_body "CGI lance dans son dossier (fichier relatif)" "data relative OK" "$A/cgi/relative.py"
expect "CGI avec erreur -> 500/502" 502 "$A/cgi/error.py"
n=0; for i in $(seq 10); do [ "$(T=3 code "$A/cgi/env.py")" = 200 ] && n=$((n+1)); done
if [ $n = 10 ]; then ok "CGI fiable (10/10)"; else ko "CGI fiable (10/10)" "$n/10 reponses"; fi
echo "   (boucle infinie : attente max 20 s)"
got=$(T=20 code "$A/cgi/infinite.py")
case "$got" in 500|502|504) ok "CGI boucle infinie -> erreur";; *) ko "CGI boucle infinie -> erreur" "recu $got apres 20 s";; esac
if alive; then ok "serveur vivant apres les CGI"; else ko "serveur vivant apres les CGI" "MORT"; fi

# ---------------------------------------------------------------------------
section "NAVIGATEUR"
h=$(curl -s -m 5 -D - -o /dev/null "$A/static/css/style.css" | tr -d '\r' | grep -i "^content-type")
case "$h" in *text/css*) ok "Content-Type css";; *) ko "Content-Type css" "$h";; esac
h=$(curl -s -m 5 -D - -o /dev/null "$A/redir" | tr -d '\r')
case "$h" in *" 301 "*Location:\ /static/*) ok "redirection 301 + Location";; *) ko "redirection 301 + Location" "$(echo "$h" | head -1)";; esac
expect_body "listing de dossier" "<li>" "$A/listing/"

# ---------------------------------------------------------------------------
section "ROBUSTESSE / SIEGE"
if [ -n "$(server_pid)" ] && [ -d /proc ]; then
	PID=$(server_pid)
	FD_END=$(ls /proc/$PID/fd 2>/dev/null | wc -l)
	if [ "$FD_END" -le "$((FD_START + 1))" ]; then ok "pas de fuite de fd ($FD_START -> $FD_END)"; else ko "pas de fuite de fd" "$FD_START -> $FD_END"; fi
	Z=$(ps -eo stat,ppid | awk -v p="$PID" '$1 ~ /^Z/ && $2 == p' | wc -l)
	if [ "$Z" = 0 ]; then ok "pas de zombie"; else ko "pas de zombie" "$Z zombies"; fi
fi

if command -v siege > /dev/null && [ -n "$(server_pid)" ] && alive; then
	av=$(siege -b -c 25 -t 10S "$A/" 2>&1 | grep -i availability | grep -o "[0-9.]*" | head -1)
	if awk "BEGIN{exit !(${av:-0} > 99.5)}"; then ok "siege -b sur / : $av %"; else ko "siege -b sur /" "$av %"; fi
	s=$(siege -b -c 25 -t 5S "$A/empty.html" 2>&1 | grep -i successful_transactions | grep -o "[0-9]*")
	if [ "${s:-0}" -gt 0 ]; then ok "siege -b sur page vide"; else ko "siege -b sur page vide" "0 transaction reussie"; fi
	if alive; then ok "serveur vivant apres siege"; else ko "serveur vivant apres siege" "MORT"; fi
else
	echo "   (siege non installe ou serveur mort : tests siege sautes)"
fi

if [ "$LONG" = 1 ] && [ -n "$(server_pid)" ] && alive; then
	echo "   (client muet : attente max 40 s)"
	# connexion ouverte sans rien envoyer : le serveur doit finir par fermer
	exec 3<>/dev/tcp/127.0.0.1/8080
	timeout 40 cat <&3 > /dev/null
	rc=$?
	exec 3<&-
	if [ $rc != 124 ]; then ok "client muet deconnecte par timeout"; else ko "client muet deconnecte par timeout" "toujours connecte apres 40 s"; fi
fi

# En dernier : ce test peut tuer le serveur
for i in 1 2 3 4 5; do curl -s -m 5 "$A/static/documents/planning-adultes.pdf" | head -c 100 > /dev/null; done
sleep 1
if [ -n "$(server_pid)" ] && alive; then ok "client qui coupe pendant l'envoi (SIGPIPE)"; else ko "client qui coupe pendant l'envoi (SIGPIPE)" "SERVEUR MORT"; fi
if [ -z "$(server_pid)" ]; then
	echo -e "${R}   Le serveur est mort.${N}"
	if timeout 3 bash -c "echo > /dev/tcp/127.0.0.1/8080" 2>/dev/null; then
		ko "port 8080 libere apres la mort du serveur" "un process (CGI ?) tient encore le port"
	fi
fi


rm -f "$UP"/*.txt "$UP"/*.bin /tmp/ws_body /tmp/ws_doc.txt /tmp/ws_big.bin
echo -e "\nResultat : ${G}$PASS OK${N} / ${R}$FAIL FAIL${N}"
