// Matteo Carnelos - mat. 1163993

#include <stdio.h>
#include <stdint.h>

int main() {
    FILE *in_fp, *out_fp;
    uint8_t head, bytes;
    // The variable quad needs to be uint64_t because in the case of an input codeword with a starting 1 the required
    // shifting amount would be 36. So, considering that the shifting amount should be always less than the variable
    // size (C std 6.5.7p3), the quad variable cannot be uint32_t but instead uint64_t.
    uint64_t quad;
    int8_t i;

    // Open files and abort if something goes wrong
    if(!(in_fp = fopen("input.data", "r")) || !(out_fp = fopen("UTF8.data", "w"))) {
        perror("An error occurred during file opening");
        return 1;
    }

    // Read four bytes at a time and put them in the quad variable until EOF
    while(fread(&quad, 1, 4, in_fp)) {
        // Get the index of the first one starting from the left side (aka head)
        for(head = 31; head > 0 && !(quad >> head & 1); head--);
        // First case, simple ASCII codeword, just write the first byte
        if(head < 7) fputc(quad, out_fp);
        else {
            // Except for the second case, "extra" bytes can be obtained with a formula, knowing that the maximum number
            // of encodable bits increases by 5 every step.
            if(head < 11) bytes = 1;
            else bytes = 2 + (head - 11) / 5;
            // Dinamically create the mask by shifting the sequence 0xFE and making the OR with quad shifted by six
            // times the number of extra bytes. The variable quad does not need to be masked because we know that all
            // the bits at the left of the head are zeros.
            fputc(0xFE << (6 - bytes) | quad >> 6 * bytes, out_fp);
            // Finally write all the extra bytes by making the OR of the sequence 0x80 with quad shifted by a variable
            // amount. This time we need to mask quad because there can be some ones to the left.
            for(i = bytes - 1; i >= 0; i--) fputc(0x80 | (quad >> 6 * i & 0x3F), out_fp);
        }
        // Resetting quad is necessary because if the number of bytes is not a multiple of four we would be left with
        // some quadruplets with leftovers of the previous iteration.
        quad = 0;
    }

    // Print results and exit
    printf("Successfully encoded %ld bytes into %ld bytes.\n", ftell(in_fp), ftell(out_fp));
}
