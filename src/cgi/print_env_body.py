#!/usr/bin/env python

import os
import sys

def main():
    # Print the CGI HTTP headers
    print("Content-Type: text/plain")  # Plain text response
    print()  # Empty line to indicate the end of headers

    # Print all environment variables
    print("Environment Variables:")
    for key, value in os.environ.items():
        print(f"{key}={value}")
    
    print("\nInput Data:")
    
    # Read from stdin (e.g., POST request body)
    content_length = os.environ.get('CONTENT_LENGTH')
    if content_length:
        try:
            # Read and print the exact number of bytes specified by CONTENT_LENGTH
            input_data = sys.stdin.read(int(content_length))
            print(input_data)
        except (ValueError, IOError):
            print("Failed to read input data.")
    else:
        print("No input data provided.")

if __name__ == "__main__":
    main()