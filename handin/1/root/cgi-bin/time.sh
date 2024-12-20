
current_time=$(date "+%A, %B %d, %Y %I:%M:%S %p")

html_content="<html>"
html_content+="<head>"
html_content+="<title>Current Time</title>"
html_content+="<style>"
html_content+="body {"
html_content+="font-family: Arial, sans-serif;"
html_content+="background-color: #f0f8ff;"
html_content+="text-align: center;"
html_content+="padding: 50px;"
html_content+="}"
html_content+="h1 {"
html_content+="color: #333;"
html_content+="font-size: 48px;"
html_content+="margin-bottom: 20px;"
html_content+="}"
html_content+=".time {"
html_content+="font-size: 36px;"
html_content+="font-weight: bold;"
html_content+="color: #4CAF50;"
html_content+="}"
html_content+=".footer {"
html_content+="margin-top: 50px;"
html_content+="font-size: 18px;"
html_content+="color: #666;"
html_content+="}"
html_content+="</style>"
html_content+="</head>"
html_content+="<body>"
html_content+="<h1>Current Time</h1>"
html_content+="<div class='time'>"
html_content+="$current_time"
html_content+="</div>"
html_content+="<div class='footer'>"
html_content+="<p>Powered by Bash</p>"
html_content+="</div>"
html_content+="</body>"
html_content+="</html>"

# Calculate the length
# length=$(echo -n "$html_content" | wc -c) # Use -m if you need multi-byte support
length=$(echo -n "$html_content" | wc -m)

echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo "Access-Control-Allow-Origin: *"
echo "Access-Control-Allow-Methods: GET, POST, DELETE"
echo "Access-Control-Allow-Headers: *"
echo -e -n "Content-Length: ${length}\r\n\r\n"
# echo "Content-Length: 500"
# echo $html_content
echo -n $html_content
