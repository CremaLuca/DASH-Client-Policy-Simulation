#include <stdio.h>

const char INPUT_FILENAME[] = "myinput.data";
const char OUTPUT_FILENAME[] = "myoutput.txt";

int getSignificativeBits(int number)
{
    return 32 - __builtin_clz(number);
}

int main(char *args[])
{
    FILE *inputDataFile = fopen(INPUT_FILENAME, "r");
    FILE *outputDataFile = fopen(OUTPUT_FILENAME, "w");
    int buffer[10000]; // Contiene temporaneamente i 4 byte che andiamo a leggere dal file
    int size, k;
    for (size = 0; k = fread(buffer, 4, sizeof(buffer) - size, inputDataFile); size += k)
        ;
    for (int j = 0; j < size; j++)
    {
        unsigned int element = buffer[j];
        int signBits = getSignificativeBits(element);
        printf("read data %d: %x, bits: %d \n", j, element, signBits);
        if (signBits <= 7)
        {
            //7 bit ASCII
            char output = (char)element;
            printf("output: %x \n", output);
            continue;
        }
        else
        {
            char extraBits = signBits - 7; // Quanti bit in più ci sono rispetto al livello precedente.
            char parts = 2;                // Segna in quante parti andranno divisi i 4 byte letti
            short header = 0xffc0;         // il primo byte a destra di questo numero sarà l'header del primo byte di output
            while (extraBits >= 5)
            {
                extraBits -= 5;
                header >>= 1;
                parts++;
            }
            // Ora siamo pronti per elaborare l'output
            char output[parts];
            header = header & 0xFF; // Rimozione del pezzo extra
            printf("parts: %d, header: %hhx \n", parts, header);
            for (int i = 0; i < parts - 1; i++)
            {
                output[parts - i - 1] = 0x80 + ((element)&0x3f);
                printf("output %d: %hhX\n", (parts - i - 1), output[parts - i - 1]);
                element >>= 6;
            }
            output[0] = header + ((element)&0x1f);
            printf("output 0: %hhX \n", output[0]);
            fwrite(output, 1, parts, outputDataFile);
        }
    }
    fclose(inputDataFile);
    fclose(outputDataFile);
}