// Copyright (C) 2001-2002 Raven Software
//
// g_active.c --

#include "g_local.h"


void P_SetTwitchInfo(gclient_t  *client)
{
    client->ps.painTime = level.time;
    client->ps.painDirection ^= 1;
}

/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
void P_DamageFeedback( gentity_t *player )
{
    gclient_t   *client;
    float       count;
    vec3_t      angles;

    client = player->client;
    if ( client->ps.pm_type == PM_DEAD )
    {
        return;
    }

    // total points of damage shot at the player this frame
    count = client->damage_blood + client->damage_armor;
    if ( count == 0 )
    {
        // didn't take any damage
        return;
    }

    if ( count > 255 )
    {
        count = 255;
    }

    // send the information to the client

    // world damage (falling, slime, etc) uses a special code
    // to make the blend blob centered instead of positional
    if ( client->damage_fromWorld )
    {
        client->ps.damagePitch = 255;
        client->ps.damageYaw = 255;

        client->damage_fromWorld = qfalse;
    }
    else
    {
        vectoangles( client->damage_from, angles );
        client->ps.damagePitch = angles[PITCH]/360.0 * 255;
        client->ps.damageYaw = angles[YAW]/360.0 * 255;
    }

    // play an apropriate pain sound
    if ( (level.time > player->pain_debounce_time))
    {
        // don't do more than two pain sounds a second
        if ( level.time - client->ps.painTime < 500 )
        {
            return;
        }

        P_SetTwitchInfo(client);
        player->pain_debounce_time = level.time + 700;
        G_AddEvent( player, EV_PAIN, player->health );
        client->ps.damageEvent++;
    }


    client->ps.damageCount = count;

    // clear totals
    client->damage_blood = 0;
    client->damage_armor = 0;
    client->damage_knockback = 0;
}

/*
=============
P_WorldEffects

Check for drowning
=============
*/
void P_WorldEffects( gentity_t *ent )
{
    int  waterlevel;

    if ( ent->client->noclip )
    {
        // don't need air
        ent->client->airOutTime = level.time + 12000;
        return;
    }

    if (level.autoSectionActive[MAPSECTION_NOLOWER] && level.noSectionEntFound[MAPSECTION_NOLOWER]) { // if enabled -- Boe!Man 6/2/12: Also check for nolower2. This is qtrue when the entity was found.
        if (ent->r.currentOrigin[2] <= level.noLR[0][2] && !G_IsClientDead(ent->client)) {
            G_printInfoMessageToAll("%s was killed for being lower.", ent->client->pers.cleanName);

            // Make sure godmode isn't an issue with being lower.
            if (ent->flags & FL_GODMODE) {
                ent->flags ^= FL_GODMODE;
            }
            G_Damage(ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_TRIGGER_HURT, 0);
        }
    }

    // Boe!Man 6/3/12: Check for roof. This is best done in a seperate function.
    checkRoof(ent);

    waterlevel = ent->waterlevel;

    if (isCurrentGametype(GT_HNS) && waterlevel >= 1) {
        ent->client->sess.speedDecrement.speedAlterationFrom = level.time;
        ent->client->sess.speedDecrement.speedAlterationReason = SPEEDALTERATION_WATER;
        ent->client->sess.speedDecrement.speedAlterationDuration = g_waterSpeedTime.integer;
        ent->client->sess.speedDecrement.speedAlterationTo = level.time + g_waterSpeedTime.integer;
    }

    // check for drowning
    if ( waterlevel == 3 && (ent->watertype & CONTENTS_WATER))
    {
        // if out of air, start drowning
        if ( ent->client->airOutTime < level.time)
        {
            // drown!
            ent->client->airOutTime += 1000;
            if ( ent->health > 0 )
            {
                // take more damage the longer underwater
                ent->damage += 2;
                if (ent->damage > 15)
                {
                    ent->damage = 15;
                }

                // play a gurp sound instead of a normal pain sound
                if (ent->health <= ent->damage)
                {
//                  G_Sound(ent, CHAN_VOICE, G_SoundIndex("sound/pain_death/mullins/drown_dead.wav"));
                }
                else
                {
                    G_AddEvent ( ent, EV_PAIN_WATER, 0 );
                }

                // don't play a normal pain sound
                ent->pain_debounce_time = level.time + 200;

                G_Damage (ent, NULL, NULL, NULL, NULL, ent->damage, DAMAGE_NO_ARMOR, MOD_WATER, HL_NONE );
            }
        }
    }
    else
    {
        ent->client->airOutTime = level.time + 12000;
        ent->damage = 2;
    }
}

/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent )
{
//  ent->client->ps.loopSound = 0;
}

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm ) {
    int     i, j;
    trace_t trace;
    gentity_t   *other;

    memset( &trace, 0, sizeof( trace ) );
    for (i=0 ; i<pm->numtouch ; i++) {
        for (j=0 ; j<i ; j++) {
            if (pm->touchents[j] == pm->touchents[i] ) {
                break;
            }
        }
        if (j != i) {
            continue;   // duplicated
        }
        other = &g_entities[ pm->touchents[i] ];

        if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
            ent->touch( ent, other, &trace );
        }

        if ( !other->touch ) {
            continue;
        }

        other->touch( other, ent, &trace );
    }

}

/*
============
G_IsClientSiameseTwin

Checks to see if the two clients should never have been separated at birth
============
*/
static qboolean G_IsClientSiameseTwin ( gentity_t* ent, gentity_t* ent2 )
{
    if ( G_IsClientSpectating ( ent->client ) || G_IsClientDead ( ent->client ) )
    {
        return qfalse;
    }

    if ( G_IsClientSpectating ( ent2->client ) || G_IsClientDead ( ent2->client ) )
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[0] + ent2->r.mins[0] > ent->r.currentOrigin[0] + ent->r.maxs[0])
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[1] + ent2->r.mins[1] > ent->r.currentOrigin[1] + ent->r.maxs[1])
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[2] + ent2->r.mins[2] > ent->r.currentOrigin[2] + ent->r.maxs[2])
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[0] + ent2->r.maxs[0] < ent->r.currentOrigin[0] + ent->r.mins[0])
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[1] + ent2->r.maxs[1] < ent->r.currentOrigin[1] + ent->r.mins[1])
    {
        return qfalse;
    }

    if (ent2->r.currentOrigin[2] + ent2->r.maxs[2] < ent->r.currentOrigin[2] + ent->r.mins[2])
    {
        return qfalse;
    }

    return qtrue;
}

/*
============
G_TouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void G_TouchTriggers( gentity_t *ent )
{
    int             i;
    int             num;
    int             touch[MAX_GENTITIES];
    gentity_t       *hit;
    trace_t         trace;
    vec3_t          mins;
    vec3_t          maxs;
    static vec3_t   range = { 20, 20, 40 };

    if ( !ent->client )
    {
        return;
    }

    // dead clients don't activate triggers!
    if ( G_IsClientDead ( ent->client ) )
    {
        return;
    }

    VectorSubtract( ent->client->ps.origin, range, mins );
    VectorAdd( ent->client->ps.origin, range, maxs );

    num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

    // can't use ent->r.absmin, because that has a one unit pad
    VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
    VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

    // Reset the players can use flag
    ent->client->ps.pm_flags &= ~(PMF_CAN_USE);
    ent->client->useEntity = 0;
    ent->client->ps.loopSound = 0;
    ent->s.modelindex  = 0;

    for ( i=0 ; i<num ; i++ )
    {
        hit = &g_entities[touch[i]];

        // pmove would have to have detected siamese twins first
        if ( hit->client && hit != ent && !hit->client->siameseTwin && (ent->client->ps.pm_flags & PMF_SIAMESETWINS) )
        {
            // See if this client has a twin
            if ( !G_IsClientSiameseTwin ( ent, hit ) )
            {
                continue;
            }

            // About time these twins were separated!!
            ent->client->siameseTwin = hit;
            hit->client->siameseTwin = ent;
        }

        if ( !( hit->r.contents & CONTENTS_TRIGGER ) )
        {
            continue;
        }

        // Look for usable gametype triggers and you cant use when zoomed
        if ( !(ent->client->ps.pm_flags & PMF_ZOOMED ) )
        {
            switch ( hit->s.eType )
            {
                case ET_GAMETYPE_TRIGGER:
                    if ( hit->use && trap_GT_SendEvent ( GTEV_TRIGGER_CANBEUSED, level.time, hit->health, ent->s.number, ent->client->sess.team, 0, 0 ) )
                    {
                        ent->client->ps.pm_flags |= PMF_CAN_USE;
                        ent->client->ps.stats[STAT_USEICON] = hit->delay;
                        ent->client->ps.stats[STAT_USETIME_MAX] = hit->soundPos1;

                        if ( ent->client->ps.stats[STAT_USETIME] )
                        {
                            ent->client->ps.loopSound = hit->soundLoop;
                        }
                        ent->client->useEntity = hit;
                        continue;
                    }
                    break;

                case ET_ITEM:
                    if ( hit->item->giType == IT_GAMETYPE && trap_GT_SendEvent ( GTEV_ITEM_CANBEUSED, level.time, hit->item->quantity, ent->s.number, ent->client->sess.team, 0, 0 ) )
                    {
                        ent->client->ps.pm_flags |= PMF_CAN_USE;
                        ent->client->ps.stats[STAT_USEICON] = level.gametypeItems[hit->item->giTag].useIcon;
                        ent->client->ps.stats[STAT_USETIME_MAX] = level.gametypeItems[hit->item->giTag].useTime;

                        if ( ent->client->ps.stats[STAT_USETIME] )
                        {
                            ent->client->ps.loopSound = level.gametypeItems[hit->item->giTag].useSound;
                        }
                        ent->client->useEntity = hit;
                        continue;
                    }
                    break;
            }
        }

        if ( !hit->touch && !ent->touch )
        {
            continue;
        }

        // ignore most entities if a spectator
        if ( G_IsClientSpectating ( ent->client ) )
        {
            if ( hit->s.eType != ET_TELEPORT_TRIGGER &&
                // this is ugly but adding a new ET_? type will
                // most likely cause network incompatibilities
                hit->touch != Touch_DoorTrigger)
            {
                continue;
            }
        }

        // use seperate code for determining if an item is picked up
        // so you don't have to actually contact its bounding box
        if ( hit->s.eType == ET_ITEM )
        {
            if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) )
            {
                continue;
            }
        }
        else
        {
            if ( !trap_EntityContact( mins, maxs, hit ) )
            {
                continue;
            }
        }

        memset( &trace, 0, sizeof(trace) );

        if ( hit->touch )
        {
            hit->touch (hit, ent, &trace);
        }

        if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) )
        {
            ent->touch( ent, hit, &trace );
        }
    }

    // Dont bother looking for twins again unless pmove says so
    ent->client->ps.pm_flags &= (~PMF_SIAMESETWINS);
}


/*
============
G_MoverTouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void G_MoverTouchPushTriggers( gentity_t *ent, vec3_t oldOrg )
{
    int         i, num;
    float       step, stepSize, dist;
    int         touch[MAX_GENTITIES];
    gentity_t   *hit;
    trace_t     trace;
    vec3_t      mins, maxs, dir, size, checkSpot;
    const vec3_t    range = { 40, 40, 52 };

    // non-moving movers don't hit triggers!
    if ( !VectorLengthSquared( ent->s.pos.trDelta ) )
    {
        return;
    }

    VectorSubtract( ent->r.mins, ent->r.maxs, size );
    stepSize = VectorLength( size );
    if ( stepSize < 1 )
    {
        stepSize = 1;
    }

    VectorSubtract( ent->r.currentOrigin, oldOrg, dir );
    dist = VectorNormalize( dir );
    for ( step = 0; step <= dist; step += stepSize )
    {
        VectorMA( ent->r.currentOrigin, step, dir, checkSpot );
        VectorSubtract( checkSpot, range, mins );
        VectorAdd( checkSpot, range, maxs );

        num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

        // can't use ent->r.absmin, because that has a one unit pad
        VectorAdd( checkSpot, ent->r.mins, mins );
        VectorAdd( checkSpot, ent->r.maxs, maxs );

        for ( i=0 ; i<num ; i++ )
        {
            hit = &g_entities[touch[i]];

            if ( hit->s.eType != ET_PUSH_TRIGGER )
            {
                continue;
            }

            if ( hit->touch == NULL )
            {
                continue;
            }

            if ( !( hit->r.contents & CONTENTS_TRIGGER ) )
            {
                continue;
            }


            if ( !trap_EntityContact( mins, maxs, hit ) )
            {
                continue;
            }

            memset( &trace, 0, sizeof(trace) );

            if ( hit->touch != NULL )
            {
                hit->touch(hit, ent, &trace);
            }
        }
    }
}

/*
=================
G_UpdatePlayerStateScores

Update the scores in the playerstate
=================
*/
void G_UpdatePlayerStateScores ( gentity_t* ent )
{
    // set the CS_SCORES1/2 configstrings, which will be visible to everyone
    if ( level.gametypeData->teams )
    {
        ent->client->ps.persistant[PERS_RED_SCORE] = level.teamScores[TEAM_RED];
        ent->client->ps.persistant[PERS_BLUE_SCORE] = level.teamScores[TEAM_BLUE];

        ent->client->ps.persistant[PERS_BLUE_ALIVE_COUNT] = level.teamAliveCount[TEAM_BLUE];
        ent->client->ps.persistant[PERS_RED_ALIVE_COUNT] = level.teamAliveCount[TEAM_RED];
    }
    else
    {
        if ( level.numConnectedClients == 0 )
        {
            ent->client->ps.persistant[PERS_RED_SCORE] = 0;
            ent->client->ps.persistant[PERS_BLUE_SCORE] = 0;
        }
        else if ( level.numConnectedClients == 1 )
        {
            ent->client->ps.persistant[PERS_RED_SCORE] = level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE];
            ent->client->ps.persistant[PERS_BLUE_SCORE] = 0;
        }
        else
        {
            ent->client->ps.persistant[PERS_RED_SCORE] = level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE];
            ent->client->ps.persistant[PERS_BLUE_SCORE] = level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE];
        }
    }
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd )
{
    pmove_t     pm;
    gclient_t   *client;

    client = ent->client;

    if ( client->sess.spectatorState != SPECTATOR_FOLLOW && (cm_state.integer == COMPMODE_NONE || (cm_state.integer > COMPMODE_NONE && g_followEnemy.integer)) )
    {
        client->ps.pm_type = PM_SPECTATOR;
        client->ps.speed = 400; // faster than normal
        client->ps.loopSound = 0;

        if (level.specLocked) {
            SetClientViewAngle(ent, (vec3_t) { 90.0, 0.0, 0.0 });
            client->ps.pm_type = PM_FREEZE;
            
        }

        // set up for pmove
        memset (&pm, 0, sizeof(pm));
        pm.ps = &client->ps;
        pm.cmd = *ucmd;
        pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;   // spectators can fly through bodies
        pm.trace = trap_Trace;
        pm.pointcontents = trap_PointContents;

        pm.animations = NULL;

        pm.legacyProtocol = client->sess.legacyProtocol;

        // perform a pmove
        Pmove (&pm);

        G_UpdatePlayerStateScores ( ent );

        // save results of pmove
        VectorCopy( client->ps.origin, ent->s.origin );

        G_TouchTriggers( ent );
        trap_UnlinkEntity( ent );
    } else if (level.specLocked) {
        G_StopFollowing(ent);
        return;
    }

    if (cm_state.integer > COMPMODE_NONE && !g_followEnemy.integer && client->sess.spectatorState != SPECTATOR_FOLLOW) {
        // We force the client to follow a player.
        Cmd_FollowCycle_f(ent, 1);
    }

    client->oldbuttons = client->buttons;
    client->buttons = ucmd->buttons;

    // attack button cycles through spectators
    if ( client->sess.spectatorState != SPECTATOR_FOLLOW && g_forceFollow.integer )
    {
        if ( g_forceFollow.integer > 1 )
        {
            client->sess.spectatorFirstPerson = qtrue;
        }

        Cmd_FollowCycle_f( ent, 1 );
    }
    if ( ( client->buttons & BUTTON_ATTACK ) && ! ( client->oldbuttons & BUTTON_ATTACK ) )
    {
        Cmd_FollowCycle_f( ent, 1 );
    }
    else if ( ( client->buttons & BUTTON_ALT_ATTACK ) && ! ( client->oldbuttons & BUTTON_ALT_ATTACK ) )
    {
        Cmd_FollowCycle_f( ent, -1 );
    }
    else if ( !g_forceFollow.integer && ucmd->upmove > 0 && (client->ps.pm_flags & PMF_FOLLOW) )
    {
        G_StopFollowing( ent );
    }
    else if ( (client->buttons & BUTTON_USE) && !( client->oldbuttons & BUTTON_USE ) )
    {
        // If not following then go to either third or first
        if ( client->sess.spectatorState != SPECTATOR_FOLLOW )
        {
            client->sess.spectatorFirstPerson = g_forceFollow.integer < 2 ? qfalse : qtrue;
            Cmd_FollowCycle_f( ent, -1 );
        }
        // If in first person then either go to free float or third person
        else if ( client->sess.spectatorFirstPerson )
        {
            if ( g_forceFollow.integer < 2 )
            {
                client->sess.spectatorFirstPerson = qfalse;
            }
        }
        // Must be in third person so just go to first
        else
        {
            client->sess.spectatorFirstPerson = qtrue;
        }
    }
}

/*
=================
ClientInactivityTimer

Returns qfalse if the client is dropped
=================
*/
qboolean ClientInactivityTimer( gclient_t *client ) {
    if ( ! g_inactivity.integer ) {
        // give everyone some time, so if the operator sets g_inactivity during
        // gameplay, everyone isn't kicked
        client->inactivityTime = level.time + 60 * 1000;
        client->inactivityWarning = qfalse;
    } else if ( level.paused || 
        client->pers.cmd.forwardmove ||
        client->pers.cmd.rightmove ||
        client->pers.cmd.upmove ||
        (client->pers.cmd.buttons & (BUTTON_ATTACK|BUTTON_ALT_ATTACK)) ) {
        client->inactivityTime = level.time + g_inactivity.integer * 1000;
        client->inactivityWarning = qfalse;
    } else if ( !client->pers.localClient ) {
        if ( level.time > client->inactivityTime ) {
            //trap_DropClient( client - level.clients, "Dropped due to inactivity" ); // Stop kicking clients for being afk.
            SetTeam(&g_entities[client - level.clients], "s", NULL, qtrue);
            G_printInfoMessageToAll("%s was forced to spectator for being AFK.", client->pers.cleanName);
            return qfalse;
        }
        if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning ) {
            client->inactivityWarning = qtrue;
            trap_SendServerCommand( client - level.clients, "cp \"Ten seconds until inactivity drop!\n\"" );
        }
    }
    return qtrue;
}

/*
==================
ClientTimerActions

Actions that happen once a second
==================
*/
void ClientTimerActions( gentity_t *ent, int msec )
{
    gclient_t   *client;

    client = ent->client;

    // Check so see if the player has moved and if so dont let them change their outfitting
    if ( !client->noOutfittingChange && ((level.time - client->respawnTime) > 3000))
    {
        vec3_t vel;

        // Check the horizontal velocity for movement
        VectorCopy ( client->ps.velocity, vel );
        vel[2] = 0;
        if ( VectorLengthSquared ( vel ) > 100 )
        {
            client->noOutfittingChange = qtrue;
        }
    }

    // Forgive voice chats
    if ( g_voiceFloodCount.integer && ent->client->voiceFloodCount )
    {
        int forgiveTime = 60000 / g_voiceFloodCount.integer;

        client->voiceFloodTimer += msec;
        while ( client->voiceFloodTimer >= forgiveTime )
        {
            // Forgive one voice chat
            client->voiceFloodCount--;

            client->voiceFloodTimer -= forgiveTime;
        }
    }

    if (client->pers.oneSecondChecks < level.time) {
        client->pers.oneSecondChecks = level.time + 1000;

        if (client->pers.burnSeconds) {
            client->pers.burnSeconds--;
            if (ent->client->ps.stats[STAT_HEALTH] >= 35)
                G_Damage(ent, NULL, NULL, NULL, NULL, 12, 0, MOD_BURN, HL_NONE);

            vec3_t fireAngs, dir;
            VectorCopy(ent->client->ps.viewangles, fireAngs);
            AngleVectors(fireAngs, dir, NULL, NULL);
            dir[0] *= -1.0;
            dir[1] *= -1.0;
            dir[2] = 0.0;
            VectorNormalize(dir);
            G_ApplyKnockback(ent, dir, 10);  //knock them back
        }
    }
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gclient_t *client )
{
    G_UpdatePlayerStateScores ( &g_entities[client->ps.clientNum] );

    client->ps.loopSound = 0;

    client->ps.eFlags &= ~EF_TALK;
    client->ps.eFlags &= ~EF_FIRING;

    // the level will exit when everyone wants to or after timeouts

    // swap and latch button actions
    client->oldbuttons = client->buttons;
    client->buttons = client->pers.cmd.buttons;

    if ( (client->buttons & BUTTON_ATTACK) & ( client->oldbuttons ^ client->buttons ) )
    {
        // this used to be an ^1 but once a player says ready, it should stick
        client->readyToExit = 1;
    }
}

/*
====================
G_Use

use key pressed
====================
*/
void G_Use ( gentity_t* ent )
{
    if ( !ent->client->useEntity )
    {
        return;
    }

    if ( ent->client->useEntity->s.eType == ET_ITEM )
    {
        // Make sure one last time that it can still be used
        if ( !trap_GT_SendEvent ( GTEV_ITEM_CANBEUSED, level.time, ent->client->useEntity->item->quantity, ent->s.number, ent->client->sess.team, 0, 0 ) )
        {
            return;
        }

        gametype_item_use ( ent->client->useEntity, ent );
        return;
    }

    // Make double sure it can still be used
    if ( !trap_GT_SendEvent ( GTEV_TRIGGER_CANBEUSED, level.time, ent->client->useEntity->health, ent->s.number, ent->client->sess.team, 0, 0 ) )
    {
        return;
    }

    ent->client->useEntity->use ( ent->client->useEntity, ent, ent );
}

/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence )
{
    int         i;
    int         event;
    gclient_t   *client;
    vec3_t      dir;

    client = ent->client;

    if ( oldEventSequence < client->ps.eventSequence - MAX_PS_EVENTS )
    {
        oldEventSequence = client->ps.eventSequence - MAX_PS_EVENTS;
    }

    for ( i = oldEventSequence ; i < client->ps.eventSequence ; i++ )
    {
        event = client->ps.events[ i & (MAX_PS_EVENTS-1) ];

        switch ( event )
        {
            case EV_FALL_MEDIUM:
            case EV_FALL_FAR:
            {
                int damage;

                damage  = client->ps.eventParms[ i & (MAX_PS_EVENTS-1) ];
                damage &= 0x000000ff;

                client->ps.eventParms[ i & (MAX_PS_EVENTS-1) ] = damage;

                if ( ent->s.eType != ET_PLAYER )
                {
                    break;      // not in the player model
                }

                if ( g_dmflags.integer & DF_NO_FALLING )
                {
                    break;
                }

                VectorSet (dir, 0, 0, 1);
                ent->pain_debounce_time = level.time + 200; // no normal pain sound
                G_Damage (ent, NULL, NULL, NULL, NULL, damage, DAMAGE_NO_ARMOR, MOD_FALLING, HL_NONE );
                break;
            }

            case EV_FIRE_WEAPON:
                ent->client->noOutfittingChange = qtrue;
                ent->client->invulnerableTime = 0;
                G_FireWeapon( ent, ATTACK_NORMAL );
                break;

            case EV_ALT_FIRE:
                ent->client->noOutfittingChange = qtrue;
                ent->client->invulnerableTime = 0;
                G_FireWeapon( ent, ATTACK_ALTERNATE );
                break;

            case EV_USE:
                G_Use ( ent );
                break;

            default:
                break;
        }
    }

}

/*
==============
StuckInOtherClient
==============
*/
static int StuckInOtherClient(gentity_t *ent)
{
    int i;
    gentity_t   *ent2;

    ent2 = &g_entities[0];
    for ( i = 0; i < MAX_CLIENTS; i++, ent2++ )
    {
        if ( ent2 == ent )
        {
            continue;
        }

        if ( !ent2->inuse )
        {
            continue;
        }

        if ( !ent2->client )
        {
            continue;
        }

        if ( ent2->health <= 0 )
        {
            continue;
        }

        //
        if (ent2->r.absmin[0] > ent->r.absmax[0])
            continue;
        if (ent2->r.absmin[1] > ent->r.absmax[1])
            continue;
        if (ent2->r.absmin[2] > ent->r.absmax[2])
            continue;
        if (ent2->r.absmax[0] < ent->r.absmin[0])
            continue;
        if (ent2->r.absmax[1] < ent->r.absmin[1])
            continue;
        if (ent2->r.absmax[2] < ent->r.absmin[2])
            continue;
        return qtrue;
    }
    return qfalse;
}

void BotTestSolid(vec3_t origin);

/*
==============
SendPendingPredictableEvents
==============
*/
void SendPendingPredictableEvents( playerState_t *ps ) {
    gentity_t *t;
    int event, seq;
    int extEvent, number;

    // if there are still events pending
    if ( ps->entityEventSequence < ps->eventSequence ) {
        // create a temporary entity for this event which is sent to everyone
        // except the client who generated the event
        seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
        event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
        // set external event to zero before calling BG_PlayerStateToEntityState
        extEvent = ps->externalEvent;
        ps->externalEvent = 0;

        // create temporary entity for event
        t = G_TempEntity( ps->origin, event );
        number = t->s.number;
        BG_PlayerStateToEntityState( ps, &t->s, qtrue );
        t->s.number = number;
        t->s.eType = ET_EVENTS + event;
        t->s.eFlags |= EF_PLAYER_EVENT;
        t->s.otherEntityNum = ps->clientNum;
        // send to everyone except the client who generated the event
        t->r.svFlags |= SVF_NOTSINGLECLIENT;
        t->r.singleClient = ps->clientNum;
        // set back external event
        ps->externalEvent = extEvent;
    }
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/
void ClientThink_real( gentity_t *ent )
{
    gclient_t   *client;
    pmove_t     pm;
    int         oldEventSequence;
    int         msec;
    usercmd_t   *ucmd;

    client = ent->client;

    // don't think if the client is not yet connected (and thus not yet spawned in)
    if (client->pers.connected != CON_CONNECTED)
    {
        return;
    }

    if (!(ent->r.svFlags & SVF_BOT)) {
        if (!client->sess.legacyProtocol && client->sess.checkClientAdditions && level.time > client->sess.clientAdditionCheckTime) {
            if (client->sess.checkClientAdditions > 10) {
                kickPlayer(ent, NULL, "kicked", "This server requires you to use 1fx. Client additions. Please turn on autodownload and reconnect (cl_allowdownload 1)");
            }
            else {
                client->sess.clientAdditionCheckTime = level.time + 2500;
                G_Broadcast(BROADCAST_AWARDS, ent, qfalse, "This server requires ^11fx. Client\n^7Turn on autodownload to get it\ncl_allowdownload 1");
                G_printInfoMessage(ent, "This server requires ^11fx. Client Additions.^7 Turn on autodownload to get it: cl_allowdownload 1");
                
            }

            trap_SendServerCommand(ent - g_entities, "ca_verify");
            client->sess.checkClientAdditions++;
        }
    }

    // mark the time, so the connection sprite can be removed
    ucmd = &ent->client->pers.cmd;

    // sanity check the command time to prevent speedup cheating
    if ( ucmd->serverTime > level.time + 200 )
    {
        ucmd->serverTime = level.time + 200;
    }

    if ( ucmd->serverTime < level.time - 1000 )
    {
        ucmd->serverTime = level.time - 1000;
    }

    msec = ucmd->serverTime - client->ps.commandTime;
    // following others may result in bad times, but we still want
    // to check for follow toggles
    if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW )
    {
        return;
    }

    if ( msec > 200 )
    {
        msec = 200;
    }

    if ( pmove_msec.integer < 8 )
    {
        trap_Cvar_Set("pmove_msec", "8");
    }
    else if (pmove_msec.integer > 33)
    {
        trap_Cvar_Set("pmove_msec", "33");
    }

    if ( pmove_fixed.integer || client->pers.pmoveFixed )
    {
        ucmd->serverTime = ((ucmd->serverTime + pmove_msec.integer-1) / pmove_msec.integer) * pmove_msec.integer;
    }

    if (level.awardTime && isCurrentGametype(GT_HNS)) {
        ent->client->ps.pm_type = PM_FREEZE;
        ent->client->ps.stats[STAT_HEALTH] = -1;
        memset(&pm, 0, sizeof(pm));
        pm.ps = &client->ps;
        pm.cmd = *ucmd;
        Pmove(&pm);
        return;
    }

    if (isCurrentGametype(GT_PROP)) {

        if (client->pers.cmd.buttons & BUTTON_RELOAD) {
            if (client->pers.prop.isMovingModel && client->pers.prop.nextModelState < level.time) {
                if (client->pers.prop.isModelStatic) {
                    client->pers.prop.isModelStatic = qfalse;
                    G_printChatInfoMessage(ent, "You can move again. Press Reload to make yourself static again (5 sec cooldown).");
                    client->pers.prop.stateCooldownMessage = level.time + 1000;
#ifndef _DEVEL
                    client->pers.prop.nextModelState = level.time + 5000;
#else 
                    client->pers.prop.nextModelState = level.time + 500;
#endif
                    client->inactivityTime = level.time + g_inactivity.integer * 1000;
                }
                else if (client->ps.pm_flags & PMF_JUMPING || client->ps.velocity[2] < 0 || client->ps.groundEntityNum == ENTITYNUM_NONE) {

                    if (level.time > client->pers.prop.stateCooldownMessage) {
                        client->pers.prop.stateCooldownMessage = level.time + 3000;
                        G_printChatInfoMessage(ent, "You cannot be jumping while trying to freeze.");
                    }
                }
                else {
                    client->pers.prop.isModelStatic = qtrue;
                    G_printChatInfoMessage(ent, "You're now static. Your model will not move. Press Reload to move again (5 sec cooldown)");
                    client->pers.prop.stateCooldownMessage = level.time + 1000;
#ifndef _DEVEL
                    client->pers.prop.nextModelState = level.time + 5000;
#else 
                    client->pers.prop.nextModelState = level.time + 500;
#endif
                }
            }
            else if (client->pers.prop.isMovingModel && client->pers.prop.stateCooldownMessage < level.time) {
                G_printChatInfoMessage(ent, "You must wait until you can %s again.", client->pers.prop.isModelStatic ? "unfreeze" : "freeze");
                client->pers.prop.stateCooldownMessage = level.time + 1000;
            }
        }

    }

    if (ent->client->pers.prop.isMovingModel && isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_PROP, GT_MAX })) {

        if (G_IsClientDead(ent->client)) {
            freeProphuntProps(ent);
        }
        else {
            prop_ThinkMovingModelSingle(&g_entities[ent->client->sess.transformedEntity]);
        }

    }

    if ((client->sess.transformed || client->pers.prop.isModelStatic) && !(client->pers.cmd.buttons & BUTTON_RELOAD)) {
        client->ps.pm_type = PM_FREEZE;
        Com_Memset(&pm, 0, sizeof(pm));
        pm.ps = &client->ps;
        ucmd->buttons = 0;
        pm.cmd = *ucmd;
        Pmove(&pm);
        return;
    }

    //
    // check for exiting intermission
    //
    if ( level.intermissiontime )
    {
        ClientIntermissionThink( client );
        return;
    }

    //Ryan june 15 2003
    if (level.paused)     //if paused stop here
    {
        ///RxCxW - 08.28.06 - 03:51pm - #paused - reset inactivity counter so we dont get kicked
        if (g_inactivity.integer)
            client->inactivityTime = level.time + g_inactivity.integer * 1000;
        else
            client->inactivityTime = level.time + 60 * 1000;
        client->inactivityWarning = qfalse;
        ///End  - 08.28.06 - 03:52pm
        return;
    }

    // Boe!Man 3/30/10: We wait for the motd.
    if (client->sess.firstTime && !client->sess.motdStartTime && !level.intermissionQueued)
    {
        if (ucmd->buttons & BUTTON_ANY)
        {
            client->sess.motdStartTime = level.time;
            client->sess.motdStopTime = level.time + 10000;
            G_printChatInfoMessage(ent, "This server is running " MODNAME_COLORED);
            G_printChatInfoMessage(ent, "Please report any bugs on GitHub or 3d-sof2.com");
            showMotd(ent);
        }
    }
    //Ryan


    // Boe!Man 5/6/15: Check for the client Mod.
    if (level.goldMod == CL_ROCMOD && !client->sess.legacyProtocol && client->sess.clientMod != CL_ROCMOD && level.time > client->sess.clientModCheckTime) {
        if (client->sess.clientModChecks > 25) {
            char* info = G_ColorizeMessage("\\Info:");

            trap_SendServerCommand(ent - g_entities, va("chat -1 \"%s This TEST server expects you to be running ^1ROCmod 2.1c^7.\n\"", info));
            trap_SendServerCommand(ent - g_entities, va("chat -1 \"%s You do not appear to be running that specific version of ^1ROCmod^7.\n\"", info));
            trap_SendServerCommand(ent - g_entities, va("chat -1 \"%s Please ^1download the mod^7, or ^1turn on auto-downloading^7, and re-join the game.\n\"", info));

            // It looks like the client doesn't have the proper client, just continue bothering him every 20 seconds.
            client->sess.clientModCheckTime = level.time + 20000;
        }
        else {
            // Get the client to verify as soon as possible.
            client->sess.clientModCheckTime = level.time + 5000;
        }

        trap_SendServerCommand(ent - g_entities, "verifymod");
        client->sess.clientModChecks++;

    }

    if (client->sess.motdStartTime) {
        // Boe!Man 10/18/15: Make sure the motd is being broadcasted several times.
        if (level.time >= client->sess.motdStartTime + 1000 && level.time < client->sess.motdStopTime - 3500) {
            client->sess.motdStartTime += 1000;
            showMotd(ent);
        }
        else if (level.time >= client->sess.motdStopTime && level.time > (ent->client->sess.lastMessage + 4000)) {
            // Boe!Man 3/16/11: Better to reset the values and actually put firstTime to qfalse so it doesn't mess up when we want to broadcast a teamchange.
            client->sess.motdStartTime = 0;
            client->sess.motdStopTime = 0;
            if (client->sess.firstTime)
            {
                BroadcastTeamChange(client, -1);
                client->sess.firstTime = qfalse;
            }
        }

    }

    // spectators don't do much
    if ( G_IsClientSpectating ( client ) )
    {
        if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD )
        {
            return;
        }
        SpectatorThink( ent, ucmd );
        return;
    }

    // check for inactivity timer, but never drop the local client of a non-dedicated server
    if ( !ClientInactivityTimer( client ) )
    {
        return;
    }

    if ( client->noclip )
    {
        client->ps.pm_type = PM_NOCLIP;
    }
    else if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        client->ps.pm_type = PM_DEAD;
    }
    else
    {
        client->ps.pm_type = PM_NORMAL;
    }

    client->ps.gravity = g_gravity.value;

    // set speed
    client->ps.speed = g_speed.value;

    if (isCurrentGametype(GT_HNS)) {

        if (client->ps.weapon == WP_MM1_GRENADE_LAUNCHER) {
            client->ps.gravity /= 1.6;
        }

        if (client->ps.weapon == WP_RPG7_LAUNCHER &&
            level.time > client->sess.speedDecrement.speedAlterationTo &&
            (
                level.time < level.gametypeDelayTime ||
                !g_rpgSpeedDrain.integer ||
                client->ps.stats[STAT_ARMOR] > 0)
            ) {
            client->ps.speed += g_rpgSpeedIncrement.integer;

            if (client->ps.stats[STAT_ARMOR] > 0 && level.time >= level.gametypeDelayTime && level.time >= client->sess.rpgAnimation && g_rpgSpeedDrain.integer && (ent->r.currentOrigin[1] != client->sess.oldvelocity[1] || ent->r.currentOrigin[2] != client->sess.oldvelocity[2])) {
                client->sess.rpgAnimation = level.time + g_rpgSpeedDrainSec.value * 1000;
                client->ps.stats[STAT_ARMOR]--;

                if (client->ps.stats[STAT_ARMOR] <= 0) {
                    G_printInfoMessage(ent, "Your RPG speed boost has been drained.");
                }
                if (ent->client->ps.clip[ATTACK_NORMAL][WP_RPG7_LAUNCHER] == 0 && ent->client->ps.clip[ATTACK_ALTERNATE][WP_RPG7_LAUNCHER] == 0 && client->ps.stats[STAT_ARMOR] <= 0 && g_rpgRemove.integer) {
                    removeWeaponFromClient(ent, WP_RPG7_LAUNCHER, qfalse, WP_KNIFE);
                    G_printInfoMessage(ent, "No more RPG rounds and boost!");
                    Com_sprintf(level.hns.RPGloc, sizeof(level.hns.RPGloc), "%s", "Disappeared");
                    G_printGametypeMessageToAll("RPG has disappeared.");
                }
            }

            if (level.time >= client->sess.speedAnimation) {
                if (ent->r.currentOrigin[1] != client->sess.oldvelocity[1] || ent->r.currentOrigin[2] != client->sess.oldvelocity[2]) {
                    G_PlayEffect(G_EffectIndex("arm2smallsmoke"), client->ps.origin, ent->pos1);
                    client->sess.speedAnimation = level.time + 10;
                    VectorCopy(ent->r.currentOrigin, client->sess.oldvelocity);
                }
            }
        }
        else if (level.time <= client->sess.speedIncrement.speedAlterationTo) {
            client->ps.speed += g_stunSpeedIncrement.integer;

            if (level.time >= client->sess.speedAnimation) {
                if (ent->r.currentOrigin[1] != client->sess.oldvelocity[1] || ent->r.currentOrigin[2] != client->sess.oldvelocity[2]) {
                    G_PlayEffect(G_EffectIndex("arm2smallsmoke"), client->ps.origin, ent->pos1);
                    client->sess.speedAnimation = level.time + 30;
                    VectorCopy(ent->r.currentOrigin, client->sess.oldvelocity);
                }
            }
        }
        else if (level.time <= client->sess.speedDecrement.speedAlterationTo) {
            int slowSpeedValue, timeLeft;
            switch (client->sess.speedDecrement.speedAlterationReason) {
            case SPEEDALTERATION_FIRENADE:
            case SPEEDALTERATION_MM1:

                timeLeft = client->sess.speedDecrement.speedAlterationTo - level.time;
                slowSpeedValue = (int)timeLeft * g_fireSpeedDecrement.integer / client->sess.speedDecrement.speedAlterationDuration;
                client->ps.speed -= slowSpeedValue;
                break;
            case SPEEDALTERATION_KNIFE:
            case SPEEDALTERATION_M4:
                client->ps.speed -= g_stunSpeedDecrement.integer;
                break;
            case SPEEDALTERATION_WATER:
                client->ps.speed -= g_waterSpeedDecrement.integer;
                break;
            case SPEEDALTERATION_STUNGUN:
                client->ps.speed = 0; // fully stunned
                break;
            default:
                client->ps.speed -= g_stunSpeedDecrement.integer;
                break;
            }
        }

        if (level.customGameStarted && client->sess.team == TEAM_BLUE) {
            vec3_t newOrigin;

            // The seeker moved.
            if (client->pers.cmd.forwardmove || client->pers.cmd.rightmove || client->pers.cmd.upmove || (client->pers.cmd.buttons & (BUTTON_ATTACK | BUTTON_ALT_ATTACK))) {
                if (client->pers.seekerAway) {
                    client->pers.seekerAway = qfalse;
                }
                client->pers.seekerAwayTime = level.time + 10000;
            }
            else if (level.time > client->pers.seekerAwayTime) {
                if (!client->pers.seekerAway) {
                    client->pers.seekerAway = qtrue;
                }
                client->pers.seekerAwayTime = level.time + 500;

                VectorCopy(client->ps.origin, newOrigin);
                newOrigin[0] += 5;
                newOrigin[2] += 75;
                G_PlayEffect(G_EffectIndex("misc/exclaimation"), newOrigin, ent->pos1);
            }
        }

        if (client->pers.cmd.buttons & BUTTON_RELOAD) {

            if (client->sess.transformed) {

                if (client->sess.transformedEntity) {
                    G_FreeEntity(&g_entities[client->sess.transformedEntity]);
                    client->sess.transformedEntity = 0;
                }

                if (client->sess.transformedEntityBBox) {
                    G_FreeEntity(&g_entities[client->sess.transformedEntityBBox]);
                    client->sess.transformedEntityBBox = 0;
                }

                client->sess.invisible = qfalse;
                client->sess.transformed = qfalse;
                client->ps.pm_type = PM_NORMAL;
                client->ps.eFlags &= ~EF_HSBOX;
                ent->s.eFlags &= ~EF_HSBOX;

                client->inactivityTime = level.time + g_inactivity.integer * 1000;

            }

        }

    }
    else if (isCurrentGametype(GT_HNZ)) {
        if (TeamCount(-1, TEAM_BLUE, NULL) == 1 && ent->client->sess.team == TEAM_BLUE) {
            client->ps.speed += 70;
            client->ps.stats[STAT_ARMOR] = Com_Clamp(0, 100, TeamCount(-1, TEAM_RED, NULL) * 10);
            if (level.time > client->sess.speedAnimation) {
                if (ent->r.currentOrigin[1] != client->sess.oldvelocity[1] || ent->r.currentOrigin[2] != client->sess.oldvelocity[2]) {
                    G_PlayEffect(G_EffectIndex("arm2smallsmoke"), client->ps.origin, ent->pos1);
                    client->sess.speedAnimation = level.time + 10;
                    VectorCopy(ent->r.currentOrigin, client->sess.oldvelocity);
                }
            }
        }
    }
    else if (isCurrentGametype(GT_CSINF)) {
        // CSINF guns slow down players.

        static const int gunWeightToSpeedDecrement[] = {
            [GUNWEIGHT_NONE] = 0,
            [GUNWEIGHT_LIGHT] = 5,
            [GUNWEIGHT_SHOTGUN] = 10,
            [GUNWEIGHT_SMG] = 8,
            [GUNWEIGHT_RIFLE] = 12,
            [GUNWEIGHT_SNIPER] = 15,
            [GUNWEIGHT_MACHINEGUN] = 20
        };

        int speedDecrement = gunWeightToSpeedDecrement[GUNWEIGHT_MACHINEGUN];

        if (ent->client->ps.weapon > WP_KNIFE) {
            speedDecrement = 20; // Assume max decrement until a weapon is found.

            for (int i = 0; i < CSINF_GUNTABLE_SIZE; i++) {
                csInfGuns_t* gun = &csInfGunsTable[i];

                if (gun->gunType != GUNTYPE_UTILITY && gun->gunId == ent->client->ps.weapon) {
                    speedDecrement = gunWeightToSpeedDecrement[gun->gunWeight];
                    break;
                }
            }

            if (speedDecrement > 0) {
                ent->client->ps.speed *= (1.0 - ((float)speedDecrement / 100.0));
            }

        }
    }
    else if (isCurrentGametype(GT_PROP)) {
        if (client->sess.team == TEAM_BLUE) {
            client->ps.speed += 35;
        }
    }

    if (client->sess.punishment && client->sess.team != TEAM_SPECTATOR && !G_IsClientDead(client)) {
        if (level.time > ent->client->sess.nextPunishmentTime) {
            int newPunishment = ent->client->sess.currentPunishment;

            ent->client->sess.nextPunishmentTime = level.time + 15000;

            while (newPunishment == ent->client->sess.currentPunishment) {
                newPunishment = irand(PUNISH_NONE + 1, PUNISH_MAX - 1);

                if (newPunishment == PUNISH_TELEENEMY) {
                    // only use teleenemy in specific conditions.

                    if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNZ, GT_HNS, GT_PROP, GT_MAX })) {
                        newPunishment = ent->client->sess.currentPunishment;
                        continue;
                    }

                    if (ent->client->sess.team != TEAM_RED) {
                        newPunishment = ent->client->sess.currentPunishment;
                        continue;
                    }

                }

            }

            ent->client->sess.currentPunishment = newPunishment;
            ent->client->sess.currentPunishmentCycleTime = level.time;
            ent->client->sess.spinView = qfalse;

            if (ent->client->pers.twisted) {
                SetClientViewAngle(ent, (vec3_t) { 0, 0, 0 });
            }

            ent->client->pers.twisted = qfalse;
            ent->client->pers.gocrazy = qfalse;

        }

        if (level.time > ent->client->sess.nextPunishmentMessage) {
            char* currentPunishment;
            ent->client->sess.nextPunishmentMessage = level.time + 1500;

            currentPunishment = getPunishmentAsText(ent);

            G_Broadcast(BROADCAST_GAME, ent, qfalse, "You're being \\punished.\nReason: %s\nCurrent punishment: \\%s\nEnjoy!", ent->client->sess.punishmentReason, currentPunishment);
        }

        if (level.time > ent->client->sess.currentPunishmentCycleTime) {
            int idle = 0;
            int randomEnemy;
            gentity_t* tent;
            int i = 0;
            switch (ent->client->sess.currentPunishment) {
            case PUNISH_CROUCH:
                ent->client->ps.pm_flags = PMF_DUCKED;

                ucmd->upmove = 128;
                break;
            case PUNISH_DEGRADINGHEALTH:

                if (ent->client->sess.currentPunishmentCycleTime < level.time) {
                    ent->client->sess.currentPunishmentCycleTime = level.time + 333;
                    ent->health--;
                    ent->client->ps.stats[STAT_HEALTH]--;

                    if (ent->health <= 0 || ent->client->ps.stats[STAT_HEALTH] <= 0) {
                        player_die(ent, ent, ent, 99999, MOD_DUGUP, HL_NONE, vec3_origin);
                    }
                }
                break;
            case PUNISH_HARDGRAVITY:
                ent->client->ps.gravity += 400;
                break;

            case PUNISH_LOCKVIEW:
                SetClientViewAngle(ent, (vec3_t) { 90, 0, 0 });
                break;

            case PUNISH_LOWSPEED:
                ent->client->ps.speed -= 100;
                break;

            case PUNISH_SPIN:
                ent->client->sess.spinView = qtrue;
                ent->client->sess.nextSpin = level.time;
                ent->client->sess.lastSpin = level.time + 15000;
                ent->client->sess.spinViewState = SPINVIEW_FAST;
                ent->client->sess.currentPunishmentCycleTime = ent->client->sess.nextPunishmentTime;
                break;

            case PUNISH_TWIST:
                ent->client->pers.twisted = qtrue;
                SetClientViewAngle(ent, (vec3_t) { 100, 0, 130 });
                ent->client->sess.currentPunishmentCycleTime = ent->client->sess.nextPunishmentTime;
                break;


            case PUNISH_USERCMD:
                ent->client->pers.gocrazy = qtrue;
                ent->client->sess.currentPunishmentCycleTime = ent->client->sess.nextPunishmentTime;
                break;

            case PUNISH_RANDOMMOVE:

                ucmd->forwardmove = 127;  
                ucmd->rightmove = 127;   
                ucmd->upmove = 127;        
                SetClientViewAngle(ent, (vec3_t) { -90, 0, 0 });

                break;

            case PUNISH_STRIP:
                stripClient(ent, qtrue);
                break;

            case PUNISH_TELEENEMY:


                while (i < 200) {

                    randomEnemy = irand(0, level.numConnectedClients);

                    tent = &g_entities[randomEnemy];

                    if (tent && tent->client && tent->client->sess.team != ent->client->sess.team && tent->client->sess.team != TEAM_SPECTATOR) {

                        TeleportPlayerNoKillbox(tent, ent->r.currentOrigin, ent->client->ps.viewangles, qtrue);
                        ent->client->sess.currentPunishmentCycleTime = level.time + 1000;
                        break;
                    }

                    i++;
                }
                i = 0;
                break;

            default:
                break;

            }

        }
    }

    if (client->sess.acceleratorCooldown) {
        if (client->sess.acceleratorCooldown > level.time) {
            client->ps.speed += (g_speed.integer / 5000) * (client->sess.acceleratorCooldown - level.time);
        }
        else {
            client->sess.acceleratorCooldown = 0;
        }
    }

    if (g_noHighFps.integer) {
        if (msec < 4) {
            client->ps.gravity += (msec <= 2) ? 500 : 111;
        }
    }

    if (client->sess.spinView) {
        spinView(ent);
    }

    if (client->pers.gocrazy) {
        ucmd->upmove *= -1;
        ucmd->forwardmove *= -1;
        ucmd->rightmove *= -1;
        ucmd->angles[0] *= -1;
        ucmd->angles[1] *= -1;
        ucmd->angles[2] *= -1;
    }

    // set up for pmove
    oldEventSequence = client->ps.eventSequence;

    memset (&pm, 0, sizeof(pm));

    pm.ps = &client->ps;
    pm.cmd = *ucmd;
    if ( pm.ps->pm_type == PM_DEAD )
    {
        pm.ps->loopSound = 0;
        pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
    }
    else if ( client->siameseTwin )
    {
        // Make sure we are still stuck, if so, clip through players.
        if ( G_IsClientSiameseTwin ( ent, client->siameseTwin ) )
        {
            pm.tracemask = MASK_PLAYERSOLID & ~(CONTENTS_BODY);
        }
        else
        {
            // Ok, we arent stuck anymore so we can clear the stuck flag.
            client->siameseTwin->client->siameseTwin = NULL;
            client->siameseTwin = NULL;

            pm.tracemask = MASK_PLAYERSOLID;
        }
    }
    else if ( ent->r.svFlags & SVF_BOT )
    {
        pm.tracemask = MASK_PLAYERSOLID | CONTENTS_BOTCLIP;
    }
    else
    {
        pm.tracemask = MASK_PLAYERSOLID;
    }
    pm.trace = trap_Trace;
    pm.pointcontents = trap_PointContents;
    pm.debugLevel = g_debugMove.integer;
    pm.noFootsteps = ( g_dmflags.integer & DF_NO_FOOTSTEPS ) > 0;

    pm.pmove_fixed = pmove_fixed.integer | client->pers.pmoveFixed;
    pm.pmove_msec = pmove_msec.integer;

    pm.animations = NULL;
    pm.legacyProtocol = client->sess.legacyProtocol;
    VectorCopy( client->ps.origin, client->oldOrigin );

    Pmove (&pm);

    G_UpdatePlayerStateScores ( ent );

    // save results of pmove
    if ( ent->client->ps.eventSequence != oldEventSequence )
    {
        ent->eventTime = level.time;
    }

    // See if the invulnerable flag should be removed for this client
    if ( ent->client->ps.eFlags & EF_INVULNERABLE )
    {
        if ( level.time - ent->client->invulnerableTime >= g_respawnInvulnerability.integer * 1000 )
        {
            ent->client->ps.eFlags &= ~EF_INVULNERABLE;
        }
    }

    if (g_smoothClients.integer)
    {
        BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue );
    }
    else
    {
        BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
    }

    SendPendingPredictableEvents( &ent->client->ps );

    if ( !( ent->client->ps.eFlags & EF_FIRING ) )
    {
        client->fireHeld = qfalse;      // for grapple
    }

    // use the snapped origin for linking so it matches client predicted versions
    VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

    VectorCopy (pm.mins, ent->r.mins);
    VectorCopy (pm.maxs, ent->r.maxs);

    ent->waterlevel = pm.waterlevel;
    ent->watertype = pm.watertype;

    // Need to cache off the firemodes to the persitant data segment so they
    // are maintained across spectating and respawning
    memcpy ( ent->client->pers.firemode, ent->client->ps.firemode, sizeof(ent->client->ps.firemode ) );

    // execute client events
    ClientEvents( ent, oldEventSequence );

    // Update the client animation info
    G_UpdateClientAnimations ( ent );

    if ( ent->client->ps.pm_flags & PMF_LEANING )
    {
        ent->r.svFlags |= SVF_LINKHACK;
    }
    else
    {
        ent->r.svFlags &= ~SVF_LINKHACK;
    }

    // link entity now, after any personal teleporters have been used
    trap_LinkEntity (ent);
    if ( !ent->client->noclip )
    {
        G_TouchTriggers( ent );
    }

    // NOTE: now copy the exact origin over otherwise clients can be snapped into solid
    VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );

    // Update the clients anti-lag history
    G_UpdateClientAntiLag ( ent );

    // touch other objects
    ClientImpacts( ent, &pm );

    // save results of triggers and client events
    if (ent->client->ps.eventSequence != oldEventSequence)
    {
        ent->eventTime = level.time;
    }

    // swap and latch button actions
    client->oldbuttons = client->buttons;
    client->buttons = ucmd->buttons;
    client->latched_buttons |= client->buttons & ~client->oldbuttons;

    // check for respawning
    if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        // wait for the attack button to be pressed
        if ( level.time > client->respawnTime )
        {
            if ( g_forcerespawn.integer > 0 &&
                ( level.time - client->respawnTime ) > g_forcerespawn.integer * 1000 )
            {
                respawn( ent );
                return;
            }

            // pressing attack or use is the normal respawn method
            if ( ucmd->buttons & BUTTON_ATTACK )
            {
                respawn( ent );
            }
        }

        return;
    }

    // perform once-a-second actions
    ClientTimerActions( ent, msec );
}

/*
==================
G_CheckClientTeamkill

Checks to see whether or not this client should be booted from the server
because they killed too many teammates
==================
*/
void G_CheckClientTeamkill ( gentity_t* ent )
{
    char userinfo[MAX_INFO_STRING];
    char *value;

    // JANFIXME - with CSINF, maybe make use of this function. But up until that time........
    return;

    if ( !g_teamkillDamageMax.integer || !level.gametypeData->teams || !ent->client->sess.teamkillDamage )
    {
        return;
    }
    // See if they crossed the max team kill damage
    else if ( ent->client->sess.teamkillDamage < g_teamkillDamageMax.integer )
    {
        // Does the client need forgiving?
        if ( ent->client->sess.teamkillForgiveTime )
        {
            // Are we in a forgiving mood yet?
            if ( level.time > ent->client->sess.teamkillForgiveTime + 60000 )
            {
                ent->client->sess.teamkillForgiveTime += 60000;
                ent->client->sess.teamkillDamage -= g_teamkillDamageForgive.integer;
            }
        }

        // All forgivin now?
        if ( ent->client->sess.teamkillDamage <= 0 )
        {
            ent->client->sess.teamkillDamage = 0;
            ent->client->sess.teamkillForgiveTime = 0;
        }

        return;
    }

    trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
    value = Info_ValueForKey (userinfo, "ip");

    G_LogPrintf( "ClientKick: %i %s - auto kick for teamkilling\n", ent->s.number, value );
    logGame(NULL, ent, "autokick", "teamkill");

    ent->client->sess.teamkillDamage      = 0;
    ent->client->sess.teamkillForgiveTime = 0;

    // Keep track of who was autokicked so we can display a list if need be
    Com_sprintf ( level.autokickedIP[level.autokickedHead], sizeof(level.autokickedIP[0]), value );
    Com_sprintf ( level.autokickedName[level.autokickedHead], sizeof(level.autokickedName[0]), ent->client->pers.netname );
    level.autokickedCount++;
    if ( level.autokickedCount >= MAX_AUTOKICKLIST )
    {
        level.autokickedCount = MAX_AUTOKICKLIST;
    }

    level.autokickedHead++;
    if ( level.autokickedHead >= MAX_AUTOKICKLIST )
    {
        level.autokickedHead = 0;
    }

    // Buh bye
    if ( g_teamkillBanTime.integer )
    {
        trap_SendConsoleCommand( EXEC_INSERT, va("banclient \"%d\" \"%d\" \"team killing\"\n", ent->s.number, g_teamkillBanTime.integer ) );
    }
    else
    {
        trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"team killing\"\n", ent->s.number ) );
    }
}

/*
==================
G_CheckClientTimeouts

Checks whether a client has exceded any timeouts and act accordingly
==================
*/
void G_CheckClientTimeouts ( gentity_t *ent )
{
    // Only timeout supported right now is the timeout to spectator mode
    if ( !g_timeouttospec.integer )
    {
        return;
    }

    // Can only do spect timeouts on dedicated servers
    if ( !g_dedicated.integer )
    {
        return;
    }

    // Already a spectator, no need to boot them to spectator
    if ( ent->client->sess.team == TEAM_SPECTATOR )
    {
        return;
    }

    // Need to be connected
    if ( ent->client->pers.connected != CON_CONNECTED )
    {
        return;
    }

    // See how long its been since a command was received by the client and if its
    // longer than the timeout to spectator then force this client into spectator mode
    if ( level.time - ent->client->pers.cmd.serverTime > g_timeouttospec.integer * 1000 )
    {
        SetTeam ( ent, "spectator", NULL, qfalse );
    }
}

/*
==================
ClientThink

A new command has arrived from the client
==================
*/
void ClientThink( int clientNum )
{
    gentity_t *ent;

    ent = g_entities + clientNum;
    trap_GetUsercmd( clientNum, &ent->client->pers.cmd );

    // mark the time we got info, so we can display the
    // phone jack if they don't get any for a while
    ent->client->lastCmdTime = level.time;

    if ( !(ent->r.svFlags & SVF_BOT) && !g_synchronousClients.integer )
    {
        ClientThink_real( ent );
    }
}

/*
==================
G_RunClient
==================
*/
void G_RunClient( gentity_t *ent )
{
    if ( !(ent->r.svFlags & SVF_BOT) && !g_synchronousClients.integer )
    {
        return;
    }

    ent->client->pers.cmd.serverTime = level.time;
    ClientThink_real( ent );
}


/*
==================
SpectatorClientEndFrame
==================
*/
void SpectatorClientEndFrame( gentity_t *ent )
{
    gclient_t   *cl;

    // if we are doing a chase cam or a remote view, grab the latest info
    if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    {
        int     clientNum, flags;

        clientNum = ent->client->sess.spectatorClient;

        // team follow1 and team follow2 go to whatever clients are playing
        if ( clientNum == -1 )
        {
            clientNum = level.follow1;
        }
        else if ( clientNum == -2 )
        {
            clientNum = level.follow2;
        }

        if ( clientNum >= 0 )
        {
            cl = &level.clients[ clientNum ];

            if ( cl->pers.connected == CON_CONNECTED && !G_IsClientSpectating ( cl ) )
            {
                int count;
                int ping;
                int score;
                int respawnTimer;

                count = ent->client->ps.persistant[PERS_SPAWN_COUNT];
                ping  = ent->client->ps.ping;
                score = ent->client->ps.persistant[PERS_SCORE];
                flags = (cl->ps.eFlags & ~(EF_VOTED)) | (ent->client->ps.eFlags & (EF_VOTED));
                respawnTimer = ent->client->ps.respawnTimer;

                ent->client->ps = cl->ps;
                ent->client->ps.pm_flags |= PMF_FOLLOW;
                if ( ent->client->sess.spectatorFirstPerson )
                {
                    ent->client->ps.pm_flags |= PMF_FOLLOWFIRST;
                }
                ent->client->ps.eFlags = flags;
                ent->client->ps.persistant[PERS_SPAWN_COUNT] = count;
                ent->client->ps.persistant[PERS_SCORE] = score;
                ent->client->ps.ping = ping;
                ent->client->ps.respawnTimer = respawnTimer;

                return;
            }
            else
            {
                // drop them to free spectators unless they are dedicated camera followers
                if ( ent->client->sess.spectatorClient >= 0 )
                {
                    Cmd_FollowCycle_f (ent, 1);
                }
            }
        }
    }

    if ( ent->client->sess.spectatorState == SPECTATOR_SCOREBOARD )
    {
        ent->client->ps.pm_flags |= PMF_SCOREBOARD;
    }
    else
    {
        ent->client->ps.pm_flags &= ~PMF_SCOREBOARD;
    }
}

/*
==============
ClientEndFrame

Called at the end of each server frame for each connected client
A fast client will have multiple ClientThink for each ClientEdFrame,
while a slow client may have multiple ClientEndFrame between ClientThink.
==============
*/
void ClientEndFrame( gentity_t *ent )
{
    clientPersistant_t  *pers;

    if ( G_IsClientSpectating ( ent->client ) )
    {
        SpectatorClientEndFrame( ent );
        return;
    }

    pers = &ent->client->pers;

    // save network bandwidth
#if 0
    if ( !g_synchronousClients->integer && ent->client->ps.pm_type == PM_NORMAL )
    {
        // FIXME: this must change eventually for non-sync demo recording
        VectorClear( ent->client->ps.viewangles );
    }
#endif

    //
    // If the end of unit layout is displayed, don't give
    // the player any normal movement attributes
    //
    if ( level.intermissiontime || level.paused )
    {
        return;
    }

    // burn from lava, etc
    P_WorldEffects (ent);

    // apply all the damage taken this frame
    P_DamageFeedback (ent);

    // add the EF_CONNECTION flag if we haven't gotten commands recently
    if ( level.time - ent->client->lastCmdTime > 1000 )
    {
        ent->s.eFlags |= EF_CONNECTION;
    }
    else
    {
        ent->s.eFlags &= ~EF_CONNECTION;
    }

    // FIXME: get rid of ent->health...
    ent->client->ps.stats[STAT_HEALTH] = ent->health;

    G_SetClientSound (ent);

    // set the latest infor
    if (g_smoothClients.integer)
    {
        BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue );
    }
    else
    {
        BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
    }

    SendPendingPredictableEvents( &ent->client->ps );

    // set the bit for the reachability area the client is currently in
//  i = trap_AAS_PointReachabilityAreaIndex( ent->client->ps.origin );
//  ent->client->areabits[i >> 3] |= 1 << (i & 7);
}


