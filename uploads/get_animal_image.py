import requests
import sys
import random
import os
from urllib.parse import parse_qs

def fetch_dog_image():
    url = "https://dog.ceo/api/breeds/image/random"
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        image_url = data['message']
        return requests.get(image_url, stream=True)
    else:
        return None

def fetch_duck_image():
    random_number = random.randint(1, 200)
    url = f"https://random-d.uk/api/v2/{random_number}.jpg"
    response = requests.get(url)
    return response


query_string = os.environ.get('QUERY_STRING', '')

params = parse_qs(query_string)
image_type = params.get('type', ['dog'])[0]

if image_type == 'duck':
    image_response = fetch_duck_image()
else:
    image_response = fetch_dog_image()


print("HTTP/1.1 200 OK")

if image_response and image_response.status_code == 200:
    image_data = image_response.content
    content_length = len(image_data)
    content_type = image_response.headers.get('Content-Type', 'application/octet-stream')
    print(f"Content-Type: {content_type}")
    print(f"Content-Length: {content_length}")
    print("Access-Control-Allow-Origin: *")
    print("Access-Control-Allow-Methods: GET, POST, DELETE")
    print("Access-Control-Allow-Headers: *\r\n\r")
    sys.stdout.flush()

    sys.stdout.buffer.write(image_data)
    sys.stdout.buffer.flush()
    sys.stdout.close()
else:
    print("Content-Type: text/plain\n")
    print("Failed to fetch the dog image.\r\n\r")

sys.exit(0)
