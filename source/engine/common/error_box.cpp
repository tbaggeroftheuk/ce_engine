#include <cstdlib>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic pop
#include "globals.hpp"

extern "C" {
#include "third_party/tiny_file_dialogs/tinyfiledialogs.h"
#include <raylib.h>

}
void ShowError(const std::string& msg) {
    if(!CE::isWinowOpen) {
         CloseWindow();
     }
    std::string safe_msg;
    safe_msg.reserve(msg.size());
    for (char c : msg) {
        if (c == '"' || c == '\'') safe_msg += '`'; // replace both single and double quotes
        else if (c == '\n' || c == '\r') safe_msg += ' '; // replace newlines
        else safe_msg += c;
    }

    tinyfd_messageBox(
        "Fatal error was caught!",
        safe_msg.c_str(),
        "ok",
        "error",
        1
    );
}
