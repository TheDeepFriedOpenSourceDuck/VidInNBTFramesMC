#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h> // Include zlib for gzip functionality

// Function to fill a byte array with a specific value and save it to a gzip file
int fill_and_save_byte_array(const char *output_file, unsigned char value) {
    // Allocate memory for the byte array (128 * 128)
    size_t array_size = 128 * 128;
    unsigned char *byte_array = malloc(array_size);
    if (!byte_array) {
        perror("Failed to allocate memory for byte array");
        return 1;
    }

    // Fill the array with the specified value
    for (size_t i = 0; i < array_size; i++) {
        byte_array[i] = value;
    }

    // Open the gzip file for writing
    gzFile dest = gzopen(output_file, "wb");
    if (!dest) {
        perror("Failed to open gzip file");
        free(byte_array);
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
        0x07, 0x00, 0x06, 'c', 'o', 'l', 'o', 'r', 's'        // Byte array tag "colors"
    };

    size_t nbt_data_size = sizeof(nbt_data);

    // Write the fixed part of the NBT data
    if (gzwrite(dest, nbt_data, nbt_data_size) != nbt_data_size) {
        perror("Failed to write NBT data to gzip file");
        gzclose(dest);
        free(byte_array);
        return 1;
    }

    // Write the length of the colors array (4 bytes, big-endian)
    unsigned char color_count_bytes[4] = {
        (array_size >> 24) & 0xFF,
        (array_size >> 16) & 0xFF,
        (array_size >> 8) & 0xFF,
        array_size & 0xFF
    };
    if (gzwrite(dest, color_count_bytes, 4) != 4) {
        perror("Failed to write color count to gzip file");
        gzclose(dest);
        free(byte_array);
        return 1;
    }

    // Write the colors array as bytes
    if (gzwrite(dest, byte_array, array_size) != array_size) {
        perror("Failed to write color data to gzip file");
        gzclose(dest);
        free(byte_array);
        return 1;
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
        free(byte_array);
        return 1;
    }

    // Clean up
    gzclose(dest);
    free(byte_array);

    printf("NBT data successfully saved to %s\n", output_file);
    return 0;
}

int main() {
    char output_file[256]; // Buffer to hold the file path
    for (size_t i = 0; i < 255; i++) {
        snprintf(output_file, sizeof(output_file), 
                 "/home/elias/.var/app/com.mojang.Minecraft/.minecraft/saves/world/data/map_%zu.dat", i);
        unsigned char value = i; // Value to fill the array with
        if (fill_and_save_byte_array(output_file, value) != 0) {
            fprintf(stderr, "Failed to save byte array for map_%zu.dat\n", i);
            return 1;
        }
    }

    return 0;
}