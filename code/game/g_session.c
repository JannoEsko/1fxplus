// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"

/*
=======================================================================
  SESSION DATA
Session data is the only data that stays persistant across level loads
and map restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client )
{
    dbRemoveSessionDataById(client - level.clients);
    dbWriteSessionDataForClient(client);
}

/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gclient_t *client )
{
    dbReadSessionDataForClient(client, level.newSession);
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, char *userinfo, qboolean firstTime )
{
    clientSession_t *sess;
    const char      *value;

    sess = &client->sess;

    // initial team determination
    if ( level.gametypeData->teams )
    {
        if ( g_teamAutoJoin.integer )
        {
            sess->team = PickTeam( -1 );
        }
        else
        {
            // always spawn as spectator in team games
            sess->team = TEAM_SPECTATOR;
        }
    }
    else
    {
        value = Info_ValueForKey( userinfo, "team" );
        if ( value[0] == 's' )
        {
            // a willing spectator, not a waiting-in-line
            sess->team = TEAM_SPECTATOR;
        }
        else
        {
            if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer )
            {
                sess->team = TEAM_SPECTATOR;
            }
            else
            {
                sess->team = TEAM_FREE;
            }
        }
    }

    sess->spectatorState = SPECTATOR_FREE;
    sess->spectatorTime = level.time;
    
    sess->adminLevel = ADMLVL_NONE;
    sess->adminType = ADMTYPE_NONE;
    sess->clanMember = qfalse;
    sess->hasRoxAC = qfalse;
    Com_Memset(sess->roxGuid, 0, sizeof(sess->roxGuid));
    Com_Memset(sess->adminName, 0, sizeof(sess->adminName));
    Com_Memset(sess->country, 0, sizeof(sess->country));
    Com_Memset(sess->countryCode, 0, sizeof(sess->countryCode));
    sess->blockseek = qfalse;
    sess->clanType = CLANTYPE_NONE;
    Com_Memset(sess->clanName, 0, sizeof(sess->clanName));

    if (firstTime) {
        G_WriteClientSessionData(client);
    }

}


/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void )
{
    char    s[MAX_STRING_CHARS];
    int     gt;

    trap_Cvar_VariableStringBuffer( "session", s, sizeof(s) );

    gt = BG_FindGametype ( s );

    // if the gametype changed since the last session, don't use any
    // client sessions
    if ( level.gametype != gt )
    {
        level.newSession = qtrue;
        logSystem(LOGLEVEL_INFO, "Gametype has changed, will discard team data.");
    }
}

/*
==================
G_WriteSessionData
==================
*/
void G_WriteSessionData( void )
{
    int     i;

    trap_Cvar_Set( "session", level.gametypeData->name );

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( level.clients[i].pers.connected == CON_CONNECTED )
        {
            G_WriteClientSessionData( &level.clients[i] );
        }
    }
}

// Mute information
void writeMutesIntoSession() {

    // First we ensure that we're writing into a clean table.
    dbClearSessionMutes();
    int pushedMutes = 0;
    for (int i = 0; i < MAX_CLIENTS && pushedMutes < level.numMutedClients; i++) {

        mute_t* muteInfo = &level.mutedClients[i];

        if (muteInfo->used) {
            pushedMutes++;
            dbWriteMuteIntoSession(muteInfo);
        }

    }

}

void readMutesFromSession() {

    // The reading will actually be done in db functions, pointless to stream it back here.

    dbReadSessionMutesBackIntoMuteInfo();

}