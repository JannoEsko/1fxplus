
#include "../g_local.h"
#define MAX_SOCK_LOG 512

void logSystem(loggingLevel_t logLevel, const char* msg, ...) {
    va_list     argptr;
    char        text[1024];

    va_start(argptr, msg);
    Q_vsnprintf(text, sizeof(text), msg, argptr);
    va_end(argptr);

    if (logLevel == LOGLEVEL_FATAL_DB) {
        trap_Error(va("%s\n", text));
        return;
    } else if (logLevel == LOGLEVEL_ERROR || logLevel == LOGLEVEL_FATAL) {

        if (g_logToDatabase.integer) {
            dbLogSystem(logLevel, text);
        }

        backupInMemoryDatabases();
        trap_Error(va("%s\n", text));
        return;
    }
    else if (logLevel == LOGLEVEL_INFO) {
        Com_PrintInfo(va("%s\n", text));
    }
    else if (logLevel == LOGLEVEL_WARN) {
        Com_PrintWarn(va("%s\n", text));
    }
    else if (logLevel == LOGLEVEL_TEXT) {
        Com_PrintLog(va("%s\n", text));
    }
    else {
        Com_Printf(va("%s\n", text));
    }

    if (g_logToDatabase.integer) {
        dbLogSystem(logLevel, text);
    }

}

void logRcon(char* ip, char* action) {

    if (g_logToDatabase.integer) {
        dbLogRcon(ip, action);
    }

    if (g_logThroughSocket.integer) {
        char maxLog[MAX_SOCK_LOG];
        Q_strncpyz(maxLog, va("logRcon\\%s\\%s\\\\%s", g_sockIdentifier.string, ip, action), sizeof(maxLog));
        enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, -1, maxLog, strlen(maxLog));
    }

}

void logAdmin(gentity_t* by, gentity_t* to, char* action, char* reason) {

    if (g_logToDatabase.integer) {

        dbLogAdmin(getIpOrArg(by, "RCON"), getNameOrArg(by, "RCON", qtrue), getIpOrArg(to, NULL), getNameOrArg(to, NULL, qtrue), action, reason, getAdminLevel(by), getAdminName(by), getAdminType(by));

    }

    if (g_logThroughSocket.integer) {
        char maxLog[MAX_SOCK_LOG];
        Q_strncpyz(maxLog, va("logAdmin\\%s\\%s\\%s\\%s\\%s\\%s\\%s\\%d\\%s\\%d", 
            g_sockIdentifier.string, 
            getNameOrArg(by, "RCON", qtrue), 
            getIpOrArg(by, "RCON"),
            getNameOrArg(to, "RCON", qtrue),
            getIpOrArg(to, "RCON"),
            action,
            reason ? reason : "",
            getAdminLevel(by), 
            getAdminName(by), 
            getAdminType(by)
            ), sizeof(maxLog));
        enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, -1, maxLog, strlen(maxLog));
    }

}

void logLogin(gentity_t* ent) {

    if (g_logToDatabase.integer) {

        dbLogLogin(ent->client->pers.ip, ent->client->pers.cleanName, ent->client->sess.adminLevel, ent->client->sess.adminType);

    }

    if (g_logThroughSocket.integer) {
        char maxLog[MAX_SOCK_LOG];

        Q_strncpyz(maxLog, va("logLogin\\%s\\%s\\%s\\%d",
            g_sockIdentifier.string,
            ent->client->pers.cleanName,
            ent->client->pers.ip,
            ent->client->sess.adminLevel
            )
            , sizeof(maxLog));
        enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, -1, maxLog, strlen(maxLog));
    }

}

void logGame(gentity_t* by, gentity_t* to, char* action, char* text) {
    
    if (g_logToDatabase.integer) {
        dbLogGame(getIpOrArg(by, "Server"), getNameOrArg(by, "Server", qtrue), getIpOrArg(to, ""), getNameOrArg(to, "", qtrue), action);
    }

    if (g_logThroughSocket.integer) {
        char maxLog[MAX_SOCK_LOG];

        Q_strncpyz(maxLog, va("logGame\\%s\\%s\\%s\\%s\\%s\\%s\\%s",
            g_sockIdentifier.string,
            getNameOrArg(by, "Server", qtrue),
            getIpOrArg(by, "Server"),
            getNameOrArg(to, "", qtrue),
            getIpOrArg(to, ""),
            text,
            action ? action : ""
        )
            , sizeof(maxLog));
        enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, -1, maxLog, strlen(maxLog));
    }

}