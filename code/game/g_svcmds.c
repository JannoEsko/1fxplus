// Copyright (C) 2001-2002 Raven Software
//

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"

char    *ConcatArgs( int start );

/*
===================
Svcmd_EntityList_f
===================
*/
void    Svcmd_EntityList_f (void) {
    int         e;
    gentity_t       *check;

    check = g_entities+1;
    for (e = 1; e < level.num_entities ; e++, check++) {
        if ( !check->inuse ) {
            continue;
        }
        Com_Printf("%3i:", e);
        switch ( check->s.eType ) {
        case ET_GENERAL:
            Com_Printf("ET_GENERAL          ");
            break;
        case ET_PLAYER:
            Com_Printf("ET_PLAYER           ");
            break;
        case ET_ITEM:
            Com_Printf("ET_ITEM             ");
            break;
        case ET_MISSILE:
            Com_Printf("ET_MISSILE          ");
            break;
        case ET_MOVER:
            Com_Printf("ET_MOVER            ");
            break;
        case ET_BEAM:
            Com_Printf("ET_BEAM             ");
            break;
        case ET_PORTAL:
            Com_Printf("ET_PORTAL           ");
            break;
        case ET_SPEAKER:
            Com_Printf("ET_SPEAKER          ");
            break;
        case ET_PUSH_TRIGGER:
            Com_Printf("ET_PUSH_TRIGGER     ");
            break;
        case ET_TELEPORT_TRIGGER:
            Com_Printf("ET_TELEPORT_TRIGGER ");
            break;
        case ET_INVISIBLE:
            Com_Printf("ET_INVISIBLE        ");
            break;
        case ET_GRAPPLE:
            Com_Printf("ET_GRAPPLE          ");
            break;
        default:
            Com_Printf("%3i                 ", check->s.eType);
            break;
        }

        if ( check->classname ) {
            Com_Printf("%s", check->classname);
        }
        Com_Printf("\n");
    }
}


void Svcmd_ExtendTime_f (void)
{
    char str[MAX_TOKEN_CHARS];
    int  time;

    if ( trap_Argc() < 2 )
    {
        Com_Printf("Usage:  extendtime <minutes>\n");
        return;
    }

    trap_Argv( 1, str, sizeof( str ) );

    time = atoi(str);
    level.timeExtension += time;

    G_LogPrintf ( "timelimit extended by %d minutes\n", time );

    trap_SendServerCommand( -1, va("print \"timelimit extended by %d minutes\n\"", time) );
}

void Svcmd_AutoKickList_f ( void )
{
    int i;

    for ( i = 0; i < level.autokickedCount; i ++ )
    {
        Com_Printf ( "%16s - %s\n", level.autokickedIP[i], level.autokickedName[i] );
    }
}

gclient_t   *ClientForString( const char *s ) {
    gclient_t   *cl;
    int         i;
    int         idnum;

    // numeric values are just slot numbers
    if ( s[0] >= '0' && s[0] <= '9' ) {
        idnum = atoi( s );
        if ( idnum < 0 || idnum >= level.maxclients ) {
            Com_Printf( "Bad client slot: %i\n", idnum );
            return NULL;
        }

        cl = &level.clients[idnum];
        if ( cl->pers.connected == CON_DISCONNECTED ) {
            Com_Printf( "Client %i is not connected\n", idnum );
            return NULL;
        }
        return cl;
    }

    // check for a name match
    for ( i=0 ; i < level.maxclients ; i++ ) {
        cl = &level.clients[i];
        if ( cl->pers.connected == CON_DISCONNECTED ) {
            continue;
        }
        if ( !Q_stricmp( cl->pers.netname, s ) ) {
            return cl;
        }
    }

    Com_Printf( "User %s is not on the server\n", s );

    return NULL;
}

/*
=================
ConsoleCommand
=================
*/
qboolean ConsoleCommand( void )
{
    char cmd[MAX_TOKEN_CHARS];

    trap_Argv( 0, cmd, sizeof( cmd ) );


    if (!Q_stricmp(cmd, "showcls")) {
        Com_Printf("%-3s %-15s %-15s %-5s %-20s\n", "Num", "Name", "Legacy", "Ctry", "Ctryname");
        for (int i = 0; i < level.numConnectedClients; i++) {

            gentity_t* tent = &g_entities[level.sortedClients[i]];

            if (tent && tent->client && tent->client->pers.connected == CON_CONNECTED) {
                Com_Printf("%3i %-15s %-15s %-5s %-20s\n", tent->s.clientNum, tent->client->pers.cleanName, tent->client->sess.legacyProtocol ? "Legacy" : "Non-legacy", tent->client->sess.countryCode, tent->client->sess.country);
            }

            

        }


        return qtrue;
    }

    if (!Q_stricmp(cmd, "showsess")) {

        char clnumArg[10];
        trap_Argv(1, clnumArg, sizeof(clnumArg));

        if (clnumArg && strlen(clnumArg)) {
            int clnum = atoi(clnumArg);

            if (clnum < 0 || clnum >= MAX_CLIENTS) {
                Com_Printf("wrong ID specified [min: 0, max: 63]\n");
                return qtrue;
            }

            gentity_t* ent = &g_entities[clnum];

            if (!ent || !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED) {
                Com_Printf("this client is not connected...\n");
                return qtrue;
            }

            clientSession_t* sess = &ent->client->sess;
            Com_Printf("Client: %s [%d]\n", ent->client->pers.cleanName, ent->s.number);
            Com_Printf("Muted: %d\n", sess->muted);
            Com_Printf("Legacy protocol: %d\n", sess->legacyProtocol);
            Com_Printf("admLevel: %d, type: %d, admName: \"%s\"\n", sess->adminLevel, sess->adminType, sess->adminName);
            Com_Printf("clMod: %s [%d]\n", sess->clientVersion, sess->clientMod);
            Com_Printf("clanMember: %d, clanType: %d, clanName: \"%s\"\n", sess->clanMember, sess->clanType, sess->clanName);
            Com_Printf("ROX: verif: %d, has: %d, guid: %s\n", sess->verifyRoxAC, sess->hasRoxAC, sess->roxGuid);
            Com_Printf("Country: \"%s\", countryCode: %s\n", sess->country, sess->countryCode);
        }
        else {
            Com_Printf("usage: showsess <id>\n");
        }
        return qtrue;
    }

    if ( Q_stricmp (cmd, "entitylist") == 0 )
    {
        Svcmd_EntityList_f();
        return qtrue;
    }

    if (!Q_stricmp(cmd, "addprofanity")) {
        char profanity[MAX_QPATH];
        trap_Argv(1, profanity, sizeof(profanity));

        if (strlen(profanity) < 3) {
            G_printInfoMessage(NULL, "Profanity minimum length is 3 characters.");
        }
        else {
            int output = dbAddProfanity(profanity);

            if (output == 1) {
                G_printInfoMessage(NULL, "Profanity '%s' added to the list.", profanity);
                logAdmin(NULL, NULL, "addprofanity", profanity);
            }
            else {
                G_printInfoMessage(NULL, "Profanity not added (most likely already exists in the list).");
            }
        }

        return qtrue;
    }

    if (!Q_stricmp(cmd, "removeprofanity")) {

        char profanity[MAX_QPATH];
        trap_Argv(1, profanity, sizeof(profanity));

        if (strlen(profanity) < 3) {
            G_printInfoMessage(NULL, "Profanity minimum length is 3 characters.");
        }
        else {
            int output = dbRemoveProfanity(profanity);

            if (output == 1) {
                G_printInfoMessage(NULL, "Profanity '%s' removed from the list.", profanity);
                logAdmin(NULL, NULL, "removeprofanity", profanity);
            }
            else {
                G_printInfoMessage(NULL, "Profanity not removed (most likely it was not on the list).");
            }
        }

        return qtrue;
    }

#ifdef _SOF2_BOTS

    if (Q_stricmp (cmd, "addbot") == 0)
    {
        Svcmd_AddBot_f();
        return qtrue;
    }

    if (Q_stricmp (cmd, "botlist") == 0)
    {
        Svcmd_BotList_f();
        return qtrue;
    }

#endif

    if (Q_stricmp (cmd, "gametype_restart" ) == 0 )
    {
        trap_Argv( 1, cmd, sizeof( cmd ) );
        G_ResetGametype ( Q_stricmp ( cmd, "full" ) == 0, qfalse );
        return qtrue;
    }

    if (Q_stricmp (cmd, "extendtime" ) == 0 )
    {
        Svcmd_ExtendTime_f();
        return qtrue;
    }

    if ( Q_stricmp ( cmd, "autokicklist" ) == 0 )
    {
        Svcmd_AutoKickList_f();
        return qtrue;
    }

    int adminCommand = -1;
    if ((adminCommand = cmdIsAdminCmd(cmd, qfalse)) != -1) {
        runAdminCommand(adminCommand, 1, NULL, qfalse);
        return qtrue;
    }

    if (!Q_stricmp(cmd, "adm")) {
        adm_printAdminCommands(NULL);
        return qtrue;
    }

    if (!Q_stricmp(cmd, "cleardb")) {
        char arg1[MAX_TOKEN_CHARS];

        trap_Argv(1, arg1, sizeof(arg1));

        dbRunTruncate(arg1);

        return qtrue;
    }

    if (g_dedicated.integer)
    {
        if (Q_stricmp (cmd, "say") == 0)
        {
            trap_SendServerCommand( -1, va("chat -1 \"%s: %s\n\"", g_rconPrefix.string, ConcatArgs(1) ) );
            logGame(NULL, NULL, "sayrcon", ConcatArgs(1));
            return qtrue;
        }

        // everything else will also be printed as a say command
        trap_SendServerCommand( -1, va("chat -1 \"%s: %s\n\"", g_rconPrefix.string, ConcatArgs(0) ) );
        logGame(NULL, NULL, "sayrcon", ConcatArgs(0));
        return qtrue;
    }

    return qfalse;
}

