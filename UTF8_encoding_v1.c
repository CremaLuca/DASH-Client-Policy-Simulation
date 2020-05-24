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
    //FILE *outputDataFile = fopen(OUTPUT_FILENAME, "r");
    int *buffer; // Contiene temporaneamente i 4 byte che andiamo a leggere dal file
    while (fread(buffer, sizeof(buffer), 1, inputDataFile))
    {
        int signBits = getSignificativeBits(*buffer);
        printf("read data %x, bits %d \n", *buffer, signBits);
        if (signBits <= 7)
        {
            //7 bit ASCII
            char output = (char)*buffer;
            printf("output: %x \n", output);
            continue;
        }
        else
        {
            char extraBits = signBits - 7; // Quanti bit in più ci sono rispetto al livello precedente.
            char parts = 2;        // Segna in quante parti andranno divisi i 4 byte letti
            short header = 0xffc0; // il primo byte a destra di questo numero sarà l'header del primo byte di output
            while (extraBits > 5)
            {
                extraBits -= 5;
                header = header >> 1;
                parts++;
            }
            // Ora siamo pronti per elaborare l'output
            char output[parts];
            printf("parts: %d \n", parts);
            header = header & 0xFF; // Rimozione del pezzo extra
            for (int i = 0; i < parts - 1; i++)
            {
                output[parts - i - 1] = 0x80 + ((*buffer) & 0x3f);
                *buffer = *buffer >> 6;
                printf("output %d: %hhX \n", (parts - i - 1), output[parts - i - 1]);
            }
            output[parts - 1] = header + ((*buffer) & 0x1f);
            printf("output %d: %hhX \n", (parts - 1), output[parts - 1]);
            // COPIA QUESTA PARTE PER CAPIRE COSA STA SUCCEDENDO
            for(int i=0; i<7; i++){
                printf("o[%d] = %hhX ",i,output[i]);
            }
            printf("\n");
        }
    }
    fclose(inputDataFile);
    //fclose(outputDataFile);
}
