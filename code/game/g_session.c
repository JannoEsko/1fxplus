// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"
#include "1fx/1fxFunctions.h"

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
void G_WriteClientSessionData( gentity_t *ent )
{
    dbWriteSession(ent);
}

/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gentity_t *ent )
{
    dbReadSession(ent);
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gentity_t* ent, char *userinfo )
{
    gclient_t* client = ent->client;
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
    resetSession(&g_entities[client->ps.clientNum]);
    G_WriteClientSessionData( ent );
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
        Com_Printf( "Gametype changed, clearing session data.\n" );
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

    for ( i = 0 ; i < level.numConnectedClients ; i++ )
    {
        if ( g_entities[level.sortedClients[i]].client->pers.connected == CON_CONNECTED )
        {
            G_WriteClientSessionData( &g_entities[level.sortedClients[i]] );
        }
    }
}
