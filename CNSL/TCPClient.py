"""
TCP Client Program - Menu Driven Interface
File: client.py
Usage: python client.py
"""

import socket
import os

HOST = '127.0.0.1'
PORT = 5555

def say_hello(client_socket):
    """Send a hello message to the server"""
    print("\n" + "-" * 50)
    print("          SAY HELLO TO SERVER")
    print("-" * 50)
    message = input("Enter your message to say hello: ")
    
    client_socket.send(message.encode('utf-8'))
    
    response = client_socket.recv(1024).decode('utf-8')
    print("\n" + "=" * 50)
    print("[SERVER RESPONSE]")
    print(response)
    print("=" * 50 + "\n")
    
    input("Press Enter to continue...")


def file_transfer(client_socket):
    """Transfer a file to the server"""
    print("\n" + "-" * 50)
    print("          FILE TRANSFER TO SERVER")
    print("-" * 50)
    filename = input("Enter the filename (with path) to transfer: ")
    
    if not os.path.exists(filename):
        print("\n" + "=" * 50)
        print(f"[ERROR] File '{filename}' not found!")
        print("=" * 50 + "\n")
        input("Press Enter to continue...")
        return
    
    # Send just the filename (not the full path)
    base_filename = os.path.basename(filename)
    client_socket.send(base_filename.encode('utf-8'))
    client_socket.recv(1024)  # Wait for READY
    
    # Send file size
    filesize = os.path.getsize(filename)
    client_socket.send(str(filesize).encode('utf-8'))
    client_socket.recv(1024)  # Wait for READY
    
    # Send file data
    print(f"\n[SENDING] Transferring '{base_filename}' ({filesize} bytes)...")
    print("[PROGRESS] ", end="", flush=True)
    
    with open(filename, 'rb') as f:
        sent = 0
        while True:
            data = f.read(4096)
            if not data:
                break
            client_socket.send(data)
            sent += len(data)
            # Show progress
            if filesize > 0:
                percent = (sent / filesize) * 100
                if int(percent) % 10 == 0:
                    print("#", end="", flush=True)
    
    print(" DONE!")
    
    response = client_socket.recv(1024).decode('utf-8')
    print("\n" + "=" * 50)
    print("[SERVER RESPONSE]")
    print(response)
    print("=" * 50 + "\n")
    
    input("Press Enter to continue...")


def calculator(client_socket):
    """Perform calculations on the server"""
    print("\n" + "-" * 50)
    print("          CALCULATOR")
    print("-" * 50)
    print("Available operations: +, -, *, /")
    print("-" * 50)
    
    operation = input("Enter operation (+, -, *, /): ")
    
    if operation not in ['+', '-', '*', '/']:
        print("\n" + "=" * 50)
        print("[ERROR] Invalid operation! Use +, -, *, or /")
        print("=" * 50 + "\n")
        input("Press Enter to continue...")
        return
    
    client_socket.send(operation.encode('utf-8'))
    client_socket.recv(1024)  # Wait for READY
    
    try:
        num1 = float(input("Enter first number: "))
        client_socket.send(str(num1).encode('utf-8'))
        client_socket.recv(1024)  # Wait for READY
        
        num2 = float(input("Enter second number: "))
        client_socket.send(str(num2).encode('utf-8'))
        
        result = client_socket.recv(1024).decode('utf-8')
        
        print("\n" + "=" * 50)
        print("[CALCULATION RESULT]")
        print(f"{num1} {operation} {num2} = {result}")
        print("=" * 50 + "\n")
    except ValueError:
        print("\n" + "=" * 50)
        print("[ERROR] Invalid number input!")
        print("=" * 50 + "\n")
    
    input("Press Enter to continue...")


def display_menu():
    """Display the menu options"""
    print("\n")
    print("=" * 60)
    print("              TCP CLIENT - MENU")
    print("=" * 60)
    print("  1. Say Hello to Server")
    print("  2. File Transfer to Server")
    print("  3. Calculator")
    print("  4. Exit")
    print("=" * 60)


def start_client():
    """Start the TCP client with menu-driven interface"""
    # Ask user for server IP and port
    print("\n" + "=" * 60)
    print("              TCP CLIENT APPLICATION")
    print("=" * 60)
    
    server_ip = input("Enter Server IP Address: ").strip()
    if not server_ip:
        server_ip = HOST
    
    port_input = input(f"Enter Server Port (default {PORT}): ").strip()
    if port_input:
        try:
            server_port = int(port_input)
        except ValueError:
            print("[WARNING] Invalid port, using default port")
            server_port = PORT
    else:
        server_port = PORT
    
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print(f"\n[CONNECTING] Attempting to connect to {server_ip}:{server_port}...")
        client.connect((server_ip, server_port))
        
        print(f"[CONNECTED] Successfully connected to server at {server_ip}:{server_port}")
        print("=" * 60)
        
        while True:
            display_menu()
            choice = input("\nEnter your choice (1-4): ")
            
            if choice not in ['1', '2', '3', '4']:
                print("\n[ERROR] Invalid choice! Please select 1-4.")
                input("Press Enter to continue...")
                continue
            
            # Send choice to server
            client.send(choice.encode('utf-8'))
            
            if choice == '1':
                say_hello(client)
            elif choice == '2':
                file_transfer(client)
            elif choice == '3':
                calculator(client)
            elif choice == '4':
                print("\n" + "=" * 60)
                print("[DISCONNECTING] Closing connection...")
                print("=" * 60 + "\n")
                break
        
        client.close()
        print("=" * 60)
        print("[DISCONNECTED] Connection closed successfully.")
        print("=" * 60 + "\n")
    
    except ConnectionRefusedError:
        print("\n" + "=" * 60)
        print("[ERROR] Could not connect to server")
        print(f"Server address: {server_ip}:{server_port}")
        print("Make sure the server is running!")
        print("=" * 60 + "\n")
    except Exception as e:
        print("\n" + "=" * 60)
        print(f"[ERROR] An error occurred: {str(e)}")
        print("=" * 60 + "\n")


if __name__ == "__main__":
    start_client()