#include <raylib.h>
#include "globals.h"

void CETraceLog(TraceLogLevel type, const char *text) {
    TraceLog(type, "%s", text);
    return;
}
