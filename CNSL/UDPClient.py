"""
UDP Client Program - Media File Transfer Only
File: client_udp.py
"""

import socket
import os
import time

HOST = '127.0.0.1'
PORT = 5555
BUFFER_SIZE = 65507  # Maximum UDP packet size
PACKET_DATA_SIZE = 65497  # Reserve 10 bytes for packet number

# Allowed media file extensions
ALLOWED_EXTENSIONS = {
    'script': ['.py', '.sh', '.bat', '.js'],
    'text': ['.txt', '.csv', '.json', '.xml'],
    'audio': ['.mp3', '.wav', '.ogg', '.flac', '.m4a'],
    'video': ['.mp4', '.avi', '.mkv', '.mov', '.flv'],
    'image': ['.jpg', '.jpeg', '.png', '.gif', '.bmp', '.svg', '.webp']
}

def is_media_file(filename):
    """Check if file is a valid media file"""
    ext = os.path.splitext(filename)[1].lower()
    for category, extensions in ALLOWED_EXTENSIONS.items():
        if ext in extensions:
            return True, category
    return False, None

def file_transfer(client_socket, server_address):    
    print("\n" + "=" * 60)
    print("       UDP MEDIA FILE TRANSFER (NO ACK)")
    print("=" * 60)
    print("Supported file types:")
    print(f"  Script: {', '.join(ALLOWED_EXTENSIONS['script'])}")
    print(f"  Text:   {', '.join(ALLOWED_EXTENSIONS['text'])}")
    print(f"  Audio:  {', '.join(ALLOWED_EXTENSIONS['audio'])}")
    print(f"  Video:  {', '.join(ALLOWED_EXTENSIONS['video'])}")
    print(f"  Image:  {', '.join(ALLOWED_EXTENSIONS['image'])}")
    print("=" * 60)
    
    filename = input("\nEnter filename (with path) to transfer: ")
    
    # Validate file exists
    if not os.path.exists(filename):
        print(f"\n[ERROR] File '{filename}' not found!")
        input("Press Enter to continue...")
        return
    
    # Validate media file
    is_valid, file_category = is_media_file(filename)
    if not is_valid:
        print(f"\n[ERROR] Only media files allowed!")
        print("Supported: script, text, audio, video files")
        input("Press Enter to continue...")
        return
    
    base_filename = os.path.basename(filename)
    filesize = os.path.getsize(filename)
    
    print(f"\n[FILE INFO]")
    print(f"  Name: {base_filename}")
    print(f"  Type: {file_category.upper()}")
    print(f"  Size: {filesize} bytes ({filesize/1024:.2f} KB)")
    
    # Calculate number of packets
    num_packets = (filesize + PACKET_DATA_SIZE - 1) // PACKET_DATA_SIZE
    print(f"  Packets: {num_packets}")
    print(f"  Packet size: {PACKET_DATA_SIZE} bytes data + 10 bytes header")
    
    print(f"\n[UDP MODE] Connectionless - NO ACK expected")
    print("[WARNING] Packets may be lost - no retransmission!")
    
    # STEP 1: Send metadata packet
    print("\n" + "=" * 60)
    print("[STEP 1] Sending metadata packet")
    metadata = f"METADATA|{base_filename}|{filesize}|{num_packets}"
    client_socket.sendto(metadata.encode('utf-8'), server_address)
    print(f"  → Sent: {metadata}")
    print("  → NO ACK received (UDP is connectionless)")
    
    # Small delay to ensure server is ready
    time.sleep(0.1)
    
    # STEP 2 & 3: Send all data packets (no ACK)
    print("\n[STEP 2] Sending data packets (fire-and-forget)")
    print("=" * 60)
    
    start_time = time.time()
    packet_num = 0
    
    with open(filename, 'rb') as f:
        while True:
            # Read chunk of file
            data = f.read(PACKET_DATA_SIZE)
            if not data:
                break
            
            # Create packet: [packet_number padded to 10 bytes][file data]
            packet_header = f"{packet_num:<10}".encode('utf-8')
            packet = packet_header + data
            
            # Send packet (no waiting for ACK!)
            client_socket.sendto(packet, server_address)
            
            packet_num += 1
            
            # Show progress
            if packet_num % 100 == 0 or packet_num == num_packets:
                percent = (packet_num / num_packets) * 100
                print(f"[SENDING] Packet {packet_num}/{num_packets} ({percent:.1f}%) - NO ACK")
            
            # Small delay to prevent overwhelming the network
            # (In real UDP, you might send without delay)
            time.sleep(0.0001)
    
    end_time = time.time()
    transfer_time = end_time - start_time
    speed = filesize / transfer_time / 1024 if transfer_time > 0 else 0
    
    # STEP 4: Transfer complete (from client's perspective)
    print("=" * 60)
    print("\n[TRANSFER COMPLETE - Client Side]")
    print(f"  File: {base_filename}")
    print(f"  Size: {filesize} bytes ({filesize/1024:.2f} KB)")
    print(f"  Packets sent: {num_packets}")
    print(f"  Time: {transfer_time:.2f} seconds")
    print(f"  Speed: {speed:.2f} KB/s")
    print("=" * 60 + "\n")
    
    input("Press Enter to continue...")

def display_menu():
    """Display menu"""
    print("\n" + "=" * 60)
    print("      UDP CLIENT - MEDIA FILE TRANSFER")
    print("=" * 60)
    print("  1. Transfer Media File to Server")
    print("  2. Exit")
    print("=" * 60)

def start_client():
    """Start UDP client"""
    print("\n" + "=" * 60)
    print("      UDP MEDIA FILE TRANSFER CLIENT")
    print("=" * 60)
    
    server_ip = input("Enter Server IP: ").strip()
    if not server_ip:
        server_ip = HOST
    
    port_input = input(f"Enter Server Port (default {PORT}): ").strip()
    if port_input:
        try:
            server_port = int(port_input)
        except ValueError:
            print("[WARNING] Invalid port, using default")
            server_port = PORT
    else:
        server_port = PORT
    
    server_address = (server_ip, server_port)
    
    try:
        # Create UDP socket (no connection needed!)
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        print(f"\n[READY] UDP Client ready")
        print(f"[SERVER] {server_ip}:{server_port}")
        print("[INFO] UDP is connectionless - no connection established")
        print("[INFO] Packets sent without acknowledgment")
        print("=" * 60)
        
        while True:
            display_menu()
            choice = input("\nEnter choice (1-2): ")
            
            if choice == '1':
                file_transfer(client, server_address)
            elif choice == '2':
                print("\n[EXIT] Closing client...")
                break
            else:
                print("[ERROR] Invalid choice!")
                input("Press Enter to continue...")
        
        client.close()
        print("\n[CLOSED] Client closed.\n")
    
    except Exception as e:
        print(f"\n[ERROR] {str(e)}\n")

if __name__ == "__main__":
    start_client()