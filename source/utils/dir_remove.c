#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/dir_remove.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h> // SHFILEOPSTRUCT
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

int remove_directory(const char *path) {
#ifdef _WIN32
    // Windows implementation using SHFileOperation
    char buf[MAX_PATH + 2];
    snprintf(buf, sizeof(buf), "%s%c%c", path, 0, 0); // double-null terminated

    SHFILEOPSTRUCTA op = {0};
    op.wFunc = FO_DELETE;
    op.pFrom = buf;
    op.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION;

    return (SHFileOperationA(&op) == 0) ? 0 : -1;

#else
    // POSIX implementation
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;

        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            // Skip "." and ".."
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);
            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = remove_directory(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
#endif
}
