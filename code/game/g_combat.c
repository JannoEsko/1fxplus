// Copyright (C) 2001-2002 Raven Software.
//
// g_combat.c

#include "g_local.h"

void BotDamageNotification  ( gclient_t *bot, gentity_t *attacker );

/*
============
G_AddScore

Adds score to both the client and his team
============
*/
void G_AddScore( gentity_t *ent, int score )
{
    if ( !ent->client )
    {
        return;
    }

    // no scoring during pre-match warmup
    if ( level.warmupTime )
    {
        return;
    }

    ent->client->sess.score += score;
    ent->client->ps.persistant[PERS_SCORE] = ent->client->sess.score;

    CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and custom gametype items for the killed player
=================
*/
void TossClientItems( gentity_t *self )
{
    int weapon;

    // drop the weapon if not a gauntlet or machinegun
    weapon = self->s.weapon;

    // make a special check to see if they are changing to a new
    // weapon that isn't the mg or gauntlet.  Without this, a client
    // can pick up a weapon, be killed, and not drop the weapon because
    // their weapon change hasn't completed yet and they are still holding the MG.
    if ( self->client->ps.weaponstate == WEAPON_DROPPING )
    {
        weapon = self->client->pers.cmd.weapon;
    }

    if ( !( self->client->ps.stats[STAT_WEAPONS] & ( 1 << weapon ) ) )
    {
        weapon = WP_NONE;
    }

    // If we have a valid weapon to drop and it has ammo then drop it

    if (isCurrentGametype(GT_HNS)) {
        if (weapon > WP_KNIFE && weapon < WP_MAX_WEAPONS && weapon != WP_RPG7_LAUNCHER && weapon != WP_M4_ASSAULT_RIFLE) {
            G_DropWeapon(self, (weapon_t)weapon, 0);
        }
        else if (self->client->ps.stats[STAT_WEAPONS] & (1 << WP_RPG7_LAUNCHER)) {
            G_DropWeapon(self, WP_RPG7_LAUNCHER, 0);
        }
        else if (self->client->ps.stats[STAT_WEAPONS] & (1 << WP_M4_ASSAULT_RIFLE)) {
            G_DropWeapon(self, WP_M4_ASSAULT_RIFLE, 0);
        }
    }
    else {

        if (weapon > WP_KNIFE && weapon < WP_NUM_WEAPONS &&
            (self->client->ps.ammo[weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex] + self->client->ps.clip[weapon]))
        {
            G_DropWeapon(self, weapon, 0);
        }
    }


    G_DropGametypeItems ( self, 0 );
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
    vec3_t      dir;
    vec3_t      angles;

    if ( attacker && attacker != self ) {
        VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
    } else if ( inflictor && inflictor != self ) {
        VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
    } else {
        self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
        return;
    }

    self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

    angles[YAW] = vectoyaw ( dir );
    angles[PITCH] = 0;
    angles[ROLL] = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir )
{
    return;
}


// these are just for logging, the client prints its own messages
char *modNames[] =
{
    "MOD_UNKNOWN",

    "MOD_KNIFE",

    "MOD_M1911A1_PISTOL",
    "MOD_US_SOCOM_PISTOL",
    "MOD_SILVER_TALON",

    "MOD_M590_SHOTGUN",
    "MOD_MICRO_UZI_SUBMACHINEGUN",
    "MOD_M3A1_SUBMACHINEGUN",
    "MOD_MP5",

    "MOD_USAS_12_SHOTGUN",
    "MOD_M4_ASSAULT_RIFLE",
    "MOD_AK74_ASSAULT_RIFLE",
    "MOD_SIG551",
    "MOD_MSG90A1_SNIPER_RIFLE",
    "MOD_M60_MACHINEGUN",
    "MOD_MM1_GRENADE_LAUNCHER",
    "MOD_RPG7_LAUNCHER",

    "MOD_M84_GRENADE",
    "MOD_SMOHG92_GRENADE",
    "MOD_ANM14_GRENADE",
    "MOD_M15_GRENADE",

    "MOD_WATER",
    "MOD_CRUSH",
    "MOD_TELEFRAG",
    "MOD_FALLING",
    "MOD_SUICIDE",
    "MOD_TEAMCHANGE",
    "MOD_TARGET_LASER",
    "MOD_TRIGGER_HURT",
    "MOD_TRIGGER_HURT_NOSUICIDE"
};

/*
==================
player_die
==================
*/
void player_die(
    gentity_t   *self,
    gentity_t   *inflictor,
    gentity_t   *attacker,
    int         damage,
    int         mod,
    int         hitLocation,
    vec3_t      hitDir
    )
{
    gentity_t       *ent;
    int             anim;
    int             contents;
    int             killer;
    int             i;
    char            *killerName, *obit;
    attackType_t    attack;
    int             meansOfDeath;

    attack       = (mod >> 8) & 0xFF;
    meansOfDeath = mod & 0xFF;

    if ( self->client->ps.pm_type == PM_DEAD )
    {
        return;
    }

    if ( level.intermissiontime )
    {
        return;
    }

    // Let the gametype know about the player death so it can adjust anything
    // it needs to adjust
    if ( attacker && attacker->client )
    {
        trap_GT_SendEvent ( GTEV_CLIENT_DEATH, level.time, self->s.number, self->client->sess.team, attacker->s.number, attacker->client->sess.team, 0 );

    }
    else
    {
        trap_GT_SendEvent ( GTEV_CLIENT_DEATH, level.time, self->s.number, self->client->sess.team, -1, -1, 0 );
    }

    if (attacker && attacker != self && attacker->client && self->client->pers.statInfo.killsinarow >= 3 && !isCurrentGametype(GT_HNS)) {
        G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s\nhis \\killing spree\nwas ended by %s", self->client->pers.netname, attacker->client->pers.netname);
    }

    // Reset kills in a row. If this is a personal best, save it.
    if (self->client->pers.statInfo.killsinarow > self->client->pers.statInfo.bestKillsInARow)
        self->client->pers.statInfo.bestKillsInARow = self->client->pers.statInfo.killsinarow;

    self->client->pers.statInfo.killsinarow = 0;

    // Add to the number of deaths for this player
    self->client->sess.deaths++;
    self->client->pers.statInfo.deaths++;
    // JANFIXME - Boe had a differentiation of deaths when in H&S + cagefight. 

    if (isCurrentGametype(GT_HNS)) {

        if (self->client->sess.transformedEntity) {
            if (self->client->pers.movingModel) {
                freeProphuntProps(self);

            }

            // Boe!Man 12/15/16: Sometimes it happens a seeker can kill a hider
            // while the hider is hidden in a ? grenade.
            // When this happens the hider body is killed just outside the
            // spawned model. To avoid the hider being stuck we perform an extra
            // check here.
            if (self->client->sess.transformedEntityBBox) {
                // We first check entity2 because when the 2nd entity is used,
                // we set the hideseek flag on this entity.
                transformPlayerBack(
                    &g_entities[self->client->sess.transformedEntityBBox],
                    attacker, NULL);
            }
            else if (self->client->sess.transformedEntity) {
                transformPlayerBack(
                    &g_entities[self->client->sess.transformedEntity],
                    attacker, NULL);
            }

            self->s.eFlags &= ~EF_HSBOX;
            self->client->ps.eFlags &= ~EF_HSBOX;

        }

        // We write the death time into the sess struct GIVEN that the gametype has started.

        if (level.customGameStarted && self->client->sess.team == TEAM_RED && !level.hns.roundOver && level.customGameWeaponsDistributed) {
            self->client->sess.hsTimeOfDeath = level.time;
        }

        if (attacker && attacker->client && level.customGameStarted && attacker->client->sess.team == TEAM_BLUE && self && self->client && self->client->sess.team == TEAM_RED) {
            attacker->client->pers.killsAsSeekCurrentRound++;

            if (attacker->client->pers.killsAsSeekCurrentRound > level.hns.previousRoundBestSeekerKills) {
                level.hns.previousRoundBestSeeker = attacker->s.number;
                level.hns.previousRoundBestSeekerKills = attacker->client->pers.killsAsSeekCurrentRound;
            }
        }

    }

    //Ryan april 22 2003
    //If they are planted unplant them before we kill them
    if (self->client->sess.planted)
    {
        self->client->ps.origin[2] += 65;
        VectorCopy(self->client->ps.origin, self->s.origin);
        self->client->sess.planted = qfalse;
    }

    // This is just to ensure that the player wont render for even a single frame
    self->s.eFlags |= EF_DEAD;

    self->client->ps.pm_type = PM_DEAD;

    if ( attacker )
    {
        killer = attacker->s.number;
        if ( attacker->client )
        {
            killerName = attacker->client->pers.netname;
        }
        else
        {
            killerName = "<non-client>";
        }
    }
    else
    {
        killer = ENTITYNUM_WORLD;
        killerName = "<world>";
    }

    if ( killer < 0 || killer >= MAX_CLIENTS )
    {
        killer = ENTITYNUM_WORLD;
        killerName = "<world>";
    }

    if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) )
    {
        obit = "<bad obituary>";
    }
    else
    {
        if ( attack == ATTACK_ALTERNATE )
        {
            obit = va ( "%s_ALT", modNames[ meansOfDeath ] );
        }
        else
        {
            obit = modNames[ meansOfDeath ];
        }
    }

    // If the weapon was charging then drop it with no forward velocity
    if ( self->client->ps.grenadeTimer )
    {
        gentity_t* missile;
        missile = G_FireWeapon( self, ATTACK_NORMAL );

        if ( attacker && attacker->client && attacker->client->sess.team != self->client->sess.team )
        {
            missile->dflags |= DAMAGE_NO_TEAMKILL;
        }

        if ( missile )
        {
            VectorClear ( missile->s.pos.trDelta );
        }
    }

    G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n",
        killer, self->s.number, meansOfDeath, killerName,
        self->client->pers.netname, obit );

    // broadcast the death event to everyone
    ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
    ent->s.eventParm = mod;
    ent->s.otherEntityNum = self->s.number;
    ent->s.otherEntityNum2 = killer;
    ent->r.svFlags = SVF_BROADCAST; // send to everyone

    self->enemy = attacker;

    if (attacker && attacker->client)
    {
        attacker->client->lastkilled_client = self->s.number;

        if ( attacker == self )
        {
            if ( mod != MOD_TEAMCHANGE && mod != MOD_TRIGGER_HURT_NOSUICIDE && !isCurrentGametypeInList((gameTypes_t[]){GT_HNS, GT_HNZ, GT_MAX}))
            {
                G_AddScore( attacker, g_suicidePenalty.integer );
            }
        }
        else if ( OnSameTeam ( self, attacker ) )
        {
            if ( mod != MOD_TELEFRAG && mod != MOD_TRIGGER_HURT_NOSUICIDE && !isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX }))
            {
                G_AddScore( attacker, g_teamkillPenalty.integer );
            }
            else if (isCurrentGametype(GT_HNS) && level.hns.cagefight) {
                G_AddScore(attacker, 10);
            }
        }
        else if (!isCurrentGametype(GT_HNS) || (isCurrentGametype(GT_HNS) && !level.hns.cagefight))
        {
            if (isCurrentGametype(GT_HNS) && self->client) {

                if (self->client->ps.stats[STAT_WEAPONS] & (1 << WP_M4_ASSAULT_RIFLE)) {
                    G_AddScore(attacker, 2);
                    attacker->client->sess.kills++;
                }
                else if (self->client->ps.stats[STAT_WEAPONS] & (1 << WP_RPG7_LAUNCHER)) {
                    G_AddScore(attacker, 3);
                    attacker->client->sess.kills += 2;
                }
                else {
                    G_AddScore(attacker, 1);
                }
                // JANFIXME add seekkills, roundkills from 1fx?

            }
            else if (isCurrentGametype(GT_VIP)) {
                if (self->client && self->client->pers.isVip) {
                    G_AddScore(attacker, 15);
                    level.vipKilledInTeam = self->client->sess.team;
                }
                else {
                    G_AddScore(attacker, 1);
                }
            }
            else {
                G_AddScore(attacker, 1);
            }
            
            attacker->client->sess.kills++;
            attacker->client->pers.statInfo.killsinarow++;
            attacker->client->pers.statInfo.kills++;

            attacker->client->lastKillTime = level.time;

            if (attacker->client->pers.statInfo.killsinarow >= 3 && !isCurrentGametype(GT_HNS)) {
                G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s\nis on \\fire\nwith %i kills in a row!", attacker->client->pers.netname, attacker->client->pers.statInfo.killsinarow);
            }
        }
    }
    else if (mod != MOD_TEAMCHANGE && mod != MOD_TRIGGER_HURT_NOSUICIDE && !isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_PROP, GT_MAX })) {
        G_AddScore( self, g_suicidePenalty.integer );
    }

    // If client is in a nodrop area, don't drop anything
    contents = trap_PointContents( self->r.currentOrigin, -1 );
    if ( !( contents & CONTENTS_NODROP ) )
    {
        // People who kill themselves dont drop guns
        if ( attacker == self && !isCurrentGametype(GT_HNS) )
        {
            self->client->ps.stats[STAT_WEAPONS] = 0;
        }

        TossClientItems( self );
    }
    else
    {
        // Any gametype items that are dropped into a no drop area need to be reported
        // to the gametype so it can handle it accordingly
        for ( i = 0 ; i < MAX_GAMETYPE_ITEMS ; i++ )
        {
            gitem_t* item;

            // skip this gametype item if the client doenst have it
            if ( !(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i)) )
            {
                continue;
            }

            item = BG_FindGametypeItem ( i );

            // Let the gametype handle the problem, if it doenst handle it and return 1 then
            // just reset the gametype item
            if ( !trap_GT_SendEvent ( GTEV_ITEM_STUCK, level.time, level.gametypeItems[item->giTag].id, 0, 0, 0, 0 ) )
            {
                G_ResetGametypeItem ( item );
            }
        }
    }

    Cmd_Score_f( self );

    // send updated scores to any clients that are following this one,
    // or they would get stale scoreboards
    for ( i = 0 ; i < level.numConnectedClients; i++ )
    {
        gclient_t   *client;

        client = g_entities[level.sortedClients[i]].client;

        if ( client->pers.connected != CON_CONNECTED )
        {
            continue;
        }

        if ( !G_IsClientSpectating ( client ) )
        {
            continue;
        }

        if ( client->sess.spectatorClient == self->s.number )
        {
            sendRoxLastSpec(self->s.number, ent->s.number);
            Cmd_Score_f( g_entities + i );
        }
    }

    self->s.weapon                  = WP_NONE;
    self->s.gametypeitems           = 0;

    // no gibbing right now
//  self->r.contents                = CONTENTS_CORPSE;
//  self->takedamage                = qtrue;            // can still be gibbed
    self->r.contents                = CONTENTS_CORPSE;
    self->takedamage                = qfalse;

    self->client->ps.zoomFov        = 0;            // Turn off zooming when we die
    self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
    self->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
    self->client->ps.loopSound = 0;

    self->s.angles[0]               = 0;
    self->s.angles[2]               = 0;
    self->s.loopSound               = 0;
    self->r.maxs[2]                 = -8;

    LookAtKiller (self, inflictor, attacker);

    VectorCopy( self->s.angles, self->client->ps.viewangles );

    // don't allow respawn until the death anim is done
    // g_forcerespawn may force spawning at some later time
    self->client->respawnTime = level.time + 1700;

    if (attacker && attacker->client) {
        statInfo_t* info = &attacker->client->pers.statInfo;

        if (self != attacker && self && attacker && attacker->client &&
            (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam(self, attacker)))) { // Make sure the attacker and self pointers are valid and actual clients.
            if ((hitLocation & (~HL_DISMEMBERBIT)) == HL_HEAD) {
                //add to the total headshot count for this player
                info->headShotKills++;
                info->weapon_headshots[attacker->client->pers.statInfo.attack * WP_NUM_WEAPONS + attacker->client->pers.statInfo.weapon]++;
            }

            switch (meansOfDeath) {
            case MOD_KNIFE:
                info->knifeKills++;
                break;
            case MOD_M4_ASSAULT_RIFLE:
                if (attack == ATTACK_ALTERNATE) {
                    info->explosiveKills++;
                }
                break;
            case MOD_MM1_GRENADE_LAUNCHER:
            case MOD_RPG7_LAUNCHER:
                //case MOD_M67_GRENADE:
            case MOD_M84_GRENADE:
                //case MOD_F1_GRENADE:
                //case MOD_L2A2_GRENADE:
                //case MOD_MDN11_GRENADE:
            case MOD_SMOHG92_GRENADE:
            case MOD_ANM14_GRENADE:
            case MOD_M15_GRENADE:
                if (mod == MOD_ANM14_GRENADE)
                {
                    info->hitcount++;
                    info->accuracy = (float)info->hitcount / (float)info->shotcount * 100;
                    info->weapon_hits[((mod > 256) ? ATTACK_ALTERNATE : ATTACK_NORMAL) * WP_NUM_WEAPONS + normalAttackMod(mod)]++;

                }

                info->explosiveKills++;
                break;
            default:
                break;
            }
        }

        if (attacker->client->pers.statInfo.deaths)
        {
            attacker->client->pers.statInfo.ratio = (float)attacker->client->pers.statInfo.kills / (float)attacker->client->pers.statInfo.deaths;
        }
        else
        {
            attacker->client->pers.statInfo.ratio = attacker->client->pers.statInfo.kills;
        }

        if (self && self->client && attacker != self && !isCurrentGametype(GT_HNS)) {
            G_printInfoMessage(self, "%s had ^3%i ^7health and ^3%i ^7armor left.", attacker->client->pers.cleanName, attacker->health, attacker->client->ps.stats[STAT_ARMOR]);
            self->client->pers.statInfo.lastKillerHealth = attacker->health;
            self->client->pers.statInfo.lastKillerArmor = attacker->client->ps.stats[STAT_ARMOR];
        }
    }

    //Careful if this "if" isnt here and they die from changing teams
    //before they have any other deaths we will divide by 0 since
    //we dont add deaths incurred from changing teams
    if (self->client->pers.statInfo.deaths)
    {
        self->client->pers.statInfo.ratio = (float)self->client->pers.statInfo.kills / (float)self->client->pers.statInfo.deaths;
    }
    else
    {
        self->client->pers.statInfo.ratio = (float)self->client->pers.statInfo.kills;
    }
    //Ryan
    

    switch ( hitLocation & (~HL_DISMEMBERBIT) )
    {
        case HL_WAIST:
            if ( rand() %2 )
            {
                anim = BOTH_DEATH_GROIN_1 + (rand()%2);
            }
            else
            {
                anim = BOTH_DEATH_GUT_1 + (rand()%2);   // GUT2 is being shot from the back.
            }
            break;

        default:
        case HL_CHEST:
            anim = BOTH_DEATH_CHEST_1 + (rand()%2);
            break;

        case HL_CHEST_RT:
            if ( irand(1,10) < 8 )
            {
                anim = BOTH_DEATH_SHOULDER_RIGHT_1 + (rand()%2);
            }
            else
            {
                anim = BOTH_DEATH_CHEST_1 + (rand()%2);
            }
            break;

        case HL_CHEST_LT:

            if ( irand(1,10) < 8 )
            {
                anim = BOTH_DEATH_SHOULDER_LEFT_1 + (rand()%2);
            }
            else
            {
                anim = BOTH_DEATH_CHEST_1 + (rand()%2);
            }

            break;

        case HL_NECK:
            anim = BOTH_DEATH_NECK;
            break;

        case HL_HEAD:
            anim = BOTH_DEATH_HEAD_1 + (rand()%2);
            break;

        case HL_LEG_UPPER_LT:
            anim = BOTH_DEATH_THIGH_LEFT_1 + (rand()%2);
            break;

        case HL_LEG_LOWER_LT:
        case HL_FOOT_LT:
            anim = BOTH_DEATH_LEGS_LEFT_1 + (rand()%3);
            break;

        case HL_ARM_LT:

            if ( rand()%2 )
                anim = BOTH_DEATH_ARMS_LEFT_1 + (rand()%2);
            else
                anim = BOTH_DEATH_SHOULDER_LEFT_1 + (rand()%2);

            break;

        case HL_ARM_RT:

            if ( rand()%2 )
                anim = BOTH_DEATH_ARMS_RIGHT_1 + (rand()%2);
            else
                anim = BOTH_DEATH_SHOULDER_RIGHT_1 + (rand()%2);

            break;

        case HL_LEG_UPPER_RT:
            anim = BOTH_DEATH_THIGH_RIGHT_1 + (rand()%2);
            break;

        case HL_LEG_LOWER_RT:
        case HL_FOOT_RT:
            anim = BOTH_DEATH_LEGS_RIGHT_1 + (rand()%3);
            break;
    }

    self->client->ps.legsAnim =
        ( ( self->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
    self->client->ps.torsoAnim =
        ( ( self->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

    // If the dismember bit is set then make sure the body queue dismembers
    // the location that was hit
    if ( hitLocation & HL_DISMEMBERBIT )
    {
        CopyToBodyQue (self, hitLocation & (~HL_DISMEMBERBIT), hitDir );
    }
    else
    {
        CopyToBodyQue (self, HL_NONE, hitDir );
    }

    // the body can still be gibbed
    self->die = body_die;

    trap_LinkEntity (self);
}

/*
================
CheckArmor
================
*/
int CheckArmor (gentity_t *ent, int damage, int dflags)
{
    gclient_t   *client;
    int         save;
    int         count;

    if (!damage)
        return 0;

    client = ent->client;

    if (!client)
        return 0;

    if (dflags & DAMAGE_NO_ARMOR)
        return 0;

    // armor
    count = client->ps.stats[STAT_ARMOR];
    save = ceil( damage * ARMOR_PROTECTION );
    if (save >= count)
        save = count;

    if (!save)
        return 0;

    client->ps.stats[STAT_ARMOR] -= save;

    return save;
}


void G_ApplyKnockback( gentity_t *targ, vec3_t newDir, float knockback )
{
    vec3_t  kvel;
    float   mass;

    if ( targ->physicsBounce > 0 )  //overide the mass
        mass = targ->physicsBounce;
    else
        mass = 200;

    if ( g_gravity.value > 0 )
    {
        VectorScale( newDir, g_knockback.value * (float)knockback / mass * 0.8, kvel );
//      kvel[2] = newDir[2] * g_knockback.value * (float)knockback / mass * 1.5;
    }
    else
    {
        VectorScale( newDir, g_knockback.value * (float)knockback / mass, kvel );
    }

    if ( targ->client )
    {
        VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );
    }
    else if ( targ->s.pos.trType != TR_STATIONARY && targ->s.pos.trType != TR_LINEAR_STOP )
    {
        VectorAdd( targ->s.pos.trDelta, kvel, targ->s.pos.trDelta );
        VectorCopy( targ->r.currentOrigin, targ->s.pos.trBase );
        targ->s.pos.trTime = level.time;
    }

    // set the timer so that the other client can't cancel
    // out the movement immediately
    if ( targ->client && !targ->client->ps.pm_time )
    {
        int     t;

        t = knockback * 2;
        if ( t < 50 ) {
            t = 50;
        }
        if ( t > 200 ) {
            t = 200;
        }
        targ->client->ps.pm_time = t;
        targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
    }
}

/*
================
RaySphereIntersections
================
*/
int RaySphereIntersections( vec3_t origin, float radius, vec3_t point, vec3_t dir, vec3_t intersections[2] ) {
    float b, c, d, t;

    //  | origin - (point + t * dir) | = radius
    //  a = dir[0]^2 + dir[1]^2 + dir[2]^2;
    //  b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
    //  c = (point[0] - origin[0])^2 + (point[1] - origin[1])^2 + (point[2] - origin[2])^2 - radius^2;

    // normalize dir so a = 1
    VectorNormalize(dir);
    b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
    c = (point[0] - origin[0]) * (point[0] - origin[0]) +
        (point[1] - origin[1]) * (point[1] - origin[1]) +
        (point[2] - origin[2]) * (point[2] - origin[2]) -
        radius * radius;

    d = b * b - 4 * c;
    if (d > 0) {
        t = (- b + sqrt(d)) / 2;
        VectorMA(point, t, dir, intersections[0]);
        t = (- b - sqrt(d)) / 2;
        VectorMA(point, t, dir, intersections[1]);
        return 2;
    }
    else if (d == 0) {
        t = (- b ) / 2;
        VectorMA(point, t, dir, intersections[0]);
        return 1;
    }
    return 0;
}

int G_GetHitLocation(gentity_t *target, vec3_t ppoint, vec3_t dir )
{
    float       fdot;
    float       rdot;
    vec3_t      tangles;
    vec3_t      forward;
    vec3_t      up;
    vec3_t      right;
    vec3_t      distance;
    vec3_t      tcenter;
    vec3_t      temp;
    vec3_t      hit;

    // We are only interested in the YAW angle of the target
    VectorSet( tangles, 0, target->client->ps.viewangles[YAW], 0);

    // Extract the forward, right, and up vectors
    AngleVectors ( tangles, forward, right, up );

    // Determine the center of the target entity
    VectorAdd(target->r.absmin, target->r.absmax, tcenter);
    VectorScale(tcenter, 0.5, tcenter);


/* NOTE: This would work to figure out shots that go across the front of someone and
         hit the opposite side, but had an error in it when a shot came from either
         the immediate left or right of the player.
*/

    // Calculate the distnace from the shooter to the target
    VectorCopy ( dir, temp );
    VectorSubtract ( tcenter, ppoint, distance );

    // Use that distnace to determine the point of tangent in relation to
    // the center of the player entity
    VectorMA ( ppoint, DotProduct ( temp, distance ), temp, hit );

    // Create a vector from the tangent point to the center.  This will
    // be used to determine which side was hit
    VectorSubtract ( tcenter, hit, temp );
    VectorCopy ( temp, distance );

    VectorSubtract ( tcenter, ppoint, temp );
    VectorNormalize ( temp );

    // Determine the shot in relation to the forward vector
    fdot = DotProduct ( forward, temp );

    // Determine the shot in relation to the right vector
    rdot = DotProduct ( right, temp );

    if ( distance[2] < -35 )
    {
        return HL_HEAD;
    }
    else if ( distance[2] < -32 )
    {
        return HL_NECK;
    }
    else if ( distance[2] < -27 )
    {
        if ( rdot > 0 )
            return HL_ARM_LT;

        return HL_ARM_RT;
    }
    else if ( distance[2] < -3 )
    {
        if ( fdot > 0 )
        {
            if ( rdot > 0 )
            {
                return HL_CHEST_LT;
            }

            return HL_CHEST_RT;
        }

        if ( rdot > 0 )
        {
            return HL_BACK_LT;
        }

        return HL_BACK_RT;
    }
    else if ( distance[2] < 4 )
    {
        return HL_WAIST;
    }
    else if ( distance[2] < 18 )
    {
        if ( rdot > 0 )
            return HL_LEG_UPPER_LT;

        return HL_LEG_UPPER_RT;
    }
    else if ( distance[2] < 33 )
    {
        if ( rdot > 0 )
            return HL_LEG_LOWER_LT;

        return HL_LEG_LOWER_RT;
    }

    if ( rdot > 0 )
        return HL_FOOT_LT;

    return HL_FOOT_RT;
}

/*
============
T_Damage

targ        entity that is being damaged
inflictor   entity that is causing the damage
attacker    entity that caused the inflictor to damage targ
    example: targ=monster, inflictor=rocket, attacker=player

dir         direction of the attack for knockback
point       point at which the damage is being inflicted, used for headshots
damage      amount of damage being inflicted
knockback   force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags      these flags are used to control how T_Damage works
    DAMAGE_RADIUS           damage was indirect (from a nearby explosion)
    DAMAGE_NO_ARMOR         armor does not protect from this damage
    DAMAGE_NO_KNOCKBACK     do not affect velocity, just view angles
    DAMAGE_NO_PROTECTION    kills godmode, armor, everything
============
*/

int G_Damage (
    gentity_t       *targ,
    gentity_t       *inflictor,
    gentity_t       *attacker,
    vec3_t          dir,
    vec3_t          point,
    int             damage,
    int             dflags,
    int             mod,
    int             location
    )
{
    gclient_t       *client;
    int             take;
    int             save;
    int             asave = 0;
    int             knockback;

    if (!targ->takedamage)
    {
        return 0;
    }

    // See if they are invulnerable
    if ( (mod&0xFF) < MOD_WATER )
    {
        if ( targ->client && (level.time - targ->client->invulnerableTime < g_respawnInvulnerability.integer * 1000) )
        {
            return 0;
        }
    }

    // Cant change outfitting after being shot
    if ( targ->client )
    {
        targ->client->noOutfittingChange = qtrue;
    }

    if ( !inflictor )
    {
        inflictor = &g_entities[ENTITYNUM_WORLD];
    }
    if ( !attacker )
    {
        attacker = &g_entities[ENTITYNUM_WORLD];
    }

    // shootable doors / buttons don't actually have any health
    if ( targ->s.eType == ET_MOVER )
    {
        if ( targ->use && targ->moverState == MOVER_POS1 )
        {
            targ->use( targ, inflictor, attacker );
        }
        return 0;
    }

    client = targ->client;

    if ( client )
    {
        if ( client->noclip )
        {
            return 0;
        }
    }

    if ( !dir )
    {
        dflags |= DAMAGE_NO_KNOCKBACK;
    }
    else
    {
        VectorNormalize(dir);
    }

    knockback = damage;
    if ( knockback > 200 )
    {
        knockback = 200;
    }
    if ( targ->flags & FL_NO_KNOCKBACK ) {
        knockback = 0;
    }
    if ( dflags & DAMAGE_NO_KNOCKBACK ) {
        knockback = 0;
    }

/*
    // figure momentum add, even if the damage won't be taken
    if ( knockback && targ->client )
    {
        G_ApplyKnockback ( targ, dir, knockback );
        vec3_t  kvel;
        float   mass;

        mass = 200;

        VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
        VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

        // set the timer so that the other client can't cancel
        // out the movement immediately
        if ( !targ->client->ps.pm_time ) {
            int     t;

            t = knockback * 2;
            if ( t < 50 ) {
                t = 50;
            }
            if ( t > 200 ) {
                t = 200;
            }
            targ->client->ps.pm_time = t;
            targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
        }
    }
*/


    if (damage < 1)
    {
        damage = 1;
    }
    int originalDamage = damage;
    // Start H&S specifics.
    if (isCurrentGametype(GT_HNS) && attacker && attacker->client && client) {
        float radius = 1000.0f;
        
        if (!level.hns.cagefight) {
            damage = 0;
        }
        
        if (client->sess.team == TEAM_RED && attacker->client->sess.team == TEAM_BLUE && mod == MOD_USSOCOM_PISTOL) {
            // Stungun.
            addSpeedAlteration(targ, qtrue, SPEEDALTERATION_STUNGUN);
            removeWeaponFromClient(attacker, WP_USSOCOM_PISTOL, qfalse, WP_KNIFE);
            G_Broadcast(BROADCAST_GAME, targ, qfalse, "You got \\tased\nby %s!", attacker->client->pers.netname);
            G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You have tased %s!", client->pers.netname);

            G_PlayEffect(G_EffectIndex("misc/electrical"), targ->r.currentOrigin, targ->r.currentAngles);

            gentity_t* tent = G_TempEntity(targ->r.currentOrigin, EV_GENERAL_SOUND);
            tent->r.svFlags |= SVF_BROADCAST;
            tent->s.time2 = (int)(radius * 1000.0f);
            G_AddEvent(tent, EV_GENERAL_SOUND, G_SoundIndex("sound/ambience/generic/sparks2.mp3"));
        } else if (mod == altAttack(MOD_KNIFE) && client->sess.team == TEAM_RED && attacker->client->sess.team == TEAM_RED && g_hsgiveknife.integer && !g_friendlyFire.integer) {
            int ammoIdx = weaponData[WP_KNIFE].attack[ATTACK_ALTERNATE].ammoIndex;
            G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You gave a knife to %s!", client->pers.netname);
            G_Broadcast(BROADCAST_GAME, targ, qfalse, "%s gave you a knife!", attacker->client->pers.netname);

            if (client->ps.ammo[ammoIdx] < 5) {
                client->ps.ammo[ammoIdx]++;
            }
        } else if (client->sess.team == TEAM_BLUE && attacker->client->sess.team == TEAM_RED) {
            if (!client->pers.seekerAway && level.customGameStarted) {
                qboolean stunned = qfalse;
                speedAlterationReason_t speedAlterationReason = SPEEDALTERATION_NONE;
                if (mod == MOD_KNIFE) {
                    stunned = qtrue;
                    speedAlterationReason = SPEEDALTERATION_KNIFE;
                    int ammoIdx = weaponData[WP_KNIFE].attack[ATTACK_ALTERNATE].ammoIndex;
                    if (attacker->client->ps.ammo[ammoIdx] < 5) {
                        attacker->client->ps.ammo[ammoIdx]++;
                    }
                    else {
                        attacker->client->pers.knifeBox = qtrue;
                    }
                }
                else if (mod == altAttack(MOD_KNIFE)) {
                    stunned = qtrue;
                    speedAlterationReason = SPEEDALTERATION_KNIFE;
                }
                else if (mod == MOD_M4_ASSAULT_RIFLE) {
                    stunned = qtrue;
                    speedAlterationReason = SPEEDALTERATION_M4;
                }
                
                if (stunned) {
                    addSpeedAlteration(targ, qtrue, speedAlterationReason);
                    addSpeedAlteration(attacker, qfalse, speedAlterationReason);
                    G_Broadcast(BROADCAST_GAME, targ, qfalse, "You got stunned by %s!", attacker->client->pers.netname);
                    targ->client->sess.gotStunned++;
                    G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You have stunned %s!", targ->client->pers.netname);
                    attacker->client->sess.stunAttacks++;
                }

            }
        }
        else if (client->sess.team == TEAM_RED && attacker->client->sess.team == TEAM_BLUE) {
            if (mod == MOD_KNIFE) {
                damage = originalDamage;
                if (!level.hns.MM1Given && hideSeek_Weapons.string[HSWPN_MM1] == '1') {
                    level.hns.MM1Given = qtrue;
                    giveWeaponToClient(attacker, WP_MM1_GRENADE_LAUNCHER, qfalse);
                    Q_strncpyz(level.hns.MM1loc, attacker->client->pers.netname, sizeof(level.hns.MM1loc));
                    level.hns.MM1ent = -1;
                    G_printGametypeMessageToAll("First blood: %s has taken the MM1", attacker->client->pers.cleanName);
                    G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You now have the \\MM1!");
                    attacker->client->sess.takenMM1++;
                }
            }
        }
        else if (client->sess.team == attacker->client->sess.team && client->sess.team == TEAM_RED) {
            if (mod == WP_M3A1_SUBMACHINEGUN) {
                TeleportPlayerToPlayer(attacker, targ, qtrue, qtrue);
                removeWeaponFromClient(attacker, WP_M3A1_SUBMACHINEGUN, qfalse, WP_KNIFE);
                G_printGametypeMessageToAll("%s used the telegun to teleport to %s.", attacker->client->pers.cleanName, client->pers.cleanName);
            }
            else if (client->ps.weaponstate == WEAPON_READY && mod == WP_KNIFE) {

                if (
                    (attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_M4_ASSAULT_RIFLE) && client->ps.weapon == WP_RPG7_LAUNCHER) ||
                    (attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_RPG7_LAUNCHER) && client->ps.weapon == WP_M4_ASSAULT_RIFLE)
                    ) {
                    if (!g_friendlyFire.integer) {
                        return 0;
                    }
                } else if (client->ps.weapon == WP_RPG7_LAUNCHER) {
                    stealWeaponWithAmmo(targ, attacker, WP_RPG7_LAUNCHER);
                    G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You stole the \\RPG!");
                    G_Broadcast(BROADCAST_GAME, targ, qfalse, "%s stole your \\RPG!", attacker->client->pers.netname);
                    G_printGametypeMessageToAll("%s took the RPG from %s.", attacker->client->pers.cleanName, client->pers.cleanName);
                    attacker->client->sess.weaponsStolen++;
                }
                else if (client->ps.weapon == WP_M4_ASSAULT_RIFLE) {
                    stealWeaponWithAmmo(targ, attacker, WP_M4_ASSAULT_RIFLE);
                    G_Broadcast(BROADCAST_GAME, attacker, qfalse, "You stole the \\M4!");
                    G_Broadcast(BROADCAST_GAME, targ, qfalse, "%s stole your \\M4!", attacker->client->pers.netname);
                    G_printGametypeMessageToAll("%s took the M4 from %s.", attacker->client->pers.cleanName, client->pers.cleanName);
                    attacker->client->sess.weaponsStolen++;
                }
            }
        }
        else if (!g_friendlyFire.integer && mod == MOD_TELEFRAG && !level.hns.cagefight) {
            damage = originalDamage;
        } else if (g_friendlyFire.integer) {
            damage = originalDamage;
        }

        if (level.hns.cagefight) {
            if (mod == altAttack(MOD_AK74_ASSAULT_RIFLE)) {
                damage = originalDamage;
            }
            else {
                damage = 0;
            }
        }

        if (attacker->client->sess.team != client->sess.team) {
            if (mod == MOD_ANM14_GRENADE) {
                addSpeedAlteration(targ, qtrue, SPEEDALTERATION_FIRENADE);
            }
            else if (mod == MOD_RPG7_LAUNCHER || mod == MOD_SMOHG92_GRENADE) {
                G_ApplyKnockback(targ, dir, 50.0f);
                targ->client->ps.velocity[2] = 230;
                vec3_t kvel;
                VectorScale(dir, g_knockback.value * 50.0f / 200.0f, kvel);
                VectorAdd(targ->client->ps.velocity, kvel, targ->client->ps.velocity);

                if (!targ->client->ps.pm_time) {
                    int pmTime = knockback * 2;
                    Com_Clamp(50, 200, pmTime);
                    targ->client->ps.pm_time = pmTime;
                    targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
                }
            }
            
        }

    }

    // check for completely getting out of the damage
    if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

        // if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
        // if the attacker was on the same team
        if ( targ != attacker && OnSameTeam (targ, attacker) && !level.hns.cagefight  )
        {
            if ( !g_friendlyFire.integer || level.warmupTime )
            {
                return 0;
            }
        }

        // check for godmode
        if ( targ->flags & FL_GODMODE )
        {
            return 0;
        }
    }


    take = damage;
    save = 0;

    // Be careful with grenades
    if ( attacker == targ )
    {
        take *= 2;
    }

    // save some from armor
    if (!isCurrentGametype(GT_HNS)) {
        asave = CheckArmor(targ, take, dflags);
        take -= asave;
    }
    
    int actualtake = Com_Clamp(0, targ->health, take);
    qboolean teamDamage = qfalse;

    // Teamkill dmage thats not caused by a telefrag?
    if ( g_teamkillDamageMax.integer && mod != MOD_TELEFRAG && !(dflags&DAMAGE_NO_TEAMKILL) )
    {
        if ( level.gametypeData->teams && targ && attacker && targ != attacker )
        {
            // Hurt your own team?
            if ( OnSameTeam ( targ, attacker ) )
            {

                if ( targ->client->ps.stats[STAT_GAMETYPE_ITEMS] )
                {
                    actualtake *= 2;
                }

                // See if this damage falls into the no excuse damage
                if ( level.gametypeData->respawnType == RT_NONE && level.time - level.gametypeDelayTime < g_teamkillNoExcuseTime.integer * 1000 )
                {
                    actualtake *= g_teamkillNoExcuseMultiplier.integer;
                }

                attacker->client->sess.teamkillDamage      += actualtake;
                attacker->client->sess.teamkillForgiveTime  = level.time;

                teamDamage = qtrue;
            }
        }
    }

    // Output hits
    if ( g_logHits.integer && attacker && targ && attacker->client && targ->client )
    {
        G_LogPrintf ( "hit: %i %i %i %i %i: %s hit %s at location %i for %i\n",
                          attacker->s.number,
                          targ->s.number,
                          location,
                          take,
                          asave,
                          attacker->client->pers.netname,
                          targ->client->pers.netname,
                          location,
                          (int)((float)take) );
    }

    if ( g_debugDamage.integer )
    {
        Com_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number, targ->health, take, asave );
    }

    // add to the damage inflicted on a player this frame
    // the total will be turned into screen blends and view angle kicks
    // at the end of the frame
    if ( client )
    {
        if ( attacker )
        {
            client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
        }
        else
        {
            client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
        }

        if ( mod != MOD_WATER )
        {
            client->damage_armor += asave;
            client->damage_blood += take;
        }

        client->damage_knockback += knockback;

        if ( dir )
        {
            VectorCopy ( dir, client->damage_from );
            client->damage_fromWorld = qfalse;
        }
        else
        {
            VectorCopy ( targ->r.currentOrigin, client->damage_from );
            client->damage_fromWorld = qtrue;
        }

        if (attacker && attacker->client)
        {
            BotDamageNotification(client, attacker);
        }
        else if (inflictor && inflictor->client)
        {
            BotDamageNotification(client, inflictor);
        }
    }

    if (targ && targ->client)
    {
        // set the last client who damaged the target
        targ->client->lasthurt_client = attacker->s.number;
        targ->client->lasthurt_time = level.time;
        targ->client->lasthurt_mod = mod;

        if (attacker && attacker->client) {
            targ->client->pers.statInfo.lasthurtby = attacker->s.number;
            attacker->client->pers.statInfo.lastclient_hurt = targ->s.number;
        }
    }

    // do the damage
    if (take)
    {
        targ->health = targ->health - take;

        if ( targ->client )
        {

            //Ryan
            //Punish them for killing themselves by not adding to
            //their damagedone
            if (targ == attacker)
            {
                targ->client->pers.statInfo.damageTaken += actualtake;
            }
            ////Punish them for killing a teammate and dont add to the
            ////victims damage taken
            ///RxCxW - 11.01.05 - 06:21am - #DLL barfs on this
            else if (level.gametypeData->teams && OnSameTeam(targ, attacker))
            {
                attacker->client->pers.statInfo.damageDone -= actualtake;
            }
            ///RxCxW - 01.08.06 - 09:50pm - all below as added by me
            
            else if (attacker && attacker->client) {
                ///End  - 01.10.06 - 02:26pm
                attacker->client->pers.statInfo.damageDone += actualtake;
                targ->client->pers.statInfo.damageTaken += actualtake;

            }


            targ->client->ps.stats[STAT_HEALTH] = targ->health;

            if ( targ->health > 0 )
            {
                // 45 damage is full slowdown, so..
                float slowdown;

                slowdown = (float)damage / 20.0f;
                slowdown  = Com_Clampf ( 0.0f, 1.0f, slowdown );
                slowdown *= 0.75f;
                slowdown = 1.0f - slowdown;

                // Slow down the client at bit when they get hit
                targ->client->ps.velocity[0] *= slowdown;
                targ->client->ps.velocity[1] *= slowdown;

                // figure momentum add, even if the damage won't be taken
                if ( knockback )
                {
                    G_ApplyKnockback ( targ, dir, knockback );
                }

                // Friendly fire
                if ( !level.warmupTime && g_friendlyFire.integer && targ != attacker && OnSameTeam ( targ, attacker ) )
                {
                    vec3_t diff;

                    // Make sure the attacker is close enough to hear the guy whining
                    VectorSubtract ( targ->r.currentOrigin, attacker->r.currentOrigin, diff );
                    if ( VectorLengthSquared ( diff ) < 800 * 800 )
                    {
                        G_VoiceGlobal ( targ, "check_fire", qfalse );
                    }
                }
            }
        }

        if ( targ->health <= 0 )
        {
            // Something dismembered?
            if ( (targ->health < DISMEMBER_HEALTH && !(dflags&DAMAGE_NO_GORE)) || (dflags&DAMAGE_FORCE_GORE) )
            {
                location |= HL_DISMEMBERBIT;
            }

            if ( client )
                targ->flags |= FL_NO_KNOCKBACK;

            if (targ->health < -999)
                targ->health = -999;

            targ->enemy = attacker;
            targ->die (targ, inflictor, attacker, take, mod, location, dir );

            if (teamDamage) {
                if (attacker->client->pers.statInfo.killsinarow > attacker->client->pers.statInfo.bestKillsInARow)
                    attacker->client->pers.statInfo.bestKillsInARow = attacker->client->pers.statInfo.killsinarow;

                attacker->client->pers.statInfo.killsinarow = 0;
            }
        }
        else if ( targ->pain )
        {
            targ->pain (targ, attacker, take);
        }
    }

    return take;
}

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
    vec3_t  dest;
    trace_t tr;
    vec3_t  midpoint;

    // use the midpoint of the bounds instead of the origin, because
    // bmodels may have their origin is 0,0,0
    VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
    VectorScale (midpoint, 0.5, midpoint);

    VectorCopy (midpoint, dest);
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0 || tr.entityNum == targ->s.number)
        return qtrue;

    // this should probably check in the plane of projection,
    // rather than in world coordinate, and also include Z
    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[2] = targ->r.absmax[2];
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[2] = targ->r.absmin[2];
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;


    return qfalse;
}
/*
============
G_MultipleDamageLocations
============
*/
int G_MultipleDamageLocations(int hitLocation)
{

    switch ( hitLocation & (~HL_DISMEMBERBIT) )
    {
        case HL_FOOT_RT:
        case HL_FOOT_LT:
            hitLocation |= (HL_FOOT_RT | HL_FOOT_LT);
            break;
        case HL_LEG_UPPER_RT:
            hitLocation |= (HL_LEG_UPPER_RT | HL_LEG_LOWER_LT);
            if ( rand() %2 )
            {
                hitLocation |= HL_HAND_RT;
            }
            break;
        case HL_LEG_UPPER_LT:
            hitLocation |= (HL_LEG_UPPER_LT | HL_LEG_LOWER_RT);
            if ( rand() %2 )
            {
                hitLocation |= HL_HAND_LT;
            }
            break;
        case HL_LEG_LOWER_RT:
            hitLocation |= (HL_LEG_LOWER_RT | HL_FOOT_LT);
            break;
        case HL_LEG_LOWER_LT:
            hitLocation |= (HL_LEG_LOWER_LT | HL_FOOT_RT);
            break;
        case HL_HAND_RT:
            hitLocation |= HL_HAND_RT;
            break;
        case HL_HAND_LT:
            hitLocation |= HL_HAND_LT;
            break;
        case HL_ARM_RT:
            hitLocation |= (HL_ARM_RT | HL_LEG_UPPER_RT) ;
            break;
        case HL_ARM_LT:
            hitLocation |= (HL_ARM_LT | HL_LEG_UPPER_LT) ;
            break;
        case HL_HEAD:
            hitLocation |= HL_HEAD ;
            if ( rand() %2 )
            {
                hitLocation |= HL_ARM_RT;
            }
            else
            {
                hitLocation |= HL_ARM_LT;
            }
            break;
        case HL_WAIST:
            hitLocation |= (HL_LEG_UPPER_RT | HL_LEG_UPPER_LT) ;

            if ( rand() %2 )
            {
                if ( rand() %2 )
                {
                    hitLocation |= HL_HAND_RT;
                }
                else
                {
                    hitLocation |= HL_HAND_LT;
                }
            }
            break;
        case HL_BACK_RT:
        case HL_CHEST_RT:
            hitLocation |= HL_ARM_RT;
            hitLocation |= HL_HEAD;
            break;
        case HL_BACK_LT:
        case HL_CHEST_LT:
            hitLocation |= HL_ARM_LT;
            hitLocation |= HL_HEAD;
            break;
        case HL_BACK:
        case HL_CHEST:
            hitLocation |= (HL_ARM_RT | HL_ARM_LT);
            hitLocation |= HL_HEAD;
            break;

    }

    return (hitLocation);
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage (
    vec3_t      origin,
    gentity_t*  attacker,
    float       damage,
    float       radius,
    gentity_t*  ignore,
    int         power,
    int         dflags,
    int         mod
    )
{
    float       points, dist;
    gentity_t   *ent, *tent;
    int         entityList[MAX_GENTITIES];
    int         numListedEntities;
    vec3_t      mins, maxs;
    vec3_t      v;
    vec3_t      dir = { 0.0f, 0.0f, 0.0f };
    int         i, e;
    qboolean    hitClient = qfalse;

    // H&S specifics.
    vec3_t mins1, maxs1, mins2, maxs2;
    vec3_t rpgAngs, rpgDir;
    qboolean cageOutOfBoundaries = qfalse;
    qboolean nadeOutOfBoundaries = qfalse;
    int ammoIdx;
    gentity_t* missile;
    int countCaught = 0;
    int lastCaught = -1;
    int     cageCaughtEntityList[MAX_GENTITIES];
    // END H&S specifics

    if ( radius < 1 )
    {
        radius = 1;
    }

    
    if (isCurrentGametype(GT_HNS) && attacker && (mod == WP_M67_GRENADE || mod == altAttack(WP_M67_GRENADE))) {
        origin = attacker->r.currentOrigin;
    }
    else {
        for (i = 0; i < 3; i++)
        {
            mins[i] = origin[i] - radius;
            maxs[i] = origin[i] + radius;
        }
    }
    

    if (isCurrentGametype(GT_HNS)) {
        if (mod == altAttack(MOD_M4_ASSAULT_RIFLE)) {
            mins1[0] = origin[0] - 115;
            mins1[1] = origin[1] - 25;
            mins1[2] = origin[2] - 25;
            maxs1[0] = origin[0] + 115;
            maxs1[1] = origin[1] + 25;
            maxs1[2] = origin[2] + 213;

            for (i = 0; i < 3; i++) {
                mins2[i] = origin[i] - 90;
                maxs2[i] = origin[i] + 90;
            }

            Com_Memset(cageCaughtEntityList, 0, sizeof(cageCaughtEntityList));
        }
        else if (mod == MOD_MDN11_GRENADE || mod == altAttack(MOD_MDN11_GRENADE)) {
            // Boe!Man 3/21/14: Custom check for the MDN11 grenade.
            mins1[0] = origin[0] - 22;
            mins1[1] = origin[1] - 17;
            mins1[2] = origin[2] - 9;

            maxs1[0] = origin[0] + 23;
            maxs1[1] = origin[1] + 58;
            maxs1[2] = origin[2] - 9;
        }
        else if (mod == MOD_M67_GRENADE || mod == altAttack(MOD_M67_GRENADE)) {
            for (i = 0; i < 3; i++) {
                mins[i] = origin[i] - DEFAULT_PLAYER_Z_MAX * 2;
                maxs[i] = origin[i] + DEFAULT_PLAYER_Z_MAX * 2;
            }
        }
        maxs1[2] += 50;
    }

    numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

    for ( e = 0 ; e < numListedEntities ; e++ )
    {
        ent = &g_entities[entityList[ e ]];

        if (isCurrentGametype(GT_HNS)) {
            if (ent->client) {
                if (mod == altAttack(MOD_M4_ASSAULT_RIFLE)) {
                    if (ent->client->sess.team == TEAM_RED && !G_IsClientDead(ent->client)) {
                        cageOutOfBoundaries = qtrue;
                        break;
                    }
                    else if (ent->client->sess.team == TEAM_BLUE) {
                        cageOutOfBoundaries = qtrue;
                        int num = trap_EntitiesInBox(mins2, maxs2, cageCaughtEntityList, MAX_GENTITIES);

                        for (int cageCaught = 0; cageCaught < num; cageCaught++) {
                            gentity_t* tent = &g_entities[cageCaughtEntityList[cageCaught]];

                            if (tent->client && tent->client->sess.team == TEAM_BLUE && tent->s.number == ent->s.number) {
                                cageOutOfBoundaries = qfalse;
                                countCaught++;
                                ent->client->sess.trappedInCage++;
                                lastCaught = ent->s.number;
                                break;
                            }
                        }

                        if (cageOutOfBoundaries) {
                            break;
                        }

                    }
                }
                else if (mod == MOD_MM1_GRENADE_LAUNCHER) {
                    if (ent->client->sess.team == TEAM_RED) {
                        ent->client->sess.MM1HitsTaken++;

                        VectorCopy(ent->client->ps.viewangles, rpgAngs);
                        rpgDir[0] = 0.0;
                        rpgDir[1] = 0.0;
                        rpgDir[2] = 0.75;
                        VectorNormalize(rpgDir);
                        G_ApplyKnockback(ent, rpgDir, 115);
                        ent->client->ps.velocity[1] -= 50.0f;
                        break;
                    }
                }
                
                else if (mod == MOD_MDN11_GRENADE || mod == altAttack(MOD_MDN11_GRENADE)) {
                    if (ent->client) {
                        if (ent->client->sess.team == TEAM_RED || ent->client->sess.team == TEAM_BLUE) {
                            nadeOutOfBoundaries = qtrue;
                            break; // break the loop to save performance, no more loops needed because box already failed no matter what.
                        }
                    }
                }
                else if (mod == MOD_L2A2_GRENADE || mod == altAttack(MOD_L2A2_GRENADE)) {
                    if (ent == attacker) {
                        G_CloseSound(attacker->r.currentOrigin, G_SoundIndex("sound/weapons/rpg7/fire01.mp3"));
                        attacker->client->ps.pm_flags |= PMF_JUMPING;
                        attacker->client->ps.groundEntityNum = ENTITYNUM_NONE;
                        attacker->client->ps.velocity[2] = g_rpgBoost.integer;
                        break;
                    }
                }
                else if (mod == MOD_M67_GRENADE || mod == altAttack(MOD_M67_GRENADE)) {
                    // Check if a player was caught in the radius.
                    if (ent != attacker && ent->client) {
                        if (ent && ent->client) {
                            nadeOutOfBoundaries = qtrue;
                            break;
                        }
                    }
                }

                else if (mod == MOD_RPG7_LAUNCHER && ent == attacker) {
                    attacker->client->ps.pm_flags |= PMF_JUMPING;
                    attacker->client->ps.groundEntityNum = ENTITYNUM_NONE;
                    attacker->client->ps.velocity[2] = g_rpgBoost.integer;
                    attacker->client->sess.rpgBoosts++;
                }
            }
            else {
                if (mod == MOD_RPG7_LAUNCHER && ent->classname && strstr(ent->classname, "booster") && !(attacker->client->ps.pm_flags & PMF_JUMPING)) {
                    attacker->client->ps.pm_flags |= PMF_JUMPING;
                    attacker->client->ps.groundEntityNum = ENTITYNUM_NONE;
                    attacker->client->ps.velocity[2] = g_rpgBoost.integer;
                    attacker->client->sess.rpgBoosts++;
                }
                else if ((mod == MOD_F1_GRENADE || mod == altAttack(MOD_F1_GRENADE)) && ent->classname && strstr(ent->classname, "f1")) { // Boe!Man 8/2/12: Fix for Altattack of tele nade not doing anything.
                    if (origin[2] <= ent->origin_from[2]) {
                        vec3_t          org1, org2;
                        trace_t         tr;

                        mins[0] = -12;
                        mins[1] = -12;
                        mins[2] = -31;

                        maxs[0] = 12;
                        maxs[1] = 12;
                        maxs[2] = 32;

                        VectorCopy(origin, org1);
                        VectorCopy(origin, org2);
                        org1[2] += 50;
                        trap_Trace(&tr, org1, mins, maxs, org2, attacker->s.number, MASK_PLAYERSOLID); // Boe!Man 7/23/13: Used to be MASK_ALL, and before that MASK_SOLID. This seems to work best (MASK_PLAYERSOLID).
                        if (!tr.startsolid && !tr.allsolid) {
                            runF1Teleport(attacker, origin);
                        }
                        else if (level.time > attacker->client->pers.lastpickup) {
                            ammoIdx = weaponData[WP_F1_GRENADE].attack[ATTACK_ALTERNATE].ammoIndex;
                            attacker->client->ps.ammo[ammoIdx] += 1;
                            if (!(attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_F1_GRENADE))) { // Boe!Man 8/22/11: Make sure the attacker has the weapon, if not, re-add it (fixes bug which made weapon disappear on last throw).
                                attacker->client->ps.stats[STAT_WEAPONS] |= (1 << WP_F1_GRENADE);
                            }

                            G_printInfoMessage(attacker, "Surface is not empty.");
                            attacker->client->pers.lastpickup = level.time + 50;
                        }
                    }
                    else if (level.time > attacker->client->pers.lastpickup) {
                        ammoIdx = weaponData[WP_F1_GRENADE].attack[ATTACK_ALTERNATE].ammoIndex;
                        attacker->client->ps.ammo[ammoIdx] += 1;
                        if (!(attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_F1_GRENADE))) { // Boe!Man 8/22/11: Make sure the attacker has the weapon, if not, re-add it (fixes bug which made weapon disappear on last throw).
                            attacker->client->ps.stats[STAT_WEAPONS] |= (1 << WP_F1_GRENADE);
                        }
                        G_printInfoMessage(attacker, "Surface is too high.");
                        attacker->client->pers.lastpickup = level.time + 50;
                    }
                }
            }
            
        }

        if (ent == ignore)
        {
            continue;
        }

        if (!ent->takedamage)
        {
            continue;
        }

        // find the distance from the edge of the bounding box
        for ( i = 0 ; i < 3 ; i++ )
        {
            if ( origin[i] < ent->r.absmin[i] )
            {
                v[i] = ent->r.absmin[i] - origin[i];
            }
            else if ( origin[i] > ent->r.absmax[i] )
            {
                v[i] = origin[i] - ent->r.absmax[i];
            }
            else
            {
                v[i] = 0;
            }
        }

        dist = VectorLength( v );
        if ( dist >= radius )
        {
            continue;
        }

        points = damage * ( 1.0 - powf((dist / radius), power));

        if( CanDamage (ent, origin) )
        {
            int     location;
            int     weapon;
            vec3_t  hitdir;
            int     d;

            VectorSubtract (ent->r.currentOrigin, origin, dir);
            // push the center of mass higher than the origin so players
            // get knocked into the air more

            location = HL_NONE;
            if ( ent->client )
            {
                VectorNormalize ( dir );
                VectorCopy(dir, hitdir);
                dir[2] = 0;
                location = G_GetHitLocation ( ent, origin, dir );
                location = G_MultipleDamageLocations ( location );
            }

            d = G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS|DAMAGE_NO_ARMOR|dflags, mod, location );

            if ( d && ent->client )
            {
                statInfo_t* stat = &attacker->client->pers.statInfo;


                //Fire nades only count a hit if they kill
                if (mod != MOD_ANM14_GRENADE && (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam(attacker, ent))))
                {
                    stat->hitcount++;
                    stat->accuracy = (float)stat->hitcount / (float)stat->shotcount * 100;
                    stat->weapon_hits[((mod > 256) ? ATTACK_ALTERNATE : ATTACK_NORMAL) * WP_NUM_WEAPONS + normalAttackMod(mod)]++;
                }
                //Ryan

                // Only one of the grenade hits will count for tk damage
                if ( ent != attacker )
                {
                    dflags |= DAMAGE_NO_TEAMKILL;
                }

                // Put some procedural gore on the target.
                tent = G_TempEntity( origin, EV_EXPLOSION_HIT_FLESH );

                // send entity and direction
                tent->s.eventParm = DirToByte( hitdir );
                if (ignore && ignore->s.weapon)
                {
                    weapon = ignore->s.weapon;      // Weapon type number
                }
                else if (points >= 10)
                {   // dangerous weapon
                    weapon = WP_SMOHG92_GRENADE;
                }
                else
                {   // Just a flesh wound
                    weapon = WP_M84_GRENADE;
                }
                tent->s.otherEntityNum2 = ent->s.number;            // Victim entity number

                // Pack the shot info into the temp end for gore
                tent->s.time  = weapon + ((((int)ent->s.apos.trBase[YAW]&0x7FFF) % 360) << 16);
                if ( attacker->s.eFlags & EF_ALT_FIRING )
                {
                    tent->s.time += (ATTACK_ALTERNATE<<8);
                }
                VectorCopy ( ent->r.currentOrigin, tent->s.angles );
                SnapVector ( tent->s.angles );
            }
        }
    }

    //JANFIXME H&Z SPECIFICS

    if (isCurrentGametype(GT_HNS)) {
        if (mod == altAttack(MOD_M4_ASSAULT_RIFLE)) {
            ammoIdx = weaponData[WP_M4_ASSAULT_RIFLE].attack[ATTACK_ALTERNATE].ammoIndex;
            int normAmmoIdx = weaponData[WP_M4_ASSAULT_RIFLE].attack[ATTACK_NORMAL].ammoIndex;
            if (cageOutOfBoundaries) {
                if (g_cageAttempts.integer) {
                    if (attacker->client->sess.cageAttempts < g_cageAttempts.integer) {
                        attacker->client->ps.ammo[ammoIdx]++;
                        attacker->client->sess.cageAttempts++;
                        G_printInfoMessage(attacker, "M4 cage failed %d of %d: Seeker at boundary or hider caught in cage", attacker->client->sess.cageAttempts, g_cageAttempts.integer);
                    }
                    else {
                        G_printInfoMessage(attacker, "M4 cage failed too many times: not adding ammo.");

                        
                        if (attacker->client->ps.ammo[ammoIdx] == 0 && attacker->client->ps.ammo[normAmmoIdx] == 0
                            && attacker->client->ps.clip[ATTACK_NORMAL][WP_M4_ASSAULT_RIFLE] == 0 && attacker->client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE] == 0) {
                            removeWeaponFromClient(attacker, WP_M4_ASSAULT_RIFLE, qfalse, WP_KNIFE);
                            Q_strncpyz(level.hns.M4loc, "Disappeared", sizeof(level.hns.M4loc));
                            G_printGametypeMessageToAll("M4 has disappeared!");
                        }
                    }
                }
                else {
                    attacker->client->ps.ammo[ammoIdx]++;
                    G_printInfoMessage(attacker, "Seeker at boundary or hider caught in cage.");
                }
            }
            else {
                if (lastCaught != -1 && countCaught == 1) {
                    G_printGametypeMessageToAll("%s was trapped in a cage by %s.", g_entities[lastCaught].client->pers.cleanName, attacker->client->pers.cleanName);
                }
                else if (countCaught > 1) {
                    G_printGametypeMessageToAll("%d seekers were trapped in a cage by %s.", countCaught, attacker->client->pers.cleanName);
                }

                attacker->client->sess.seekersCaged++;
                if (attacker->client->ps.ammo[ammoIdx] == 0 && attacker->client->ps.ammo[normAmmoIdx] == 0
                    && attacker->client->ps.clip[ATTACK_NORMAL][WP_M4_ASSAULT_RIFLE] == 0 && attacker->client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE] == 0) {
                    removeWeaponFromClient(attacker, WP_M4_ASSAULT_RIFLE, qfalse, WP_KNIFE);
                    G_printGametypeMessageToAll("M4 has disappeared");
                    Q_strncpyz(level.hns.M4loc, "Disappeared", sizeof(level.hns.M4loc));
                }

                spawnCage(origin, qfalse, qfalse);
            }
        }
        else if (mod == WP_MM1_GRENADE_LAUNCHER) {
            missile = NV_projectile(attacker, origin, dir, WP_ANM14_GRENADE, 0);
            missile->nextthink = level.time + 100;
        }
        else if (mod == MOD_MDN11_GRENADE || mod == altAttack(MOD_MDN11_GRENADE)) {
            if (nadeOutOfBoundaries) {
                ammoIdx = weaponData[WP_MDN11_GRENADE].attack[ATTACK_ALTERNATE].ammoIndex;
                if (g_boxAttempts.integer != 0) {
                    if (attacker->client->sess.mdnAttempts < g_boxAttempts.integer) {
                        attacker->client->sess.mdnAttempts += 1;
                        attacker->client->ps.ammo[ammoIdx] += 1;

                        if (!(attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_MDN11_GRENADE))) { // Boe!Man 8/22/11: Make sure the attacker has the weapon, if not, re-add it (fixes bug which made weapon disappear on last throw).
                            attacker->client->ps.stats[STAT_WEAPONS] |= (1 << WP_MDN11_GRENADE);
                        }

                        G_printInfoMessage(attacker, "MDN box failed %d of %d: You cannot throw a box at a person.", attacker->client->sess.mdnAttempts, g_boxAttempts.integer);
                    }
                    else {
                        G_printInfoMessage(attacker, "MDN box failed too many times: not adding ammo.");
                    }
                }
                else {
                    attacker->client->ps.ammo[ammoIdx] += 1;
                    G_printInfoMessage(attacker, "You cannot throw a box at a person.");
                }
            }
            else {
                spawnBox(origin);
            }
        }
        else if (mod == MOD_M67_GRENADE || mod == altAttack(MOD_M67_GRENADE)) {
            if (!(attacker->client->ps.pm_flags & PMF_JUMPING) && !nadeOutOfBoundaries) {
                G_printGametypeMessageToAll("%s transformed into something...", attacker->client->pers.cleanName);
                G_printInfoMessage(attacker, "Hit your Reload button to get out (usually 'R').");
                transformPlayerToObject(attacker);
            }
            else {
                // Give them their nade back if they don't have it.
                ammoIdx = weaponData[WP_M67_GRENADE].attack[ATTACK_ALTERNATE].ammoIndex;

                if (attacker->client->ps.pm_flags & PMF_JUMPING) {
                    G_printInfoMessage(attacker, "You're not allowed to jump while using this grenade.");

                    if (!(attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_M67_GRENADE))) {
                        attacker->client->ps.stats[STAT_WEAPONS] |= (1 << WP_M67_GRENADE);
                    }
                    attacker->client->ps.ammo[ammoIdx] += 1;
                }
                else {
                    G_printInfoMessage(attacker, "Another object caught in radius.");

                    if (!(attacker->client->ps.stats[STAT_WEAPONS] & (1 << WP_M67_GRENADE))) {
                        attacker->client->ps.stats[STAT_WEAPONS] |= (1 << WP_M67_GRENADE);
                    }
                    attacker->client->ps.ammo[ammoIdx] += 1;
                }

            }

            attacker->client->pers.lastpickup = level.time + 50;
        }
        // JANFIXME CUSTOM NADES FROM 1.00
        // JANFIXME readd RPG disappearing?
    }

    return hitClient;
}
