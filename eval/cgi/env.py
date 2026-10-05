import os, sys
body = sys.stdin.read()
print("Content-Type: text/plain\r\n\r\n", end="")
print("METHOD=" + os.environ.get("REQUEST_METHOD", ""))
print("QUERY=" + os.environ.get("QUERY_STRING", ""))
print("BODY=" + body)
print("CWD=" + os.getcwd())
