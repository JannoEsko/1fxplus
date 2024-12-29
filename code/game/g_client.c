// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"

// g_client.c -- client functions that don't happen every frame

static vec3_t   playerMins = {-15, -15, -46};
static vec3_t   playerMaxs = {15, 15, 48};


/*
================
G_AddClientSpawn

adds a spawnpoint to the spawnpoint array using the given entity for
origin and angles as well as the team for filtering teams.
================
*/
void G_AddClientSpawn ( gentity_t* ent, team_t team )
{
    static vec3_t   mins = {-15,-15,-45};
    static vec3_t   maxs = {15,15,46};
    vec3_t          end;
    trace_t         tr;

    // Drop it to the ground, and if it starts solid just throw it out
    VectorCopy ( ent->s.origin, end );
    end[2] -= 1024;

    tr.fraction = 0.0f;
    trap_Trace ( &tr, ent->s.origin, mins, maxs, end, ent->s.number, MASK_SOLID );

    // We are only looking for terrain collisions at this point
    if ( tr.contents & CONTENTS_TERRAIN )
    {
        // If its in the ground then throw it awway
        if ( tr.startsolid )
        {
            return;
        }
        // Drop it down to the ground now
        else if ( tr.fraction < 1.0f && tr.fraction > 0.0f )
        {
            VectorCopy ( tr.endpos, ent->s.origin );
            ent->s.origin[2] += 1.0f;
            tr.startsolid = qfalse;
        }
    }

    if ( tr.startsolid )
    {
        Com_Printf ( S_COLOR_YELLOW "WARNING: gametype_player starting in solid at %.2f,%.2f,%.2f\n", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
    }

    level.spawns[level.spawnCount].team = team;

    // Release the entity and store the spawn in its own array
    VectorCopy ( ent->s.origin, level.spawns[level.spawnCount].origin );
    VectorCopy ( ent->s.angles, level.spawns[level.spawnCount].angles );

    // Increase the spawn count
    level.spawnCount++;
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -46) (16 16 48) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
*/
void SP_info_player_deathmatch( gentity_t *ent )
{
    // Cant take any more spawns!!
    if ( level.spawnCount >= MAX_SPAWNS )
    {
        G_FreeEntity ( ent );
        return;
    }

    G_AddClientSpawn ( ent, TEAM_FREE );

    G_FreeEntity ( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -46) (16 16 48)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent )
{
}

/*
================
G_SpotWouldTelefrag
================
*/
qboolean G_SpotWouldTelefrag( gspawn_t* spawn )
{
    int         i, num;
    int         touch[MAX_GENTITIES];
    gentity_t   *hit;
    vec3_t      mins, maxs;

    VectorAdd( spawn->origin, playerMins, mins );
    VectorAdd( spawn->origin, playerMaxs, maxs );
    num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

    for (i=0 ; i<num ; i++)
    {
        hit = &g_entities[touch[i]];

        if ( hit->client)
        {
            if ( G_IsClientSpectating ( hit->client ) )
            {
                continue;
            }

            if ( G_IsClientDead ( hit->client ) )
            {
                continue;
            }

            return qtrue;
        }
    }

    return qfalse;
}

/*
================
G_SelectRandomSpawnPoint

go to a random point that doesn't telefrag
================
*/
gspawn_t* G_SelectRandomSpawnPoint ( team_t team )
{
    int         i;
    int         count;
    int         tfcount;
    gspawn_t    *spawns[MAX_SPAWNS];
    gspawn_t    *tfspawns[MAX_SPAWNS];

    count = 0;
    tfcount = 0;

    for ( i = 0; i < level.spawnCount; i ++ )
    {
        gspawn_t* spawn = &level.spawns[i];

        if ( team != -1 && team != spawn->team )
        {
            continue;
        }

        if ( G_SpotWouldTelefrag( spawn ) )
        {
            tfspawns[tfcount++] = spawn;
            continue;
        }

        spawns[ count++ ] = spawn;
    }

    // no spots that won't telefrag so just pick one that will
    if ( !count )
    {
        // No telefrag spots, just return NULL since there is no more to find
        if ( !tfcount )
        {
            return NULL;
        }

        // telefrag someone
        return tfspawns[ rand() % tfcount ];
    }

    i = rand() % count;

    return spawns[ i ];
}

/*
===========
G_SelectRandomSafeSpawnPoint

Select a random spawn point that is safe for the client to spawn at.  A safe spawn point
is one that is at least a certain distance from another client.
============
*/
gspawn_t* G_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance )
{
    gspawn_t*   spawns[MAX_SPAWNS];
    float       safeDistanceSquared;
    int         count;
    int         i;

    // Square the distance for faster comparisons
    safeDistanceSquared = safeDistance * safeDistance;

    // Build a list of spawns
    for ( i = 0, count = 0; i < level.spawnCount; i ++ )
    {
        gspawn_t* spawn = &level.spawns[i];
        int       j;

        // Ensure the team matches
        if ( team != -1 && team != spawn->team )
        {
            continue;
        }

        // Make sure this spot wont kill another player
        if ( G_SpotWouldTelefrag( spawn ) )
        {
            continue;
        }

        // Loop through connected clients
        for ( j = 0; j < level.numConnectedClients && count < MAX_SPAWNS; j ++ )
        {
            gentity_t* other = &g_entities[level.sortedClients[j]];
            vec3_t     diff;

            if ( other->client->pers.connected != CON_CONNECTED )
            {
                continue;
            }

            // Skip clients that are spectating or dead
            if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
            {
                continue;
            }

            // on safe team, dont count this guy
            if ( level.gametypeData->teams && team == other->client->sess.team )
            {
                continue;
            }

            VectorSubtract ( other->r.currentOrigin, spawn->origin, diff );

            // Far enough away to qualify
            if ( VectorLengthSquared ( diff ) < safeDistanceSquared )
            {
                break;
            }
        }

        // If we didnt go through the whole list of clients then we must
        // have hit one that was too close.  But if we did go through teh whole
        // list then this spawn point is good to go
        if ( j >= level.numConnectedClients )
        {
            spawns[count++] = spawn;
        }
    }

    // Nothing found, try it at half the safe distance
    if ( !count )
    {
        // Gotta stop somewhere
        if ( safeDistance / 2 < 250 )
        {
            return G_SelectRandomSpawnPoint ( team );
        }
        else
        {
            return G_SelectRandomSafeSpawnPoint ( team, safeDistance / 2 );
        }
    }

    // Spawn them at one of the spots
    return spawns[ rand() % count ];
}

/*
===========
G_SelectSpectatorSpawnPoint
============
*/
gspawn_t* G_SelectSpectatorSpawnPoint( void )
{
    static gspawn_t spawn;

    FindIntermissionPoint();

    VectorCopy( level.intermission_origin, spawn.origin );
    VectorCopy( level.intermission_angle, spawn.angles );

    return &spawn;
}

/*
===============
G_InitBodyQueue
===============
*/
void G_InitBodyQueue (void)
{
    gentity_t   *ent;
    int         max;

    if ( level.gametypeData->respawnType == RT_NONE )
    {
        level.bodySinkTime = 0;
        max = BODY_QUEUE_SIZE_MAX;
    }
    else
    {
        level.bodySinkTime = BODY_SINK_DELAY;
        max = BODY_QUEUE_SIZE;
    }

    level.bodyQueIndex = 0;
    for ( level.bodyQueSize = 0;
          level.bodyQueSize < max && level.bodyQueSize < level.maxclients;
          level.bodyQueSize++)
    {
        ent = G_Spawn();
        ent->classname = "bodyque";
        ent->neverFree = qtrue;
        level.bodyQue[level.bodyQueSize] = ent;
    }
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent )
{
    if ( level.time - ent->timestamp > level.bodySinkTime + BODY_SINK_TIME )
    {
        // the body ques are never actually freed, they are just unlinked
        trap_UnlinkEntity( ent );
        ent->physicsObject = qfalse;
        return;
    }

    ent->s.eFlags |= EF_NOSHADOW;

    ent->nextthink = level.time + 100;
    ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent, int hitLocation, vec3_t direction )
{
    gentity_t   *body;
    int         contents;
    int         parm;

    trap_UnlinkEntity (ent);

    // if client is in a nodrop area, don't leave the body
    contents = trap_PointContents( ent->r.currentOrigin, -1 );
    if ( contents & CONTENTS_NODROP )
    {
        return;
    }

    // grab a body que and cycle to the next one
    body = level.bodyQue[ level.bodyQueIndex ];
    level.bodyQueIndex = (level.bodyQueIndex + 1) % level.bodyQueSize;

    trap_UnlinkEntity (body);

    body->s                 = ent->s;
    body->s.eType           = ET_BODY;
    body->s.eFlags          = EF_DEAD;
    body->s.gametypeitems   = 0;
    body->s.loopSound       = 0;
    body->s.number          = body - g_entities;
    body->timestamp         = level.time;
    body->physicsObject     = qtrue;
    body->physicsBounce     = 0;
    body->s.otherEntityNum  = ent->s.clientNum;

    if ( body->s.groundEntityNum == ENTITYNUM_NONE )
    {
        body->s.pos.trType = TR_GRAVITY;
        body->s.pos.trTime = level.time;
        VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
    }
    else
    {
        body->s.pos.trType = TR_STATIONARY;
    }

    body->s.event = 0;

    parm  = (DirToByte( direction )&0xFF);
    parm += (hitLocation<<8);
    G_AddEvent(body, EV_BODY_QUEUE_COPY, parm);

    body->r.svFlags = ent->r.svFlags | SVF_BROADCAST;
    VectorCopy (ent->r.mins, body->r.mins);
    VectorCopy (ent->r.maxs, body->r.maxs);
    VectorCopy (ent->r.absmin, body->r.absmin);
    VectorCopy (ent->r.absmax, body->r.absmax);

    body->s.torsoAnim = body->s.legsAnim = ent->client->ps.legsAnim & ~ANIM_TOGGLEBIT;

    body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
    body->r.contents = 0; // CONTENTS_CORPSE;
    body->r.ownerNum = ent->s.number;

    if ( level.bodySinkTime )
    {
        body->nextthink = level.time + level.bodySinkTime;
        body->think = BodySink;
        body->s.time2 = 0;
    }
    else
    {
        // Store the time the body was spawned so the client can make them
        // dissapear if need be.
        body->s.time2 = level.time;
    }

    body->die = body_die;
    body->takedamage = qtrue;

    body->s.apos.trBase[PITCH] = 0;

    body->s.pos.trBase[2] = ent->client->ps.origin[2];
    VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );

    trap_LinkEntity (body);
}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle )
{
    int         i;

    // set the delta angle
    for (i=0 ; i<3 ; i++)
    {
        int     cmdAngle;

        cmdAngle = ANGLE2SHORT(angle[i]);
        ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
    }

    VectorCopy( angle, ent->s.angles );
    VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

/*
================
G_SetRespawnTimer
================
*/
void G_SetRespawnTimer ( gentity_t* ent )
{
    // Start a new respawn interval if the old one has passed
    if ( level.time > level.gametypeRespawnTime[ent->client->sess.team] )
    {
        level.gametypeRespawnTime[ent->client->sess.team] = level.time + g_respawnInterval.integer * 1000;
    }

    // start the interval if its not already started
    ent->client->ps.respawnTimer = level.gametypeRespawnTime[ent->client->sess.team] + 1000;
}

/*
================
respawn
================
*/
void respawn( gentity_t *ent )
{
    gentity_t   *tent;
    qboolean    ghost = qfalse;

    // No respawning when intermission is queued
    if ( level.intermissionQueued )
    {
        return;
    }

    // When we get here the user has just accepted their fate and now
    // needs to wait for the ability to respawn
    switch ( level.gametypeData->respawnType )
    {
        case RT_INTERVAL:
            G_SetRespawnTimer ( ent );
            ghost = qtrue;
            break;

        case RT_NONE:

            // Turn into a ghost
            ghost = qtrue;
            break;
    }

    // If they are a ghost then give a health point, but dont respawn
    if ( ghost )
    {
        G_StartGhosting ( ent );
        return;
    }

    trap_UnlinkEntity (ent);
    ClientSpawn(ent);

    // Add a teleportation effect.
    tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
    tent->s.clientNum = ent->s.clientNum;
}

/*
================
G_GhostCount

Returns number of ghosts on a team, if -1 is given for a team all ghosts in the game
are returned instead
================
*/
int G_GhostCount ( team_t team )
{
    int i;
    int count;

    for ( i = 0, count = 0; i < level.numConnectedClients; i ++ )
    {
        if (g_entities[level.sortedClients[i]].client->pers.connected != CON_CONNECTED )
        {
            continue;
        }

        if ( g_entities[level.sortedClients[i]].client->sess.ghost )
        {
            if ( team != -1 && team != g_entities[level.sortedClients[i]].client->sess.ghost )
            {
                continue;
            }

            count ++;
        }
    }

    return count;
}

/*
================
G_IsClientDead

Returns qtrue if the client is dead and qfalse if not
================
*/
qboolean G_IsClientDead ( gclient_t* client )
{
    if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        return qtrue;
    }

    if ( client->ps.pm_type == PM_DEAD )
    {
        return qtrue;
    }

    if ( client->sess.ghost )
    {
        return qtrue;
    }

    return qfalse;
}

/*
================
G_IsClientSpectating

Returns qtrue if the client is spectating and qfalse if not
================
*/
qboolean G_IsClientSpectating ( gclient_t* client )
{
    if ( client->pers.connected != CON_CONNECTED )
    {
        return qtrue;
    }

    if ( client->sess.team == TEAM_SPECTATOR )
    {
        return qtrue;
    }

    if ( client->sess.ghost )
    {
        return qtrue;
    }

    return qfalse;
}

/*
================
TeamCount

Returns number of players on a team
================
*/
int TeamCount( int ignoreClientNum, team_t team, int *alive )
{
    int     i;
    int     count = 0;

    if ( alive )
    {
        *alive = 0;
    }

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( i == ignoreClientNum )
        {
            continue;
        }
        if ( level.clients[i].pers.connected == CON_DISCONNECTED )
        {
            continue;
        }

        if ( level.clients[i].sess.team == team )
        {
            if ( !level.clients[i].sess.ghost && alive )
            {
                (*alive)++;
            }

            count++;
        }
    }

    return count;
}

/*
================
PickTeam
================
*/
team_t PickTeam( int ignoreClientNum )
{
    int     counts[TEAM_NUM_TEAMS];

    counts[TEAM_BLUE] = TeamCount( ignoreClientNum, TEAM_BLUE, NULL );
    counts[TEAM_RED] = TeamCount( ignoreClientNum, TEAM_RED, NULL );

    if ( counts[TEAM_BLUE] > counts[TEAM_RED] )
    {
        return TEAM_RED;
    }

    if ( counts[TEAM_RED] > counts[TEAM_BLUE] )
    {
        return TEAM_BLUE;
    }

    // equal team count, so join the team with the lowest score
    if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] )
    {
        return TEAM_RED;
    }

    return TEAM_BLUE;
}

/*
===========
G_ClientCleanName
============
*/
void G_ClientCleanName ( const char *in, char *out, int outSize, qboolean colors )
{
    int     len;
    int     colorlessLen;
    char    ch;
    char    *p;
    int     spaces;

    //save room for trailing null byte
    outSize--;

    len = 0;
    colorlessLen = 0;
    p = out;
    *p = 0;
    spaces = 0;

    while( 1 )
    {
        ch = *in++;
        if( !ch )
        {
            break;
        }

        // don't allow leading spaces
        if( !*p && ch == ' ' )
        {
            continue;
        }

        // check colors
        if( ch == Q_COLOR_ESCAPE )
        {
            // solo trailing carat is not a color prefix
            if( !*in )
            {
                break;
            }

            if ( *in != Q_COLOR_ESCAPE )
            {
                // don't allow black in a name, period
                if( !colors || ColorIndex(*in) == 0 )
                {
                    in++;
                    continue;
                }

                // make sure room in dest for both chars
                if( len > outSize - 2 )
                {
                    break;
                }

                *out++ = ch;
                *out++ = *in++;
                len += 2;
                continue;
            }
            else
            {
                *out++ = ch;
                *out++ = ch;
                in++;
                continue;
            }
        }

        // don't allow too many consecutive spaces
        if( ch == ' ' )
        {
            spaces++;
            if( spaces > 3 )
            {
                continue;
            }
        }
        else
        {
            spaces = 0;
        }

        if( len > outSize - 1 )
        {
            break;
        }

        *out++ = ch;
        colorlessLen++;
        len++;
    }

    *out = 0;

    // Trim whitespace off the end of the name
    for ( out --; out >= p && (*out == ' ' || *out == '\t'); out -- )
    {
        *out = 0;
    }

    // don't allow empty names
    if( *p == 0 || colorlessLen == 0 )
    {
        Q_strncpyz( p, "UnnamedPlayer", outSize );
    }
}

/*
===========
Updates the clients current outfittin
===========
*/
void G_UpdateOutfitting ( int clientNum )
{
    gentity_t   *ent;
    gclient_t   *client;
    int         group;
    int         ammoIndex;
    int         idle;

    int         equipWeapon;
    int         equipWeaponGroup;

    ent    = g_entities + clientNum;
    client = ent->client;

    // No can do if
    if ( client->noOutfittingChange )
    {
        return;
    }

    // Clear all ammo, clips, and weapons
    if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_CSINF, GT_MAX })) {
        client->ps.stats[STAT_WEAPONS] = 0;
        memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
        memset(client->ps.clip, 0, sizeof(client->ps.clip));

    }
    
    // Everyone gets some knives
    client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
    ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
    client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
    client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

    if ( BG_IsWeaponAvailableForOutfitting ( WP_KNIFE, 2 ) && !isCurrentGametype(GT_HNS) )
    {
        client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
    }

    equipWeapon = WP_KNIFE;
    equipWeaponGroup = OUTFITTING_GROUP_KNIFE;

    // Give all the outfitting groups to the player
    for ( group = 0; group < OUTFITTING_GROUP_ACCESSORY; group ++ )
    {
        gitem_t* item;
        int      ammoIndex;

        // Nothing available in this group
        if ( client->pers.outfitting.items[group] == -1 )
        {
            continue;
        }

        // Grab the item that represents the weapon
        if (client->sess.legacyProtocol) {
            item = &bg_itemlist[legacy_bg_outfittingGroups[group][client->pers.outfitting.items[group]]];
        }
        else {
            item = &bg_itemlist[bg_outfittingGroups[group][client->pers.outfitting.items[group]]];
        }
        

        client->ps.stats[STAT_WEAPONS] |= (1 << item->giTag);
        ammoIndex = weaponData[item->giTag].attack[ATTACK_NORMAL].ammoIndex;
        client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_NORMAL].extraClips * weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;
        client->ps.clip[ATTACK_NORMAL][item->giTag] = weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;

        // Lower group numbers are bigger guns
        if ( group < equipWeaponGroup )
        {
            equipWeaponGroup = group;
            equipWeapon = item->giTag;
        }

        // alt-fire ammo
        ammoIndex = weaponData[item->giTag].attack[ATTACK_ALTERNATE].ammoIndex;
        if ( weaponData[item->giTag].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
        {
            client->ps.clip[ATTACK_ALTERNATE][item->giTag] = weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
            client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_ALTERNATE].extraClips * weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
        }

        // Set the default firemode for this weapon
        if ( client->ps.firemode[item->giTag] == WP_FIREMODE_NONE )
        {
            client->ps.firemode[item->giTag] = BG_FindFireMode ( item->giTag, ATTACK_NORMAL, WP_FIREMODE_AUTO );
        }
    }

    // Disable zooming
    client->ps.zoomFov    = 0;
    client->ps.zoomTime  = 0;
    client->ps.pm_flags &= ~(PMF_ZOOM_FLAGS);

    if (!isCurrentGametype(GT_HNS)) {
        client->ps.weapon = equipWeapon;
        client->ps.weaponstate = WEAPON_READY; //WEAPON_SPAWNING;
    }

    if (!isCurrentGametype(GT_HNS) || client->ps.weaponTime == 0 || client->sess.team != TEAM_RED) {
        client->ps.weaponTime = 0;
        // Default to auto (or next available fire mode).
        BG_GetInviewAnim(client->ps.weapon, "idle", &idle);
        client->ps.weaponAnimId = idle;
        client->ps.weaponAnimIdChoice = 0;
        client->ps.weaponCallbackStep = 0;
    }

    
    
    client->ps.weaponAnimTime = 0;

    // Bot clients cant use the spawning state
#ifdef _SOF2_BOTS
    if ( ent->r.svFlags & SVF_BOT )
    {
        client->ps.weaponstate = WEAPON_READY;
    }
#endif


    // Armor?
    //client->ps.stats[STAT_ARMOR]   = 0;
    if (!isCurrentGametype(GT_CSINF)) {
        client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
        switch (bg_outfittingGroups[OUTFITTING_GROUP_ACCESSORY][client->pers.outfitting.items[OUTFITTING_GROUP_ACCESSORY]])
        {
        default:
        case MODELINDEX_ARMOR:
            // Boe!Man 9/16/12: Give them invisible goggles in H&S if they're enabled.
            if (isCurrentGametype(GT_HNS) && hideSeek_Extra.string[HSEXTRA_GOGGLES] == '1' && ent->client->sess.team == TEAM_BLUE) {
                client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
                client->ps.stats[STAT_ARMOR] = MAX_ARMOR;
            }
            else {
                client->ps.stats[STAT_ARMOR] = MAX_ARMOR;
            }
            break;

        case MODELINDEX_THERMAL:
            client->ps.stats[STAT_GOGGLES] = GOGGLES_INFRARED;
            break;

        case MODELINDEX_NIGHTVISION:
            if (isCurrentGametype(GT_HNS)) {
                if (hideSeek_Extra.string[HSEXTRA_GOGGLES] == '1' && ent->client->sess.team == TEAM_BLUE) {
                    client->ps.stats[STAT_ARMOR] = MAX_ARMOR;
                }
                // Boe!Man 9/16/12: Do give them goggles if the invisible goggles are disabled, they just don't have any effect. And hiders will get 'em as well.
                client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
            }
            else {
                client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
            }
            break;
        }
    }

    // Stuff which grenade is being used into stats for later use by
    // the backpack code
    if (!isCurrentGametype(GT_HNS)) {
        client->ps.ammo[weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex] = weaponData[WP_KNIFE].attack->extraClips;
    }
    else if (client->sess.team == TEAM_BLUE) {
        client->ps.ammo[weaponData[WP_KNIFE].attack[ATTACK_ALTERNATE].ammoIndex] = 0;
    }
    client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]].giTag;

    if (isCurrentGametype(GT_VIP) && client->pers.isVip) {
        client->ps.stats[STAT_ARMOR] = 200;
    }
}

/*
===========
G_FindClientByName

Looks up a player by its case insensitive name
============
*/
gclient_t* G_FindClientByName ( const char* name, int ignoreNum )
{
    int     i;

    for ( i = 0; i < level.numConnectedClients; i ++ )
    {
        gentity_t* ent = &g_entities[level.sortedClients[i]];

        if ( level.sortedClients[i] == ignoreNum )
        {
            continue;
        }

        if ( Q_stricmp ( name, ent->client->pers.netname ) == 0 )
        {
            return ent->client;
        }
    }

    return NULL;
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum )
{
    gentity_t   *ent;
    int         team;
    int         health;
    char        *s;
    gclient_t   *client;
    char        oldname[MAX_STRING_CHARS];
    char        userinfo[MAX_INFO_STRING];
    char        origname[MAX_NETNAME];
    int         namecount = 1;
    TIdentity   *oldidentity;

    ent = g_entities + clientNum;
    client = ent->client;

    trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

    // check for malformed or illegal info strings
    if ( !Info_Validate(userinfo) )
    {
        strcpy (userinfo, "\\name\\badinfo");
    }

    // check for local client
    s = Info_ValueForKey( userinfo, "ip" );
    if ( !strcmp( s, "localhost" ) )
    {
        client->pers.localClient = qtrue;
    }

    Q_strncpyz(client->pers.ip, s, sizeof(client->pers.ip));
    getSubnet(client->pers.ip, client->pers.subnet, sizeof(client->pers.subnet));

    // check the item prediction
    s = Info_ValueForKey( userinfo, "cg_predictItems" );
    if ( !atoi( s ) )
    {
        client->pers.predictItemPickup = qfalse;
    }
    else
    {
        client->pers.predictItemPickup = qtrue;
    }

    // Is anti-lag turned on?
    s = Info_ValueForKey ( userinfo, "cg_antiLag" );
    client->pers.antiLag = atoi( s )?qtrue:qfalse;

    // Is auto-reload turned on?
    s = Info_ValueForKey ( userinfo, "cg_autoReload" );
    client->pers.autoReload = atoi( s )?qtrue:qfalse;
    if ( client->pers.autoReload )
    {
        client->ps.pm_flags |= PMF_AUTORELOAD;
    }
    else
    {
        client->ps.pm_flags &= ~PMF_AUTORELOAD;
    }

    // JANFIXME Profanity

    if (client->sess.nameChangeBlock == NAMECHANGEBLOCK_NONE) {
        // set name
        Q_strncpyz(oldname, client->pers.netname, sizeof(oldname));
        s = Info_ValueForKey(userinfo, "name");
        G_ClientCleanName(s, client->pers.cleanName, sizeof(client->pers.cleanName), qfalse);
        G_ClientCleanName(s, client->pers.netname, sizeof(client->pers.netname), qtrue);
        dbAddAlias(ent);
    }    

    if ( client->sess.team == TEAM_SPECTATOR )
    {
        if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD )
        {
            Q_strncpyz( client->pers.netname, "scoreboard", sizeof(client->pers.netname) );
        }
    }

    // See if we need to find a new name because the one they chose is taken
    Q_strncpyz ( origname, client->pers.netname,  MAX_NETNAME - 5 );
    while ( G_FindClientByName ( client->pers.netname, clientNum ) )
    {
        Com_sprintf ( client->pers.netname, MAX_NETNAME, "%s(%d)", origname, namecount );
        namecount++;
    }

    // set max health
    health = atoi( Info_ValueForKey( userinfo, "handicap" ) );

    // bots set their team a few frames later
    if ( level.gametypeData->teams && (g_entities[clientNum].r.svFlags & SVF_BOT))
    {
        s = Info_ValueForKey( userinfo, "team" );
        if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) )
        {
            team = TEAM_RED;
        }
        else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) )
        {
            team = TEAM_BLUE;
        }
        else
        {
            // pick the team with the least number of players
            team = PickTeam( clientNum );
        }
    }
    else
    {
        team = client->sess.team;
    }

    // Enforce the identities
    oldidentity = client->pers.identity;

    if( level.gametypeData->teams )
    {

        if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX }) && team != TEAM_BLUE) {
            s = Info_ValueForKey(userinfo, "identity");
        }
        else {
            s = Info_ValueForKey(userinfo, "team_identity");
        }

        
        if (client->sess.lastIdentityChange + 3000 < level.time) {

            // Lookup the identity by name and if it cant be found then pick a random one
            client->pers.identity = BG_FindIdentity(s);

            if (team != TEAM_SPECTATOR)
            {

                // No identity or a team mismatch means they dont get to be that skin

                if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX })) {
                    // Check that the identity is allowed for current team.
                    if (!client->pers.identity || client->pers.identity->customGametypeTeam != team) {
                        client->pers.identity = getRandomCustomTeamIdentity(team);
                        
                        if (client->sess.lastIdentityChange + 1000 < level.time) {
                            G_printInfoMessage(ent, "Your skin was changed as it didn't match your team.");
                        }
                    }

                }
                else if (!client->pers.identity || (Q_stricmp(level.gametypeTeam[team], client->pers.identity->mTeam) && !isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX })))
                {
                    // Get first matching team identity
                    client->pers.identity = BG_FindTeamIdentity(level.gametypeTeam[team], -1);
                }



            }
            else
            {
                // Spectators are going to have to choose one of the two team skins and
                // the chance of them having the proper one in team_identity is slim, so just
                // give them a model they may use later
                client->pers.identity = BG_FindTeamIdentity(level.gametypeTeam[TEAM_RED], 0);
            }
        }
        else if (client->pers.connected == CON_CONNECTED && client->pers.identity && Q_stricmp(s, client->pers.identity->mName)) {
            G_printInfoMessage(ent, "You need to wait before you can pick a next identity.");
        }
    }
    else
    {
        s = Info_ValueForKey ( userinfo, "identity" );

        // Lookup the identity by name and if it cant be found then pick a random one
        client->pers.identity = BG_FindIdentity ( s );
    }

    // If the identity wasnt in the list then just give them the first identity.  We could
    // be fancy here and give them a random one, but this way you get less unwanted models
    // loaded
    if ( !client->pers.identity  )
    {
        client->pers.identity = &bg_identities[0];
    }

    // Report the identity change
    if ( client->pers.connected == CON_CONNECTED )
    {
        if ( client->pers.identity && oldidentity && client->pers.identity != oldidentity && team != TEAM_SPECTATOR && level.time > ent->client->sess.lastIdentityChange + 1000 )
        {
            trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " has changed identities\n\"", client->pers.netname ) );
            client->sess.identityChangeCount++;
            client->sess.lastIdentityChange = level.time;
            
        }

        // If the client is changing their name then handle some delayed name changes
        if ( strcmp( oldname, client->pers.netname ) && client->sess.nameChangeBlock == NAMECHANGEBLOCK_NONE )
        {
            // Dont let them change their name too much
            if ( level.time - client->pers.netnameTime < 5000 )
            {
                trap_SendServerCommand ( client - &level.clients[0], "print \"You must wait 5 seconds before changing your name again.\n\"" );
                strcpy ( client->pers.netname, oldname );
            }
            // voting clients cannot change their names
            else if ( (level.voteTime || level.voteExecuteTime) && strstr ( level.voteDisplayString, oldname ) )
            {
                trap_SendServerCommand ( client - &level.clients[0], "print \"You are not allowed to change your name while there is an active vote against you.\n\"" );
                strcpy ( client->pers.netname, oldname );
            }
            // If they are a ghost or spectating in an inf game their name is deferred
            else if ( level.gametypeData->respawnType == RT_NONE && (client->sess.ghost || G_IsClientDead ( client ) ) )
            {
                trap_SendServerCommand ( client - &level.clients[0], "print \"Name changes while dead will be deferred until you spawn again.\n\"" );
                strcpy ( client->pers.deferredname, client->pers.netname );
                strcpy ( client->pers.netname, oldname );
            }
            else
            {
                trap_SendServerCommand( -1, va("print \"%s renamed to %s\n\"", oldname, client->pers.netname) );
                client->pers.netnameTime = level.time;
                dbAddAlias(ent);
            }
        }
    }

    // Outfitting if pickups are disabled
    if ( level.pickupsDisabled )
    {
        // Parse out the new outfitting
        BG_DecompressOutfitting ( Info_ValueForKey ( userinfo, "outfitting" ), &client->pers.outfitting, client->sess.legacyProtocol );
        
        if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_GUNGAME, GT_CSINF, GT_MAX })) {
            G_UpdateOutfitting(clientNum);
        }
    }

    // Client mods.
    if (client->sess.legacyProtocol) {
        memset(client->sess.clientVersion, 0, sizeof(client->sess.clientVersion));
        if (level.legacyMod == CL_RPM) {
            s = Info_ValueForKey(userinfo, "cg_rpm");
            if (*s)
            {
                client->sess.clientMod = CL_RPM;
                Q_strncpyz(client->sess.clientVersion, "0.5", sizeof(client->sess.clientVersion));
            }
            // not using older client so lets test for new client
            else
            {
                s = Info_ValueForKey(userinfo, "cg_rpmClient");
                if (*s)
                {

                    if (!client->sess.verifyRoxAC && !client->sess.hasRoxAC) {
                        char* rox = Info_ValueForKey(userinfo, "cg_roxclient");
                        if (*rox) {
                            ent->client->sess.verifyRoxAC = qtrue;
                            ent->client->sess.nextRoxVerificationMessage = level.time + 5000;
                        }
                    }

                    // new client sends the version of the client mod eg. 0.6
                    Q_strncpyz(client->sess.clientVersion, s, sizeof(client->sess.clientVersion));

                    client->sess.clientMod = CL_RPM;


                }
                else { // if no rpm client
                    client->sess.clientMod = CL_NONE; // To be fair, we could display RPMPro info here, but without proper RPMPro support, I do not see the point.
                }
            }
        }

    }

    // send over a subset of the userinfo keys so other clients can
    // print scoreboards, display models, and play custom sounds
    if ( ent->r.svFlags & SVF_BOT )
    {
        s = va("n\\%s\\t\\%i\\identity\\%s\\skill\\%s",
            client->pers.netname, team, client->pers.identity->mName,
            Info_ValueForKey( userinfo, "skill" ) );
    }
    else
    {
        s = va("n\\%s\\t\\%i\\identity\\%s",
               client->pers.netname, team, client->pers.identity->mName );
    }

    trap_SetConfigstring( CS_PLAYERS+clientNum, s );

    G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or map restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and map
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot )
{
    char        *value;
    gclient_t   *client;
    char        userinfo[MAX_INFO_STRING];
    char        ip[128];
    char        guid[64];
    gentity_t   *ent;

    ent = &g_entities[ clientNum ];

    trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

    value = Info_ValueForKey (userinfo, "cl_guid");
    Com_sprintf ( guid, sizeof(guid), value );

    // check to see if they are on the banned IP list
    value = Info_ValueForKey (userinfo, "ip");
    Com_sprintf ( ip, sizeof(ip), value );

    // we don't check password for bots and local client
    // NOTE: local client <-> "ip" "localhost"
    //   this means this client is not running in our current process
    if ( !( isBot ) && (strcmp(ip, "localhost") != 0))
    {
        // check for a password
        value = Info_ValueForKey (userinfo, "password");
        if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) &&
            strcmp( g_password.string, value) != 0)
        {
            return va("Invalid password: %s", value );
        }

        // Check for ban information.
        char banReason[128];
        int endOfMap = 0, banEnd = 0;
        qboolean banned = dbCheckBan(ip, banReason, sizeof(banReason), &endOfMap, &banEnd);

        if (banned) {
            return va("Banned: %s%s", banReason, endOfMap ? " (end of map)" : va(", left: %dd, %02dh, %02dm", banEnd / (60 * 24), (banEnd / 60) % 24, banEnd % 60));
        }

    }

    // they can connect
    ent->client = level.clients + clientNum;
    client = ent->client;

    memset( client, 0, sizeof(*client) );

    client->pers.connected = CON_CONNECTING;

    client->sess.team = TEAM_SPECTATOR;

    client->sess.legacyProtocol = trap_IsClientLegacy(clientNum);
    // read or initialize the session data
    if ( firstTime || level.newSession )
    {
        G_InitSessionData( client, userinfo, firstTime );
    }

    G_ReadSessionData( client );

    if( isBot )
    {
        ent->r.svFlags |= SVF_BOT;
        ent->inuse = qtrue;
        if( !G_BotConnect( clientNum, !firstTime ) )
        {
            return "BotConnectfailed";
        }
    }

    // get and distribute relevent paramters
    G_LogPrintf( "ClientConnect: %i - %s [%s]\n", clientNum, ip, guid );
    ClientUserinfoChanged( clientNum );

    // Check whether country is set.

    if (!strlen(ent->client->sess.countryCode) && g_useCountryDb.integer) {
        int blockLevel = 0;

        qboolean hasCountry = dbGetCountry(ip, ent->client->sess.countryCode, sizeof(ent->client->sess.countryCode), ent->client->sess.country, sizeof(ent->client->sess.country), &blockLevel);
        if (!hasCountry) {
            if (g_useCountryAPI.integer && strlen(g_iphubAPIKey.string) > 0) {
                int enqueue = enqueueOutbound(THREADACTION_IPHUB_DATA_REQUEST, clientNum, ent->client->pers.ip, sizeof(ent->client->pers.ip));

                if (enqueue == THREADRESPONSE_ENQUEUE_COULDNT_MALLOC) {
                    logSystem(LOGLEVEL_WARN, "Couldn't malloc in thread!");
                }
            }
        }
        else if (g_vpnAutoKick.integer && blockLevel == IPHUBBLOCK_VPN) {
            return "VPN Detected!";
        }
    }

    // don't do the "xxx connected" messages if they were caried over from previous level
    if ( firstTime )
    {
        trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " is connecting...\n\"", client->pers.netname) );

        // Broadcast team change if not going to spectator
        if ( level.gametypeData->teams && client->sess.team != TEAM_SPECTATOR )
        {
            BroadcastTeamChange( client, -1 );
        }

        client->sess.firstTime = qtrue;
    }


    if (ent->client->sess.adminLevel == ADMLVL_NONE) {

        int admlvl = dbGetAdminLevel(ADMTYPE_IP, ent, NULL);

        if (admlvl > ADMLVL_NONE) {
            ent->client->sess.adminLevel = admlvl;
            ent->client->sess.adminType = ADMTYPE_IP;
            Q_strncpyz(ent->client->sess.adminName, ent->client->pers.cleanName, sizeof(ent->client->sess.adminName));
        }

    }

    if (!ent->client->sess.clanMember) {

        qboolean isClanMember = dbGetClan(CLANTYPE_IP, ent, NULL);

        if (isClanMember) {
            ent->client->sess.clanMember = qtrue;
            ent->client->sess.clanType = CLANTYPE_IP;
            Q_strncpyz(ent->client->sess.clanName, ent->client->pers.cleanName, sizeof(ent->client->sess.clanName));
        }

    }

    // count current clients and rank for scoreboard
    CalculateRanks();

    // Make sure they are unlinked
    ent->s.number = clientNum;
    trap_UnlinkEntity ( ent );

    reapplyMuteAfterConnect(ent);

    // Zero out statinfo.
    Com_Memset(&ent->client->pers.statInfo, 0, sizeof(ent->client->pers.statInfo));
    ent->client->pers.statInfo.lastclient_hurt = -1;
    ent->client->pers.statInfo.lasthurtby = -1;
    ent->client->pers.statInfo.lastKillerHealth = -1;
    ent->client->pers.statInfo.lastKillerArmor = -1;

    return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum, qboolean setTime)
{
    gentity_t   *ent;
    gclient_t   *client;
    gentity_t   *tent;
    int         flags;
    int         spawncount;

    ent = g_entities + clientNum;

    client = level.clients + clientNum;

    if ( ent->r.linked )
    {
        trap_UnlinkEntity( ent );
    }

    // Run a gametype check just in case the game hasnt started yet
    if ( !level.gametypeStartTime )
    {
        CheckGametype ( );
    }

    G_InitGentity( ent );
    ent->touch = 0;
    ent->pain = 0;
    ent->client = client;

    client->pers.connected = CON_CONNECTED;

    if (setTime) {
        client->pers.enterTime = level.time;
    }

    client->pers.teamState.state = TEAM_BEGIN;

    // save eflags around this, because changing teams will
    // cause this to happen with a valid entity, and we
    // want to make sure the teleport bit is set right
    // so the viewpoint doesn't interpolate through the
    // world to the new position
    flags = client->ps.eFlags;
    spawncount = client->ps.persistant[PERS_SPAWN_COUNT];
    memset( &client->ps, 0, sizeof( client->ps ) );
    client->ps.eFlags = flags;
    client->ps.persistant[PERS_SPAWN_COUNT] = spawncount;

    // locate ent at a spawn point
    ClientSpawn( ent );

    if ( client->sess.team != TEAM_SPECTATOR )
    {
        // send event
        tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
        tent->s.clientNum = ent->s.clientNum;

        trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " entered the game\n\"", client->pers.netname) );
    }

    G_LogPrintf( "ClientBegin: %i\n", clientNum );

    // See if we should spawn as a ghost
    if ( client->sess.team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE )
    {
        // If there are ghosts already then spawn as a ghost because
        // the game is already in progress.
        if ( !level.warmupTime && (level.gametypeJoinTime && (level.time - level.gametypeJoinTime) > (g_roundjointime.integer * 1000)) )
        {
            G_StartGhosting ( ent );
        }
    }

    // count current clients and rank for scoreboard
    CalculateRanks();
    
    if (g_teamAutoJoin.integer && client->sess.team == TEAM_SPECTATOR && /*setTime &&*/ client->pers.connected == CON_CONNECTED && isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_PROP, GT_MAX })) {
        if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNZ, GT_PROP, GT_MAX })) {
            // Boe!Man 10/26/14: Force clients after shotguns are distributed to blue instead of red.

            if (g_teamAutoJoin.integer == 1 || (g_teamAutoJoin.integer == 2 && !client->sess.afkSpec)) {
                if (level.customGameStarted) {
                    SetTeam(ent, "blue", NULL, qfalse);
                }
                else {
                    SetTeam(ent, "red", NULL, qfalse);
                }
            }



        }
        else if (!(ent->r.svFlags & SVF_BOT)) { //differentiate between 1 and 2
            if (g_teamAutoJoin.integer == 1) {
                SetTeam(ent, chooseTeam(), NULL, qfalse);
            }
            else if (!client->sess.afkSpec && g_teamAutoJoin.integer == 2) {
                SetTeam(ent, chooseTeam(), NULL, qfalse);
            }

        }
    }

    // Boe!Man 7/6/15: Check if we require the use of the modified cgame.
    if (g_enforce1fxAdditions.integer) {
        client->sess.checkClientAdditions = 1;
        client->sess.clientAdditionCheckTime = level.time + 2500; // Check after 2.5 second to avoid flooding the client.
    }

    if (isCurrentGametype(GT_CSINF)) {
        resetCSInfStruct(ent);
    }

}

/*
===========
G_SelectClientSpawnPoint

Selects a spawn point for the given client entity
============
*/
gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent )
{
    gclient_t*  client = ent->client;
    gspawn_t*   spawnPoint;

        // find a spawn point
    // do it before setting health back up, so farthest
    // ranging doesn't count this client
    if ( client->sess.team == TEAM_SPECTATOR )
    {
        spawnPoint = G_SelectSpectatorSpawnPoint ( );
    }
    else
    {
        if ( level.gametypeData->teams && level.gametypeData->respawnType != RT_NORMAL )
        {
            // Dont bother selecting a safe spawn on non-respawn games, the map creator should
            // have done this for us.
            if ( level.gametypeData->respawnType == RT_NONE )
            {
                spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
            }
            else
            {
                spawnPoint = G_SelectRandomSafeSpawnPoint ( ent->client->sess.team, 1500 );
            }

            if ( !spawnPoint )
            {
                // don't spawn near other players if possible
                spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
            }

            // Spawn at any deathmatch spawn, telefrag if needed
            if ( !spawnPoint )
            {
                spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
            }
        }
        else
        {
            // Try deathmatch spawns first
            spawnPoint = G_SelectRandomSafeSpawnPoint ( TEAM_FREE, 1500 );

            // If none found use any spawn
            if ( !spawnPoint )
            {
                spawnPoint = G_SelectRandomSafeSpawnPoint ( -1, 1500 );
            }

            // Spawn at any deathmatch spawn, telefrag if needed
            if ( !spawnPoint )
            {
                spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
            }

            // Spawn at any gametype spawn, telefrag if needed
            if ( !spawnPoint )
            {
                spawnPoint = G_SelectRandomSpawnPoint ( -1 );
            }
        }
    }

    return spawnPoint;
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/

void ClientSpawn(gentity_t *ent)
{
    int                 index;
    vec3_t              spawn_origin;
    vec3_t              spawn_angles;
    gclient_t           *client;
    int                 i;
    clientPersistant_t  saved;
    clientSession_t     savedSess;
    int                 persistant[MAX_PERSISTANT];
    gspawn_t            *spawnPoint;
    int                 flags;
    int                 savedPing;
    int                 eventSequence;
    char                userinfo[MAX_INFO_STRING];
    int                 start_ammo_type;
    int                 ammoIndex;
    int                 idle;

    index  = ent - g_entities;
    client = ent->client;

    // Where do we spawn?
    spawnPoint = G_SelectClientSpawnPoint ( ent );
    if ( spawnPoint )
    {
        VectorCopy ( spawnPoint->angles, spawn_angles );
        VectorCopy ( spawnPoint->origin, spawn_origin );
        spawn_origin[2] += 9;
    }
    else
    {
        SetTeam ( ent, "s", NULL, qfalse );
        return;
    }

    client->pers.teamState.state = TEAM_ACTIVE;

    // toggle the teleport bit so the client knows to not lerp
    // and never clear the voted flag
    flags = ent->client->ps.eFlags & (EF_TELEPORT_BIT | EF_VOTED);
    flags ^= EF_TELEPORT_BIT;

    // clear everything but the persistant data
    saved = client->pers;
    savedSess = client->sess;
    savedPing = client->ps.ping;
    for ( i = 0 ; i < MAX_PERSISTANT ; i++ )
    {
        persistant[i] = client->ps.persistant[i];
    }
    eventSequence = client->ps.eventSequence;

    int currWeapons = client->ps.stats[STAT_WEAPONS];
    int currGoggles = client->ps.stats[STAT_GOGGLES];
    int currArmor = client->ps.stats[STAT_ARMOR];

    memset (client, 0, sizeof(*client));

    client->pers = saved;
    client->sess = savedSess;
    client->ps.ping = savedPing;
    client->lastkilled_client = -1;

    if (isCurrentGametype(GT_CSINF) && !ent->client->pers.csinf.resetGuns) {

        client->ps.stats[STAT_GOGGLES] = currGoggles;
        client->ps.stats[STAT_ARMOR] = currArmor;

        for (int csinfWpn = 0; csinfWpn < CSINF_GUNTABLE_SIZE; csinfWpn++) {
            csInfGuns_t* gun = &csInfGunsTable[csinfWpn];

            if (gun->gunType != GUNTYPE_UTILITY && currWeapons & (1 << gun->gunId)) {
                giveWeaponToClient(ent, gun->gunId, qtrue);
            }
        }

    }

    for ( i = 0 ; i < MAX_PERSISTANT ; i++ )
    {
        client->ps.persistant[i] = persistant[i];
    }
    client->ps.eventSequence = eventSequence;

    // increment the spawncount so the client will detect the respawn
    client->ps.persistant[PERS_SPAWN_COUNT]++;
    client->ps.persistant[PERS_TEAM] = client->sess.team;
    client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;

    client->airOutTime = level.time + 12000;

    trap_GetUserinfo( index, userinfo, sizeof(userinfo) );

    // clear entity values
    client->ps.eFlags = flags;

    ent->s.groundEntityNum = ENTITYNUM_NONE;
    ent->client = &level.clients[index];
    ent->takedamage = qtrue;
    ent->inuse = qtrue;
    ent->classname = "player";
    ent->r.contents = CONTENTS_BODY;
    ent->clipmask = MASK_PLAYERSOLID;
    ent->die = player_die;
    ent->waterlevel = 0;
    ent->watertype = 0;
    ent->flags = 0;

    VectorCopy (playerMins, ent->r.mins);
    VectorCopy (playerMaxs, ent->r.maxs);

    client->ps.clientNum = index;
    ent->s.number = index;

    // Bring back the saved firemodes
    memcpy ( client->ps.firemode, client->pers.firemode, sizeof(client->ps.firemode) );

    //give default weapons

    if (!isCurrentGametype(GT_CSINF) || ent->client->pers.csinf.resetGuns) {
        client->ps.stats[STAT_WEAPONS] = (1 << WP_NONE);
    }

    client->noOutfittingChange = qfalse;

    // Give the client their weapons depending on whether or not pickups are enabled
    if ( level.pickupsDisabled )
    {

        if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_GUNGAME, GT_MAX })) {

            giveWeaponToClient(ent, WP_KNIFE, qtrue);

            client->sess.invisible = qfalse;
            // Reset the transformed entity if there is one.
            if (client->sess.transformedEntity) {
                G_FreeEntity(&g_entities[client->sess.transformedEntity]);
                client->sess.transformedEntity = 0;
                client->sess.transformed = qfalse;
            }
            if (client->sess.transformedEntityBBox) {
                G_FreeEntity(&g_entities[client->sess.transformedEntityBBox]);
                client->sess.transformedEntityBBox = 0;
            }

            client->ps.eFlags &= ~EF_HSBOX;
            ent->s.eFlags &= ~EF_HSBOX;
        }
        else {
            G_UpdateOutfitting(index);
        }

        // Prevent the client from picking up a whole bunch of stuff
        client->ps.pm_flags |= PMF_LIMITED_INVENTORY;
    }
    else
    {
        // Knife.
        client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
        ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
        client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
        client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
        client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

        // Set up some weapons and ammo for the player.
        client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_USSOCOM_PISTOL );
        start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].ammoIndex;
        client->ps.ammo[start_ammo_type] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
        client->ps.clip[ATTACK_NORMAL][WP_USSOCOM_PISTOL] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
        client->ps.firemode[WP_USSOCOM_PISTOL] = BG_FindFireMode ( WP_USSOCOM_PISTOL, ATTACK_NORMAL, WP_FIREMODE_AUTO );

        // alt-fire ammo
        start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_ALTERNATE].ammoIndex;
        if (AMMO_NONE != start_ammo_type)
        {
            client->ps.ammo[start_ammo_type] = ammoData[start_ammo_type].max;
        }

        // Everyone gets full armor in deathmatch
        client->ps.stats[STAT_ARMOR] = MAX_HEALTH;
    }

    client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;

    G_SetOrigin( ent, spawn_origin );
    VectorCopy( spawn_origin, client->ps.origin );

    // the respawned flag will be cleared after the attack and jump keys come up
    client->ps.pm_flags |= PMF_RESPAWNED;
    if ( client->pers.autoReload )
    {
        client->ps.pm_flags |= PMF_AUTORELOAD;
    }
    else
    {
        client->ps.pm_flags &= ~PMF_AUTORELOAD;
    }

    trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
    SetClientViewAngle( ent, spawn_angles );

    if ( ent->client->sess.team != TEAM_SPECTATOR )
    {
        G_KillBox( ent );
        trap_LinkEntity (ent);

        // force the base weapon up
        if ( !level.pickupsDisabled )
        {
            client->ps.weapon = WP_USSOCOM_PISTOL;
            client->ps.weaponstate = WEAPON_RAISING;
            client->ps.weaponTime = 500;

            // Default to auto (or next available fire mode).
            client->ps.firemode[client->ps.weapon] = BG_FindFireMode ( client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
            BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
            client->ps.weaponAnimId = idle;
            client->ps.weaponAnimIdChoice = 0;
            client->ps.weaponCallbackStep = 0;
        }

        if (g_voiceFloodCount.integer) {
            client->sess.voiceFloodPenalty = 0;
            client->sess.voiceFloodTimer = 0;
            client->sess.voiceFloodCount = 0;
        }
    }
    else
    {
        client->ps.weapon = WP_KNIFE;
        BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
        client->ps.weaponAnimId = idle;
    }

    // don't allow full run speed for a bit
    client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
    client->ps.pm_time = 100;

    client->respawnTime = level.time;
    client->invulnerableTime = level.time;
    client->ps.eFlags |= EF_INVULNERABLE;
    client->inactivityTime = level.time + g_inactivity.integer * 1000;
    client->latched_buttons = 0;

    // set default animations
    client->ps.weaponstate = WEAPON_READY;
    client->ps.torsoAnim = -1;
    client->ps.legsAnim = LEGS_IDLE;

    client->ps.weaponAnimIdChoice = 0;
    client->ps.weaponCallbackStep = 0;
    client->ps.weaponCallbackTime = 0;

    // Not on a ladder
    client->ps.ladder = -1;

    // Not leaning
    client->ps.leanTime = LEAN_TIME;

    if ( level.intermissiontime )
    {
        MoveClientToIntermission( ent );
    }

    if (level.paused) {
        ent->client->ps.pm_type = PM_INTERMISSION;
    }

    // Frozen?
    if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_PROP, GT_MAX })) {
        if (level.gametypeDelayTime > level.time)
        {
            ent->client->ps.stats[STAT_FROZEN] = level.gametypeDelayTime - level.time;
        }
    }
    else {
        // Frozen?
        if (level.gametypeDelayTime > level.time && ent->client->sess.team == TEAM_BLUE) // Henk 21/01/10 -> Only seekers have a start delay
        {
            ent->client->ps.stats[STAT_FROZEN] = level.gametypeDelayTime - level.time;
        }
    }
    if (isCurrentGametype(GT_HNZ)) {
        ent->client->ps.stats[STAT_FROZEN] = 0;
    }
        
    // run a client frame to drop exactly to the floor,
    // initialize animations and other things
    client->ps.commandTime = level.time - 100;
    ent->client->pers.cmd.serverTime = level.time;
    ClientThink( ent-g_entities );

    // positively link the client, even if the command times are weird
    if ( ent->client->sess.team != TEAM_SPECTATOR )
    {
        BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
        VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
        trap_LinkEntity( ent );
    }

    // run the presend to set anything else
    ClientEndFrame( ent );

    // clear entity state values
    BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

    // Handle a deferred name change
    if ( client->pers.deferredname[0] )
    {
        trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " renamed to %s\n\"", client->pers.netname, client->pers.deferredname) );
        strcpy ( client->pers.netname, client->pers.deferredname );
        dbAddAlias(ent);
        client->pers.deferredname[0] = '\0';
        client->pers.netnameTime = level.time;
        ClientUserinfoChanged ( client->ps.clientNum );
    }

    // Update the time when other people can join the game
    if ( !level.gametypeJoinTime && level.gametypeData->teams )
    {
        // As soon as both teams have people on them the counter starts
        if ( TeamCount ( -1, TEAM_RED, NULL ) && TeamCount ( -1, TEAM_BLUE, NULL ) )
        {
            level.gametypeJoinTime = level.time;
        }
    }


    if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX })) {
        // Henk 19/01/10 -> Start with knife
        ent->client->ps.ammo[weaponData[WP_KNIFE].attack[ATTACK_ALTERNATE].ammoIndex] = 0;
        ent->client->ps.weapon = WP_KNIFE;
        ent->client->ps.weaponstate = WEAPON_READY;

        client->ps.stats[STAT_ARMOR] = 0; // Henk 27/02/10 -> Fix that ppl start with no armor
        client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;

        // Boe!Man 1/28/14: Also (re-)set some inactivity stuff.
        if (!g_inactivity.integer || g_inactivity.integer >= 10) {
            if (!level.customGameStarted) { // Seekers are still waiting to be released.
                client->pers.seekerAwayTime = level.time + (level.gametypeDelayTime - level.time) + 2000; // Give the seeker two initial seconds to get away prior to being away.
            }
            else {
                client->pers.seekerAwayTime = level.time + 10000; // Or 10 seconds if he joins the team in the middle of a running game.
            }
        }
        else {
            client->pers.seekerAwayTime = -1;
        }
        client->pers.seekerAway = qfalse;
        client->pers.seekerAwayEnt = -1;
    }

    if (isCurrentGametype(GT_CSINF)) {
        ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_KNIFE);
        giveWeaponToClient(ent, WP_USSOCOM_PISTOL, qfalse);
        ent->client->pers.csinf.resetGuns = qfalse;
    }
    
    if (isCurrentGametype(GT_GUNGAME)) {
        client->pers.gg.level--;
        gungame_giveGuns(ent);
    }

}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum )
{
    gentity_t   *ent;
    gentity_t   *tent;
    int         i;

    // cleanup if we are kicking a bot that
    // hasn't spawned yet
    G_RemoveQueuedBotBegin( clientNum );

    ent = g_entities + clientNum;
    if ( !ent->client )
    {
        return;
    }

    // stop any following clients
    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( G_IsClientSpectating ( &level.clients[i] ) &&
             level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW &&
             level.clients[i].sess.spectatorClient == clientNum )
        {
            G_StopFollowing( &g_entities[i] );
        }
    }

    // send effect if they were completely connected
    if ( ent->client->pers.connected == CON_CONNECTED &&
         !G_IsClientSpectating ( ent->client )  &&
         !G_IsClientDead ( ent->client ) )
    {
        tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
        tent->s.clientNum = ent->s.clientNum;

        if (isCurrentGametype(GT_PROP) && ent->client->pers.prop.isMovingModel) {
            freeProphuntProps(ent);
        }

        // Dont drop weapons
        if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNZ, GT_HNZ, GT_MAX })) {
            ent->client->ps.stats[STAT_WEAPONS] = 0;
        }
        else if (isCurrentGametype(GT_HNS)) {
            // Also check the random grenade.
            if (hideSeek_Extra.string[HSEXTRA_RANDOMGRENADE] == '1' && ent->client->sess.transformedEntity) {
                G_FreeEntity(&g_entities[ent->client->sess.transformedEntity]);
                ent->client->sess.transformedEntity = 0;

                if (ent->client->sess.transformedEntityBBox) {
                    G_FreeEntity(&g_entities[ent->client->sess.transformedEntityBBox]);
                    ent->client->sess.transformedEntityBBox = 0;
                }
                ent->s.eFlags &= ~EF_HSBOX;
                ent->client->ps.eFlags &= ~EF_HSBOX;
                strncpy(level.hns.randomNadeLoc, "Disappeared", sizeof(level.hns.randomNadeLoc));
            }
        }
        

        // Get rid of things that need to drop
        TossClientItems( ent );
    }

    G_LogPrintf( "ClientDisconnect: %i\n", clientNum );

    trap_UnlinkEntity (ent);
    ent->s.modelindex = 0;
    ent->inuse = qfalse;
    ent->classname = "disconnected";
    ent->client->pers.connected = CON_DISCONNECTED;
    ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
    ent->client->sess.team = TEAM_FREE;

    trap_SetConfigstring( CS_PLAYERS + clientNum, "");

    CalculateRanks();

#ifdef _SOF2_BOTS
    if ( ent->r.svFlags & SVF_BOT )
    {
        BotAIShutdownClient( clientNum, qfalse );
    }
#endif

    backupInMemoryDatabases();
}

/*
===========
G_UpdateClientAnimations

Updates the animation information for the client
============
*/
void G_UpdateClientAnimations ( gentity_t* ent )
{
    gclient_t*  client;

    client = ent->client;

    // Check for anim change in the legs
    if ( client->legs.anim != ent->s.legsAnim )
    {
        client->legs.anim = ent->s.legsAnim;
        client->legs.animTime = level.time;
    }

    // Check for anim change in the torso
    if ( client->torso.anim != ent->s.torsoAnim )
    {
        client->torso.anim = ent->s.torsoAnim;
        client->torso.animTime = level.time;
    }

    // Force the legs and torso to stay aligned for now to ensure the client
    // and server are in sync with the angles.
    // TODO: Come up with a way to keep these in sync on both client and server
    client->torso.yawing   = qtrue;
    client->torso.pitching = qtrue;
    client->legs.yawing    = qtrue;

    // Calculate the real torso and leg angles
    BG_PlayerAngles ( ent->client->ps.viewangles,
                      NULL,

                      ent->client->ghoulLegsAngles,
                      ent->client->ghoulLowerTorsoAngles,
                      ent->client->ghoulUpperTorsoAngles,
                      ent->client->ghoulHeadAngles,

                      (float)(ent->client->ps.leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET,

                      0,
                      0,
                      level.time,

                      &client->torso,
                      &client->legs,

                      level.time - level.previousTime,

                      client->ps.velocity,

                      qfalse,
                      ent->s.angles2[YAW],
                      NULL );
}

/*
===========
G_FindNearbyClient

Locates a client thats near the given origin
============
*/
gentity_t* G_FindNearbyClient ( vec3_t origin, team_t team, float radius, gentity_t* ignore )
{
    int i;

    for ( i = 0; i < level.numConnectedClients; i ++ )
    {
        gentity_t* other = &g_entities[ level.sortedClients[i] ];
        vec3_t     diff;

        if ( other->client->pers.connected != CON_CONNECTED )
        {
            continue;
        }

        if ( other == ignore )
        {
            continue;
        }

        if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
        {
            continue;
        }

        if ( other->client->sess.team != team )
        {
            continue;
        }

        // See if this client is close enough to yell sniper
        VectorSubtract ( other->r.currentOrigin, origin, diff );
        if ( VectorLengthSquared ( diff ) < radius * radius )
        {
            return other;
        }
    }

    return NULL;
}

/*
===========
G_IgnoreClientChat

Instructs all chat to be ignored by the given
============
*/
void G_IgnoreClientChat ( int ignorer, int ignoree, qboolean ignore )
{
    // Cant ignore yourself
    if ( ignorer == ignoree )
    {
        return;
    }

    // If there is no client connected then dont bother
    if ( g_entities[ignoree].client->pers.connected != CON_CONNECTED )
    {
        return;
    }

    if ( ignore )
    {
        g_entities[ignoree].client->sess.chatIgnoreClients[ignorer/32] |= (1<<(ignorer%32));
    }
    else
    {
        g_entities[ignoree].client->sess.chatIgnoreClients[ignorer/32] &= ~(1<<(ignorer%32));
    }
}

/*
===========
G_IsClientChatIgnored

Checks to see if the given client is being ignored by a specific client
============
*/
qboolean G_IsClientChatIgnored ( int ignorer, int ignoree )
{
    if ( g_entities[ignoree].client->sess.chatIgnoreClients[ignorer/32] & (1<<(ignorer%32)) )
    {
        return qtrue;
    }

    return qfalse;
}
