#!/usr/bin/env python

import os
import sys
import html

def main():
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print("Access-Control-Allow-Origin: *")
    print("Access-Control-Allow-Methods: GET, POST, DELETE")
    print("Access-Control-Allow-Headers: *")

    content_length = os.environ.get('CONTENT_LENGTH')
    if content_length:
        try:
            input_data = sys.stdin.read(int(content_length))
        except (ValueError, IOError):
            input_data = ""
    else:
        input_data = ""
    env_vars = "\n".join(
        [f"<li>{key}={value}</li>" for key, value in os.environ.items()])
    html_content = "<html>\n"
    html_content += "<head>\n"
    html_content += "<title>Environment Variables</title>\n"
    html_content += "</head>\n"
    html_content += "<body>\n"
    html_content += "<h1>Environment Variables</h1>\n"
    html_content += f"<ul>{env_vars}</ul>\n"
    html_content += "<h2>Body</h2>\n"
    html_content += f"<div>{html.escape(input_data)}</div>\n"
    html_content += "</body>\n"
    html_content += "</html>\n"
    # html_content = "<html><body>HELLO WORLD!</body></html>"
    print(f"Content-Length: {len(html_content.encode('utf-8'))}", end="\r\n\r\n")
    print(html_content, end="")

if __name__ == "__main__":
    main()


