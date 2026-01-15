#include "engine/fio/tcf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define TCF_MAGIC      "TCF"
#define SHIFT_BITS     2
#define BUFFER_SIZE    8192


#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #define MKDIR(path) mkdir(path, 0755)
#endif


/* ============================================================
   CRC32
   ============================================================ */

static uint32_t crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            uint32_t mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}


/* ============================================================
   Byte unshift (lookup table)
   ============================================================ */

static uint8_t unshift_table[256];
static int unshift_initialized = 0;

static void init_unshift_table(void)
{
    if (unshift_initialized)
        return;

    for (int i = 0; i < 256; i++) {
        unshift_table[i] =
            (uint8_t)((i >> SHIFT_BITS) | (i << (8 - SHIFT_BITS)));
    }

    unshift_initialized = 1;
}


/* ============================================================
   Helpers
   ============================================================ */

static int ensure_dirs(const char *path)
{
    char tmp[1024];
    size_t len = strlen(path);

    if (len >= sizeof(tmp))
        return -1;

    strcpy(tmp, path);

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char saved = *p;
            *p = 0;
            MKDIR(tmp);
            *p = saved;
        }
    }
    return 0;
}

static uint16_t read_u16(FILE *f)
{
    uint8_t b[2];
    if (fread(b, 1, 2, f) != 2)
        return 0;
    return (uint16_t)(b[0] | (b[1] << 8));
}

static uint32_t read_u32(FILE *f)
{
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4)
        return 0;
    return (uint32_t)(
        b[0] |
        (b[1] << 8) |
        (b[2] << 16) |
        (b[3] << 24)
    );
}


/* ============================================================
   Main extractor
   ============================================================ */

int tcf_extract(const char *tcf_path, const char *output_dir)
{
    init_unshift_table();

    FILE *f = fopen(tcf_path, "rb");
    if (!f)
        return TCF_ERR_IO;

    uint8_t header[18];
    if (fread(header, 1, sizeof(header), f) != sizeof(header)) {
        fclose(f);
        return TCF_ERR_FORMAT;
    }

    if (memcmp(header, TCF_MAGIC, 3) != 0) {
        fclose(f);
        return TCF_ERR_FORMAT;
    }

    uint32_t index_offset =
        header[6]  |
        header[7]  << 8 |
        header[8]  << 16 |
        header[9]  << 24;

    uint32_t file_count =
        header[10] |
        header[11] << 8 |
        header[12] << 16 |
        header[13] << 24;

    uint32_t expected_crc =
        header[14] |
        header[15] << 8 |
        header[16] << 16 |
        header[17] << 24;

    if (crc32(header, 14) != expected_crc) {
        fclose(f);
        return TCF_ERR_CRC;
    }

    /* Read payload */
    size_t payload_size = index_offset - sizeof(header);
    uint8_t *payload = (uint8_t *)malloc(payload_size);
    if (!payload) {
        fclose(f);
        return TCF_ERR_MEMORY;
    }

    if (fread(payload, 1, payload_size, f) != payload_size) {
        free(payload);
        fclose(f);
        return TCF_ERR_IO;
    }

    uint8_t buffer[BUFFER_SIZE];

    for (uint32_t i = 0; i < file_count; i++) {
        uint16_t path_len = read_u16(f);

        char *path = (char *)malloc(path_len + 1);
        if (!path) {
            free(payload);
            fclose(f);
            return TCF_ERR_MEMORY;
        }

        fread(path, 1, path_len, f);
        path[path_len] = 0;

        /* Prevent directory traversal */
        if (strstr(path, "..")) {
            free(path);
            continue;
        }

        uint32_t offset = read_u32(f);
        uint32_t size   = read_u32(f);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", output_dir, path);
        ensure_dirs(full_path);

        FILE *out = fopen(full_path, "wb");
        if (!out) {
            free(path);
            free(payload);
            fclose(f);
            return TCF_ERR_IO;
        }

        uint32_t remaining = size;
        uint32_t pos = offset;

        while (remaining > 0) {
            uint32_t chunk =
                remaining > BUFFER_SIZE ? BUFFER_SIZE : remaining;

            for (uint32_t j = 0; j < chunk; j++) {
                buffer[j] = unshift_table[payload[pos + j]];
            }

            fwrite(buffer, 1, chunk, out);
            pos += chunk;
            remaining -= chunk;
        }

        fclose(out);
        free(path);
    }

    free(payload);
    fclose(f);
    return TCF_OK;
}
