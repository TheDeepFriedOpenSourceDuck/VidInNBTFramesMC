#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h> // Include zlib for gzip functionality

int main() {
    const char *output_file = "map_x.dat";

    // Example colors array
    static int colors[128 * 128]; // Declare as static to ensure it has a fixed memory location
    for (int i = 0; i < 128 * 128; i++) {
        colors[i] = 45; // Set all elements to 45
    }
    
    size_t color_count = sizeof(colors) / sizeof(colors[0]);

    // Open the gzip file for writing (but name it without .gz extension)
    gzFile dest = gzopen(output_file, "wb");
    if (!dest) {
        perror("Failed to open gzip file");
        return 1;
    }

    // Example NBT data for a Minecraft map item (uncompressed binary format)
    unsigned char nbt_data[] = {
        0x0A, 0x00, 0x00,                   // Compound tag (no name)
        0x0A, 0x00, 0x04, 'd', 'a', 't', 'a', // Compound tag with name "data"
        0x01, 0x00, 0x05, 's', 'c', 'a', 'l', 'e', 0x03, // Byte tag "scale" = 3
        0x03, 0x00, 0x09, 'd', 'i', 'm', 'e', 'n', 's', 'i', 'o', 'n', 0x00, 0x00, 0x00, 0x00, // Int tag "dimension" = 0
        0x01, 0x00, 0x10, 't', 'r', 'a', 'c', 'k', 'i', 'n', 'g', 'P', 'o', 's', 'i', 't', 'i', 'o', 'n', 0x00, // Byte tag "trackingPosition" = 1
        0x01, 0x00, 0x06, 'l', 'o', 'c', 'k', 'e', 'd', 0x01, // Byte tag "locked" = 0
        0x07, 0x00, 0x06, 'c', 'o', 'l', 'o', 'r', 's' // Byte array tag "colors"
    };

    size_t data_size = sizeof(nbt_data);

    // Write the fixed part of the NBT data
    if (gzwrite(dest, nbt_data, data_size) != data_size) {
        perror("Failed to write NBT data to gzip file");
        gzclose(dest);
        return 1;
    }

    // Write the length of the colors array (4 bytes, big-endian)
    unsigned char color_count_bytes[4] = {
        (color_count >> 24) & 0xFF,
        (color_count >> 16) & 0xFF,
        (color_count >> 8) & 0xFF,
        color_count & 0xFF
    };
    if (gzwrite(dest, color_count_bytes, 4) != 4) {
        perror("Failed to write color count to gzip file");
        gzclose(dest);
        return 1;
    }

    // Write the colors array as bytes
    for (size_t i = 0; i < color_count; i++) {
        unsigned char color_byte = (unsigned char)(colors[i] & 0xFF);
        if (gzwrite(dest, &color_byte, 1) != 1) {
            perror("Failed to write color data to gzip file");
            gzclose(dest);
            return 1;
        }
    }

    // Write the end of the "data" compound tag and the root compound tag
    unsigned char end_tags[] = {
        0x00,                             // End of "data" compound tag
        0x03, 0x00, 0x0B, 'D', 'a', 't', 'a', 'V', 'e', 'r', 's', 'i', 'o', 'n', 0x00, 0x00, 0x0F, 0xA0, // Int tag "DataVersion" = 4000
        0x00                              // End of root compound tag
    };
    if (gzwrite(dest, end_tags, sizeof(end_tags)) != sizeof(end_tags)) {
        perror("Failed to write end tags to gzip file");
        gzclose(dest);
        return 1;
    }

    gzclose(dest);

    printf("NBT data successfully gzipped to %s\n", output_file);
    return 0;
}