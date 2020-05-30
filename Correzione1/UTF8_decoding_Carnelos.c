// Matteo Carnelos - mat. 1163993

#include <stdio.h>
#include <stdint.h>

int main() {
    FILE *in_fp, *out_fp;
    uint8_t ones, bytes;
    uint32_t quad;
    // The variable buf needs to be uint64_t because in the case of an output codeword with a starting 1 the required
    // shifting amount would be 36. So, considering that the shifting amount should be always less than the variable
    // size (C std 6.5.7p3), the buf variable cannot be uint32_t but instead uint64_t.
    uint64_t buf;

    // Open files and abort if something goes wrong
    if(!(in_fp = fopen("UTF8.data", "r")) || !(out_fp = fopen("output.data", "w"))) {
        perror("An error occurred during file opening");
        return 1;
    }

    // Read one byte at a time and put it in the buf variable until EOF
    while(fread(&buf, 1, 1, in_fp)) {
        // Get the number of starting ones by shifting and masking buf and checking the boolean value of the bit
        for(ones = 0; ones < 8 && buf >> (7 - ones) & 1; ones++);
        // First case, no starting ones, simple ASCII codeword, just write the word to the file. Other cases can be
        // handled modularly. The case with all ones is considered wrong and thus skipped.
        if(!ones) putw(buf, out_fp);
        else if(ones < 8) {
            // If there are more than one byte it means that we are approaching a new encoded codeword. Set the number
            // of bytes that we expect to receive.
            if(ones != 1) bytes = ones;
            // Mask and shift the buffer with a dinamically created mask and set the correspondent bits of the quad
            // variable. We do not have to mask the quad variable because it will always have zeros in the corresponding
            // positions. Also decrement the bytes left to decode.
            quad |= (buf & 0xFF >> (1 + ones)) << 6 * --bytes;
            // When we have zero bytes left to decode we can write the quadruplet in the file and reset the quad
            // variable, this because if the next encoded codeword is shorter than the current one we will have some
            // leftovers that will infect the next decoding.
            if(!bytes) { putw(quad, out_fp); quad = 0; }
        }
    }

    // Print results and exit
    printf("Successfully decoded %ld bytes into %ld bytes.\n", ftell(in_fp), ftell(out_fp));
}
