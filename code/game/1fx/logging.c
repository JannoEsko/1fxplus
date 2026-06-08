
#include "../g_local.h"
#include "../../ext/yyjson/yyjson.h"

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

        yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);

        if (doc) {
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);

            yyjson_mut_obj_add_str(doc, root, "socketAction", "logRcon");
            yyjson_mut_obj_add_str(doc, root, "socketIdentifier", g_sockIdentifier.string);
            yyjson_mut_obj_add_str(doc, root, "ip", ip);
            yyjson_mut_obj_add_str(doc, root, "rconAction", action);

            size_t json_len;
            char* json = yyjson_mut_write(doc, 0, &json_len);
            if (json) {
                enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, json, (int)json_len);
                free(json);
            }

            yyjson_mut_doc_free(doc);
        }
    }

}

void logAdmin(gentity_t* by, gentity_t* to, char* action, char* reason) {

    if (g_logToDatabase.integer) {

        dbLogAdmin(getIpOrArg(by, "RCON"), getNameOrArg(by, "RCON", qtrue), getIpOrArg(to, NULL), getNameOrArg(to, NULL, qtrue), action, reason, getAdminLevel(by), getAdminName(by), getAdminType(by));

    }

    if (g_logThroughSocket.integer) {

        yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);

        if (doc) {
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);

            yyjson_mut_obj_add_str(doc, root, "socketAction", "logAdmin");
            yyjson_mut_obj_add_str(doc, root, "socketIdentifier", g_sockIdentifier.string);
            yyjson_mut_obj_add_str(doc, root, "byname", getNameOrArg(by, "RCON", qtrue));
            yyjson_mut_obj_add_str(doc, root, "byip", getIpOrArg(by, "RCON"));
            yyjson_mut_obj_add_str(doc, root, "toname", getNameOrArg(to, "RCON", qtrue));
            yyjson_mut_obj_add_str(doc, root, "toip", getIpOrArg(to, "RCON"));
            yyjson_mut_obj_add_str(doc, root, "action", action);
            yyjson_mut_obj_add_str(doc, root, "reason", reason ? reason : "");
            yyjson_mut_obj_add_int(doc, root, "adminLevel", getAdminLevel(by));
            yyjson_mut_obj_add_str(doc, root, "adminName", getAdminName(by));
            yyjson_mut_obj_add_int(doc, root, "adminType", getAdminType(by));

            size_t json_len;
            char* json = yyjson_mut_write(doc, 0, &json_len);
            if (json) {
                enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, json, (int)json_len);
                free(json);
            }

            yyjson_mut_doc_free(doc);
        }

    }

}

void logLogin(gentity_t* ent) {

    if (g_logToDatabase.integer) {

        dbLogLogin(ent->client->pers.ip, ent->client->pers.cleanName, ent->client->sess.adminLevel, ent->client->sess.adminType);

    }

    if (g_logThroughSocket.integer) {

        yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);

        if (doc) {
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);

            yyjson_mut_obj_add_str(doc, root, "socketAction", "logLogin");
            yyjson_mut_obj_add_str(doc, root, "socketIdentifier", g_sockIdentifier.string);
            yyjson_mut_obj_add_str(doc, root, "loginName", ent->client->pers.cleanName);
            yyjson_mut_obj_add_str(doc, root, "ip", ent->client->pers.ip);
            yyjson_mut_obj_add_int(doc, root, "adminLevel", ent->client->sess.adminLevel);
            yyjson_mut_obj_add_int(doc, root, "adminType", ent->client->sess.adminType);

            size_t json_len;
            char* json = yyjson_mut_write(doc, 0, &json_len);
            if (json) {
                enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, json, (int)json_len);
                free(json);
            }

            yyjson_mut_doc_free(doc);
        }
    }

}

void logGame(gentity_t* by, gentity_t* to, char* action, char* text) {

    if (g_logToDatabase.integer) {
        dbLogGame(getIpOrArg(by, "Server"), getNameOrArg(by, "Server", qtrue), getIpOrArg(to, ""), getNameOrArg(to, "", qtrue), action);
    }

    if (g_logThroughSocket.integer) {

        yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);

        if (doc) {
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);

            yyjson_mut_obj_add_str(doc, root, "socketAction", "logGame");
            yyjson_mut_obj_add_str(doc, root, "socketIdentifier", g_sockIdentifier.string);
            yyjson_mut_obj_add_str(doc, root, "byname", getNameOrArg(by, "Server", qtrue));
            yyjson_mut_obj_add_str(doc, root, "byip", getIpOrArg(by, "Server"));
            yyjson_mut_obj_add_str(doc, root, "toname", getNameOrArg(to, "", qtrue));
            yyjson_mut_obj_add_str(doc, root, "toip", getIpOrArg(to, ""));
            yyjson_mut_obj_add_str(doc, root, "action", action ? action : "");
            yyjson_mut_obj_add_str(doc, root, "text", text);

            size_t json_len;
            char* json = yyjson_mut_write(doc, 0, &json_len);

            if (json) {
                enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, json, (int)json_len);
                free(json);
            }

            yyjson_mut_doc_free(doc);
        }
    }

}
