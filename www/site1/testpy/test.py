import os
import sys

print("Content-Type: text/html\r\n")

print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Test CGI Webserv</title></head>")
print("<body>")
print("<h1>Test CGI Python : Succes</h1>")

print("<h2>1. Variables d'Environnement</h2>")
print("<ul>")
for key, value in os.environ.items():
    print(f"<li><strong>{key}</strong> : {value}</li>")
print("</ul>")

print("<h2>2. Corps de la requete (Stdin)</h2>")
method = os.environ.get("REQUEST_METHOD", "GET")

if method == "POST":
    print("<pre>")
    try:
        body = sys.stdin.read()
        if body:
            print(body)
        else:
            print("[Corps de requete vide]")
    except Exception as e:
        print(f"Erreur de lecture : {e}")
    print("</pre>")
else:
    print("<p>Requete GET : aucun corps a lire.</p>")

print("</body>")
print("</html>")