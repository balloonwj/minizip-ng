/* mz_zip_helper_example.c -- Example program demonstrating zip helper functions
   part of the minizip-ng project

   Copyright (C) 2025
     https://github.com/zlib-ng/minizip-ng

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#include "mz.h"
#include "mz_zip_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************/

/* Callback function for listing files */
static int32_t list_callback(void *userdata, const char *filename,
                             int64_t uncompressed_size, int64_t compressed_size) {
    MZ_UNUSED(userdata);
    printf("  %s (%" PRId64 " bytes, compressed: %" PRId64 " bytes)\n",
           filename, uncompressed_size, compressed_size);
    return MZ_OK;
}

/***************************************************************************/

static void print_usage(void) {
    printf("Minizip-ng Helper Example\n");
    printf("=========================\n\n");
    printf("Usage:\n");
    printf("  Compress a file:\n");
    printf("    mz_zip_helper_example -c <zip_file> <file_to_compress> [password]\n\n");
    printf("  Compress a directory:\n");
    printf("    mz_zip_helper_example -cd <zip_file> <directory> [password]\n\n");
    printf("  Extract a zip file:\n");
    printf("    mz_zip_helper_example -x <zip_file> <dest_directory> [password]\n\n");
    printf("  List files in a zip:\n");
    printf("    mz_zip_helper_example -l <zip_file>\n\n");
    printf("Examples:\n");
    printf("  mz_zip_helper_example -c output.zip myfile.txt\n");
    printf("  mz_zip_helper_example -c output.zip myfile.txt mypassword\n");
    printf("  mz_zip_helper_example -cd backup.zip MyDocuments\n");
    printf("  mz_zip_helper_example -x archive.zip ./extracted\n");
    printf("  mz_zip_helper_example -l archive.zip\n");
}

/***************************************************************************/

int main(int argc, char *argv[]) {
    int32_t err = MZ_OK;
    const char *password = NULL;
    int16_t compress_level = MZ_HELPER_COMPRESS_LEVEL_DEFAULT;

    if (argc < 3) {
        print_usage();
        return 1;
    }

    /* Compress a single file */
    if (strcmp(argv[1], "-c") == 0) {
        if (argc < 4) {
            printf("Error: Missing arguments for compress file\n\n");
            print_usage();
            return 1;
        }

        if (argc >= 5)
            password = argv[4];

        printf("Compressing file '%s' to '%s'...\n", argv[3], argv[2]);
        err = mz_helper_compress_file(argv[2], argv[3], password, compress_level);

        if (err == MZ_OK)
            printf("Successfully compressed file\n");
        else
            printf("Error compressing file: %" PRId32 "\n", err);
    }
    /* Compress a directory */
    else if (strcmp(argv[1], "-cd") == 0) {
        if (argc < 4) {
            printf("Error: Missing arguments for compress directory\n\n");
            print_usage();
            return 1;
        }

        if (argc >= 5)
            password = argv[4];

        printf("Compressing directory '%s' to '%s'...\n", argv[3], argv[2]);
        err = mz_helper_compress_directory(argv[2], argv[3], 1, password, compress_level);

        if (err == MZ_OK)
            printf("Successfully compressed directory\n");
        else
            printf("Error compressing directory: %" PRId32 "\n", err);
    }
    /* Extract a zip file */
    else if (strcmp(argv[1], "-x") == 0) {
        if (argc < 4) {
            printf("Error: Missing arguments for extract\n\n");
            print_usage();
            return 1;
        }

        if (argc >= 5)
            password = argv[4];

        printf("Extracting '%s' to '%s'...\n", argv[2], argv[3]);
        err = mz_helper_extract(argv[2], argv[3], password, 1);

        if (err == MZ_OK)
            printf("Successfully extracted files\n");
        else
            printf("Error extracting files: %" PRId32 "\n", err);
    }
    /* List files in a zip */
    else if (strcmp(argv[1], "-l") == 0) {
        printf("Listing files in '%s':\n", argv[2]);
        err = mz_helper_list_files(argv[2], list_callback, NULL);

        if (err == MZ_OK)
            printf("\nSuccessfully listed files\n");
        else
            printf("\nError listing files: %" PRId32 "\n", err);
    }
    else {
        printf("Error: Unknown option '%s'\n\n", argv[1]);
        print_usage();
        return 1;
    }

    return (err == MZ_OK) ? 0 : 1;
}

/***************************************************************************/
