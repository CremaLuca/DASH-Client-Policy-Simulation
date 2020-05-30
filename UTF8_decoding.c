#include <stdio.h>

const char INPUT_FILENAME[] = "UTF8.data";
const char OUTPUT_FILENAME[] = "output.data";

/**
 *  Calcola il numero di parti del simbolo da decodificare dato l'header.
 *  L'header è incluso nel numero.
 */
int getSymbolPartsCount(unsigned char header)
{
    int i = 0;
    // Cicla finchè il primo bit dell'header non è 0
    for (i = 0; (header & 0x80) != 0x00; i++)
    {
        header <<= 1;
    }
    if(i==0){
        return 1;
    }
    return i;
}

/**
 *  Decodifica il contenuto del file INPUT_FILENAME da UTF8 a numeri interi 32 bit e lo salva in OUTPUT_FILENAME.
 *  I simobli all'interno del file INPUT_FILENAME devono seguire la codifica UTF8, con al più 7 byte per simbolo.
 */
int main(char *args[])
{
    FILE *inputDataFile = fopen(INPUT_FILENAME, "r");
    FILE *outputDataFile = fopen(OUTPUT_FILENAME, "w");
    unsigned char buffer[10000]; // Contiene i byte che leggiamo dal file
    int bufferLength, k;
    // Leggiamo il file e posizioniamolo nel buffer
    for (bufferLength = 0; k = fread(buffer, 1, sizeof(buffer) - (bufferLength), inputDataFile); bufferLength += k);
    unsigned int output = 0;
    for (int j = 0; j < bufferLength; j++)
    {
        int parts = getSymbolPartsCount(buffer[j]);
        printf("Header: %d Parts: %d \n", buffer[j], parts);
        output = buffer[j] & (0xFF >> parts); // Rimuovo i bit a 1 prima dello 0 dall'header (es 110xxxxx -> 0xxxxx00 -> 000xxxxx)
        output <<= 6 * (parts - 1); // Moltiplico per 2^#bit nei trailing bytes che verranno, ovvero 6 per ogni byte
        //Leggiamo gli elementi successivi
        for (int i = 1; i < parts; i++)
        {
            j++; // Passaggio al byte successivo
            output += ((buffer[j] & 0x3F) << 6 * (parts - i - 1));
        }
        printf("Output: %x \n", output);
        fwrite(&output, sizeof(int), 1, outputDataFile);
    }
    fclose(inputDataFile);
    fclose(outputDataFile);
}