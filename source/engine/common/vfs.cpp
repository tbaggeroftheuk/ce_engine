#include "engine/common/vfs.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

uint64_t LoadedFile::size() const { return static_cast<uint64_t>(data.size()); }
const uint8_t* LoadedFile::data_ptr() const { return data.data(); }

MountPoint::MountPoint(const std::string& mount,
                       const std::string& source,
                       bool archive,
                       LoadMode mode)
    : mount_path(mount),
      source_path(source),
      is_archive(archive),
      load_mode(mode),
      tcf_handle(nullptr)
{
}

MountPoint::~MountPoint()
{
    if (is_archive && tcf_handle)
        tcf_vfs_close(tcf_handle);
}

VirtualFile::VirtualFile()
    : mount(nullptr),
      position(0),
      size(0),
      tcf_handle(nullptr),
      dir_handle(nullptr),
      loaded_data(nullptr)
{
}

VirtualFile::~VirtualFile()
{
    if (tcf_handle)
        tcf_vfs_close_file(tcf_handle);
    if (dir_handle)
        std::fclose(dir_handle);
}

namespace CE::VFS::Returns {
    const int LOAD_SUCCESS = 0;
    const int LOAD_FAIL = 1;
    const int NO_SUCH_FILE_OR_DIRECTORY = 2;
}

namespace CE::VFS {

int VFS::MountArchive(const char* archive_path, const char* v_mount_path, const LoadMode loadmode)
{
    if (!archive_path || !v_mount_path)
        return Returns::LOAD_FAIL;

    std::string mount_path = NormalizePath(v_mount_path);

    for (auto& mount : mounts) {
        if (mount->mount_path == mount_path)
            return Returns::LOAD_FAIL;
    }

    auto mount = std::make_unique<MountPoint>(mount_path, archive_path, true, loadmode);

    int result = tcf_vfs_open(archive_path, &mount->tcf_handle);
    if (result != TCF_OK)
        return Returns::LOAD_FAIL;

    if (loadmode == All) {
        if (!LoadEntireArchive(mount.get()))
            return Returns::LOAD_FAIL;
    }

    mounts.push_back(std::move(mount));
    return Returns::LOAD_SUCCESS;
}

int VFS::MountFolder(const char* folder_path, const char* v_mount_path, const LoadMode loadmode)
{
    if (!folder_path || !v_mount_path)
        return Returns::LOAD_FAIL;

    if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path))
        return Returns::LOAD_FAIL;

    std::string mount_path = NormalizePath(v_mount_path);

    for (auto& mount : mounts) {
        if (mount->mount_path == mount_path)
            return Returns::LOAD_FAIL;
    }

    auto mount = std::make_unique<MountPoint>(mount_path, folder_path, false, loadmode);

    if (loadmode == All) {
        if (!LoadEntireFolder(mount.get()))
            return Returns::LOAD_FAIL;
    }

    mounts.push_back(std::move(mount));
    return Returns::LOAD_SUCCESS;
}

bool VFS::Unmount(const char* v_mount_path)
{
    if (!v_mount_path)
        return false;

    std::string mount_path = NormalizePath(v_mount_path);

    auto it = std::find_if(mounts.begin(), mounts.end(),
                           [&mount_path](const std::unique_ptr<MountPoint>& m) { return m->mount_path == mount_path; });
    if (it == mounts.end())
        return false;

    mounts.erase(it);
    return true;
}

bool VFS::FileExists(const char* virtual_path)
{
    if (!virtual_path)
        return false;

    std::string rel_path;
    MountPoint* mount = FindMount(virtual_path, rel_path);
    if (!mount)
        return false;

    if (mount->load_mode == All)
        return mount->loaded_files.find(rel_path) != mount->loaded_files.end();

    if (mount->is_archive) {
        tcf_file_t* file = nullptr;
        int result = tcf_vfs_open_file(mount->tcf_handle, rel_path.c_str(), &file);
        if (result == TCF_OK) {
            tcf_vfs_close_file(file);
            return true;
        }
        return false;
    }

    std::filesystem::path full_path = std::filesystem::path(mount->source_path) / rel_path;
    return std::filesystem::exists(full_path) && std::filesystem::is_regular_file(full_path);
}

bool VFS::GetFileSize(const char* virtual_path, uint64_t& out_size)
{
    out_size = 0;
    if (!virtual_path)
        return false;

    std::string rel_path;
    MountPoint* mount = FindMount(virtual_path, rel_path);
    if (!mount)
        return false;

    if (mount->load_mode == All) {
        auto it = mount->loaded_files.find(rel_path);
        if (it == mount->loaded_files.end())
            return false;
        out_size = it->second->size();
        return true;
    }

    if (mount->is_archive) {
        tcf_file_t* file = nullptr;
        int result = tcf_vfs_open_file(mount->tcf_handle, rel_path.c_str(), &file);
        if (result != TCF_OK)
            return false;
        out_size = tcf_vfs_file_size(file);
        tcf_vfs_close_file(file);
        return true;
    }

    std::filesystem::path full_path = std::filesystem::path(mount->source_path) / rel_path;
    if (!std::filesystem::exists(full_path) || !std::filesystem::is_regular_file(full_path))
        return false;

    out_size = static_cast<uint64_t>(std::filesystem::file_size(full_path));
    return true;
}

VirtualFile* VFS::OpenFile(const char* virtual_path)
{
    if (!virtual_path)
        return nullptr;

    std::string rel_path;
    MountPoint* mount = FindMount(virtual_path, rel_path);
    if (!mount)
        return nullptr;

    auto* vfile = new VirtualFile();
    vfile->mount = mount;
    vfile->path = rel_path;

    if (mount->load_mode == All) {
        auto it = mount->loaded_files.find(rel_path);
        if (it == mount->loaded_files.end()) {
            delete vfile;
            return nullptr;
        }

        vfile->loaded_data = it->second.get();
        vfile->size = vfile->loaded_data->size();
        vfile->position = 0;
        return vfile;
    }

    if (mount->is_archive) {
        tcf_file_t* file = nullptr;
        int result = tcf_vfs_open_file(mount->tcf_handle, rel_path.c_str(), &file);
        if (result != TCF_OK) {
            delete vfile;
            return nullptr;
        }

        vfile->tcf_handle = file;
        vfile->size = tcf_vfs_file_size(file);
        vfile->position = 0;
        return vfile;
    }

    std::filesystem::path full_path = std::filesystem::path(mount->source_path) / rel_path;
    FILE* file = std::fopen(full_path.string().c_str(), "rb");
    if (!file) {
        delete vfile;
        return nullptr;
    }

    vfile->size = static_cast<uint64_t>(std::filesystem::file_size(full_path));
    vfile->position = 0;
    std::fseek(file, 0, SEEK_SET);
    vfile->dir_handle = file;
    return vfile;
}

size_t VFS::ReadFile(VirtualFile* file, void* buffer, size_t size)
{
    if (!file || !buffer)
        return 0;

    if (file->position >= file->size)
        return 0;

    uint64_t remaining64 = file->size - file->position;
    uint64_t to_read64 = std::min<uint64_t>(remaining64, static_cast<uint64_t>(size));
    size_t to_read = static_cast<size_t>(to_read64);
    if (to_read == 0)
        return 0;

    if (file->loaded_data) {
        if (file->position > file->loaded_data->size())
            return 0;
        std::memcpy(buffer, file->loaded_data->data_ptr() + static_cast<size_t>(file->position), to_read);
        file->position += to_read64;
        return to_read;
    }

    if (file->tcf_handle) {
        size_t bytes_read = 0;
        int result = tcf_vfs_read(file->tcf_handle, buffer, to_read, &bytes_read);
        if (result == TCF_OK) {
            file->position += static_cast<uint64_t>(bytes_read);
            return bytes_read;
        }
        return 0;
    }

    if (file->dir_handle) {
        size_t bytes_read = std::fread(buffer, 1, to_read, file->dir_handle);
        file->position += static_cast<uint64_t>(bytes_read);
        return bytes_read;
    }

    return 0;
}

bool VFS::SeekFile(VirtualFile* file, int64_t offset, int whence)
{
    if (!file)
        return false;

    if (file->size > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
        return false;

    int64_t new_pos;
    switch (whence) {
        case SEEK_SET: new_pos = offset; break;
        case SEEK_CUR: new_pos = static_cast<int64_t>(file->position) + offset; break;
        case SEEK_END: new_pos = static_cast<int64_t>(file->size) + offset; break;
        default: return false;
    }

    if (new_pos < 0 || new_pos > static_cast<int64_t>(file->size))
        return false;

    if (file->loaded_data) {
        file->position = static_cast<uint64_t>(new_pos);
        return true;
    }

    if (file->tcf_handle) {
        if (tcf_vfs_seek(file->tcf_handle, offset, whence) == TCF_OK) {
            file->position = static_cast<uint64_t>(new_pos);
            return true;
        }
        return false;
    }

    if (file->dir_handle) {
        if (std::fseek(file->dir_handle, offset, whence) == 0) {
            file->position = static_cast<uint64_t>(new_pos);
            return true;
        }
        return false;
    }

    return false;
}

int64_t VFS::TellFile(VirtualFile* file)
{
    if (!file)
        return -1;
    if (file->position > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
        return -1;
    return static_cast<int64_t>(file->position);
}

void VFS::CloseFile(VirtualFile* file) { delete file; }

void VFS::ListMounts()
{
    for (const auto& mount : mounts) {
        std::cout << mount->mount_path << " -> " << mount->source_path
                  << (mount->is_archive ? " [archive]" : " [folder]") << "\n";
    }
}

std::string VFS::NormalizePath(const std::string& path)
{
    std::string result;
    bool last_was_slash = false;

    for (char c : path) {
        if (c == '/' || c == '\\') {
            if (!last_was_slash) {
                result += '/';
                last_was_slash = true;
            }
        } else {
            result += c;
            last_was_slash = false;
        }
    }

    if (!result.empty() && result.back() == '/')
        result.pop_back();

    if (result.empty() || result[0] != '/')
        result = "/" + result;

    return result;
}

std::string VFS::GetRelativePath(const std::string& virtual_path, const std::string& mount_path)
{
    if (mount_path.length() >= virtual_path.length())
        return "";

    std::string rel = virtual_path.substr(mount_path.length());
    if (!rel.empty() && rel[0] == '/')
        rel = rel.substr(1);

    return rel;
}

MountPoint* VFS::FindMount(const std::string& virtual_path, std::string& relative_path)
{
    std::string norm_path = NormalizePath(virtual_path);

    MountPoint* best_match = nullptr;
    size_t best_match_len = 0;

    for (auto& mount : mounts) {
        if (norm_path.rfind(mount->mount_path, 0) == 0) {
            size_t mount_len = mount->mount_path.length();
            if (mount_len > best_match_len) {
                if (norm_path.length() == mount_len || norm_path[mount_len] == '/') {
                    best_match = mount.get();
                    best_match_len = mount_len;
                }
            }
        }
    }

    if (best_match)
        relative_path = GetRelativePath(norm_path, best_match->mount_path);

    return best_match;
}

bool VFS::LoadEntireFolder(MountPoint* mount)
{
    std::filesystem::path base_path(mount->source_path);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(base_path)) {
        if (!entry.is_regular_file())
            continue;

        std::filesystem::path relative = std::filesystem::relative(entry.path(), base_path);
        std::string rel_path = relative.generic_string();

        std::ifstream file(entry.path(), std::ios::binary);
        if (!file)
            continue;

        auto loaded = std::make_unique<LoadedFile>();

        file.seekg(0, std::ios::end);
        std::streampos end = file.tellg();
        if (end < 0)
            continue;

        size_t size = static_cast<size_t>(end);
        file.seekg(0, std::ios::beg);

        loaded->data.resize(size);
        file.read(reinterpret_cast<char*>(loaded->data.data()), static_cast<std::streamsize>(size));

        mount->loaded_files[rel_path] = std::move(loaded);
    }

    return true;
}

bool VFS::LoadEntireArchive(MountPoint* mount)
{
    if (!mount->tcf_handle)
        return false;

    const uint64_t entry_count = tcf_vfs_entry_count(mount->tcf_handle);
    if (entry_count > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
        return false;

    mount->loaded_files.clear();

    for (uint32_t i = 0; i < static_cast<uint32_t>(entry_count); ++i) {
        const char* path = nullptr;
        uint32_t path_len = 0;
        uint64_t entry_size = 0;
        if (tcf_vfs_entry_info(mount->tcf_handle, i, &path, &path_len, &entry_size) != TCF_OK || !path)
            return false;

        if (entry_size > static_cast<uint64_t>(std::numeric_limits<size_t>::max()))
            return false;

        auto loaded = std::make_unique<LoadedFile>();
        loaded->data.resize(static_cast<size_t>(entry_size));

        if (entry_size > 0) {
            if (tcf_vfs_read_entry(mount->tcf_handle, i, loaded->data.data(), loaded->data.size()) != TCF_OK)
                return false;
        }

        std::string rel_path(path, path_len);
        mount->loaded_files.try_emplace(std::move(rel_path), std::move(loaded));
    }

    return true;
}

bool VFS::LoadFromTCF(MountPoint* mount, const std::string& rel_path, std::vector<uint8_t>& out_data)
{
    if (!mount->tcf_handle)
        return false;

    tcf_file_t* file = nullptr;
    int result = tcf_vfs_open_file(mount->tcf_handle, rel_path.c_str(), &file);
    if (result != TCF_OK)
        return false;

    uint64_t size = tcf_vfs_file_size(file);
    if (size > static_cast<uint64_t>(std::numeric_limits<size_t>::max())) {
        tcf_vfs_close_file(file);
        return false;
    }

    out_data.resize(static_cast<size_t>(size));

    size_t bytes_read = 0;
    result = tcf_vfs_read(file, out_data.data(), out_data.size(), &bytes_read);
    tcf_vfs_close_file(file);

    return (result == TCF_OK && bytes_read == out_data.size());
}

bool VFS::load_file_from_directory(MountPoint* mount, const std::string& rel_path, std::vector<uint8_t>& out_data)
{
    std::filesystem::path full_path = std::filesystem::path(mount->source_path) / rel_path;

    if (!std::filesystem::exists(full_path) || !std::filesystem::is_regular_file(full_path))
        return false;

    std::ifstream file(full_path, std::ios::binary);
    if (!file)
        return false;

    file.seekg(0, std::ios::end);
    std::streampos end = file.tellg();
    if (end < 0)
        return false;
    size_t size = static_cast<size_t>(end);
    file.seekg(0, std::ios::beg);

    out_data.resize(size);
    file.read(reinterpret_cast<char*>(out_data.data()), static_cast<std::streamsize>(size));
    return true;
}

} // namespace CE::VFS

