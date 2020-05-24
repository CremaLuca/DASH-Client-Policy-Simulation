#include <stdio.h>

const char INPUT_FILENAME[] = "input.data";
const char OUTPUT_FILENAME[] = "UTF8.data";

/**
 *  Calcola la posizione del bit più significativo del parametro intero.
 *  I valori variano da 0 a 31.
 */
int getSignificativeBitPosition(unsigned int number)
{
    // builtin_clz è una funzione che calcola quanti zeri ci sono prima del bit pìu significativo.
    return 31 - __builtin_clz(number);
}

/**
 *  Codifica i numeri interi a 32 bit contenuti nel file INPUT_FILENAME in UTF8 e li salva in OUTPUT_FILENAME.
 */
int main(char *args[])
{
    FILE *inputDataFile = fopen(INPUT_FILENAME, "r");
    FILE *outputDataFile = fopen(OUTPUT_FILENAME, "w");
    int buffer[10000]; // Contiene gli interi che leggiamo dal file
    int bufferLength, k;
    // Leggiamo il file e posizioniamolo nel buffer
    for (bufferLength = 0; k = fread(buffer, 4, sizeof(buffer) - (bufferLength * 4), inputDataFile); bufferLength += k);
    // Andiamo a leggere il buffer
    for (int j = 0; j < bufferLength; j++)
    {
        unsigned int element = buffer[j]; // Unsigned perchè così lo shift verso destra viene riempito con valori 0 a sinistra
        char output[7] = {0};  // Non sappiamo a priori quanto spazio occupa, inizializzamo alla lunghezza massima: 7 byte (caso primo bit = 1)
        if (getSignificativeBitPosition(element) < 7) //Se ci sta in un solo byte allora è un ascii
        {
            output[6] = (char)element;
            fwrite(&output[6], 1, 1, outputDataFile);
            continue; // Per evitare altre linee di codice 
        }
        else
        {
            short header = 0xff80; // Il primo byte a destra diventerà l'header del primo byte di output
            int parts = 1; // Numero di byte dell'output
            // Strappiamo 6 bit alla volta da destra dal numero finchè non ha sufficientemente pochi bit da starci nel primo byte insieme all'header
            for (parts = 1; getSignificativeBitPosition(element) > 6 - parts && parts < 7; parts++)
            {
                header >>= 1; // Aggiungiamo un bit a sinistra al primo byte
                // Salviamo i valori a partire dalle ultime celle dell'array
                output[7 - parts] = 0x80 + ((element)&0x3f); // 10xxxxxx + applicazione del filtro 00111111 sull'elemento
                element >>= 6;
            }
            // Rendiamo l'header di un solo byte e sommiamogli quello che rimane del numero originale
            output[7 - parts] = (header & 0xFF) + element;
            fwrite(&output[7 - parts], parts, 1, outputDataFile);
        }
    }
    fclose(inputDataFile);
    fclose(outputDataFile);
}