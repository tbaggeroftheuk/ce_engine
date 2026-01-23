#include <cstdlib>
#include <string>

extern "C" {
#include "third_party/tiny_file_dialogs/tinyfiledialogs.h"
}

void ShowError(const std::string& msg) {
    tinyfd_messageBox(
        "Fatal Error thrown by the engine",
        msg.c_str(),
        "ok",
        "error",
        1
    );
}
