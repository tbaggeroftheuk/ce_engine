#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <cstdio>

#include "common/tcf/tcf.h"

enum LoadMode {
    OnDemand,
    All,
};

struct LoadedFile {
    std::vector<uint8_t> data;
    uint64_t size() const;
    const uint8_t* data_ptr() const;
};

struct MountPoint {
    std::string mount_path;
    std::string source_path;
    bool is_archive;
    LoadMode load_mode;
    tcf_vfs_t* tcf_handle;

    std::unordered_map<std::string, std::unique_ptr<LoadedFile>> loaded_files;

    MountPoint(const std::string& mount, const std::string& source, 
               bool archive, LoadMode mode);
    
    ~MountPoint();
};

struct VirtualFile {
    MountPoint* mount;
    std::string path;
    uint64_t position;
    uint64_t size;

    tcf_file_t* tcf_handle;    
    FILE* dir_handle;         

    const LoadedFile* loaded_data;
    
    VirtualFile();
    ~VirtualFile();
};

namespace CE::VFS::Returns {
    extern const int LOAD_SUCCESS;
    extern const int LOAD_FAIL;
    extern const int NO_SUCH_FILE_OR_DIRECTORY;
}

namespace CE::VFS {
    class VFS {        
    public:
        int MountArchive(const char* archive_path, const char* v_mount_path, const LoadMode loadmode);

        int MountFolder(const char* folder_path, const char* v_mount_path, const LoadMode loadmode);

        bool Unmount(const char* v_mount_path);

        bool FileExists(const char* virtual_path);

        bool GetFileSize(const char* virtual_path, uint64_t& out_size);

        VirtualFile* OpenFile(const char* virtual_path);

        size_t ReadFile(VirtualFile* file, void* buffer, size_t size);

        bool SeekFile(VirtualFile* file, int64_t offset, int whence);

        int64_t TellFile(VirtualFile* file);

        void CloseFile(VirtualFile* file);

        void ListMounts();

    private:
        std::vector<std::unique_ptr<MountPoint>> mounts;

        std::string NormalizePath(const std::string& path);

        std::string GetRelativePath(const std::string& virtual_path, 
                                    const std::string& mount_path);

        MountPoint* FindMount(const std::string& virtual_path, std::string& relative_path);

        bool LoadEntireFolder(MountPoint* mount);

        bool LoadEntireArchive(MountPoint* mount);

        bool LoadFromTCF(MountPoint* mount, const std::string& rel_path, 
                         std::vector<uint8_t>& out_data);

        bool load_file_from_directory(MountPoint* mount, const std::string& rel_path,
                                     std::vector<uint8_t>& out_data);
    };
}
