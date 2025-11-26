/* mz_zip_helper.h -- Helper functions for zip compression and extraction
   part of the minizip-ng project

   Copyright (C) 2025
     https://github.com/zlib-ng/minizip-ng

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#ifndef MZ_ZIP_HELPER_H
#define MZ_ZIP_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/

/* Compression level constants */
#define MZ_HELPER_COMPRESS_LEVEL_DEFAULT    -1  /* Default compression */
#define MZ_HELPER_COMPRESS_LEVEL_FAST        1  /* Fastest compression */
#define MZ_HELPER_COMPRESS_LEVEL_BEST        9  /* Best compression */
#define MZ_HELPER_COMPRESS_LEVEL_STORE       0  /* No compression */

/* Compression method constants */
#define MZ_HELPER_COMPRESS_METHOD_STORE      0  /* Store (no compression) */
#define MZ_HELPER_COMPRESS_METHOD_DEFLATE    8  /* Deflate compression */
#define MZ_HELPER_COMPRESS_METHOD_BZIP2     12  /* BZIP2 compression */
#define MZ_HELPER_COMPRESS_METHOD_LZMA      14  /* LZMA compression */
#define MZ_HELPER_COMPRESS_METHOD_ZSTD      93  /* ZSTD compression */
#define MZ_HELPER_COMPRESS_METHOD_XZ        95  /* XZ compression */

/***************************************************************************/

/**
 * Compress a single file into a zip archive
 *
 * @param zip_path      Path to the output zip file
 * @param file_path     Path to the file to compress
 * @param password      Password for encryption (NULL for no encryption)
 * @param compress_level Compression level (0-9, or MZ_HELPER_COMPRESS_LEVEL_DEFAULT)
 * @return MZ_OK on success, error code otherwise
 */
int32_t mz_helper_compress_file(const char *zip_path, const char *file_path,
                                const char *password, int16_t compress_level);

/**
 * Compress a directory (recursively) into a zip archive
 *
 * @param zip_path       Path to the output zip file
 * @param dir_path       Path to the directory to compress
 * @param include_path   1 to include the directory path in archive, 0 otherwise
 * @param password       Password for encryption (NULL for no encryption)
 * @param compress_level Compression level (0-9, or MZ_HELPER_COMPRESS_LEVEL_DEFAULT)
 * @return MZ_OK on success, error code otherwise
 */
int32_t mz_helper_compress_directory(const char *zip_path, const char *dir_path,
                                     uint8_t include_path, const char *password,
                                     int16_t compress_level);

/**
 * Compress multiple files and/or directories into a zip archive
 *
 * @param zip_path       Path to the output zip file
 * @param paths          Array of file/directory paths to compress
 * @param path_count     Number of paths in the array
 * @param include_path   1 to include paths in archive, 0 otherwise
 * @param password       Password for encryption (NULL for no encryption)
 * @param compress_level Compression level (0-9, or MZ_HELPER_COMPRESS_LEVEL_DEFAULT)
 * @return MZ_OK on success, error code otherwise
 */
int32_t mz_helper_compress_paths(const char *zip_path, const char **paths,
                                 int32_t path_count, uint8_t include_path,
                                 const char *password, int16_t compress_level);

/**
 * Extract a zip archive to a destination directory
 *
 * @param zip_path       Path to the zip file to extract
 * @param dest_dir       Destination directory (NULL for current directory)
 * @param password       Password for decryption (NULL if not encrypted)
 * @param overwrite      1 to overwrite existing files, 0 to skip
 * @return MZ_OK on success, error code otherwise
 */
int32_t mz_helper_extract(const char *zip_path, const char *dest_dir,
                          const char *password, uint8_t overwrite);

/**
 * Extract a specific file from a zip archive
 *
 * @param zip_path       Path to the zip file
 * @param filename       Name of the file in the archive to extract
 * @param dest_dir       Destination directory (NULL for current directory)
 * @param password       Password for decryption (NULL if not encrypted)
 * @param overwrite      1 to overwrite existing files, 0 to skip
 * @return MZ_OK on success, error code otherwise
 */
int32_t mz_helper_extract_file(const char *zip_path, const char *filename,
                               const char *dest_dir, const char *password,
                               uint8_t overwrite);

/**
 * List all files in a zip archive
 *
 * @param zip_path       Path to the zip file
 * @param callback       Callback function called for each entry (can be NULL)
 * @param userdata       User data passed to callback
 * @return MZ_OK on success, error code otherwise
 *
 * Callback signature: int32_t callback(void *userdata, const char *filename,
 *                                      int64_t uncompressed_size, int64_t compressed_size)
 */
typedef int32_t (*mz_helper_list_cb)(void *userdata, const char *filename,
                                     int64_t uncompressed_size, int64_t compressed_size);

int32_t mz_helper_list_files(const char *zip_path, mz_helper_list_cb callback,
                             void *userdata);

/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MZ_ZIP_HELPER_H */
