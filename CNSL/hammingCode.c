#include <stdio.h>

void generateHammingCode(int dataBits[8], int hamming[12])
{
    int d = 0;
    for (int j = 0; j < 12; j++)
    {
        if (j == 0 || j == 1 || j == 3 || j == 7)
            hamming[j] = 0; 
        else
            hamming[j] = dataBits[d++];
    }

    hamming[0] = hamming[2] ^ hamming[4] ^ hamming[6] ^ hamming[8] ^ hamming[10];
    hamming[1] = hamming[2] ^ hamming[5] ^ hamming[6] ^ hamming[9] ^ hamming[10];
    hamming[3] = hamming[4] ^ hamming[5] ^ hamming[6] ^ hamming[11];
    hamming[7] = hamming[8] ^ hamming[9] ^ hamming[10] ^ hamming[11];
}

void getAsciiBinary(const char *text)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        char ch = text[i];
        if (ch == '\n')
            continue;

        if (ch == ' ')
            printf("Character: [space] -> ");
        else
            printf("Character: %c -> ", ch);

        printf("ASCII: %d -> Binary: ", (int)ch);

        int dataBits[8];
        for (int j = 7; j >= 0; j--)
        {
            int bit = (ch >> j) & 1;
            dataBits[7 - j] = bit; 
            printf("%d", bit);
        }

        int hamming[12];
        generateHammingCode(dataBits, hamming);

        printf(" -> Hamming Code: ");
        for (int j = 0; j < 12; j++)
        {
            printf("%d", hamming[j]);
        }

        printf("\n");
    }
}

int main()
{
    printf("Enter the Text: ");
    char str[100];
    fgets(str, sizeof(str), stdin);
    getAsciiBinary(str);
    return 0;
}
