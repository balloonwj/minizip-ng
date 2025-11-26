/* mz_zip_helper_test.c -- Test program for zip helper functions with absolute paths
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

/* Test results counter */
typedef struct test_stats_s {
    int32_t total;
    int32_t passed;
    int32_t failed;
} test_stats;

static test_stats stats = {0, 0, 0};

/***************************************************************************/

/* Print test result */
static void print_test_result(const char *test_name, int32_t result) {
    stats.total++;
    if (result == MZ_OK) {
        printf("[PASS] %s\n", test_name);
        stats.passed++;
    } else {
        printf("[FAIL] %s (error: %" PRId32 ")\n", test_name, result);
        stats.failed++;
    }
}

/***************************************************************************/

/* Create a test file with some content */
static int32_t create_test_file(const char *file_path, const char *content) {
    FILE *file = NULL;
    size_t written = 0;

#ifdef _WIN32
    fopen_s(&file, file_path, "wb");
#else
    file = fopen(file_path, "wb");
#endif

    if (!file) {
        printf("Error: Failed to create test file: %s\n", file_path);
        return MZ_STREAM_ERROR;
    }

    if (content) {
        written = fwrite(content, 1, strlen(content), file);
        if (written != strlen(content)) {
            fclose(file);
            return MZ_STREAM_ERROR;
        }
    }

    fclose(file);
    return MZ_OK;
}

/***************************************************************************/

/* Create test directory structure */
static int32_t create_test_structure(const char *base_path) {
    char path[1024];
    int32_t err = MZ_OK;

    /* Create base directory */
    err = mz_os_make_dir(base_path);
    if (err != MZ_OK && err != MZ_EXIST_ERROR) {
        printf("Error: Failed to create base directory: %s\n", base_path);
        return err;
    }

    /* Create subdirectory */
    snprintf(path, sizeof(path), "%s/subdir", base_path);
    err = mz_os_make_dir(path);
    if (err != MZ_OK && err != MZ_EXIST_ERROR) {
        printf("Error: Failed to create subdirectory: %s\n", path);
        return err;
    }

    /* Create test files */
    snprintf(path, sizeof(path), "%s/test1.txt", base_path);
    err = create_test_file(path, "This is test file 1 in the base directory.");
    if (err != MZ_OK)
        return err;

    snprintf(path, sizeof(path), "%s/test2.txt", base_path);
    err = create_test_file(path, "This is test file 2 in the base directory.");
    if (err != MZ_OK)
        return err;

    snprintf(path, sizeof(path), "%s/subdir/test3.txt", base_path);
    err = create_test_file(path, "This is test file 3 in the subdirectory.");
    if (err != MZ_OK)
        return err;

    return MZ_OK;
}

/***************************************************************************/

/* Test: Compress a single file with absolute path */
static void test_compress_file_absolute(const char *test_dir) {
    char file_path[1024];
    char zip_path[1024];
    int32_t err;

    snprintf(file_path, sizeof(file_path), "%s/test1.txt", test_dir);
    snprintf(zip_path, sizeof(zip_path), "%s/test_single.zip", test_dir);

    err = mz_helper_compress_file(zip_path, file_path, NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);
    print_test_result("Compress single file (absolute path)", err);
}

/***************************************************************************/

/* Test: Compress a single file with password */
static void test_compress_file_password(const char *test_dir) {
    char file_path[1024];
    char zip_path[1024];
    int32_t err;

    snprintf(file_path, sizeof(file_path), "%s/test2.txt", test_dir);
    snprintf(zip_path, sizeof(zip_path), "%s/test_password.zip", test_dir);

    err = mz_helper_compress_file(zip_path, file_path, "testpass123", MZ_HELPER_COMPRESS_LEVEL_BEST);
    print_test_result("Compress single file with password", err);
}

/***************************************************************************/

/* Test: Compress a directory with absolute path */
static void test_compress_directory_absolute(const char *test_dir) {
    char zip_path[1024];
    int32_t err;

    snprintf(zip_path, sizeof(zip_path), "%s/test_directory.zip", test_dir);

    err = mz_helper_compress_directory(zip_path, test_dir, 0, NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);
    print_test_result("Compress directory (absolute path, no path included)", err);
}

/***************************************************************************/

/* Test: Compress a directory with path included */
static void test_compress_directory_with_path(const char *test_dir) {
    char zip_path[1024];
    int32_t err;

    snprintf(zip_path, sizeof(zip_path), "%s/test_dir_with_path.zip", test_dir);

    err = mz_helper_compress_directory(zip_path, test_dir, 1, NULL, MZ_HELPER_COMPRESS_LEVEL_FAST);
    print_test_result("Compress directory (absolute path, path included)", err);
}

/***************************************************************************/

/* Test: Compress multiple paths */
static void test_compress_multiple_paths(const char *test_dir) {
    char file1[1024], file2[1024], subdir[1024], zip_path[1024];
    const char *paths[3];
    int32_t err;

    snprintf(file1, sizeof(file1), "%s/test1.txt", test_dir);
    snprintf(file2, sizeof(file2), "%s/test2.txt", test_dir);
    snprintf(subdir, sizeof(subdir), "%s/subdir", test_dir);
    snprintf(zip_path, sizeof(zip_path), "%s/test_multiple.zip", test_dir);

    paths[0] = file1;
    paths[1] = file2;
    paths[2] = subdir;

    err = mz_helper_compress_paths(zip_path, paths, 3, 0, NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);
    print_test_result("Compress multiple paths", err);
}

/***************************************************************************/

/* Callback for listing files */
static int32_t list_callback(void *userdata, const char *filename,
                             int64_t uncompressed_size, int64_t compressed_size) {
    int32_t *count = (int32_t *)userdata;
    printf("    [%d] %s (%" PRId64 " bytes -> %" PRId64 " bytes)\n",
           *count, filename, uncompressed_size, compressed_size);
    (*count)++;
    return MZ_OK;
}

/* Test: List files in archive */
static void test_list_files(const char *test_dir) {
    char zip_path[1024];
    int32_t err;
    int32_t count = 0;

    snprintf(zip_path, sizeof(zip_path), "%s/test_directory.zip", test_dir);

    printf("  Listing files in test_directory.zip:\n");
    err = mz_helper_list_files(zip_path, list_callback, &count);
    print_test_result("List files in archive", err);
}

/***************************************************************************/

/* Test: Extract archive to absolute path */
static void test_extract_absolute(const char *test_dir) {
    char zip_path[1024];
    char extract_path[1024];
    int32_t err;

    snprintf(zip_path, sizeof(zip_path), "%s/test_directory.zip", test_dir);
    snprintf(extract_path, sizeof(extract_path), "%s/extracted", test_dir);

    /* Create extraction directory */
    mz_os_make_dir(extract_path);

    err = mz_helper_extract(zip_path, extract_path, NULL, 1);
    print_test_result("Extract archive (absolute path)", err);
}

/***************************************************************************/

/* Test: Extract archive with password */
static void test_extract_password(const char *test_dir) {
    char zip_path[1024];
    char extract_path[1024];
    int32_t err;

    snprintf(zip_path, sizeof(zip_path), "%s/test_password.zip", test_dir);
    snprintf(extract_path, sizeof(extract_path), "%s/extracted_password", test_dir);

    /* Create extraction directory */
    mz_os_make_dir(extract_path);

    err = mz_helper_extract(zip_path, extract_path, "testpass123", 1);
    print_test_result("Extract archive with password", err);
}

/***************************************************************************/

/* Test: Extract single file */
static void test_extract_single_file(const char *test_dir) {
    char zip_path[1024];
    char extract_path[1024];
    int32_t err;

    snprintf(zip_path, sizeof(zip_path), "%s/test_directory.zip", test_dir);
    snprintf(extract_path, sizeof(extract_path), "%s/extracted_single", test_dir);

    /* Create extraction directory */
    mz_os_make_dir(extract_path);

    err = mz_helper_extract_file(zip_path, "test1.txt", extract_path, NULL, 1);
    print_test_result("Extract single file", err);
}

/***************************************************************************/

/* Test: Compression levels */
static void test_compression_levels(const char *test_dir) {
    char file_path[1024];
    char zip_path[1024];
    int32_t err;

    snprintf(file_path, sizeof(file_path), "%s/test1.txt", test_dir);

    /* Test store (no compression) */
    snprintf(zip_path, sizeof(zip_path), "%s/test_store.zip", test_dir);
    err = mz_helper_compress_file(zip_path, file_path, NULL, MZ_HELPER_COMPRESS_LEVEL_STORE);
    print_test_result("Compress with STORE level (no compression)", err);

    /* Test fast compression */
    snprintf(zip_path, sizeof(zip_path), "%s/test_fast.zip", test_dir);
    err = mz_helper_compress_file(zip_path, file_path, NULL, MZ_HELPER_COMPRESS_LEVEL_FAST);
    print_test_result("Compress with FAST level", err);

    /* Test best compression */
    snprintf(zip_path, sizeof(zip_path), "%s/test_best.zip", test_dir);
    err = mz_helper_compress_file(zip_path, file_path, NULL, MZ_HELPER_COMPRESS_LEVEL_BEST);
    print_test_result("Compress with BEST level", err);
}

/***************************************************************************/

/* Main test runner */
int main(int argc, char *argv[]) {
    char test_dir[2048];
    int32_t err;

    printf("========================================\n");
    printf("Minizip-ng Helper Functions Test Suite\n");
    printf("========================================\n\n");

    /* Determine test directory path */
    if (argc > 1) {
        /* Use user-provided path (absolute path) */
        strncpy(test_dir, argv[1], sizeof(test_dir) - 1);
        test_dir[sizeof(test_dir) - 1] = 0;
    } else {
        /* Use default test directory relative to current path */
        strncpy(test_dir, "mz_test_dir", sizeof(test_dir) - 1);
        test_dir[sizeof(test_dir) - 1] = 0;
    }

    printf("Test directory: %s\n", test_dir);
    printf("  (You can specify a custom path as command line argument)\n\n");

    /* Create test directory structure */
    printf("Creating test directory structure...\n");
    err = create_test_structure(test_dir);
    if (err != MZ_OK) {
        printf("Error: Failed to create test structure\n");
        return 1;
    }
    printf("Test structure created successfully.\n\n");

    /* Run tests */
    printf("Running tests:\n");
    printf("----------------------------------------\n");

    test_compress_file_absolute(test_dir);
    test_compress_file_password(test_dir);
    test_compress_directory_absolute(test_dir);
    test_compress_directory_with_path(test_dir);
    test_compress_multiple_paths(test_dir);
    test_compression_levels(test_dir);
    test_list_files(test_dir);
    test_extract_absolute(test_dir);
    test_extract_password(test_dir);
    test_extract_single_file(test_dir);

    /* Print summary */
    printf("----------------------------------------\n");
    printf("\nTest Summary:\n");
    printf("  Total:  %d\n", stats.total);
    printf("  Passed: %d\n", stats.passed);
    printf("  Failed: %d\n", stats.failed);
    printf("\n");

    if (stats.failed > 0) {
        printf("RESULT: Some tests FAILED!\n");
        return 1;
    } else {
        printf("RESULT: All tests PASSED!\n");
        return 0;
    }
}

/***************************************************************************/
