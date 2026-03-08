/* SPDX-License-Identifier: Zlib
 * Copyright (c) 2026 Tbaggerofsteam, Tbaggeroftheuk
 */

#include "common/tcf/tcf.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

#define TCF_MAGIC      "TCF"
#define VERSION        1
#define ENDIANNESS     0
#define SHIFT_BITS     2
#define BUFFER_SIZE    8192

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define PATH_SEP '\\'
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#define PATH_SEP '/'
#endif

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

static uint8_t shift_table[256];
static uint8_t unshift_table[256];
static bool tables_initialized = false;

static void init_tables()
{
    if (tables_initialized)
        return;

    for (int i = 0; i < 256; i++) {
        shift_table[i] = (uint8_t)(((i << SHIFT_BITS) & 0xFF) |
                                   (i >> (8 - SHIFT_BITS)));
        unshift_table[i] = (uint8_t)((i >> SHIFT_BITS) | 
                                     (i << (8 - SHIFT_BITS)));
    }

    tables_initialized = true;
}

struct TCFEntry {
    std::string path;
    uint32_t offset;
    uint32_t size;
};

static std::vector<TCFEntry> entries;

static void add_entry(const std::string& path, uint32_t size)
{
    entries.emplace_back(TCFEntry{path, 0, size});
}

int ensure_dirs(const char *path) {
    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());
    return 0;
}

static uint16_t read_u16(std::ifstream& f)
{
    uint8_t b[2];
    f.read(reinterpret_cast<char*>(b), 2);
    if (!f) return 0;
    return static_cast<uint16_t>(b[0] | (b[1] << 8));
}

static uint32_t read_u32(std::ifstream& f)
{
    uint8_t b[4];
    f.read(reinterpret_cast<char*>(b), 4);
    if (!f) return 0;
    return static_cast<uint32_t>(
        b[0] |
        (b[1] << 8) |
        (b[2] << 16) |
        (b[3] << 24)
    );
}

static void walk_dir(const std::filesystem::path& base, const std::filesystem::path& rel = "")
{
    std::filesystem::path current = base / rel;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(current)) {
        if (entry.is_regular_file()) {
            std::filesystem::path relative_path = std::filesystem::relative(entry.path(), base);
            std::string path_str = relative_path.generic_string(); // Use forward slashes
            uint32_t size = static_cast<uint32_t>(entry.file_size());
            add_entry(path_str, size);
        }
    }
}

int tcf_pack(const char *input_dir, const char *out_path)
{
    init_tables();
    entries.clear();
    walk_dir(input_dir);

    std::ofstream out(out_path, std::ios::binary);
    if (!out) return TCF_ERR_IO;

    uint8_t header[18] = {0};
    out.write(reinterpret_cast<char*>(header), sizeof(header));

    std::vector<uint8_t> buffer(BUFFER_SIZE);
    uint32_t offset = 0;

    for (auto& entry : entries) {
        entry.offset = offset;

        std::filesystem::path full_path = std::filesystem::path(input_dir) / entry.path;
        std::ifstream in(full_path, std::ios::binary);
        if (!in) return TCF_ERR_IO;

        size_t r;
        while ((r = in.readsome(reinterpret_cast<char*>(buffer.data()), BUFFER_SIZE)) > 0) {
            for (size_t j = 0; j < r; j++)
                buffer[j] = shift_table[buffer[j]];

            out.write(reinterpret_cast<char*>(buffer.data()), r);
        }

        offset += entry.size;
    }

    uint32_t index_offset = static_cast<uint32_t>(out.tellp());

    for (const auto& entry : entries) {
        uint16_t len = static_cast<uint16_t>(entry.path.size());
        out.write(reinterpret_cast<char*>(&len), 2);
        out.write(entry.path.data(), len);
        uint32_t offset_temp = entry.offset;
        out.write(reinterpret_cast<char*>(&offset_temp), 4);
        uint32_t size_temp = entry.size;
        out.write(reinterpret_cast<char*>(&size_temp), 4);
    }

    out.write("EOF", 3);

    std::memcpy(header + 0, TCF_MAGIC, 3);
    header[3] = VERSION;
    header[4] = 0;
    header[5] = ENDIANNESS;

    std::memcpy(header + 6,  &index_offset, 4);
    size_t entry_count = entries.size();
    std::memcpy(header + 10, &entry_count,  4);

    uint32_t crc = crc32(header, 14);
    std::memcpy(header + 14, &crc, 4);

    out.seekp(0);
    out.write(reinterpret_cast<char*>(header), sizeof(header));

    return TCF_OK;
}

int tcf_extract(const char *tcf_path, const char *output_dir)
{
    init_tables();

    std::ifstream f(tcf_path, std::ios::binary);
    if (!f)
        return TCF_ERR_IO;

    uint8_t header[18];
    f.read(reinterpret_cast<char*>(header), sizeof(header));
    if (!f || f.gcount() != sizeof(header)) {
        return TCF_ERR_FORMAT;
    }

    if (std::memcmp(header, TCF_MAGIC, 3) != 0) {
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
        return TCF_ERR_CRC;
    }

    /* Read payload */
    size_t payload_size = index_offset - sizeof(header);
    std::vector<uint8_t> payload(payload_size);
    f.read(reinterpret_cast<char*>(payload.data()), payload_size);
    if (!f || f.gcount() != static_cast<std::streamsize>(payload_size)) {
        return TCF_ERR_IO;
    }

    std::vector<uint8_t> buffer(BUFFER_SIZE);

    for (uint32_t i = 0; i < file_count; i++) {
        uint16_t path_len = read_u16(f);
        if (!f) return TCF_ERR_IO;

        std::string path(path_len, '\0');
        f.read(&path[0], path_len);
        if (!f) return TCF_ERR_IO;

        if (path.find("..") != std::string::npos) {
            continue;
        }

        uint32_t offset = read_u32(f);
        if (!f) return TCF_ERR_IO;
        uint32_t size = read_u32(f);
        if (!f) return TCF_ERR_IO;

        std::filesystem::path full_path = std::filesystem::path(output_dir) / path;
        ensure_dirs(full_path.string().c_str());

        std::ofstream out(full_path, std::ios::binary);
        if (!out) {
            return TCF_ERR_IO;
        }

        uint32_t remaining = size;
        uint32_t pos = offset;

        while (remaining > 0) {
            uint32_t chunk = std::min(remaining, static_cast<uint32_t>(BUFFER_SIZE));

            for (uint32_t j = 0; j < chunk; j++) {
                buffer[j] = unshift_table[payload[pos + j]];
            }

            out.write(reinterpret_cast<char*>(buffer.data()), chunk);
            pos += chunk;
            remaining -= chunk;
        }
    }

    return TCF_OK;
}