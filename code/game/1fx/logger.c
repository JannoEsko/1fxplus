#include "../g_local.h"
#include "1fxFunctions.h"

// point of this file is to coordinate all logging activities in one place.
// this file will call out whatever logging function it needs to perform the action

void logGame() {

}

void logSystem(int faultLevel, char* message) {
    Com_Printf("logSystem faultLevel %d message %s\n", faultLevel, message);
}

void logAdmin(gentity_t* by, gentity_t* to, char* action, char* reason) {
    char byip[MAX_IP];
    char toip[MAX_IP];
    char byname[MAX_NETNAME];
    char toname[MAX_NETNAME];
    int adminlevel;
    char adminname[MAX_NETNAME];
    int admintype;

    if (by && by->client) {
        Q_strncpyz(byip, by->client->pers.ip, sizeof(byip));
        Q_strncpyz(byname, by->client->pers.netname, sizeof(byname));
        adminlevel = by->client->sess.adminLevel;
        Q_strncpyz(adminname, by->client->sess.adminName, sizeof(adminname));
        admintype = by->client->sess.adminType;
    }
    else {
        Q_strncpyz(byip, "RCON", sizeof(byip));
        Q_strncpyz(byname, "RCON", sizeof(byname));
        adminlevel = LEVEL_RCON;
        admintype = ADMINTYPE_RCON;
        Q_strncpyz(adminname, "RCON", sizeof(adminname));
    }

    if (to && to->client) {
        Q_strncpyz(toip, to->client->pers.ip, sizeof(toip));
        Q_strncpyz(toname, to->client->pers.netname, sizeof(toname));
    }
    else {
        // if there's no 'to' then assume it's to all clients.
        // in some cases it might be weird, e.g. !tl 10 means 1fx. # Janno did timelimit 10 to all clients.
        // but from another angle, it does affect all clients, so.... :)
        Q_strncpyz(toip, "All clients", sizeof(toip));
        Q_strncpyz(toname, "All clients", sizeof(toname));
    }

    if (g_logToDatabase.integer) {
        dbLogAdmin(byip, byname, toip, toname, action, reason, adminlevel, adminname, admintype);
    }
}

void logRcon(char* ip, char* action) {
    if (g_logToDatabase.integer) {
        dbLogRcon(ip, action);
    }
}

void logDamage() {

}

void logObjective() {

}

void logLogin(char* player, char* ip, int level, int method, char* reference) {

    if (g_logToDatabase.integer) {
        dbLogLogin(player, ip, level, method, reference);
    }

}
