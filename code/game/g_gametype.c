// Copyright (C) 2001-2002 Raven Software
//
// g_gametype.c -- dynamic gametype handling

#include "g_local.h"
#include "../qcommon/q_shared.h"
#include "g_team.h"
#include "inv.h"

#define MAX_GAMETYPE_SPAWN_POINTS   32

void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator );
void target_effect_delayed_use ( gentity_t* self );


int         g_gametypeItemCount = 0;
vec3_t      g_effectOrigin;

/*QUAKED gametype_player (0 1 0) (-16 -16 -46) (16 16 48) REDTEAM BLUETEAM
Potential spawning position for red or blue team in custom gametype games.
*/
void SP_gametype_player ( gentity_t *ent )
{
    team_t  team;

    // Cant take any more spawns!!
    if ( level.spawnCount >= MAX_SPAWNS )
    {
        G_FreeEntity ( ent );
        return;
    }

    // If a team filter is set then override any team settings for the spawns
    if ( level.mTeamFilter[0] )
    {
        if ( Q_stricmp ( level.mTeamFilter, "red") == 0 )
        {
            team = TEAM_RED;
        }
        else if ( Q_stricmp ( level.mTeamFilter, "blue") == 0 )
        {
            team = TEAM_BLUE;
        }
        else
        {
            G_FreeEntity ( ent );
            return;
        }
    }
    else
    {
        // Red team only
        if ( ent->spawnflags & 0x1 )
        {
            team = TEAM_RED;
        }
        else if ( ent->spawnflags & 0x2 )
        {
            team = TEAM_BLUE;
        }
        else
        {
            team = TEAM_FREE;
        }
    }

    G_AddClientSpawn ( ent, team );

    G_FreeEntity ( ent );
}

void SP_mission_player ( gentity_t* ent )
{
    ent->classname = "gametype_player";

    SP_gametype_player ( ent );
}

void gametype_item_use ( gentity_t* self, gentity_t* other )
{
    if ( level.gametypeResetTime )
    {
        return;
    }

    if ( trap_GT_SendEvent ( GTEV_ITEM_USED, level.time, self->item->quantity, other->s.number, other->client->sess.team, 0, 0 ) )
    {
        G_UseTargets ( self, other );
    }
}

void gametype_trigger_use ( gentity_t *self, gentity_t *other, gentity_t *activator )
{
    if ( level.gametypeResetTime )
    {
        return;
    }

    if ( trap_GT_SendEvent ( GTEV_TRIGGER_USED, level.time, self->health, other->s.number, other->client->sess.team, 0, 0 ) )
    {
        G_UseTargets ( self, other );
    }
}

void gametype_trigger_touch ( gentity_t *self, gentity_t *other, trace_t *trace )
{
    if ( level.gametypeResetTime )
    {
        return;
    }

    if ( trap_GT_SendEvent ( GTEV_TRIGGER_TOUCHED, level.time, self->health, other->s.number, other->client->sess.team, 0, 0 ) )
    {
        other->client->pers.statInfo.itemCaptures++;
        G_UseTargets ( self, other );
    }
}

/*QUAKED gametype_trigger (0 0 .8) ?
*/
void SP_gametype_trigger ( gentity_t* ent )
{
    // The target name is only used by the gametype system, so it shouldnt
    // be adjusted by the RMG
    if ( level.mTargetAdjust )
    {
        if ( ent->targetname )
            ent->targetname = strchr ( ent->targetname, '-' ) + 1;
    }

    InitTrigger (ent);

    ent->s.eType = ET_GAMETYPE_TRIGGER;
}

gentity_t* G_RealSpawnGametypeItem ( gitem_t* item, vec3_t origin, vec3_t angles, qboolean dropped )
{
    gentity_t* it_ent;

    it_ent = G_Spawn();

    it_ent->flags |= FL_DROPPED_ITEM;
    it_ent->item = item;

    VectorCopy( origin, it_ent->s.origin );
    VectorCopy ( angles, it_ent->s.apos.trBase );
    VectorCopy ( angles, it_ent->s.angles );
    it_ent->classname = item->classname;
    G_SpawnItem ( it_ent, it_ent->item );
    FinishSpawningItem(it_ent);

    VectorSet( it_ent->r.mins, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS );
    VectorSet( it_ent->r.maxs, ITEM_RADIUS * 4 / 3, ITEM_RADIUS * 4 / 3, ITEM_RADIUS );

    return it_ent;
}

gentity_t* G_SpawnGametypeItem ( const char* pickup_name, qboolean dropped, vec3_t origin )
{
    gentity_t* ent;

    if ( dropped )
    {
        gitem_t* item = BG_FindItem ( pickup_name );
        if ( item )
        {
            return G_RealSpawnGametypeItem ( item, origin, vec3_origin, dropped );
        }

        return NULL;
    }

    // Look for the gametype item in the map
    ent = NULL;
    while ( NULL != (ent = G_Find ( ent, FOFS(classname), "gametype_item" ) ) )
    {
        // Match?
        if ( !Q_stricmp ( ent->item->pickup_name, pickup_name ) )
        {
            break;
        }
    }

    // If we couldnt find the item spawner then we have a problem
    if ( !ent )
    {
        Com_Error ( ERR_FATAL, "Could not spawn gametype item '%s'\n", pickup_name );
        return NULL;
    }

    return G_RealSpawnGametypeItem ( ent->item, ent->r.currentOrigin, ent->s.angles, dropped );
}

void G_GametypeItemThink ( gentity_t* ent )
{
    G_RealSpawnGametypeItem ( ent->item, ent->r.currentOrigin, ent->s.angles, qfalse );
}

/*QUAKED gametype_item (0 0 1) (-16 -16 -16) (16 16 16)
"name"          name of the item to spawn (defined in gametype script)
*/
void SP_gametype_item ( gentity_t* ent )
{
    // TEMPORARY HACK
    if ( level.mTargetAdjust )
    {
        // Boe!Man 2/11/13: Safe target(name) checking.
        if (ent->targetname && ent->targetname[0]) {
            if (strstr(ent->targetname, "-")) {
                ent->targetname = strchr(ent->targetname, '-') + 1;
            }
            else {
                Q_strncpyz(ent->targetname, ent->targetname, sizeof(ent->targetname));
            }
        }

        if (ent->target && ent->target[0]) {
            if (strstr(ent->target, "-")) {
                ent->target = strchr(ent->target, '-') + 1;
            }
            else {
                Q_strncpyz(ent->target, ent->target, sizeof(ent->target));
            }
        }
    }

    G_SetOrigin( ent, ent->s.origin );
}

/*
===============
G_ResetGametypeItem
===============
*/
void G_ResetGametypeItem ( gitem_t* item )
{
    gentity_t *find;
    int       i;

    // Convience check
    if ( !item )
    {
        return;
    }

    if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNZ, GT_HNS, GT_MAX })) {
        return;
    }

    // Remove all spawned instances of the item on the map
    find = NULL;
    while ( NULL != (find = G_Find ( find, FOFS(classname), item->classname ) ) )
    {
        // Free this entity soon
        find->nextthink = level.time + 10;
        find->think = G_FreeEntity;
    }

    // Strip the item from all connected clients
    for ( i = 0; i < level.numConnectedClients; i ++ )
    {
        g_entities[level.sortedClients[i]].client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<item->giTag);
    }

    // Respawn the item in all of its locations
    find = NULL;
    while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
    {
        if ( find->item != item )
        {
            continue;
        }

        G_RealSpawnGametypeItem ( find->item, find->r.currentOrigin, find->s.angles, qfalse );
    }
}

/*
===============
G_ResetGametypeEntities
===============
*/
void G_ResetGametypeEntities ( void )
{
    gentity_t* find;
    qboolean   initVisible;

    // Show total time
    initVisible = qtrue;
    trap_SetConfigstring ( CS_GAMETYPE_TIMER, "0" );

    // Reset all of the gametype items.  This must be done last because the spawn
    // function may alter enabled states of triggers or scripts
    find = NULL;
    while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
    {
        G_ResetGametypeItem ( find->item );
    }
}

/*
===============
G_RespawnClients
===============
*/
void G_RespawnClients ( qboolean force, team_t team, qboolean fullRestart )
{
    int i;

    // Respawn all clients back at a spawn pointer
    for ( i = 0; i < level.numConnectedClients; i ++ )
    {
        // Save the clients weapons
        playerState_t   ps;
        gentity_t*      ent;
        qboolean        ghost;

        ent = &g_entities[level.sortedClients[i]];

        // Make sure they are actually connected
        if ( ent->client->pers.connected != CON_CONNECTED )
        {
            continue;
        }

        // Save the entire player state so certain things
        // can be maintained across rounds
        ps = ent->client->ps;

        ghost = ent->client->sess.ghost;

        // Spawn the requested team
        if ( ent->client->sess.team != team )
        {
            continue;
        }

        // Dont force them to respawn if they arent a ghost
        if ( !ghost && !force )
        {
            continue;
        }

        // If the client is a ghost then revert them
        if ( ent->client->sess.ghost )
        {
            // Clean up any following monkey business
            G_StopFollowing ( ent );

            // Disable being a ghost
            ent->client->ps.pm_flags &= ~PMF_GHOST;
            ent->client->ps.pm_type = PM_NORMAL;
            ent->client->sess.ghost = qfalse;
        }

        ent->client->sess.noTeamChange = qfalse;

        trap_UnlinkEntity (ent);
        ClientSpawn ( ent );

        if ( fullRestart )
        {
            ent->client->sess.score = 0;
            ent->client->sess.kills = 0;
            ent->client->sess.deaths = 0;
            ent->client->sess.teamkillDamage = 0;
            ent->client->sess.teamkillForgiveTime = 0;
            ent->client->pers.enterTime = level.time;
        }
    }
}

/*
===============
G_ResetPickups
===============
*/
void G_ResetEntities ( void )
{
    int i;

    // Run through all the entities in the level and reset those which
    // need to be reset
    for ( i = 0; i < level.num_entities; i ++ )
    {
        gentity_t* ent;

        ent = &g_entities[i];

        if (isCurrentGametype(GT_HNS)) {
            if (ent->classname && strstr(ent->classname, "1fx_play_effect")) {
                G_FreeEntity(ent);
                continue;
            }
        }

        // Skip entities not in use
        if ( !ent->inuse )
        {
            continue;
        }

        // If this is a player then unlink it so then clients
        // spawned in dont telefrag
        if ( ent->s.eType == ET_PLAYER || ent->s.eType == ET_BODY )
        {
            trap_UnlinkEntity ( ent );
        }
        // If this is a missile
        else if ( ent->s.eType == ET_MISSILE )
        {
            G_FreeEntity ( ent );
        }
        // func_wall's can be toggled off/on
        else if ( ent->s.eType == ET_WALL )
        {
            if ( ent->spawnflags & 1 )
            {
                trap_UnlinkEntity ( ent );
            }
            else
            {
                trap_LinkEntity ( ent );
            }
        }
        // If the dropped flag is set then free it
        else if ( ent->flags & FL_DROPPED_ITEM )
        {
            G_FreeEntity ( ent );
        }
        // If this item is waiting to be respawned, then respawn it
        else if ( ent->think == RespawnItem )
        {
            RespawnItem ( ent );
        }
        else if ( ent->s.eType == ET_DAMAGEAREA )
        {
            G_FreeEntity ( ent );
        }
        else if ( ent->use == hurt_use )
        {
            if ( ent->spawnflags & 1 )
            {
                trap_UnlinkEntity ( ent );
            }
        }
        else if ( ent->think == target_effect_delayed_use )
        {
            ent->think = 0;
            ent->nextthink = 0;
        }
    }
}

/*
===============
G_ResetGametype
===============
*/
void G_ResetGametype ( qboolean fullRestart, qboolean cagefight )
{
    gentity_t*  tent;

    // Reset the glass in the level
    G_ResetGlass ( );

    // Reset all pickups in the world
    G_ResetEntities ( );

    if (isCurrentGametype(GT_HNS)) {
        resetCages();

        for (int i = 0; i < level.numConnectedClients; i++) {
            gentity_t* ent = &g_entities[level.sortedClients[i]];

            if (ent->client->pers.connected != CON_CONNECTED || ent->client->sess.team == TEAM_SPECTATOR) {
                continue;
            }

            ent->client->sess.transformed = qfalse;

            if (ent->client->sess.transformedEntity) {
                G_FreeEntity(&g_entities[ent->client->sess.transformedEntity]);
                ent->client->sess.transformedEntity = 0;
            }

            if (ent->client->sess.transformedEntityBBox) {
                G_FreeEntity(&g_entities[ent->client->sess.transformedEntityBBox]);
                ent->client->sess.transformedEntityBBox = 0;
            }

            ent->client->ps.eFlags &= ~EF_HSBOX;
            ent->s.eFlags &= ~EF_HSBOX;
        }
    }
    else if (isCurrentGametype(GT_CSINF)) {

        for (int i = 0; i < level.numConnectedClients; i++) {
            gentity_t* ent = &g_entities[level.sortedClients[i]];

            for (int j = 0; j < CSINF_MAX_NADES; j++) {
                ent->client->pers.csinf.boughtUtility[j] = qfalse;
            }

        }

    }

    // Reset the gametype itself
    G_ResetGametypeEntities ( );

    // Cant have a 0 roundtimelimit
    if ( g_roundtimelimit.integer < 1 )
    {
        trap_Cvar_Set ( "g_roundtimelimit", "1" );
        trap_Cvar_Update ( &g_roundtimelimit );
    }

    // Initialize the respawn interval since this is a interval gametype
    switch ( level.gametypeData->respawnType )
    {
        case RT_INTERVAL:
            level.gametypeRespawnTime[TEAM_RED]  = 0;
            level.gametypeRespawnTime[TEAM_BLUE] = 0;
            level.gametypeRespawnTime[TEAM_FREE] = 0;
            break;

        case RT_NONE:

            if (isCurrentGametype(GT_HNS)) {
                level.gametypeDelayTime = level.time + hideSeek_roundstartdelay.integer * 1000;
                if (cagefight) {
                    level.gametypeRoundTime = level.time + 60000;
                }
                else {
                    level.gametypeRoundTime = level.time + g_roundtimelimit.integer * 60000;
                }
            }
            else {
                level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 1000;
                level.gametypeRoundTime = level.time + (g_roundtimelimit.integer * 60000) + g_roundstartdelay.integer * 1000;

                if (level.gametypeDelayTime != level.time)
                {
                    trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@Get Ready", level.gametypeDelayTime));
                }

            }

            trap_SetConfigstring ( CS_GAMETYPE_TIMER, va("%i", level.gametypeRoundTime) );
            if (!isCurrentGametype(GT_HNS)) {
                evenTeams(qtrue);
            }
            break;
    }

    level.gametypeJoinTime  = 0;

    // Allow people to join 20 seconds after both teams have people on them
    if ( !level.gametypeData->teams )
    {
        level.gametypeJoinTime = level.time;
    }

    // Respawn all clients
    G_RespawnClients ( qtrue, TEAM_RED, fullRestart );
    G_RespawnClients ( qtrue, TEAM_BLUE, fullRestart );

    level.gametypeStartTime = level.time;
    level.gametypeResetTime = 0;

    if (isCurrentGametype(GT_HNS)) {
        // Henk 19/01/10 -> Reset level variables
        level.hns.cagefightdone = qfalse;
        //level.lastaliveCheck[0] = qfalse;
        //level.lastaliveCheck[1] = qfalse;
        level.customGameStarted = qfalse;
        level.customGameWeaponsDistributed = qfalse;
        level.hns.teleGunGiven = qfalse;
        level.hns.taserGiven = qfalse;
        level.hns.smokeactive = qfalse;
        level.hns.MM1Given = qfalse;
        //level.rememberSeekKills = level.SeekKills;
        //level.SeekKills = 0;
        level.hns.runMM1Flare = qfalse;
        level.hns.runRPGFlare = qfalse;
        level.hns.runM4Flare = qfalse;
        level.timelimitHit = qfalse; // allow timelimit hit message
        level.hns.secondBatchCustomWeaponsDistributed = qfalse;
        level.hns.roundOver = qfalse;
        level.hns.cagefight = qfalse;
        level.hns.RPGent = -1;
        level.hns.M4ent = -1;
        level.hns.MM1ent = -1;
        level.hns.RPGFlare = -1;
        level.hns.M4Flare = -1;
        level.hns.MM1Flare = -1;
        // Boe!Man 6/29/11: Also set the appropriate message if the weapon's been disabled.
        if (hideSeek_Weapons.string[HSWPN_M4] == '0') { // M4
            Com_sprintf(level.hns.M4loc, sizeof(level.hns.M4loc), "%s", "Disabled");
        }
        else {
            Com_sprintf(level.hns.M4loc, sizeof(level.hns.M4loc), "%s", "Not given yet");
        }
        if (hideSeek_Weapons.string[HSWPN_RPG] == '0') { // RPG
            Com_sprintf(level.hns.RPGloc, sizeof(level.hns.RPGloc), "%s", "Disabled");
        }
        else {
            Com_sprintf(level.hns.RPGloc, sizeof(level.hns.RPGloc), "%s", "Not given yet");
        }
        if (hideSeek_Weapons.string[HSWPN_MM1] == '0') { // MM1
            Com_sprintf(level.hns.MM1loc, sizeof(level.hns.MM1loc), "%s", "Disabled");
        }
        else {
            Com_sprintf(level.hns.MM1loc, sizeof(level.hns.MM1loc), "%s", "Not given yet");
        }

        /*
        if (TeamCount1(TEAM_RED) == 0) {
            level.lastalive[0] = -1;
            level.lastalive[1] = -1;
        }
        else if (TeamCount1(TEAM_RED) <= 2) {
            level.lastalive[1] = -1;
        }
        */
    }

    if ( fullRestart )
    {
        level.warmupTime = 0;
        level.startTime = level.time;
        memset ( level.teamScores, 0, sizeof(level.teamScores) );
        trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
        trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );
    }

    // Reset the clients local effects
    tent = G_TempEntity( vec3_origin, EV_GAMETYPE_RESTART );
    tent->r.svFlags |= SVF_BROADCAST;

    // Start the gametype
    trap_GT_Start ( level.gametypeStartTime );
}

/*
===============
G_ParseGametypeItems
===============
*/
qboolean G_ParseGametypeItems ( TGPGroup* itemsGroup )
{
    TGPGroup    itemGroup;
    int         itemCount;
    char        temp[MAX_QPATH];
    gentity_t   *ent;

    // Handle NULL for convienience
    if ( !itemsGroup )
    {
        return qfalse;
    }

    // Loop over all the items and add each
    itemGroup = trap_GPG_GetSubGroups ( itemsGroup );
    itemCount = 0;

    while ( itemGroup )
    {
        gitem_t*   item;

        // Parse out the pickup name
        trap_GPG_GetName ( itemGroup, temp, sizeof(temp) );

        item = BG_FindItem ( temp );
        if ( !item )
        {
            item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM + itemCount ];
            item->pickup_name = (char *)G_StringAlloc ( temp );
            itemCount++;
        }

        // Handle the entity specific stuff by finding all matching items that
        // were spawned.
        ent = NULL;
        while ( NULL != (ent = G_Find ( ent, FOFS(targetname), item->pickup_name ) ) )
        {
            // If not a gametype item then skip it
            if ( Q_stricmp ( ent->classname, "gametype_item" ) )
            {
                continue;
            }

            // Setup the gametype data
            ent->item      = item;
            ent->nextthink = level.time + 200;
            ent->think     = G_GametypeItemThink;
        }

        // Next sub group
        itemGroup = trap_GPG_GetNext(itemGroup);
    }

    return qtrue;
}

/*
===============
G_ParseGametypeFile
===============
*/
qboolean G_ParseGametypeFile ( void )
{
    TGenericParser2 GP2;
    TGPGroup        topGroup;
    TGPGroup        gametypeGroup;
    char            value[4096];

    // Default the gametype config strings
    trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, "0,0" );

    // Open the given gametype file
    GP2 = trap_GP_ParseFile ( level.gametypeData->script );
    if (!GP2)
    {
        return qfalse;
    }

    // Grab the top group and the list of sub groups
    topGroup = trap_GP_GetBaseParseGroup(GP2);
    gametypeGroup = trap_GPG_FindSubGroup(topGroup, "gametype" );
    if ( !gametypeGroup )
    {
        trap_GP_Delete(&GP2);
        return qfalse;
    }

    // Look for the respawn type
    trap_GPG_FindPairValue ( gametypeGroup, "respawn", "normal", value, sizeof(value) );
    if ( !Q_stricmp ( value, "none" ) )
    {
        level.gametypeData->respawnType = RT_NONE;
    }
    else if ( !Q_stricmp ( value, "interval" ) )
    {
        level.gametypeData->respawnType = RT_INTERVAL;
    }
    else
    {
        level.gametypeData->respawnType = RT_NORMAL;
    }

    // Grab the defined items
    G_ParseGametypeItems ( trap_GPG_FindSubGroup ( gametypeGroup, "items" ) );

    // Free up the parser
    trap_GP_Delete(&GP2);

    return qtrue;
}

/*
=================
G_EnableGametypeItemPickup

Drops all of the gametype items held by the player
=================
*/
void G_EnableGametypeItemPickup ( gentity_t* ent )
{
    ent->s.eFlags &= ~EF_NOPICKUP;
}

/*
=================
G_DropGametypeItems

Drops all of the gametype items held by the player
=================
*/
void G_DropGametypeItems ( gentity_t* self, int delayPickup )
{
    float       angle;
    int         i;
    gentity_t   *drop;
    gitem_t     *item;

    // drop all custom gametype items
    angle = 0;
    for ( i = 0 ; i < MAX_GAMETYPE_ITEMS ; i++ )
    {
        // skip this gametype item if the client doenst have it
        if ( !(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i)) )
        {
            continue;
        }

        item = BG_FindGametypeItem ( i );
        if ( !item )
        {
            continue;
        }

        drop = G_DropItem( self, item, angle );
        drop->count = 1;
        angle += 45;

        if ( delayPickup )
        {
            drop->nextthink = level.time + delayPickup;
            drop->s.eFlags |= EF_NOPICKUP;
            drop->think = G_EnableGametypeItemPickup;
        }

        // TAke it away from the client just in case
        self->client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<i);

        if ( self->enemy && self->enemy->client && !OnSameTeam ( self->enemy, self ) )
        {
            trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, level.gametypeItems[item->giTag].id, self->enemy->s.clientNum, self->enemy->client->sess.team, 0, 0  );
            self->enemy->client->pers.statInfo.itemDefends++;

        }
    }

    self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
}

/*
=================
CheckGametype
=================
*/
void CheckGametype ( void )
{

    if (isCurrentGametype(GT_HNS) && level.hns.startCage) {
        initCageFight();
    }

    // If the level is over then forget checking gametype stuff.
    if ( level.intermissiontime || level.paused )
    {
        return;
    }

    // Dont bother starting the gametype until
    // the first person joins
    if ( !level.gametypeStartTime )
    {
        int i;

        memset ( level.teamAliveCount, 0, sizeof(level.teamAliveCount) );

        for ( i = 0; i < level.numConnectedClients; i ++ )
        {
            gentity_t* other = &g_entities[level.sortedClients[i]];

            if ( other->client->pers.connected != CON_CONNECTED )
            {
                continue;
            }

            if ( other->client->pers.connected != CON_CONNECTED )
            {
                continue;
            }

            level.teamAliveCount[other->client->sess.team]++;
        }

        if ( level.teamAliveCount[TEAM_RED] || level.teamAliveCount[TEAM_BLUE] || level.teamAliveCount[TEAM_FREE] )
        {
            G_ResetGametype ( qfalse, qfalse );
            return;
        }
    }

    // Check for delayed gametype reset
    if ( level.gametypeResetTime )
    {
        if ( level.time > level.gametypeResetTime )
        {
            // Dont do this again
            level.gametypeResetTime = 0;

            G_ResetGametype ( qfalse, qfalse );
        }

        return;
    }

    // Handle respawn interval spawning
    if ( level.gametypeData->respawnType == RT_INTERVAL )
    {
        team_t team;
        qboolean autoETDone = qfalse;
        for ( team = TEAM_RED; team < TEAM_SPECTATOR; team ++ )
        {
            if ( level.gametypeRespawnTime[team] && level.time > level.gametypeRespawnTime[team] )
            {
                if (!autoETDone && g_autoEvenTeams.integer) {
                    evenTeams(qtrue);
                }
                autoETDone = qtrue;
                // Respawn all dead clients
                G_RespawnClients ( qfalse, team, qfalse );

                // Next interval
                level.gametypeRespawnTime[team] = 0;
            }
        }
    }

    // If we are in RT_NONE respawn mode then we need to look for everyone being dead
    if ( level.gametypeData->respawnType == RT_NONE && level.gametypeStartTime )
    {
        int i;
        int dead[TEAM_NUM_TEAMS];
        int players[TEAM_NUM_TEAMS];
        int lastalive[TEAM_NUM_TEAMS];

        memset ( &level.teamAliveCount[0], 0, sizeof(level.teamAliveCount) );
        memset ( &dead[0], 0, sizeof(dead) );
        memset(players, 0, sizeof(players));
        for ( i = 0; i < level.numConnectedClients; i ++ )
        {
            gentity_t* ent = &g_entities[level.sortedClients[i]];

            if ( ent->client->pers.connected != CON_CONNECTED )
            {
                continue;
            }

            players[ent->client->sess.team]++;

            if ( G_IsClientDead ( ent->client ) )
            {
                dead[ent->client->sess.team] ++;
            }
            else
            {
                level.teamAliveCount[ent->client->sess.team] ++;
                lastalive[ent->client->sess.team] = ent->s.number;
            }
        }

        for (int i = TEAM_RED; i <= TEAM_BLUE; i++) {
            if (level.teamAliveCount[i] == 1 && !level.teamLastAliveSent[i] && players[i] > 1 && !isCurrentGametype(GT_HNS)) {
               
                gentity_t* tent = &g_entities[level.sortedClients[lastalive[i]]];

                if (tent && tent->client) {
                    level.teamLastAliveSent[i] = qtrue;
                    G_Broadcast(BROADCAST_GAME, tent, qfalse, "You are the \\last player alive!");
                    G_printInfoMessageToAll("%s is the last player alive in the %s team.", tent->client->pers.cleanName, i == TEAM_RED ? "Red" : "Blue");
                    G_ClientSound(tent, G_SoundIndex("sound/misc/events/tut_door01.mp3"));
                }
            }
        }

        if (!level.teamAliveCount[TEAM_RED] && (
                (
                    dead[TEAM_RED] &&
                    !isCurrentGametype(GT_HNZ) &&
                    (!isCurrentGametype(GT_HNS) || level.customGameStarted)
                    ) ||
                (
                    isCurrentGametype(GT_HNZ) &&
                    players[TEAM_BLUE] > 1
                    )
                )
            ) {

            qboolean alreadyHit = qfalse;
            if (level.timelimitHit && !level.hns.cagefight) {

                if (isCurrentGametype(GT_HNS)) {
                    fillHnsStats();
                    int redPlayers = TeamCount(-1, TEAM_RED, NULL);
                    if (redPlayers < 2 || !level.hns.cageFightLoaded) {
                        gentity_t* tent = G_TempEntity(vec3_origin, EV_GAME_OVER);
                        tent->s.eventParm = GAME_OVER_TIMELIMIT;
                        tent->r.svFlags = SVF_BROADCAST;
                        level.timelimitHit = qfalse;

                        //UpdateScores(); // JANFIXME H&S scoring table?
                        trap_GT_SendEvent(GTEV_TEAM_ELIMINATED, level.time, TEAM_RED, 0, 0, 0, 0); // Boe!Man 9/6/11: Add this here to prevent the gametype not being properly ended (when timelimit's hit).
                        alreadyHit = qtrue;
                        LogExit("Timelimit hit.");
                    }
                    else {
                        G_printGametypeMessageToAll("Prepare for cage round...");
                        level.hns.cagefightTimer = level.time + 3000;
                        level.hns.startCage = qtrue;
                    }
                }
                else {
                    gentity_t* tent = G_TempEntity(vec3_origin, EV_GAME_OVER);
                    if (cm_state.integer > 0) { // Boe!Man 3/18/11: Only change the entry if competition mode's enabled.
                        tent->s.eventParm = LEEG;
                    }
                    else {
                        tent->s.eventParm = GAME_OVER_TIMELIMIT;
                    }
                    tent->r.svFlags = SVF_BROADCAST;

                    notifyPlayersOfTeamScores();
                    LogExit("Timelimit hit.");
                }
            }

            // Boe!Man 8/1/16: Only do this if we haven't done so already.
            if (!alreadyHit) {
                trap_GT_SendEvent(GTEV_TEAM_ELIMINATED, level.time, TEAM_RED, 0, 0, 0, 0);
            }
        } else if (!level.teamAliveCount[TEAM_BLUE] && dead[TEAM_BLUE] && !isCurrentGametype(GT_HNS)) {
            if (level.timelimitHit) {
                gentity_t* tent;
                tent = G_TempEntity(vec3_origin, EV_GAME_OVER);
                tent->s.eventParm = GAME_OVER_TIMELIMIT;
                tent->r.svFlags = SVF_BROADCAST;
                level.timelimitHit = qfalse;
                LogExit("Timelimit hit.");
            }

            trap_GT_SendEvent(GTEV_TEAM_ELIMINATED, level.time, TEAM_BLUE, 0, 0, 0, 0);
        } else if (level.hns.cagefight && level.teamAliveCount[TEAM_RED] == 1) {
            for (i = 0; i < level.numConnectedClients; i++) {
                if (!G_IsClientDead(g_entities[level.sortedClients[i]].client) && g_entities[level.sortedClients[i]].client->sess.team == TEAM_RED) {
                    g_entities[level.sortedClients[i]].client->sess.kills += 1; // round winner should get 1 point more.
                    G_AddScore(&g_entities[level.sortedClients[i]], 100);
                    Q_strncpyz(level.hns.cagewinner, &g_entities[level.sortedClients[i]].client->pers.netname, sizeof(level.hns.cagewinner));
                }
            }

            G_printGametypeMessageToAll("Fight ended.");
            G_Broadcast(BROADCAST_GAME, NULL, qtrue, "\\Fight ended!");

            //UpdateScores(); // JANFIXME H&S scores
            level.timelimitHit = qfalse;
            level.hns.cagefight = qfalse;
            level.hns.cagefightdone = qtrue;

            // Reset the game type timer.
            trap_SetConfigstring(CS_GAMETYPE_TIMER, va("%d", level.time));

            LogExit("Cagefight is done.");
            return;
        }
        // See if the time has expired
        if ( level.time > level.gametypeRoundTime )
        {

            if (level.hns.cagefight && isCurrentGametype(GT_HNS)) {
                for (i = 0; i < level.numConnectedClients; i++) {
                    if (!G_IsClientDead(g_entities[level.sortedClients[i]].client) && g_entities[level.sortedClients[i]].client->sess.team == TEAM_RED) {
                        g_entities[level.sortedClients[i]].client->sess.kills += 1; // round winner should get 1 point more.
                        G_AddScore(&g_entities[level.sortedClients[i]], 100);
                    }
                }
                trap_SendServerCommand(-1, va("print \"^3[H&S] ^7Fight ended.\n\""));
                G_printGametypeMessageToAll("Fight ended.");
                G_Broadcast(BROADCAST_GAME, NULL, qtrue, "\\Fight ended.");

                //UpdateScores(); // JANFIXME H&S scores
                level.timelimitHit = qfalse;
                level.hns.cagefight = qfalse;
                LogExit("Timelimit hit.");
                return;
            }

            if (isCurrentGametype(GT_HNS)) {
                stripTeam(TEAM_RED, qtrue);

                // For the alive players, we set the dead timers.
                // This ensures they will end up with weapons as well.

                for (i = 0; i < level.numConnectedClients; i++) {
                    gentity_t* tent = &g_entities[level.sortedClients[i]];

                    if (!G_IsClientDead(tent->client) && tent->client->sess.team == TEAM_RED) {
                        tent->client->sess.hsTimeOfDeath = level.time;
                        tent->client->sess.roundsWonAsHider++;
                        tent->client->sess.kills++;
                        tent->client->sess.score++;
                    }
                }

                level.hns.roundOver = qtrue; // This will limit abusing /kill after round end.

            }
            else if (isCurrentGametype(GT_HNZ)) {
                // Add 5 points for each human that's still alive.
                for (i = 0; i < level.numConnectedClients; i++) {
                    if (g_entities[level.sortedClients[i]].client->sess.team == TEAM_RED && !G_IsClientDead(g_entities[level.sortedClients[i]].client)) {
                        g_entities[level.sortedClients[i]].client->sess.score += 5;
                        g_entities[level.sortedClients[i]].client->sess.kills += 5;
                    }
                }
            }

            trap_GT_SendEvent ( GTEV_TIME_EXPIRED, level.time, 0, 0, 0, 0, 0 );

            if (level.timelimitHit) {

                gentity_t* tent = G_TempEntity(vec3_origin, EV_GAME_OVER);

                if (cm_state.integer > 0) { // Boe!Man 3/18/11: Only change the entry if competition mode's enabled.
                    tent->s.eventParm = LEEG;
                }
                else {
                    tent->s.eventParm = GAME_OVER_TIMELIMIT;
                }
                tent->r.svFlags = SVF_BROADCAST;

                if (isCurrentGametype(GT_HNS)) {
                    fillHnsStats();
                    int redPlayers = TeamCount(-1, TEAM_RED, NULL);
                    if (redPlayers < 2 || !level.hns.cageFightLoaded) {
                        //UpdateScores(); // JANFIXME H&S SCORES
                        level.timelimitHit = qfalse;
                        LogExit("Timelimit hit.");
                    }
                    else {
                        G_printGametypeMessageToAll("Prepare for cage round...");
                        level.hns.cagefightTimer = level.time + 3000;
                        level.hns.startCage = qtrue;
                    }
                }
                else {
                    notifyPlayersOfTeamScores();
                    LogExit("Timelimit hit.");
                }
            }
        }
    }
}

/*
=================
G_GametypeCommand

Handles a command coming from the current gametype VM
=================
*/

intptr_t G_GametypeCommand(int command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4)
{
    switch ( command )
    {
        case GT_RESTART:
            if ( arg0 <= 0 )
            {
                G_ResetGametype ( qfalse, qfalse );
            }
            else
            {
                level.gametypeResetTime = level.time + arg0 * 1000;
            }
            break;

        case GT_TEXTMESSAGE:
            trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, (const char*)G_ColorizeMessage(arg1) ) );
            break;

        case GT_CONSOLETEXTMESSAGE:
            trap_SendServerCommand(arg0, va("print \"^3[%s]^7 %s\n\"", arg1, arg2));
            break;

        case GT_BROADCAST: {

            gentity_t* ent = NULL;
            if (arg0 >= 0 && arg0 < MAX_CLIENTS) {
                ent = &g_entities[arg0];

                if (!ent || !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED) {
                    ent = NULL;
                }
            }

            G_Broadcast(BROADCAST_GAME, ent, (qboolean)arg2, (const char*) arg1);
            break;
        }
        case GT_RADIOMESSAGE:
            G_Voice ( &g_entities[arg0], NULL, SAY_TEAM, (const char*) arg1, qfalse );
            break;

        case GT_REGISTERSOUND:
            return G_SoundIndex ( (char*) arg0 );

        case GT_STARTGLOBALSOUND:
        {
            gentity_t* tent;
            tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
            tent->s.eventParm = arg0;
            tent->r.svFlags = SVF_BROADCAST;
            break;
        }

        case GT_STARTSOUND:
            G_SoundAtLoc ( (float*)arg1, CHAN_AUTO, arg0 );
            break;

        case GT_REGISTEREFFECT:
            return G_EffectIndex ( (char*) arg0 );

        case GT_REGISTERICON:
            return G_IconIndex ( (char*) arg0 );

        case GT_SETHUDICON:
            G_SetHUDIcon ( arg0, arg1 );
            break;

        case GT_PLAYEFFECT:
            G_PlayEffect ( arg0, (float*) arg1, (float*) arg2 );
            break;

        case GT_ADDCLIENTSCORE:
            G_AddScore ( &g_entities[arg0], arg1 );
            break;

        case GT_ADDTEAMSCORE:
            G_AddTeamScore ( (team_t) arg0, arg1 );
            break;

        case GT_RESETITEM:
        {
            gitem_t* item;

            item = BG_FindGametypeItemByID ( arg0 );
            if ( item )
            {
                G_ResetGametypeItem ( item );
                return qtrue;
            }
            break;
        }

        case GT_GETCLIENTNAME:
            Com_sprintf ( (char*) arg1, arg2, "%s", arg3 ? g_entities[arg0].client->pers.cleanName : g_entities[arg0].client->pers.netname );
            break;

        case GT_GETTRIGGERTARGET:
        {
            gentity_t   *find;

            Com_sprintf ( (char*) arg1, arg2, "" );

            find = NULL;
            while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
            {
                if ( find->health == arg0 )
                {
                    Com_sprintf ( (char*) arg1, arg2, "%s", find->target );
                    break;
                }
            }

            break;
        }

        case GT_GETCLIENTORIGIN:
            VectorCopy ( g_entities[arg0].client->ps.origin, (float*) arg1 );
            break;

        case GT_GIVECLIENTITEM:
        {
            gitem_t* item;

            item = BG_FindGametypeItemByID ( arg1 );
            if ( item )
            {
                level.clients[arg0].ps.stats[STAT_GAMETYPE_ITEMS] |= (1<<item->giTag);
            }
            break;
        }

        case GT_GETCLIENTLIST:
        {
            int  i;
            int  count;
            int *clients = (int*)arg1;

            for ( i = 0, count = 0; i < level.numConnectedClients && count < arg2; i ++ )
            {
                gclient_t* client = &level.clients[level.sortedClients[i]];

                if ( client->pers.connected != CON_CONNECTED )
                {
                    continue;
                }

                if ( client->sess.team == arg0 )
                {
                    *clients = level.sortedClients[i];
                    clients++;
                    count++;
                }
            }

            return count;
        }

        case GT_TAKECLIENTITEM:
        {
            gitem_t* item;

            item = BG_FindGametypeItemByID ( arg1 );
            if ( item )
            {
                level.clients[arg0].ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<item->giTag);
            }
            break;
        }

        case GT_SPAWNITEM:
        {
            gitem_t* item;

            item = BG_FindGametypeItemByID ( arg0 );
            if ( item )
            {
                gentity_t* ent = LaunchItem ( item, (float*)arg1, vec3_origin );
                if ( ent )
                {
                    VectorCopy ( (float*)arg2, ent->s.angles );
                }
            }
            break;
        }

        case GT_DOESCLIENTHAVEITEM:
        {
            gitem_t*    item;
            gentity_t*  ent;

            ent  = &g_entities[arg0];
            item = BG_FindGametypeItemByID ( arg1 );

            if ( item )
            {
                if ( ent->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<item->giTag) )
                {
                    return 1;
                }
            }

            return 0;
        }

        case GT_REGISTERITEM:
        {
            gitem_t     *item;
            gtItemDef_t *def;

            def = (gtItemDef_t*)arg2;

            item = BG_FindItem ( (const char*) arg1 );
            if ( item )
            {
                gtitem_t *gtitem;

                gtitem = &level.gametypeItems[item->giTag];

                gtitem->id = arg0;
                item->quantity = arg0;

                // See if the trigger needs to be used.
                if ( def && def->size == sizeof(gtItemDef_t) && def->use )
                {
                    gtitem->useIcon  = def->useIcon;
                    gtitem->useTime  = def->useTime;
                    gtitem->useSound = def->useSound;
                }

                return qtrue;
            }

            return qfalse;
        }

        case GT_REGISTERTRIGGER:
        {
            gentity_t       *find;
            gtTriggerDef_t  *def;

            def = (gtTriggerDef_t*)arg2;

            find = NULL;
            while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
            {
                if ( Q_stricmp ( find->targetname, (const char*) arg1 ) )
                {
                    continue;
                }

                // Assign the id to it.
                find->health = arg0;
                find->touch  = gametype_trigger_touch;

                // See if the trigger needs to be used.
                if ( def && def->size == sizeof(gtTriggerDef_t) && def->use )
                {
                    find->use       = gametype_trigger_use;
                    find->delay     = def->useIcon;
                    find->soundPos1 = def->useTime;
                    find->soundLoop = def->useSound;
                }

                trap_LinkEntity (find);
            }

            return 0;
        }

        case GT_USETARGETS:
            G_UseTargetsByName ( (const char*) arg0, NULL, NULL );
            break;

        case GT_REPORT_TEAMNAMES:
            writeGametypeTeamNames((const char*)arg0, (const char*)arg1);
            break;

        case GT_CSINF_ADDCASHTOCLIENT:

            if (arg0 < 0 || arg0 >= MAX_CLIENTS) {
                logSystem(LOGLEVEL_FATAL, "GT_CSINF_ADDCASHTOCLIENT arg0 = %d", arg0);
            }

            gentity_t* ent = &g_entities[arg0];

            if (ent && ent->client) {
                csinf_handleCash(ent, arg1, arg2, qtrue);
            }

            break;

        case GT_CSINF_ADDCASHTOTEAM:

            for (int i = 0; i < level.numConnectedClients; i++) {
                gentity_t* ent = &g_entities[level.sortedClients[i]];

                if (ent->client->sess.team == arg0) {
                    if ((arg3 && !G_IsClientDead(ent)) || (arg4 && G_IsClientDead(ent))) { // arg3 = toAlivePlayers, arg4 = toDeadPlayers
                        csinf_handleCash(ent, arg1, arg2, qtrue);
                    }
                }
            }

            break;

        default:
            break;
    }

    return -1;
}


/*QUAKED monkey_player (0 1 0) (-16 -16 -46) (16 16 48)
Potential spawning position for dead monkey players in the Hide&Seek gametype.
*/
void SP_monkey_player(gentity_t* ent)
{
    // Cant take any more spawns!!
    if (level.hns.monkeySpawnCount >= MAX_SPAWNS)
    {
        G_FreeEntity(ent);
        return;
    }

    //G_AddClientSpawn(ent, (team_t)TEAM_RED, qtrue);
    G_FreeEntity(ent);
}

void SP_hideseek_cageplayer(gentity_t* ent)
{
    trap_UnlinkEntity(ent);
}