#!/bin/bash

# Read request body if CONTENT_LENGTH is set
if [[ -n "$CONTENT_LENGTH" ]]; then
    read -n "$CONTENT_LENGTH" REQUEST_BODY
else
    REQUEST_BODY=""
fi

# Generate HTML content and store it in a variable
HTML_CONTENT="<html>"
HTML_CONTENT+="<head><title>Environment Variables</title></head>"
HTML_CONTENT+="<body>"
HTML_CONTENT+="<h1>Environment Variables</h1>"
HTML_CONTENT+="<ul>"

# Iterate through environment variables and display them
for VAR in $(printenv); do
    KEY=$(echo "$VAR" | cut -d= -f1)
    VALUE=$(echo "$VAR" | cut -d= -f2-)
    HTML_CONTENT+="<li>${KEY}=${VALUE}</li>"
done

HTML_CONTENT+="</ul>"
HTML_CONTENT+="<h2>Body</h2>"
HTML_CONTENT+="<div>${REQUEST_BODY}</div>"
HTML_CONTENT+="</body>"
HTML_CONTENT+="</html>"

# Output HTTP headers with Content-Length
CONTENT_LENGTH=${#HTML_CONTENT}
echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo "Access-Control-Allow-Origin: *"
echo "Access-Control-Allow-Methods: GET, POST, DELETE"
echo "Access-Control-Allow-Headers: *"
echo "Content-Length: $CONTENT_LENGTH"
echo

# Output the HTML content
echo "$HTML_CONTENT"
