/* mz_zip_helper_progress_example.c -- Example with progress callback and cancel support
   part of the minizip-ng project

   Copyright (C) 2025
     https://github.com/zlib-ng/minizip-ng

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#include "mz.h"
#include "mz_os.h"
#include "mz_zip_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************/

/* Progress context for user data */
typedef struct {
    int32_t last_percent;
    int32_t cancel_at_percent;  /* Cancel operation at this percent (0 = no cancel) */
} progress_data;

/* Progress callback function */
static int32_t progress_callback(void *userdata, const char *current_file,
                                int64_t bytes_done, int64_t bytes_total,
                                int32_t file_index, int32_t file_count) {
    progress_data *data = (progress_data *)userdata;
    int32_t percent = 0;

    /* Calculate percentage */
    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    /* Only print when percentage changes */
    if (percent != data->last_percent || bytes_done == 0) {
        printf("\r[File %d/%d] %s: %d%% (%" PRId64 "/%" PRId64 " bytes)    ",
               file_index + 1, file_count, current_file,
               percent, bytes_done, bytes_total);
        fflush(stdout);
        data->last_percent = percent;
    }

    /* Check if we should cancel */
    if (data->cancel_at_percent > 0 && percent >= data->cancel_at_percent) {
        printf("\n\nCanceling operation at %d%%...\n", percent);
        return MZ_HELPER_PROGRESS_CANCEL;
    }

    return MZ_HELPER_PROGRESS_CONTINUE;
}

/***************************************************************************/

static void print_usage(void) {
    printf("Minizip-ng Helper Progress Example\n");
    printf("===================================\n\n");
    printf("This example demonstrates progress callback and cancel support.\n\n");
    printf("Usage:\n");
    printf("  Compress with progress:\n");
    printf("    mz_zip_helper_progress_example -c <zip_file> <file_or_dir>\n\n");
    printf("  Extract with progress:\n");
    printf("    mz_zip_helper_progress_example -x <zip_file> <dest_dir>\n\n");
    printf("  Test cancel at 50%%:\n");
    printf("    mz_zip_helper_progress_example -c <zip_file> <file_or_dir> 50\n\n");
    printf("Examples:\n");
    printf("  mz_zip_helper_progress_example -c output.zip MyFolder\n");
    printf("  mz_zip_helper_progress_example -x archive.zip ./extracted\n");
    printf("  mz_zip_helper_progress_example -c test.zip LargeFile.dat 30\n");
}

/***************************************************************************/

int main(int argc, char *argv[]) {
    int32_t err = MZ_OK;
    progress_data pdata;
    int32_t is_dir = 0;

    /* Initialize progress data */
    pdata.last_percent = -1;
    pdata.cancel_at_percent = 0;

    if (argc < 4) {
        print_usage();
        return 1;
    }

    /* Parse cancel percentage if provided */
    if (argc >= 5) {
        pdata.cancel_at_percent = atoi(argv[4]);
        printf("Will cancel operation at %d%%\n\n", pdata.cancel_at_percent);
    }

    /* Compress operation */
    if (strcmp(argv[1], "-c") == 0) {
        const char *zip_path = argv[2];
        const char *source_path = argv[3];

        /* Check if source is a directory */
        is_dir = mz_os_is_dir(source_path);

        if (is_dir == MZ_OK) {
            printf("Compressing directory '%s' to '%s'...\n", source_path, zip_path);
            err = mz_helper_compress_directory_ex(
                zip_path,
                source_path,
                1,  /* include_path */
                NULL,  /* no password */
                MZ_HELPER_COMPRESS_LEVEL_DEFAULT,
                progress_callback,
                &pdata
            );
        } else {
            printf("Compressing file '%s' to '%s'...\n", source_path, zip_path);
            err = mz_helper_compress_file_ex(
                zip_path,
                source_path,
                NULL,  /* no password */
                MZ_HELPER_COMPRESS_LEVEL_DEFAULT,
                progress_callback,
                &pdata
            );
        }

        printf("\n");
        if (err == MZ_OK) {
            printf("✓ Successfully compressed!\n");
        } else if (err == MZ_USER_CANCELED) {
            printf("✗ Operation was canceled by user\n");
        } else {
            printf("✗ Error compressing: %" PRId32 "\n", err);
        }
    }
    /* Extract operation */
    else if (strcmp(argv[1], "-x") == 0) {
        const char *zip_path = argv[2];
        const char *dest_dir = argv[3];

        printf("Extracting '%s' to '%s'...\n", zip_path, dest_dir);
        err = mz_helper_extract_ex(
            zip_path,
            dest_dir,
            NULL,  /* no password */
            1,  /* overwrite */
            progress_callback,
            &pdata
        );

        printf("\n");
        if (err == MZ_OK) {
            printf("✓ Successfully extracted!\n");
        } else if (err == MZ_USER_CANCELED) {
            printf("✗ Operation was canceled by user\n");
        } else {
            printf("✗ Error extracting: %" PRId32 "\n", err);
        }
    }
    else {
        printf("Error: Unknown option '%s'\n\n", argv[1]);
        print_usage();
        return 1;
    }

    return (err == MZ_OK || err == MZ_USER_CANCELED) ? 0 : 1;
}

/***************************************************************************/
