#include <cstdlib>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic pop
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
