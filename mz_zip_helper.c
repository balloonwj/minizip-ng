/* mz_zip_helper.c -- Helper functions for zip compression and extraction
   part of the minizip-ng project

   Copyright (C) 2025
     https://github.com/zlib-ng/minizip-ng

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#include "mz.h"
#include "mz_os.h"
#include "mz_strm.h"
#include "mz_strm_os.h"
#include "mz_zip.h"
#include "mz_zip_rw.h"
#include "mz_zip_helper.h"

#include <stdio.h>
#include <string.h>

/***************************************************************************/

int32_t mz_helper_compress_file(const char *zip_path, const char *file_path,
                                const char *password, int16_t compress_level) {
    void *writer = NULL;
    int32_t err = MZ_OK;
    const char *filename_in_zip = NULL;

    if (!zip_path || !file_path)
        return MZ_PARAM_ERROR;

    /* Create zip writer */
    writer = mz_zip_writer_create();
    if (!writer)
        return MZ_MEM_ERROR;

    /* Set compression level */
    mz_zip_writer_set_compress_level(writer, compress_level);

    /* Set password if provided */
    if (password)
        mz_zip_writer_set_password(writer, password);

    /* Open the zip file for writing */
    err = mz_zip_writer_open_file(writer, zip_path, 0, 0);
    if (err != MZ_OK) {
        mz_zip_writer_delete(&writer);
        return err;
    }

    /* Extract filename from path */
    filename_in_zip = strrchr(file_path, '/');
    if (!filename_in_zip)
        filename_in_zip = strrchr(file_path, '\\');
    if (filename_in_zip)
        filename_in_zip++;
    else
        filename_in_zip = file_path;

    /* Add the file to the zip */
    err = mz_zip_writer_add_file(writer, file_path, filename_in_zip);

    /* Close the zip file */
    mz_zip_writer_close(writer);
    mz_zip_writer_delete(&writer);

    return err;
}

/***************************************************************************/

int32_t mz_helper_compress_directory(const char *zip_path, const char *dir_path,
                                     uint8_t include_path, const char *password,
                                     int16_t compress_level) {
    void *writer = NULL;
    int32_t err = MZ_OK;

    if (!zip_path || !dir_path)
        return MZ_PARAM_ERROR;

    /* Create zip writer */
    writer = mz_zip_writer_create();
    if (!writer)
        return MZ_MEM_ERROR;

    /* Set compression level */
    mz_zip_writer_set_compress_level(writer, compress_level);

    /* Set password if provided */
    if (password)
        mz_zip_writer_set_password(writer, password);

    /* Open the zip file for writing */
    err = mz_zip_writer_open_file(writer, zip_path, 0, 0);
    if (err != MZ_OK) {
        mz_zip_writer_delete(&writer);
        return err;
    }

    /* Add the directory to the zip (recursively) */
    err = mz_zip_writer_add_path(writer, dir_path, NULL, include_path, 1);

    /* Close the zip file */
    mz_zip_writer_close(writer);
    mz_zip_writer_delete(&writer);

    return err;
}

/***************************************************************************/

int32_t mz_helper_compress_paths(const char *zip_path, const char **paths,
                                 int32_t path_count, uint8_t include_path,
                                 const char *password, int16_t compress_level) {
    void *writer = NULL;
    int32_t err = MZ_OK;
    int32_t i = 0;
    int32_t is_dir = 0;

    if (!zip_path || !paths || path_count <= 0)
        return MZ_PARAM_ERROR;

    /* Create zip writer */
    writer = mz_zip_writer_create();
    if (!writer)
        return MZ_MEM_ERROR;

    /* Set compression level */
    mz_zip_writer_set_compress_level(writer, compress_level);

    /* Set password if provided */
    if (password)
        mz_zip_writer_set_password(writer, password);

    /* Open the zip file for writing */
    err = mz_zip_writer_open_file(writer, zip_path, 0, 0);
    if (err != MZ_OK) {
        mz_zip_writer_delete(&writer);
        return err;
    }

    /* Add each path to the zip */
    for (i = 0; i < path_count; i++) {
        if (!paths[i])
            continue;

        /* Check if path is a directory */
        is_dir = mz_os_is_dir(paths[i]);
        if (is_dir == MZ_OK) {
            /* Add directory recursively */
            err = mz_zip_writer_add_path(writer, paths[i], NULL, include_path, 1);
        } else {
            /* Add file */
            const char *filename_in_zip = NULL;
            if (!include_path) {
                filename_in_zip = strrchr(paths[i], '/');
                if (!filename_in_zip)
                    filename_in_zip = strrchr(paths[i], '\\');
                if (filename_in_zip)
                    filename_in_zip++;
                else
                    filename_in_zip = paths[i];
            } else {
                filename_in_zip = paths[i];
            }
            err = mz_zip_writer_add_file(writer, paths[i], filename_in_zip);
        }

        if (err != MZ_OK)
            break;
    }

    /* Close the zip file */
    mz_zip_writer_close(writer);
    mz_zip_writer_delete(&writer);

    return err;
}

/***************************************************************************/

/* Helper callback for overwrite confirmation */
static int32_t mz_helper_overwrite_cb(void *handle, void *userdata,
                                      mz_zip_file *file_info, const char *path) {
    uint8_t *overwrite = (uint8_t *)userdata;
    MZ_UNUSED(handle);
    MZ_UNUSED(file_info);
    MZ_UNUSED(path);

    if (overwrite && *overwrite)
        return MZ_OK; /* Overwrite */
    return MZ_EXIST_ERROR; /* Don't overwrite */
}

int32_t mz_helper_extract(const char *zip_path, const char *dest_dir,
                          const char *password, uint8_t overwrite) {
    void *reader = NULL;
    int32_t err = MZ_OK;
    char dest_path[1024];

    if (!zip_path)
        return MZ_PARAM_ERROR;

    /* Create zip reader */
    reader = mz_zip_reader_create();
    if (!reader)
        return MZ_MEM_ERROR;

    /* Set password if provided */
    if (password)
        mz_zip_reader_set_password(reader, password);

    /* Set overwrite callback */
    mz_zip_reader_set_overwrite_cb(reader, &overwrite, mz_helper_overwrite_cb);

    /* Open the zip file for reading */
    err = mz_zip_reader_open_file(reader, zip_path);
    if (err != MZ_OK) {
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Determine destination directory */
    if (dest_dir) {
        strncpy(dest_path, dest_dir, sizeof(dest_path) - 1);
        dest_path[sizeof(dest_path) - 1] = 0;
    } else {
        dest_path[0] = '.';
        dest_path[1] = 0;
    }

    /* Extract all files */
    err = mz_zip_reader_save_all(reader, dest_path);

    /* Close the zip file */
    mz_zip_reader_close(reader);
    mz_zip_reader_delete(&reader);

    return err;
}

/***************************************************************************/

int32_t mz_helper_extract_file(const char *zip_path, const char *filename,
                               const char *dest_dir, const char *password,
                               uint8_t overwrite) {
    void *reader = NULL;
    int32_t err = MZ_OK;
    char dest_path[1024];
    char full_path[2048];

    if (!zip_path || !filename)
        return MZ_PARAM_ERROR;

    /* Create zip reader */
    reader = mz_zip_reader_create();
    if (!reader)
        return MZ_MEM_ERROR;

    /* Set password if provided */
    if (password)
        mz_zip_reader_set_password(reader, password);

    /* Open the zip file for reading */
    err = mz_zip_reader_open_file(reader, zip_path);
    if (err != MZ_OK) {
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Locate the file in the archive */
    err = mz_zip_reader_locate_entry(reader, filename, 0);
    if (err != MZ_OK) {
        mz_zip_reader_close(reader);
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Open the entry for reading */
    err = mz_zip_reader_entry_open(reader);
    if (err != MZ_OK) {
        mz_zip_reader_close(reader);
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Determine destination directory */
    if (dest_dir) {
        strncpy(dest_path, dest_dir, sizeof(dest_path) - 1);
        dest_path[sizeof(dest_path) - 1] = 0;
    } else {
        dest_path[0] = '.';
        dest_path[1] = 0;
    }

    /* Build full destination path */
    snprintf(full_path, sizeof(full_path), "%s/%s", dest_path, filename);

    /* Check if file exists and if we should overwrite */
    if (!overwrite && mz_os_file_exists(full_path) == MZ_OK) {
        mz_zip_reader_entry_close(reader);
        mz_zip_reader_close(reader);
        mz_zip_reader_delete(&reader);
        return MZ_EXIST_ERROR;
    }

    /* Save the file */
    err = mz_zip_reader_entry_save_file(reader, full_path);

    /* Close the entry and zip file */
    mz_zip_reader_entry_close(reader);
    mz_zip_reader_close(reader);
    mz_zip_reader_delete(&reader);

    return err;
}

/***************************************************************************/

int32_t mz_helper_list_files(const char *zip_path, mz_helper_list_cb callback,
                             void *userdata) {
    void *reader = NULL;
    mz_zip_file *file_info = NULL;
    int32_t err = MZ_OK;

    if (!zip_path)
        return MZ_PARAM_ERROR;

    /* Create zip reader */
    reader = mz_zip_reader_create();
    if (!reader)
        return MZ_MEM_ERROR;

    /* Open the zip file for reading */
    err = mz_zip_reader_open_file(reader, zip_path);
    if (err != MZ_OK) {
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Go to first entry */
    err = mz_zip_reader_goto_first_entry(reader);
    if (err != MZ_OK && err != MZ_END_OF_LIST) {
        mz_zip_reader_close(reader);
        mz_zip_reader_delete(&reader);
        return err;
    }

    /* Enumerate all entries */
    while (err == MZ_OK) {
        err = mz_zip_reader_entry_get_info(reader, &file_info);
        if (err != MZ_OK)
            break;

        /* Call user callback if provided */
        if (callback) {
            err = callback(userdata, file_info->filename,
                          file_info->uncompressed_size,
                          file_info->compressed_size);
            if (err != MZ_OK)
                break;
        }

        /* Go to next entry */
        err = mz_zip_reader_goto_next_entry(reader);
    }

    /* Close the zip file */
    mz_zip_reader_close(reader);
    mz_zip_reader_delete(&reader);

    if (err == MZ_END_OF_LIST)
        err = MZ_OK;

    return err;
}

/***************************************************************************/
