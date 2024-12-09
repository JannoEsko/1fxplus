
#include "../g_local.h"


void logSystem(loggingLevel_t logLevel, const char* msg, ...) {
    va_list     argptr;
    char        text[1024];

    va_start(argptr, msg);
    Q_vsnprintf(text, sizeof(text), msg, argptr);
    va_end(argptr);

    if (logLevel == LOGLEVEL_ERROR || logLevel == LOGLEVEL_FATAL || logLevel == LOGLEVEL_FATAL_DB) {
        trap_Error(text);
    }
    else if (logLevel == LOGLEVEL_INFO) {
        Com_PrintInfo(text);
    }
    else if (logLevel == LOGLEVEL_WARN) {
        Com_PrintWarn(text);
    }
    else if (logLevel == LOGLEVEL_TEXT) {
        Com_PrintLog(text);
    }
    else {
        Com_Printf(text);
    }

    
}

void logRcon() {

}

void logAdmin() {

}