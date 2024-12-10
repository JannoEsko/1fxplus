
#include "../g_local.h"


void logSystem(loggingLevel_t logLevel, const char* msg, ...) {
    va_list     argptr;
    char        text[1024];

    va_start(argptr, msg);
    Q_vsnprintf(text, sizeof(text), msg, argptr);
    va_end(argptr);

    if (logLevel == LOGLEVEL_FATAL_DB) {
        trap_Error(text);
        return;
    } else if (logLevel == LOGLEVEL_ERROR || logLevel == LOGLEVEL_FATAL) {

        if (g_logToDatabase.integer) {
            dbLogSystem(logLevel, text);
        }

        backupInMemoryDatabases();
        trap_Error(text);
        return;
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

    if (g_logToDatabase.integer) {
        dbLogSystem(logLevel, text);
    }

}

void logRcon(char* ip, char* action) {

    if (g_logToDatabase.integer) {
        dbLogRcon(ip, action);
    }

}

void logAdmin(char* byIp, char* byName, char* toIp, char* toName, char* action, char* reason, admLevel_t adminLevel, char* adminName, admType_t adminType) {

    if (g_logToDatabase.integer) {

        dbLogAdmin(byIp, byName, toIp, toName, action, reason, adminLevel, adminName, adminType);

    }

}

void logLogin(gentity_t* ent) {

    if (g_logToDatabase.integer) {

        dbLogLogin(ent->client->pers.ip, ent->client->pers.cleanName, ent->client->sess.adminLevel, ent->client->sess.adminType);

    }

}