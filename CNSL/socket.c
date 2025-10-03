// SERVER PROGRAM - server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_hello(int sock) {
    char buffer[BUFFER_SIZE] = {0};
    
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Client: %s\n", buffer);
    
    char *response = "Hello from Server!";
    send(sock, response, strlen(response), 0);
    printf("Hello message sent to client\n");
}

void handle_file_transfer(int sock) {
    char filename[256] = {0};
    char buffer[BUFFER_SIZE] = {0};
    
    recv(sock, filename, sizeof(filename), 0);
    printf("Receiving file: %s\n", filename);
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("File open failed");
        return;
    }
    
    int bytes;
    long total = 0;
    while ((bytes = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strcmp(buffer, "EOF") == 0) break;
        fwrite(buffer, 1, bytes, fp);
        total += bytes;
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    fclose(fp);
    printf("File received successfully. Total bytes: %ld\n", total);
    
    send(sock, "File received", 13, 0);
}

void handle_calculator(int sock) {
    char buffer[BUFFER_SIZE] = {0};
    double num1, num2, result;
    char op;
    
    recv(sock, buffer, BUFFER_SIZE, 0);
    sscanf(buffer, "%lf %c %lf", &num1, &op, &num2);
    
    printf("Calculation: %.2f %c %.2f\n", num1, op, num2);
    
    switch(op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 != 0) result = num1 / num2;
            else {
                sprintf(buffer, "Error: Division by zero");
                send(sock, buffer, strlen(buffer), 0);
                return;
            }
            break;
        default:
            sprintf(buffer, "Error: Invalid operator");
            send(sock, buffer, strlen(buffer), 0);
            return;
    }
    
    sprintf(buffer, "Result: %.2f", result);
    send(sock, buffer, strlen(buffer), 0);
    printf("Result sent: %.2f\n", result);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    while (1) {
        printf("\nWaiting for client connection...\n");
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        printf("Client connected!\n");
        
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
            
            if (valread <= 0) {
                printf("Client disconnected\n");
                break;
            }
            
            int choice = atoi(buffer);
            
            switch(choice) {
                case 1:
                    handle_hello(new_socket);
                    break;
                case 2:
                    handle_file_transfer(new_socket);
                    break;
                case 3:
                    handle_calculator(new_socket);
                    break;
                case 4:
                    printf("Client requested exit\n");
                    close(new_socket);
                    goto next_client;
                default:
                    send(new_socket, "Invalid choice", 14, 0);
            }
        }
        
        next_client:
        close(new_socket);
    }
    
    close(server_fd);
    return 0;
}

// CLIENT PROGRAM - client.c
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void say_hello(int sock) {
    char buffer[BUFFER_SIZE] = {0};
    char *message = "Hello from Client!";
    
    send(sock, message, strlen(message), 0);
    printf("Hello message sent to server\n");
    
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);
}

void send_file(int sock) {
    char filename[256];
    char buffer[BUFFER_SIZE] = {0};
    
    printf("Enter filename to send: ");
    scanf("%s", filename);
    
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        return;
    }
    
    send(sock, filename, strlen(filename), 0);
    sleep(1);
    
    int bytes;
    long total = 0;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, bytes, 0);
        total += bytes;
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    send(sock, "EOF", 3, 0);
    fclose(fp);
    
    printf("File sent successfully. Total bytes: %ld\n", total);
    
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);
}

void calculator(int sock) {
    char buffer[BUFFER_SIZE] = {0};
    double num1, num2;
    char op;
    
    printf("Enter first number: ");
    scanf("%lf", &num1);
    printf("Enter operator (+, -, *, /): ");
    scanf(" %c", &op);
    printf("Enter second number: ");
    scanf("%lf", &num2);
    
    sprintf(buffer, "%.2f %c %.2f", num1, op, num2);
    send(sock, buffer, strlen(buffer), 0);
    
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);
}

void display_menu() {
    printf("\n=== TCP Socket Menu ===\n");
    printf("1. Say Hello\n");
    printf("2. File Transfer\n");
    printf("3. Calculator\n");
    printf("4. Exit\n");
    printf("Enter choice: ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }
    
    printf("Connected to server!\n");
    
    int choice;
    while (1) {
        display_menu();
        scanf("%d", &choice);
        
        sprintf(buffer, "%d", choice);
        send(sock, buffer, strlen(buffer), 0);
        
        if (choice == 4) {
            printf("Exiting...\n");
            break;
        }
        
        switch(choice) {
            case 1:
                say_hello(sock);
                break;
            case 2:
                send_file(sock);
                break;
            case 3:
                calculator(sock);
                break;
            default:
                printf("Invalid choice!\n");
        }
    }
    
    close(sock);
    return 0;
}
*/