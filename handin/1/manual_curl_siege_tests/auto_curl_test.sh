#!/bin/bash

print_in_color() {
    local color=$1   # The color name (e.g., "red", "green", "blue")
    local text=$2    # The text to print

    # Define ANSI color codes
    local RED='\033[0;31m'
    local GREEN='\033[0;32m'
    local BLUE='\033[0;34m'
    local YELLOW='\033[1;33m'
    local CYAN='\033[0;36m'
    local RESET='\033[0m' # Reset color to default

    # Match the color name to the corresponding color code
    case $color in
        red) echo -e "${RED}${text}${RESET}" ;;
        green) echo -e "${GREEN}${text}${RESET}" ;;
        blue) echo -e "${BLUE}${text}${RESET}" ;;
        yellow) echo -e "${YELLOW}${text}${RESET}" ;;
        cyan) echo -e "${CYAN}${text}${RESET}" ;;
        *) echo -e "${text}" ;; # Default: No color
    esac
}

rm image_obtained_with_get.jpeg
rm ../uploads/test_image.jpeg

clear


print_in_color yellow "SIMPLE GET REQUEST"
print_in_color red "curl -i -v -X GET http://127.0.0.1:8004/index.html"
# echo -e "${RED}"
curl -i -v -X GET http://127.0.0.1:8004/index.html
read -r

print_in_color yellow "UPLOAD AN IMAGE USING POST"
print_in_color red "curl -i -v -X POST -F "image=@test_image.jpeg" 127.0.0.1:8004/fileupload"
curl -i -v -X POST -F "image=@test_image.jpeg" 127.0.0.1:8004/fileupload
read -r

print_in_color yellow "GET THE IMAGE WITH GET"
print_in_color red "curl -v -X GET http://127.0.0.1:8004/uploads/test_image.jpeg --output "image_obtained_with_get.jpeg"
"
curl -v -X GET http://127.0.0.1:8004/uploads/test_image.jpeg --output "image_obtained_with_get.jpeg"
open image_obtained_with_get.jpeg
read -r

print_in_color yellow "DELETE THE UPLOADED FILE"
print_in_color red "curl -v -i -X DELETE 127.0.0.1:8004/uploads/test_image.jpeg "
curl -v -i -X DELETE 127.0.0.1:8004/uploads/test_image.jpeg 
read -r

print_in_color yellow "CALL A CGI USING GET"
print_in_color red "curl -v -i 127.0.0.1:8004/cgi-bin/time.sh"
curl -v -i 127.0.0.1:8004/cgi-bin/time.sh
read -r

print_in_color yellow "CALL A CGI USING POST"
print_in_color red "curl -v -i -X POST -H "Content-Type: plain/text" --data "HELLO WORLD!" 127.0.0.1:8004/cgi-bin/print_env_body.py"
curl -v -i -X POST -H "Content-Type: plain/text" --data "HELLO WORLD!" 127.0.0.1:8004/cgi-bin/print_env_body.py
read -r

