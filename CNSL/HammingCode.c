#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int isPowerOfTwo(int n)
{
    return (n > 0) && (n & (n - 1)) == 0;
}

int calculateParityBits(int dataBits)
{
    int r = 0;
    int powerOf2 = 1;
    while (powerOf2 < dataBits + r + 1)
    {
        r++;
        powerOf2 = powerOf2 << 1;
    }
    return r;
}

void generateHammingCode(int dataBits[], int dataSize, int hamming[], int hammingSize)
{
    int dataIndex = 0;
    for (int i = 0; i < hammingSize; i++)
    {
        hamming[i] = 0;
    }

    for (int i = 1; i <= hammingSize; i++)
    {
        if (!isPowerOfTwo(i))
        {
            if (dataIndex < dataSize)
            {
                hamming[i - 1] = dataBits[dataIndex++];
            }
        }
    }
    int parityBits = calculateParityBits(dataSize);
    for (int i = 0; i < parityBits; i++)
    {
        int parityPos = 1 << i;
        int parity = 0;

        for (int j = 1; j <= hammingSize; j++)
        {
            if (j & parityPos)
            {
                parity ^= hamming[j - 1];
            }
        }
        hamming[parityPos - 1] = parity;
    }
}

void processCharacterEncode(char ch, int allHamming[][12], int *hammingCount)
{
    if (ch == '\n')
        return;
    if (ch == ' ')
        printf("\033[41m Character: [space] \033[0m -> ");
    else
        printf("\033[41m Character: %c \033[0m -> ", ch);

    printf("\033[42m ASCII: %d \033[0m -> \033[44m Binary: \033[0m", (int)ch);

    int dataBits[8];
    for (int j = 7; j >= 0; j--)
    {
        int bit = (ch >> j) & 1;
        dataBits[7 - j] = bit;
        printf("\033[44m%d\033[0m", bit);
    }

    int parityBits = calculateParityBits(8);
    int hammingSize = 8 + parityBits;
    int hamming[hammingSize];
    generateHammingCode(dataBits, 8, hamming, hammingSize);

    for (int i = 0; i < 12; i++)
    {
        allHamming[*hammingCount][i] = hamming[i];
    }
    (*hammingCount)++;

    // Display Hamming code
    printf(" -> \033[43m Hamming Code (%d bits): \033[0m", hammingSize);
    for (int i = 0; i < hammingSize; i++)
    {
        if (isPowerOfTwo(i + 1))
        {
            printf("\033[45m%d\033[0m", hamming[i]); // Parity bits in magenta
        }
        else
        {
            printf("\033[46m%d\033[0m", hamming[i]); // Data bits in cyan
        }
    }
    printf("\n");
}

int detectAndCorrectError(int hamming[], int hammingSize)
{
    int parityBits = calculateParityBits(8);
    int errorPosition = 0;
    for (int i = 0; i < parityBits; i++)
    {
        int parityPos = 1 << i;
        int parity = 0;
        for (int j = 1; j <= hammingSize; j++)
        {
            if (j & parityPos)
            {
                parity ^= hamming[j - 1];
            }
        }
        if (parity != 0)
        {
            errorPosition += parityPos;
        }
    }
    return errorPosition;
}

void extractDataBits(int hamming[], int hammingSize, int dataBits[], int dataSize)
{
    int dataIndex = 0;
    for (int i = 1; i <= hammingSize && dataIndex < dataSize; i++)
    {
        if (!isPowerOfTwo(i))
        {

            dataBits[dataIndex++] = hamming[i - 1];
        }
    }
}

char binaryToChar(int binary[], int size)
{

    char ch = 0;
    for (int i = 0; i < size; i++)
    {
        ch = (ch << 1) | binary[i];
    }
    return ch;
}

char processCharacterDecode(int hamming[], int hammingSize)
{
    printf("\033[43m Received \033[0m");
    for (int i = 0; i < hammingSize; i++)
    {

        if (isPowerOfTwo(i + 1))
        {
            printf("\033[45m%d\033[0m", hamming[i]); // Parity bits in magenta
        }
        else
        {
            printf("\033[46m%d\033[0m", hamming[i]); // Data bits in cyan
        }
    }

    int errorPosition = detectAndCorrectError(hamming, hammingSize);
    if (errorPosition == 0)
    {

        printf(" -> \033[42m No error detected \033[0m");
    }
    else
    {

        printf(" -> \033[41m Error detected at position %d, correcting... \033[0m", errorPosition);
        hamming[errorPosition - 1] = 1 - hamming[errorPosition - 1];
    }

    int dataBits[8];
    extractDataBits(hamming, hammingSize, dataBits, 8);
    char ch = binaryToChar(dataBits, 8);
    printf(" -> \033[44m Binary: \033[0m");
    for (int i = 0; i < 8; i++)
    {
        printf("\033[44m%d\033[0m", dataBits[i]);
    }
    printf(" -> \033[42m ASCII: %d \033[0m -> ", (int)ch);
    if (ch == ' ')
        printf("\033[41m Character: [space] \033[0m");

    else if (ch >= 32 && ch <= 126)
        printf("\033[41m Character: %c \033[0m", ch);

    else
        printf("\033[41m Character: [non-printable] \033[0m");

    printf("\n");
    return ch;
}

int main()
{

    printf("Enter the Text: ");
    char str[100];
    fgets(str, sizeof(str), stdin);
    // Remove newline if present
    str[strcspn(str, "\n")] = 0;
    int allHamming[100][12];
    int hammingCount = 0;
    printf("\n========== ENCODING ==========\n\n");
    for (int i = 0; str[i] != '\0'; i++)
    {
        char ch = str[i];
        if (ch == '\n')
            continue;
        processCharacterEncode(ch, allHamming, &hammingCount);
    }
    printf("\n========== DECODING ==========\n\n");
    char decodedText[100];
    for (int i = 0; i < hammingCount; i++)
    {
        char ch = processCharacterDecode(allHamming[i], 12);
        decodedText[i] = ch;
    }
    decodedText[hammingCount] = '\0';
    printf("\n\033[47mDECODED TEXT -> %s \033[0m \n", decodedText);
    return 0;
}