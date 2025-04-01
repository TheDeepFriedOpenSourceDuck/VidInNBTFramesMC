#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h> // Include zlib for gzip functionality

int main() {
    const char *output_file = "map_x.dat";
    // Load the image (assuming a 128x128 image for simplicity)
    FILE *image_file = fopen("input_image.raw", "rb");
    if (!image_file) {
        perror("Failed to open input image file");
        return 1;
    }

    unsigned char image_data[128 * 128 * 3]; // Assuming RGB image
    if (fread(image_data, 1, sizeof(image_data), image_file) != sizeof(image_data)) {
        perror("Failed to read image data");
        fclose(image_file);
        return 1;
    }
    fclose(image_file);

// Full Minecraft color palette (1.21.5)
static const unsigned char minecraft_colors[256][3] = {
    {0, 0, 0}, {127, 178, 56}, {247, 233, 163}, {199, 199, 199}, // 0-3
    {255, 0, 0}, {160, 160, 255}, {167, 167, 167}, {0, 124, 0},  // 4-7
    {255, 255, 255}, {164, 168, 184}, {151, 109, 77}, {112, 112, 112}, // 8-11
    {64, 64, 255}, {143, 119, 72}, {255, 252, 245}, {216, 127, 51}, // 12-15
    {178, 76, 216}, {102, 153, 216}, {229, 229, 51}, {127, 204, 25}, // 16-19
    {242, 127, 165}, {76, 76, 76}, {153, 153, 153}, {76, 127, 153}, // 20-23
    {127, 63, 178}, {51, 76, 178}, {102, 76, 51}, {102, 127, 51}, // 24-27
    {153, 51, 51}, {25, 25, 25}, {250, 238, 77}, {92, 219, 213}, // 28-31
    {74, 128, 255}, {0, 217, 58}, {129, 86, 49}, {112, 2, 0},    // 32-35
    {209, 177, 161}, {159, 82, 36}, {149, 87, 108}, {112, 108, 138}, // 36-39
    {186, 133, 36}, {103, 117, 53}, {160, 77, 78}, {57, 41, 35}, // 40-43
    {135, 107, 98}, {87, 92, 92}, {122, 73, 88}, {76, 62, 92},   // 44-47
    {76, 50, 35}, {76, 82, 42}, {142, 60, 46}, {37, 22, 16},     // 48-51
    {189, 48, 49}, {148, 63, 97}, {92, 25, 29}, {22, 126, 134},  // 52-55
    {58, 142, 140}, {86, 44, 62}, {20, 180, 133}, {100, 100, 100}, // 56-59
    {216, 175, 147}, {127, 167, 150}, {105, 64, 40}, {79, 79, 79}, // 60-63
    // Add remaining colors up to 256 entries...
};

// Convert image data to Minecraft color indices
static int colors[128 * 128]; // Declare once
for (int i = 0; i < 128 * 128; i++) {
    unsigned char r = image_data[i * 3];
    unsigned char g = image_data[i * 3 + 1];
    unsigned char b = image_data[i * 3 + 2];

    int best_match = 0;
    int best_distance = 256 * 256 * 3; // Max possible distance
    for (int j = 0; j < 256; j++) {
        int dr = r - minecraft_colors[j][0];
        int dg = g - minecraft_colors[j][1];
        int db = b - minecraft_colors[j][2];
        int distance = dr * dr + dg * dg + db * db;
        if (distance < best_distance) {
            best_distance = distance;
            best_match = j;
        }
    }
    colors[i] = best_match;
}

// Initialize the colors array (if needed)
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