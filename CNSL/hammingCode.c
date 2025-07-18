#include <stdio.h>
#include <string.h>

int isPowerOfTwo(int n) {
    return (n > 0) && (n & (n - 1)) == 0;
}

int calculateParityBits(int dataBits) {
    int r = 0;
    int powerOf2 = 1;
    while (powerOf2 < dataBits + r + 1) {
        r++;
        powerOf2 = powerOf2 << 1; // Equivalent to powerOf2 * 2
    }
    return r;
}

void generateHammingCode(int dataBits[], int dataSize, int hamming[], int hammingSize) {
    int dataIndex = 0;
    
    // Initialize all positions to 0
    for (int i = 0; i < hammingSize; i++) {
        hamming[i] = 0;
    }
    
    // Place data bits in non-parity positions (skip positions that are powers of 2)
    for (int i = 1; i <= hammingSize; i++) {
        if (!isPowerOfTwo(i)) {
            if (dataIndex < dataSize) {
                hamming[i-1] = dataBits[dataIndex++];
            }
        }
    }
    
    // Calculate parity bits using loops
    int parityBits = calculateParityBits(dataSize);
    
    for (int i = 0; i < parityBits; i++) {
        int parityPos = 1 << i; // Position of parity bit (1, 2, 4, 8, ...) using bit shift
        int parity = 0;
        
        // Check all positions that have this parity bit set in their binary representation
        for (int j = 1; j <= hammingSize; j++) {
            if (j & parityPos) { // If bit i is set in position j
                parity ^= hamming[j-1];
            }
        }
        
        hamming[parityPos-1] = parity;
    }
}

void processCharacter(char ch) {
    if (ch == '\n') return;
    
    // Display character info
    if (ch == ' ')
        printf("\033[41m Character: [space] \033[0m -> ");
    else
        printf("\033[41m Character: %c \033[0m -> ", ch);
    
    printf("\033[42m ASCII: %d \033[0m -> \033[44m Binary: \033[0m", (int)ch);
    
    // Convert to binary (8 bits for ASCII)
    int dataBits[8];
    for (int j = 7; j >= 0; j--) {
        int bit = (ch >> j) & 1;
        dataBits[7 - j] = bit;
        printf("\033[44m%d\033[0m", bit);
    }
    
    // Calculate Hamming code
    int parityBits = calculateParityBits(8);
    int hammingSize = 8 + parityBits;
    int hamming[hammingSize];
    
    generateHammingCode(dataBits, 8, hamming, hammingSize);
    
    // Display Hamming code
    printf(" -> \033[43m Hamming Code (%d bits): \033[0m", hammingSize);
    for (int i = 0; i < hammingSize; i++) {
        if (isPowerOfTwo(i + 1)) {
            printf("\033[45m%d\033[0m", hamming[i]); // Parity bits in magenta
        } else {
            printf("\033[46m%d\033[0m", hamming[i]); // Data bits in cyan
        }
    }
    
    printf("\n");
}

void getAsciiBinary(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        if (ch == '\n')
            continue;

        // Call processCharacter for each character
        processCharacter(ch);
    }
}

int main() {

    printf("Enter the Text: ");
    char str[100];
    fgets(str, sizeof(str), stdin);
    
    // Remove newline if present
    str[strcspn(str, "\n")] = 0;
    
    getAsciiBinary(str);
    return 0;
}
