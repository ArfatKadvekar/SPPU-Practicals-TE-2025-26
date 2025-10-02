#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold IP address
typedef struct {
    int oct[4];
} IP;

// Function to parse IP address from string
int parseIP(char *ipStr, IP *ip) {
    if (sscanf(ipStr, "%d.%d.%d.%d", &ip->oct[0], &ip->oct[1], &ip->oct[2], &ip->oct[3]) != 4) {
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (ip->oct[i] < 0 || ip->oct[i] > 255) {
            return 0;
        }
    }
    return 1;
}

// Function to determine IP class
char determineClass(IP ip) {
    if (ip.oct[0] >= 1 && ip.oct[0] <= 126) return 'A';
    if (ip.oct[0] >= 128 && ip.oct[0] <= 191) return 'B';
    if (ip.oct[0] >= 192 && ip.oct[0] <= 223) return 'C';
    if (ip.oct[0] >= 224 && ip.oct[0] <= 239) return 'D';
    return 'E';
}

// Function to get default subnet mask based on class
void getDefaultMask(char class, IP *mask) {
    if (class == 'A') {
        mask->oct[0] = 255; mask->oct[1] = 0; mask->oct[2] = 0; mask->oct[3] = 0;
    } else if (class == 'B') {
        mask->oct[0] = 255; mask->oct[1] = 255; mask->oct[2] = 0; mask->oct[3] = 0;
    } else if (class == 'C') {
        mask->oct[0] = 255; mask->oct[1] = 255; mask->oct[2] = 255; mask->oct[3] = 0;
    }
}

// Function to calculate subnet mask based on number of subnets
void calculateSubnetMask(char class, int numSubnets, IP *newMask) {
    // Calculate bits needed without using log2 and ceil
    int bitsNeeded = 0;
    int temp = numSubnets - 1;
    while (temp > 0) {
        bitsNeeded++;
        temp >>= 1;
    }
    int defaultBits;
    
    if (class == 'A') defaultBits = 8;
    else if (class == 'B') defaultBits = 16;
    else defaultBits = 24;
    
    int totalBits = defaultBits + bitsNeeded;
    
    // Create mask
    for (int i = 0; i < 4; i++) {
        if (totalBits >= 8) {
            newMask->oct[i] = 255;
            totalBits -= 8;
        } else if (totalBits > 0) {
            newMask->oct[i] = 256 - (1 << (8 - totalBits));
            totalBits = 0;
        } else {
            newMask->oct[i] = 0;
        }
    }
}

// Function to calculate network address
void calculateNetwork(IP ip, IP mask, IP *network) {
    for (int i = 0; i < 4; i++) {
        network->oct[i] = ip.oct[i] & mask.oct[i];
    }
}

// Function to calculate broadcast address
void calculateBroadcast(IP network, IP mask, IP *broadcast) {
    for (int i = 0; i < 4; i++) {
        broadcast->oct[i] = network.oct[i] | (~mask.oct[i] & 255);
    }
}

// Function to print IP
void printIP(IP ip) {
    printf("%d.%d.%d.%d", ip.oct[0], ip.oct[1], ip.oct[2], ip.oct[3]);
}

// Function to convert IP to long for increment
unsigned long ipToLong(IP ip) {
    return ((unsigned long)ip.oct[0] << 24) | 
           ((unsigned long)ip.oct[1] << 16) | 
           ((unsigned long)ip.oct[2] << 8) | 
           ((unsigned long)ip.oct[3]);
}

// Function to convert long to IP
void longToIP(unsigned long val, IP *ip) {
    ip->oct[0] = (val >> 24) & 0xFF;
    ip->oct[1] = (val >> 16) & 0xFF;
    ip->oct[2] = (val >> 8) & 0xFF;
    ip->oct[3] = val & 0xFF;
}

// Function to check if two IPs are equal
int ipEqual(IP ip1, IP ip2) {
    return (ip1.oct[0] == ip2.oct[0] && ip1.oct[1] == ip2.oct[1] && 
            ip1.oct[2] == ip2.oct[2] && ip1.oct[3] == ip2.oct[3]);
}

// Function to check if IP is in same subnet
int sameSubnet(IP ip1, IP ip2, IP mask) {
    IP net1, net2;
    calculateNetwork(ip1, mask, &net1);
    calculateNetwork(ip2, mask, &net2);
    return ipEqual(net1, net2);
}

// Function to perform actual ping
int performPing(IP ip) {
    char cmd[100];
    sprintf(cmd, "ping -c 1 -W 1 %d.%d.%d.%d > /dev/null 2>&1", 
            ip.oct[0], ip.oct[1], ip.oct[2], ip.oct[3]);
    return (system(cmd) == 0);
}

int main() {
    char ipStr[20];
    IP originalIP, defaultMask, newMask, network, broadcast, pingIP;
    char class;
    int numSubnets;
    
    // Input IP address
    printf("Enter the IP: ");
    scanf("%s", ipStr);
    
    if (!parseIP(ipStr, &originalIP)) {
        printf("Invalid IP address!\n");
        return 1;
    }
    
    printf("\n");
    printIP(originalIP);
    printf("\n\n");
    
    // Determine class
    class = determineClass(originalIP);
    printf("Belongs to class: %c\n\n", class);
    
    if (class == 'D' || class == 'E') {
        printf("Class %c addresses are not used for subnetting!\n", class);
        return 1;
    }
    
    // Get default subnet mask
    getDefaultMask(class, &defaultMask);
    printf("Default SUBNET mask: ");
    printIP(defaultMask);
    printf("\n\n");
    
    // Input number of subnets
    printf("No. of Subnets: ");
    scanf("%d", &numSubnets);
    
    if (numSubnets < 2) {
        printf("Number of subnets must be at least 2!\n");
        return 1;
    }
    
    printf("\n%d\n\n", numSubnets);
    
    // Calculate new subnet mask
    calculateSubnetMask(class, numSubnets, &newMask);
    printf("Calculated subnet mask: ");
    printIP(newMask);
    printf("\n\n");
    
    // Calculate base network address using DEFAULT mask first
    IP baseNetwork;
    calculateNetwork(originalIP, defaultMask, &baseNetwork);
    
    // Calculate subnet size (hosts per subnet)
    unsigned long subnetSize = 1;
    for (int i = 3; i >= 0; i--) {
        if (newMask.oct[i] != 255) {
            subnetSize = 256 - newMask.oct[i];
            break;
        }
    }
    
    // Display all subnets
    printf("Range of IPs:\n\n");
    
    // Start from the base network of the original class
    IP currentNetwork = baseNetwork;
    int userSubnet = -1;  // Track which subnet the user's IP belongs to
    
    for (int i = 0; i < numSubnets; i++) {
        IP firstUsable, lastUsable, subnetBroadcast;
        
        calculateBroadcast(currentNetwork, newMask, &subnetBroadcast);
        
        // Calculate first and last usable IPs
        unsigned long netLong = ipToLong(currentNetwork);
        unsigned long bcastLong = ipToLong(subnetBroadcast);
        
        longToIP(netLong + 1, &firstUsable);
        longToIP(bcastLong - 1, &lastUsable);
        
        // Check if user's IP belongs to this subnet
        unsigned long userIPLong = ipToLong(originalIP);
        if (userIPLong >= netLong && userIPLong <= bcastLong) {
            userSubnet = i + 1;
        }
        
        printf("Subnet %d:\n", i + 1);
        printf("  Network IP:    ");
        printIP(currentNetwork);
        printf("\n");
        printf("  Start IP:      ");
        printIP(firstUsable);
        printf("\n");
        printf("  End IP:        ");
        printIP(lastUsable);
        printf("\n");
        printf("  Broadcast IP:  ");
        printIP(subnetBroadcast);
        printf("\n\n");
        
        // Move to next subnet by adding subnet size
        longToIP(netLong + subnetSize, &currentNetwork);
    }
    
    // Display which subnet the user's IP belongs to
    if (userSubnet != -1) {
        printf("The given IP ");
        printIP(originalIP);
        printf(" belongs to Subnet %d\n\n", userSubnet);
    }
    
    // Input IP to ping
    printf("Which IP to ping: ");
    scanf("%s", ipStr);
    
    if (!parseIP(ipStr, &pingIP)) {
        printf("Invalid IP address!\n");
        return 1;
    }
    
    printf("\n");
    printIP(pingIP);
    printf("\n\n");
    
    // Check conditions first
    calculateNetwork(originalIP, newMask, &network);
    calculateBroadcast(network, newMask, &broadcast);
    
    if (ipEqual(pingIP, originalIP)) {
        printf("Yes, it is pinging\n");
        printf("Reason: This is the host's own IP address (self IP).\n");
    }
    else if (ipEqual(pingIP, broadcast)) {
        printf("No, it is not pinging\n");
        printf("Reason: The IP is the broadcast address of the subnet.\n");
    }
    else if (ipEqual(pingIP, network)) {
        printf("No, it is not pinging\n");
        printf("Reason: The IP is the network address and cannot be assigned to a host.\n");
    }
    else if (!sameSubnet(originalIP, pingIP, newMask)) {
        printf("No, it is not pinging\n");
        printf("Reason: The IP belongs to a different subnet (across subnet).\n");
    }
    else {
        // Perform actual ping
        printf("Pinging... Please wait.\n");
        if (performPing(pingIP)) {
            printf("\nYes, it is pinging\n");
            printf("Reason: The host is reachable and in the same subnet.\n");
        } else {
            printf("\nNo, it is not pinging\n");
            printf("Reason: The host is down or unreachable (system shutdown/not responding).\n");
        }
    }
    
    return 0;
}
