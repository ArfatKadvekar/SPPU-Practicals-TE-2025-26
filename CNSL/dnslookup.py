import socket

def forward_lookup(domain):
    try:
        ip = socket.gethostbyname(domain)
        print("\nForward Lookup:")
        print(f"Domain : {domain}")
        print(f"IP     : {ip}")

        try:
            host = socket.gethostbyaddr(ip)
            print("\nReverse Lookup:")
            print(f"IP     : {ip}")
            print(f"Host   : {host[0]}")
        except socket.herror:
            print("\nReverse Lookup failed: No PTR record found.")

    except socket.gaierror:
        print("Invalid domain name or DNS lookup failed.")


def reverse_lookup(ip):
    try:
        host = socket.gethostbyaddr(ip)
        print("\nReverse Lookup:")
        print(f"IP     : {ip}")
        print(f"Host   : {host[0]}")
        try:
            ip_check = socket.gethostbyname(host[0])
            print("\nForward Verification:")
            print(f"Host   : {host[0]}")
            print(f"IP     : {ip_check}")
        except socket.gaierror:
            print("\nForward verification failed.")
    except socket.herror:
        print("Invalid IP address or reverse DNS lookup failed.")


def main():
    while True:
        print("\n====== DNS Lookup Tool ======")
        print("1. Domain -> IP (Forward Lookup)")
        print("2. IP -> Domain (Reverse Lookup)")
        print("3. Exit")

        choice = input("Enter your choice: ").strip()

        if choice == '1':
            domain = input("Enter domain name: ").strip()
            forward_lookup(domain)
        elif choice == '2':
            ip = input("Enter IP address: ").strip()
            reverse_lookup(ip)
        elif choice == '3':
            print("Exiting...")
            break
        else:
            print("Invalid choice! Please enter 1, 2, or 3.")

if __name__ == "__main__":
    main()
