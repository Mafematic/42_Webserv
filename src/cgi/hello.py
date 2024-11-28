#!/usr/bin/python3

import os
import sys

# Read the input from stdin if there's a POST body
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
body = sys.stdin.read(content_length) if content_length > 0 else ""

# Output a basic HTTP response
print("Content-Type: text/html\r\n")
print("<html><body>")
print("<h1>Hello from CGI!</h1>")
print("<p>Path Info: {}</p>".format(os.environ.get("PATH_INFO", "")))
print("<p>Query String: {}</p>".format(os.environ.get("QUERY_STRING", "")))
print("<p>Body: {}</p>".format(body))
print("</body></html>")
