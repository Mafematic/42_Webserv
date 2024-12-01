#!/usr/bin/env python

import os
import sys
import time


def main():
    # Print all environment variables
    print("Environment Variables:")
    print()
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
    time.sleep(5)

if __name__ == "__main__":
    main()




