"""
UDP Server Program - Media File Transfer Only
"""

import socket
import os

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
HOST = get_local_ip()
PORT = 5555
BUFFER_SIZE = 65507  # Maximum UDP packet size

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

def handle_file_transfer(server_socket):
    
    print("\n[WAITING] Listening for file transfer packets...")
    
    try:
        # STEP 1: Receive metadata packet (filename, filesize, num_packets)
        metadata_packet, client_address = server_socket.recvfrom(BUFFER_SIZE)
        metadata = metadata_packet.decode('utf-8')
        
        print(f"[RECEIVED] Metadata from {client_address}: {metadata}")
        
        # Parse metadata: "METADATA|filename|filesize|num_packets"
        parts = metadata.split('|')
        if parts[0] != "METADATA":
            print(f"[ERROR] Invalid metadata packet from {client_address}")
            return
        
        filename = parts[1]
        filesize = int(parts[2])
        num_packets = int(parts[3])
        
    except Exception as e:
        print(f"[ERROR] Failed to receive metadata: {e}")
        return
    
    # Validate media file
    is_valid, file_category = is_media_file(filename)
    if not is_valid:
        print(f"[REJECTED] Non-media file from {client_address}: {filename}\n")
        return
    
    print(f"\n[FILE TRANSFER] Receiving '{filename}' from {client_address}")
    print(f"[FILE TYPE] {file_category.upper()}")
    print(f"[FILE SIZE] {filesize} bytes ({filesize/1024:.2f} KB)")
    print(f"[PACKETS] Expecting {num_packets} UDP packets")
    print(f"[UDP MODE] Connectionless - NO ACK will be sent")
    print("-" * 60)
    
    received_packets = {}
    packets_received = 0
    
    print("[RECEIVING] Listening for data packets...")
    
    # Set a timeout to avoid waiting forever
    server_socket.settimeout(5.0)
    
    # Keep receiving until we get all packets or timeout
    while packets_received < num_packets:
        try:
            # Receive packet (blocking call with timeout)
            packet_data, _ = server_socket.recvfrom(BUFFER_SIZE)
            
            # Extract packet number (first 10 bytes)
            packet_num_str = packet_data[:10].decode('utf-8').strip()
            packet_num = int(packet_num_str)
            
            # Extract file data (remaining bytes)
            data = packet_data[10:]
            
            # Store packet (may arrive out of order)
            if packet_num not in received_packets:
                received_packets[packet_num] = data
                packets_received += 1
            
            # Show progress (no ACK sent!)
            if packets_received % 100 == 0 or packets_received == num_packets:
                percent = (packets_received / num_packets) * 100
                print(f"[PROGRESS] Received {packets_received}/{num_packets} packets ({percent:.1f}%) - NO ACK SENT")
        
        except socket.timeout:
            # No more packets received for 5 seconds, assume transfer complete
            print(f"[TIMEOUT] No more packets received. Stopping...")
            break
        except Exception as e:
            print(f"[ERROR] Error receiving packet: {e}")
            break
    
    # Reset timeout
    server_socket.settimeout(None)
    
    # STEP 3: Check for packet loss
    missing_packets = []
    for i in range(num_packets):
        if i not in received_packets:
            missing_packets.append(i)
    
    if missing_packets:
        print(f"\n[PACKET LOSS] {len(missing_packets)} packets lost!")
        print(f"[MISSING] Packet numbers: {missing_packets[:10]}{'...' if len(missing_packets) > 10 else ''}")
        print("[WARNING] File may be incomplete or corrupted!")
    else:
        print(f"\n[SUCCESS] All {num_packets} packets received!")
    
    # STEP 4: Reassemble file from received packets
    filepath = os.path.join("received_files", filename)
    os.makedirs("received_files", exist_ok=True)
    
    print(f"[ASSEMBLING] Writing received packets to file...")
    
    bytes_written = 0
    try:
        with open(filepath, 'wb') as f:
            for i in range(num_packets):
                if i in received_packets:
                    f.write(received_packets[i])
                    bytes_written += len(received_packets[i])
                else:
                    # Packet missing - write zeros or skip
                    print(f"[WARNING] Packet {i} missing!")
        
        print(f"[FILE SAVED] '{filepath}'")
        print(f"[BYTES] Expected: {filesize}, Received: {bytes_written}")
        
        if bytes_written == filesize:
            print(f"[STATUS] Complete transfer")
        else:
            print(f"[STATUS] Incomplete transfer ({filesize - bytes_written} bytes lost)")
    
    except Exception as e:
        print(f"[ERROR] Failed to save file: {e}")
    
    print("=" * 60 + "\n")

def start_server():
    """Start the UDP server"""
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    try:
        server.bind((HOST, PORT))
    except OSError as e:
        print(f"\n[ERROR] Could not bind to {HOST}:{PORT}")
        print(f"Error: {e}")
        return
    
    print("=" * 60)
    print("         UDP MEDIA FILE TRANSFER SERVER")
    print("=" * 60)
    print(f"[SERVER IP] {HOST}")
    print(f"[SERVER PORT] {PORT}")
    print(f"[PROTOCOL] UDP (Connectionless, Unreliable)")
    print(f"[BUFFER SIZE] {BUFFER_SIZE} bytes")
    print("\n[SUPPORTED FILES]")

    print(f"  Script: {', '.join(ALLOWED_EXTENSIONS['script'])}")
    print(f"  Text:   {', '.join(ALLOWED_EXTENSIONS['text'])}")
    print(f"  Audio:  {', '.join(ALLOWED_EXTENSIONS['audio'])}")
    print(f"  Video:  {', '.join(ALLOWED_EXTENSIONS['video'])}")
    print(f"  Images:  {', '.join(ALLOWED_EXTENSIONS['image'])}")
    print("=" * 60)
    print("[READY] Server is ready to receive files")
    print("Press Ctrl+C to stop")
    print("=" * 60)
    
    try:
        while True:
            handle_file_transfer(server)
    except KeyboardInterrupt:
        print("\n\n[SHUTDOWN] Server stopping...")
    finally:
        server.close()
        print("[CLOSED] Server socket closed.\n")

if __name__ == "__main__":
    start_server()