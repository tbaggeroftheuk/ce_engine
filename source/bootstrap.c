#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "bootstrap.h"
#include "tcf.h"
#include "utils.h"


#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
#endif

void extract_game_data(void) {
    int tmp_dir_random = bag_rand(1000, 10000);
    char path[65];
    snprintf(path, sizeof(path), "/tmp/ZmFsbGluZ19wb3RhdG9fZ2FtZXM/%d", tmp_dir_random);
    mkdir(path, 0755);

}