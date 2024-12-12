import datetime
import sys

def main():
    current_time = datetime.datetime.now()

    formatted_time = current_time.strftime("%A, %B %d, %Y %I:%M:%S %p")

    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print("Access-Control-Allow-Origin: *")
    print("Access-Control-Allow-Methods: GET, POST, DELETE")
    print("Access-Control-Allow-Headers: *")
    print()

    html_content = f"""
    <html>
        <head>
            <title>Current Time</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    background-color: #f0f8ff;
                    text-align: center;
                    padding: 50px;
                }}
                h1 {{
                    color: #333;
                    font-size: 48px;
                    margin-bottom: 20px;
                }}
                .time {{
                    font-size: 36px;
                    font-weight: bold;
                    color: #4CAF50;
                }}
                .footer {{
                    margin-top: 50px;
                    font-size: 18px;
                    color: #666;
                }}
            </style>
        </head>
        <body>
            <h1>Current Time</h1>
            <div class="time">
                {formatted_time}
            </div>
            <div class="footer">
                <p>Powered by Python</p>
            </div>
        </body>
    </html>
    """

    print(html_content)
    

if __name__ == "__main__":
    main()
