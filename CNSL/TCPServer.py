"""
TCP Server Program
File: server.py
"""

import socket
import os
import threading

def get_local_ip():
    """Get the local IP address of this machine"""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
        s.close()
        return local_ip
    except Exception:
        return '127.0.0.1'

# Server Configuration
HOST = get_local_ip()  # Automatically detect IP
PORT = 5555            # Server Port

def handle_client(client_socket, client_address):
    """Handle individual client connections"""
    print(f"\n[NEW CONNECTION] {client_address} connected.")
    
    try:
        while True:
            # Receive the choice from client
            choice = client_socket.recv(1024).decode('utf-8')
            
            if not choice or choice == '4':
                print(f"[DISCONNECTED] {client_address} disconnected.")
                break
            
            if choice == '1':
                # Say Hello functionality
                message = client_socket.recv(1024).decode('utf-8')
                print(f"\n[HELLO FROM CLIENT {client_address}]")
                print(f"Message: {message}")
                response = f"Hello from Server! Received your message: '{message}'"
                client_socket.send(response.encode('utf-8'))
                print(f"[RESPONSE SENT] Hello response sent to {client_address}\n")
            
            elif choice == '2':
                # File Transfer functionality
                # Receive filename
                filename = client_socket.recv(1024).decode('utf-8')
                client_socket.send("READY".encode('utf-8'))
                
                # Receive file size
                filesize = int(client_socket.recv(1024).decode('utf-8'))
                client_socket.send("READY".encode('utf-8'))
                
                # Create directory if it doesn't exist
                filepath = os.path.join("received_files", filename)
                os.makedirs("received_files", exist_ok=True)
                
                # Receive file data
                print(f"\n[FILE TRANSFER] Receiving '{filename}' ({filesize} bytes) from {client_address}")
                with open(filepath, 'wb') as f:
                    received = 0
                    while received < filesize:
                        data = client_socket.recv(4096)
                        if not data:
                            break
                        f.write(data)
                        received += len(data)
                
                print(f"[FILE RECEIVED] Successfully saved to '{filepath}'")
                client_socket.send(f"File '{filename}' received successfully!".encode('utf-8'))
                print(f"[CONFIRMATION SENT] Confirmation sent to {client_address}\n")
            
            elif choice == '3':
                # Calculator functionality
                # Receive operation
                operation = client_socket.recv(1024).decode('utf-8')
                client_socket.send("READY".encode('utf-8'))
                
                # Receive num1
                num1 = float(client_socket.recv(1024).decode('utf-8'))
                client_socket.send("READY".encode('utf-8'))
                
                # Receive num2
                num2 = float(client_socket.recv(1024).decode('utf-8'))
                
                # Perform calculation
                print(f"\n[CALCULATOR REQUEST from {client_address}]")
                print(f"Operation: {num1} {operation} {num2}")
                
                try:
                    if operation == '+':
                        result = num1 + num2
                    elif operation == '-':
                        result = num1 - num2
                    elif operation == '*':
                        result = num1 * num2
                    elif operation == '/':
                        if num2 == 0:
                            result = "Error: Division by zero"
                        else:
                            result = num1 / num2
                    else:
                        result = "Error: Invalid operation"
                    
                    client_socket.send(str(result).encode('utf-8'))
                    print(f"[RESULT] {result}")
                    print(f"[RESULT SENT] Calculation result sent to {client_address}\n")
                except Exception as e:
                    error_msg = f"Error: {str(e)}"
                    client_socket.send(error_msg.encode('utf-8'))
                    print(f"[ERROR] {error_msg}\n")
    
    except Exception as e:
        print(f"\n[ERROR] Connection error with {client_address}: {str(e)}\n")
    
    finally:
        client_socket.close()


def start_server():
    """Start the TCP server"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server.bind((HOST, PORT))
        server.listen()
    except OSError as e:
        print(f"\n[ERROR] Could not bind to {HOST}:{PORT}")
        print(f"Error: {e}")
        print("\nPossible reasons:")
        print("  - Port already in use")
        print("  - IP address doesn't exist on this machine")
        return
    
    print("=" * 60)
    print("                  TCP SERVER STARTED")
    print("=" * 60)
    print(f"[SERVER IP] {HOST}")
    print(f"[SERVER PORT] {PORT}")
    print(f"[LISTENING] Server is listening on {HOST}:{PORT}")
    print("\n[INFO] Clients should connect to:")
    print(f"       - Same computer: use 127.0.0.1 or {HOST}")
    print(f"       - Other computers: use {HOST}")
    print("[INFO] Server supports:")
    print("       1. Say Hello")
    print("       2. File Transfer")
    print("       3. Calculator")
    print("=" * 60)
    print("[WAITING] Waiting for client connections...")
    print("Press Ctrl+C to stop the server")
    print("=" * 60)
    
    try:
        while True:
            client_socket, client_address = server.accept()
            thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
            thread.start()
            print(f"\n[ACTIVE CONNECTIONS] {threading.active_count() - 1} client(s) connected")
    except KeyboardInterrupt:
        print("\n\n" + "=" * 60)
        print("[SHUTTING DOWN] Server is shutting down...")
        print("=" * 60)
    finally:
        server.close()
        print("[CLOSED] Server socket closed.\n")


if __name__ == "__main__":
    start_server()