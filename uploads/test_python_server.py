import socket

def run_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("127.0.0.1", 8000))
    server_socket.listen(1)

    print("Server listening on port 8000...")

    while True:
        client_socket, client_address = server_socket.accept()
        request = client_socket.recv(1024).decode()
        print(f"Request received:\n{request}")

        response = """HTTP/1.1 200 OK
Content-Type: text/html
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, DELETE
Access-Control-Allow-Headers: *
Content-Length: 38\r\n\r\n
<html><body>HELLO WORLD!</body></html>"""

        client_socket.send(response.encode())
        client_socket.close()

if __name__ == "__main__":
    run_server()
