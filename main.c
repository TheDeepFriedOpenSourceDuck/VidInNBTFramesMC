#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <zlib.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

// Minecraft color palette
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
    {45, 45, 180}, {100, 100, 255}, {150, 150, 255}, {200, 200, 255}, // 64-67
    {255, 255, 255}, {255, 255, 200}, {255, 255, 150}, {255, 255, 100}, // 68-71
    {255, 255, 50}, {255, 255, 0}, {255, 200, 0}, {255, 150, 0}, // 72-75
    {255, 100, 0}, {255, 50, 0}, {255, 0, 0}, {200, 0, 0},       // 76-79
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 80-83
    {0, 50, 0}, {0, 100, 0}, {0, 150, 0}, {0, 200, 0},           // 84-87
    {0, 255, 0}, {50, 255, 0}, {100, 255, 0}, {150, 255, 0},     // 88-91
    {200, 255, 0}, {255, 255, 0}, {255, 200, 0}, {255, 150, 0},  // 92-95
    {255, 100, 0}, {255, 50, 0}, {255, 0, 0}, {200, 0, 0},       // 96-99
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 100-103
    {0, 0, 50}, {0, 0, 100}, {0, 0, 150}, {0, 0, 200},           // 104-107
    {0, 0, 255}, {50, 0, 255}, {100, 0, 255}, {150, 0, 255},     // 108-111
    {200, 0, 255}, {255, 0, 255}, {255, 50, 255}, {255, 100, 255}, // 112-115
    {255, 150, 255}, {255, 200, 255}, {255, 255, 255}, {200, 200, 200}, // 116-119
    {150, 150, 150}, {100, 100, 100}, {50, 50, 50}, {0, 0, 0},   // 120-123
    {0, 50, 50}, {0, 100, 100}, {0, 150, 150}, {0, 200, 200},    // 124-127
    {0, 255, 255}, {50, 255, 255}, {100, 255, 255}, {150, 255, 255}, // 128-131
    {200, 255, 255}, {255, 255, 255}, {255, 200, 200}, {255, 150, 150}, // 132-135
    {255, 100, 100}, {255, 50, 50}, {255, 0, 0}, {200, 0, 0},    // 136-139
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 140-143
    {0, 50, 0}, {0, 100, 0}, {0, 150, 0}, {0, 200, 0},           // 144-147
    {0, 255, 0}, {50, 255, 0}, {100, 255, 0}, {150, 255, 0},     // 148-151
    {200, 255, 0}, {255, 255, 0}, {255, 200, 0}, {255, 150, 0},  // 152-155
    {255, 100, 0}, {255, 50, 0}, {255, 0, 0}, {200, 0, 0},       // 156-159
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 160-163
    {0, 0, 50}, {0, 0, 100}, {0, 0, 150}, {0, 0, 200},           // 164-167
    {0, 0, 255}, {50, 0, 255}, {100, 0, 255}, {150, 0, 255},     // 168-171
    {200, 0, 255}, {255, 0, 255}, {255, 50, 255}, {255, 100, 255}, // 172-175
    {255, 150, 255}, {255, 200, 255}, {255, 255, 255}, {200, 200, 200}, // 176-179
    {150, 150, 150}, {100, 100, 100}, {50, 50, 50}, {0, 0, 0},   // 180-183
    {0, 50, 50}, {0, 100, 100}, {0, 150, 150}, {0, 200, 200},    // 184-187
    {0, 255, 255}, {50, 255, 255}, {100, 255, 255}, {150, 255, 255}, // 188-191
    {200, 255, 255}, {255, 255, 255}, {255, 200, 200}, {255, 150, 150}, // 192-195
    {255, 100, 100}, {255, 50, 50}, {255, 0, 0}, {200, 0, 0},    // 196-199
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 200-203
    {0, 50, 0}, {0, 100, 0}, {0, 150, 0}, {0, 200, 0},           // 204-207
    {0, 255, 0}, {50, 255, 0}, {100, 255, 0}, {150, 255, 0},     // 208-211
    {200, 255, 0}, {255, 255, 0}, {255, 200, 0}, {255, 150, 0},  // 212-215
    {255, 100, 0}, {255, 50, 0}, {255, 0, 0}, {200, 0, 0},       // 216-219
    {150, 0, 0}, {100, 0, 0}, {50, 0, 0}, {0, 0, 0},             // 220-223
    {0, 0, 50}, {0, 0, 100}, {0, 0, 150}, {0, 0, 200},           // 224-227
    {0, 0, 255}, {50, 0, 255}, {100, 0, 255}, {150, 0, 255},     // 228-231
    {200, 0, 255}, {255, 0, 255}, {255, 50, 255}, {255, 100, 255}, // 232-235
    {255, 150, 255}, {255, 200, 255}, {255, 255, 255}, {200, 200, 200}, // 236-239
    {150, 150, 150}, {100, 100, 100}, {50, 50, 50}, {0, 0, 0},   // 240-243
    {0, 50, 50}, {0, 100, 100}, {0, 150, 150}, {0, 200, 200},    // 244-247
    {0, 255, 255}, {50, 255, 255}, {100, 255, 255}, {150, 255, 255}, // 248-251
    {200, 255, 255}, {255, 255, 255}, {255, 200, 200}, {255, 150, 150}, // 252-255
};

// Function to find the nearest Minecraft color code
int find_nearest_minecraft_color(unsigned char r, unsigned char g, unsigned char b) {
    int nearest_color_index = 0;
    int min_distance = 256 * 256 * 3; // Maximum possible distance

    for (int i = 0; i < 256; i++) {
        int dr = r - minecraft_colors[i][0];
        int dg = g - minecraft_colors[i][1];
        int db = b - minecraft_colors[i][2];
        int distance = dr * dr + dg * dg + db * db; // Squared Euclidean distance

        if (distance < min_distance) {
            min_distance = distance;
            nearest_color_index = i;
        }
    }

    return nearest_color_index;
}


// Function to save Minecraft map data to a file
int fill_and_save_byte_array(const char *output_file, const int *colors) {
    gzFile dest = gzopen(output_file, "wb");
    if (!dest) {
        perror("Failed to open gzip file");
        return 1;
    }

    // Example NBT data for a Minecraft map item (uncompressed binary format)
    unsigned char nbt_header[] = {
        0x0A, 0x00, 0x00,                   // Compound tag (no name)
        0x0A, 0x00, 0x04, 'd', 'a', 't', 'a', // Compound tag with name "data"
        0x01, 0x00, 0x05, 's', 'c', 'a', 'l', 'e', 0x03, // Byte tag "scale" = 3
        0x03, 0x00, 0x09, 'd', 'i', 'm', 'e', 'n', 's', 'i', 'o', 'n', 0x00, 0x00, 0x00, 0x00, // Int tag "dimension" = 0
        0x01, 0x00, 0x10, 't', 'r', 'a', 'c', 'k', 'i', 'n', 'g', 'P', 'o', 's', 'i', 't', 'i', 'o', 'n', 0x00, // Byte tag "trackingPosition" = 0
        0x01, 0x00, 0x06, 'l', 'o', 'c', 'k', 'e', 'd', 0x01, // Byte tag "locked" = 1
        0x07, 0x00, 0x06, 'c', 'o', 'l', 'o', 'r', 's'        // Byte array tag "colors"
    };

    size_t header_size = sizeof(nbt_header);

    // Write the fixed part of the NBT data
    if (gzwrite(dest, nbt_header, header_size) != header_size) {
        perror("Failed to write NBT header to gzip file");
        gzclose(dest);
        return 1;
    }

    // Write the length of the colors array (4 bytes, big-endian)
    unsigned char color_count_bytes[4] = {
        (128 * 128 >> 24) & 0xFF,
        (128 * 128 >> 16) & 0xFF,
        (128 * 128 >> 8) & 0xFF,
        128 * 128 & 0xFF
    };
    if (gzwrite(dest, color_count_bytes, 4) != 4) {
        perror("Failed to write color count to gzip file");
        gzclose(dest);
        return 1;
    }

    // Write the colors array as bytes
    for (size_t i = 0; i < 128 * 128; i++) {
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
    return 0;
}
void process_directory(const char *input_directory, const char *output_directory) {
    struct dirent *entry;
    DIR *dp = opendir(input_directory);

    if (!dp) {
        perror("Failed to open input directory");
        return;
    }

    int map_count = 0; // Counter for the processed maps

    while ((entry = readdir(dp)) != NULL) {
        // Check if the file has a .png or .jpg extension
        if (strstr(entry->d_name, ".png") || strstr(entry->d_name, ".jpg")) {
            char input_path[512];
            snprintf(input_path, sizeof(input_path), "%s/%s", input_directory, entry->d_name);

            // Load the image
            int width, height, channels;
            unsigned char *image = stbi_load(input_path, &width, &height, &channels, 3);
            if (!image) {
                fprintf(stderr, "Failed to load image: %s\n", input_path);
                continue;
            }

            // Calculate new dimensions
            int new_width, new_height;
            if (width > height) {
                new_width = 128;
                new_height = (height * 128) / width;
            } else {
                new_height = 128;
                new_width = (width * 128) / height;
            }

            // Resize the image
            unsigned char *resized_image = malloc(128 * 128 * 3);
            memset(resized_image, 0, 128 * 128 * 3); // Fill with black (0, 0, 0)
            stbir_resize_uint8(image, width, height, 0, resized_image, new_width, new_height, 0, 3);

            // Free the original image
            stbi_image_free(image);

            // Convert the resized image to Minecraft color codes
            int colors[128 * 128];
            for (int y = 0; y < 128; y++) {
                for (int x = 0; x < 128; x++) {
                    int index = (y * 128 + x) * 3;
                    unsigned char r = resized_image[index];
                    unsigned char g = resized_image[index + 1];
                    unsigned char b = resized_image[index + 2];
                    colors[y * 128 + x] = find_nearest_minecraft_color(r, g, b);
                }
            }

            // Create the output file path
            char output_path[512];
            snprintf(output_path, sizeof(output_path), "%s/map_%d.dat", output_directory, map_count++);

            // Save the Minecraft map
            if (fill_and_save_byte_array(output_path, colors) != 0) {
                fprintf(stderr, "Failed to save Minecraft map: %s\n", output_path);
            }

            // Free the resized image
            free(resized_image);
        }
    }

    closedir(dp);
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_directory> <output_directory>\n", argv[0]);
        return 1;
    }

    const char *input_directory = argv[1];
    const char *output_directory = argv[2];

    process_directory(input_directory, output_directory);
    return 0;
}