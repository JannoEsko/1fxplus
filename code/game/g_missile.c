// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"

int G_MultipleDamageLocations(int hitLocation);

#define MISSILE_PRESTEP_TIME    50

#define AIRSTRIKE_MISSILE_COUNT    5
#define AIRSTRIKE_SPACING_MULTIPLIER    200
#define AIRSTRIKE_RANDOMNESS    30


void dropMissile(gentity_t* ent){
    gentity_t* missile = NV_projectile(ent->parent, ent->origin_from, ent->angles_from, WP_MM1_GRENADE_LAUNCHER, 0);
    missile->splashDamage = 250;
    missile->splashRadius = 300;
    missile->nextthink = level.time + 10000;

    G_FreeEntity(ent);
}

void planeSoundAirstrike(gentity_t* ent) {
    G_GlobalSound(G_SoundIndex("sound/misc/events/f16_flyby.mp3"));
    G_FreeEntity(ent);
}
void announceSoundAirstrike(gentity_t* ent) {
    G_SoundAtLoc(ent->r.currentOrigin, CHAN_WEAPON, G_SoundIndex("sound/radio/male/take_cover.mp3"));
    G_FreeEntity(ent);
}

void airstrike(vec3_t originFrom, vec3_t originTo, gentity_t* attacker){


    // determine if nade is thrown inside.
    trace_t tr;
    vec3_t upVec;
    VectorCopy(originTo, upVec);
    upVec[2] += 4096;

    // TODO: should probably do this for each missile as not every map has a nice 
    // flat skybox (e.g. liner1)
    trap_Trace(&tr, originTo, NULL, NULL, upVec, -1, MASK_SOLID);

    if (tr.surfaceFlags & SURF_NODRAW || tr.surfaceFlags & SURF_SKY || tr.fraction == 1){
        vec3_t strafeAngle;
        VectorSubtract(originTo, originFrom, strafeAngle);
        strafeAngle[2] = 0; // Don't care about vertical angle
        VectorNormalize(strafeAngle);


        AddSpawnField("classname", "1fx_play_effect");
        AddSpawnField("effect", "flare_blue");
        AddSpawnField("origin", va("%.0f %.0f %.0f", originTo[0], originTo[1], originTo[2]));
        AddSpawnField("wait", "1");
        AddSpawnField("count", "1");

        G_SpawnGEntityFromSpawnVars(qtrue);

        gentity_t* announceDelayEnt = G_Spawn();
        VectorCopy(originTo, announceDelayEnt->r.currentOrigin);
        announceDelayEnt->think = announceSoundAirstrike;
        announceDelayEnt->nextthink = level.time + 1000;
        gentity_t* planeSoundDelayEnt = G_Spawn();
        planeSoundDelayEnt->think = planeSoundAirstrike;
        planeSoundDelayEnt->nextthink = level.time + 2000;

        vec3_t startPoint;
        VectorCopy(originTo, startPoint);
        int offsetMultiplier = ceil(AIRSTRIKE_MISSILE_COUNT / 2.0f * AIRSTRIKE_SPACING_MULTIPLIER);
        startPoint[0] -= offsetMultiplier * strafeAngle[0];
        startPoint[1] -= offsetMultiplier * strafeAngle[1]; 
        for (int i=0; i<AIRSTRIKE_MISSILE_COUNT; i++){
            // Use a dummy entity to delay the missile
            gentity_t* delayEnt = G_Spawn();
            delayEnt->origin_from[0] = startPoint[0] + i * AIRSTRIKE_SPACING_MULTIPLIER * strafeAngle[0] + irand(-AIRSTRIKE_RANDOMNESS, AIRSTRIKE_RANDOMNESS);
            delayEnt->origin_from[1] = startPoint[1] + i * AIRSTRIKE_SPACING_MULTIPLIER * strafeAngle[1] + irand(-AIRSTRIKE_RANDOMNESS, AIRSTRIKE_RANDOMNESS);
            delayEnt->origin_from[2] = tr.endpos[2] - 20; // Just some margin so we don't spawn missiles too close to a solid
            delayEnt->angles_from[0] = strafeAngle[0] * 50;
            delayEnt->angles_from[1] = strafeAngle[1] * 50;
            delayEnt->angles_from[2] = 100;
            delayEnt->parent = attacker;
            delayEnt->think = dropMissile;
            delayEnt->nextthink = level.time + 2500 + i * 500;
        }
    }
    else {
        if (attacker->client->pers.hnz.airstrikeAttempts < hnz_airstrikeAttempts.integer){
            G_printInfoMessage(attacker, "Airstrike failed (%i/%i), can only be used outside!", ++attacker->client->pers.hnz.airstrikeAttempts, hnz_airstrikeAttempts.integer);
            // because of the delay, the client may no longer have the nades.
            if (attacker->client->ps.clip[ATTACK_NORMAL][WP_M15_GRENADE]){
                int ammoIdx = weaponData[WP_M15_GRENADE].attack[ATTACK_NORMAL].ammoIndex;
                attacker->client->ps.ammo[ammoIdx] += 1;
            } else {
                giveWeaponWithCustomAmmoToClient(attacker, WP_M15_GRENADE, qfalse, 1, -1, -1, -1);
            }
        } else {
            G_printInfoMessage(attacker, "Airstrike failed too many times: not adding ammo");
        }
    }

}


/*
================
G_BounceMissile

================
*/
void G_BounceMissile( gentity_t *ent, trace_t *trace )
{
    vec3_t  velocity;
    float   dot;
    int     hitTime;

    // nothing to do if already stationary
    if ( ent->s.pos.trType == TR_STATIONARY )
    {
        return;
    }

    // reflect the velocity on the trace plane
    hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
    BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
    dot = DotProduct( velocity, trace->plane.normal );
    VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

    if ( ent->s.eFlags & EF_BOUNCE_HALF )
    {
        VectorScale( ent->s.pos.trDelta, 0.65, ent->s.pos.trDelta );
        // check for stop
        if ( trace->plane.normal[2] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 )
        {
            G_SetOrigin( ent, trace->endpos );
            return;
        }
    }
    else if ( ent->s.eFlags & EF_BOUNCE_SCALE )
    {
        // IF it hit a client then barely bounce off of them since they are "soft"
        if ( trace->entityNum < MAX_CLIENTS )
        {
            VectorScale( ent->s.pos.trDelta, 0.04f, ent->s.pos.trDelta );

            // Make sure the grenade doesnt continuously collide with teh player it hit
            ent->target_ent = &g_entities[trace->entityNum];
        }
        else
        {
            VectorScale( ent->s.pos.trDelta, ent->bounceScale, ent->s.pos.trDelta );
        }

        // check for stop
        if ( trace->plane.normal[2] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 )
        {
            G_SetOrigin( ent, trace->endpos );

            if ( ent->parent && ent->parent->client )
            {
                gentity_t* nearby;

                // Find someone on the opposite team near wher ethe grenade landed
                nearby = G_FindNearbyClient ( trace->endpos, ent->parent->client->sess.team==TEAM_RED?TEAM_BLUE:TEAM_RED, 800, NULL );

                if ( nearby )
                {
                    // Make sure there is someone around to hear them scream
                    nearby = G_FindNearbyClient ( trace->endpos, nearby->client->sess.team, 800, nearby );
                    G_VoiceGlobal ( nearby, "grenade", qtrue );
                }
            }

            return;
        }
    }

    VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
    VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
    ent->s.pos.trTime = level.time;

    G_AddEvent( ent, EV_GRENADE_BOUNCE, trace->surfaceFlags& MATERIAL_MASK );
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent ) {
    vec3_t      dir;
    vec3_t      origin;

    if (isCurrentGametype(GT_HNZ) && (ent->methodOfDeath == MOD_M15_GRENADE || ent->methodOfDeath == altAttack(MOD_M15_GRENADE))){
        // Don't handle regular smoke but trigger airstrike instead
        airstrike(ent->origin_from, ent->r.currentOrigin, ent->parent);
        G_FreeEntity(ent);
        return;
    }

    BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
    SnapVector( origin );
    G_SetOrigin( ent, origin );

    // we don't have a valid direction, so just point straight up
    dir[0] = dir[1] = 0;
    dir[2] = 1;

    ent->s.eType = ET_GENERAL;
    G_AddEvent( ent, EV_MISSILE_MISS, (DirToByte( dir ) << MATERIAL_BITS) | MATERIAL_NONE);

    ent->freeAfterEvent = qtrue;
    // All grenade explosions are now broadcast to ensure that fire and smoke is always seen
    ent->r.svFlags |= SVF_BROADCAST;

    // splash damage
    if ( ent->splashDamage )
    {
        if (ent->dflags & DAMAGE_AREA_DAMAGE)
        {
            // do damage over time rather than instantly
            G_CreateDamageArea ( ent->r.currentOrigin, ent->parent, ent->splashDamage*0.06f,ent->splashRadius, 8000,ent->methodOfDeath );

            // do some instant damage
            G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->damage, ent->splashRadius, ent,
                            1, ent->dflags, ent->splashMethodOfDeath );
        }
        else
        {   // normal radius of effect damage
            G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent,
                            1, ent->dflags, ent->splashMethodOfDeath );
        }
    }

    trap_LinkEntity( ent );
}

/*
================
G_GrenadeThink

Marks the grenade ready to explode
================
*/
void G_GrenadeThink ( gentity_t* ent )
{
    ent->s.eFlags |= EF_EXPLODE;
}

void G_RunStuckMissile( gentity_t *ent )
{
    if ( ent->takedamage )
    {
        if ( ent->s.groundEntityNum >= 0 && ent->s.groundEntityNum < ENTITYNUM_WORLD )
        {
            gentity_t *other = &g_entities[ent->s.groundEntityNum];

            if ( (!VectorCompare( vec3_origin, other->s.pos.trDelta ) && other->s.pos.trType != TR_STATIONARY) ||
                (!VectorCompare( vec3_origin, other->s.apos.trDelta ) && other->s.apos.trType != TR_STATIONARY) )
            {//thing I stuck to is moving or rotating now, kill me
                G_Damage( ent, other, other, NULL, NULL, 99999, 0, MOD_CRUSH, HL_NONE );
                return;
            }
        }
    }
    // check think function
    G_RunThink( ent );
}

/*
================
G_BounceProjectile
================
*/
void G_BounceProjectile( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout ) {
    vec3_t v, newv;
    float dot;

    VectorSubtract( impact, start, v );
    dot = DotProduct( v, dir );
    VectorMA( v, -2*dot, dir, newv );

    VectorNormalize(newv);
    VectorMA(impact, 8192, newv, endout);
}


/*
================
G_CreateMissile
================
*/
gentity_t* G_CreateMissile( vec3_t org, vec3_t dir, float vel, int life, gentity_t *owner, attackType_t attack  )
{
    gentity_t   *missile;

    missile = G_Spawn();

    missile->nextthink = level.time + life;
    missile->think = G_FreeEntity;
    missile->s.eType = ET_MISSILE;
    missile->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    missile->parent = owner;
    missile->r.ownerNum = owner->s.number;

    if ( attack == ATTACK_ALTERNATE )
    {
        missile->s.eFlags |= EF_ALT_FIRING;
    }

    missile->s.pos.trType = TR_LINEAR;
    missile->s.pos.trTime = level.time;// - MISSILE_PRESTEP_TIME;   // NOTENOTE This is a Quake 3 addition over JK2
    missile->target_ent = NULL;

    SnapVector(org);
    VectorCopy( org, missile->s.pos.trBase );
    VectorScale( dir, vel, missile->s.pos.trDelta );
    VectorCopy( org, missile->r.currentOrigin);
    VectorCopy(org, missile->origin_from); // Boe!Man 7/10/13: Use this for grenades like the F1 (in H&S), to determine where the seeker threw it from.
    SnapVector(missile->s.pos.trDelta);

    return missile;
}

/*
================
G_CauseAreaDamage
================
*/
void G_CauseAreaDamage( gentity_t *ent )
{
    G_RadiusDamage ( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent, 3, DAMAGE_NO_TEAMKILL, ent->methodOfDeath );

    ent->s.time2--;

    if ( ent->s.time2 <= 0 )
    {
        G_FreeEntity ( ent );
        return;
    }

    ent->nextthink = level.time + 350;
    trap_LinkEntity( ent );
}

/*
================
G_CreateDamageArea
================
*/
gentity_t* G_CreateDamageArea ( vec3_t origin, gentity_t* attacker, float damage, float radius, int duration, int mod )
{
    gentity_t   *damageArea;

    damageArea = G_Spawn();

    if (isCurrentGametype(GT_HNZ) && (mod == MOD_M67_GRENADE || mod == altAttack(MOD_M67_GRENADE))) {
        damageArea->nextthink = level.time + 500;
        damageArea->think = think_forcefield;
    }
    else if (!(isCurrentGametype(GT_HNZ) && (mod == MOD_L2A2_GRENADE || mod == altAttack(MOD_L2A2_GRENADE)))) {
        damageArea->nextthink = level.time + 350;
        damageArea->think = G_CauseAreaDamage;
    }

    damageArea->s.eType = ET_DAMAGEAREA;
    damageArea->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    damageArea->parent = attacker;
    damageArea->r.ownerNum = attacker->s.number;

    damageArea->s.pos.trType = TR_STATIONARY;
    damageArea->s.pos.trTime = level.time;
    damageArea->s.time2 = duration / 350;
    damageArea->target_ent = NULL;

    damageArea->classname = "DamageArea";

    VectorSet( damageArea->r.maxs, 1, 1, 1 );
    VectorScale( damageArea->r.maxs, -1, damageArea->r.mins );

    damageArea->splashDamage = damage;
    damageArea->splashRadius = radius;
    damageArea->methodOfDeath = mod;

    damageArea->dflags = DAMAGE_RADIUS;
    damageArea->clipmask = MASK_SHOT;

    VectorCopy( origin, damageArea->s.pos.trBase );
    VectorCopy( origin, damageArea->r.currentOrigin);
    SnapVector( damageArea->r.currentOrigin  );

    return damageArea;
}


int isBounceNade(gentity_t* ent){
    if (currentGametype.integer == GT_HNZ && !Q_stricmp(ent->classname,"m15")){
        return 1;
    }
    if (!isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX }) && ( ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF | EF_BOUNCE_SCALE ) ) ){
        return 1;
    }
    return 0;
}

/*
================
G_MissileImpact
================
*/
extern gentity_t *CreateWeaponPickup(vec3_t pos,weapon_t weapon);
extern int G_GetHitLocation(gentity_t *target, vec3_t ppoint, vec3_t dir );
void G_MissileImpact( gentity_t *ent, trace_t *trace )
{
    gentity_t       *other;
    vec3_t  velocity;
    int d;

    static vec3_t   mins = { -15,-15,-45 };
    static vec3_t   maxs = { 15,15,46 };

    other = &g_entities[trace->entityNum];

    d = 0;

    // check for bounce
    if (isBounceNade( ent ))
    {
        G_BounceMissile( ent, trace );
        return;
    }

    // impact damage
    if (other->takedamage)
    {
        // FIXME: wrong damage direction?
        BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
        if ( VectorLength( velocity ) == 0 )
        {
            velocity[2] = 1;    // stepped on a grenade
        }
        if ( ent->damage )
        {
            int location;

            location = HL_NONE;
            if ( other->client )
            {
                VectorNormalize ( velocity );
                location = G_GetHitLocation ( other, ent->r.currentOrigin, velocity );
                if ( ent->splashDamage )
                {
                    location = G_MultipleDamageLocations(location);
                }
            }

            d = G_Damage(other, ent, &g_entities[ent->r.ownerNum], velocity,
                     ent->s.origin, ent->damage, ent->dflags,
                     ent->methodOfDeath, location );

            if ( d && other->client )
            {
                gentity_t *tent;
                vec3_t hitdir;

                statInfo_t* stat = &g_entities[ent->r.ownerNum].client->pers.statInfo;

                if (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam(&g_entities[ent->r.ownerNum], other)))
                {
                    stat->hitcount++;
                    stat->accuracy = (float)stat->hitcount / (float)stat->shotcount * 100;

                    if (ent->s.weapon == WP_M4_ASSAULT_RIFLE)
                    {
                        stat->weapon_hits[ATTACK_ALTERNATE * WP_NUM_WEAPONS + WP_M4_ASSAULT_RIFLE]++;
                    }
                    else
                    {
                        stat->weapon_hits[ATTACK_NORMAL * WP_NUM_WEAPONS + ent->s.weapon]++;
                    }
                }

                // Put some procedural gore on the target.
                tent = G_TempEntity( ent->r.currentOrigin, EV_EXPLOSION_HIT_FLESH );

                // send entity and direction
                VectorSubtract(other->r.currentOrigin, ent->r.currentOrigin, hitdir);
                VectorNormalize(hitdir);
                tent->s.eventParm = DirToByte( hitdir );
                tent->s.otherEntityNum2 = other->s.number;          // Victim entity number

                // Pack the shot info into the temp end for gore
                tent->s.time  = ent->s.weapon + ((((int)other->s.apos.trBase[YAW]&0x7FFF) % 360) << 16);
                if ( ent->s.eFlags & EF_ALT_FIRING )
                {
                    tent->s.time += (ATTACK_ALTERNATE<<8);
                }

                VectorCopy ( other->r.currentOrigin, tent->s.angles );
                SnapVector ( tent->s.angles );
            }
        }
    }

    // is it cheaper in bandwidth to just remove this ent and create a new
    // one, rather than changing the missile into the explosion?

    if ( d && other->client )
    {
        // JANFIXME - Boe had a F1 check here.
        if (!isCurrentGametype(GT_HNZ) || (isCurrentGametype(GT_HNZ) && ent->methodOfDeath != MOD_M67_GRENADE)) {
            G_AddEvent(ent, EV_MISSILE_HIT,
                (DirToByte(trace->plane.normal) << MATERIAL_BITS) | (trace->surfaceFlags & MATERIAL_MASK));
            ent->s.otherEntityNum = other->s.number;
            if (ent->damage)
            {
                // FIXME: might be able to use the value from inside G_Damage to avoid recalc???
                ent->s.otherEntityNum2 = G_GetHitLocation(other, g_entities[ent->r.ownerNum].r.currentOrigin, velocity);
            }
        }

        
    }
    else
    {
        qboolean createClaymore = qfalse;
        if (!isCurrentGametype(GT_HNZ) || (isCurrentGametype(GT_HNZ) && (ent->methodOfDeath != MOD_M67_GRENADE && ent->methodOfDeath != altAttack(MOD_M67_GRENADE)))) {
            G_AddEvent(ent, EV_MISSILE_MISS,
                (DirToByte(trace->plane.normal) << MATERIAL_BITS) | (trace->surfaceFlags & MATERIAL_MASK));



            if (isCurrentGametype(GT_HNZ) && ent->think != HZ_Claymore && (ent->methodOfDeath == MOD_L2A2_GRENADE || ent->methodOfDeath == altAttack(MOD_L2A2_GRENADE))) {
                createClaymore = qtrue;
            }
        }

        // If missile should stick into impact point (e.g. a thrown knife).

        if (!Q_stricmp(ent->classname, "knife") && isCurrentGametype(GT_HNS)) {
            Spawn_KnifeBox(ent->parent, trace->endpos);
        }

        if((!Q_stricmp(ent->classname,"Knife") && !isCurrentGametype(GT_HNS)) || (isCurrentGametype(GT_HNZ) && createClaymore))
        {
            // Create a pickup where we impacted.
            vec3_t      pickupPos;
            gentity_t   *pickupEnt;

            VectorMA(trace->endpos,1,trace->plane.normal,pickupPos);

            pickupEnt=CreateWeaponPickup(pickupPos, createClaymore ? WP_L2A2_GRENADE : WP_KNIFE);
            if(pickupEnt)
            {
                vec3_t knifeDir,knifeAngles;

                BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, knifeDir );

                //FIXME: needs work to set model angles!
                VectorNormalize ( knifeDir );
                vectoangles(knifeDir,knifeAngles);
                knifeAngles[YAW] += 90;
                knifeAngles[ROLL] = knifeAngles[PITCH];
                knifeAngles[PITCH] = 0;

                pickupEnt->s.angles[0]=knifeAngles[0];
                pickupEnt->s.angles[1]=knifeAngles[1];
                pickupEnt->s.angles[2]=knifeAngles[2];

                if (!Q_stricmp(ent->classname, "Knife") && isCurrentGametype(GT_HNS)) {
                    pickupEnt->think = G_FreeEntity;
                    pickupEnt->nextthink = level.time + 30000;  // Stick around for 30 seconds
                }
                else if (isCurrentGametype(GT_HNZ) && createClaymore) {

                    // Boe!Man 12/13/14: Claymore specifics.
                    // Make sure it doesn't get picked up.
                    pickupEnt->touch = NULL;

                    // Its main think function.
                    pickupEnt->think = HZ_Claymore;
                    pickupEnt->nextthink = level.time + 500;

                    // We need to make sure the server knows this entity can take damage.
                    // With this, we can shoot at the entity.
                    pickupEnt->takedamage = qtrue;
                    pickupEnt->r.contents = -1;
                    pickupEnt->die = HZ_ClaymoreShoot;

                    // Copy over some essentials, so we can mimic a proper blast + damage area later.
                    pickupEnt->parent = ent->parent;
                    pickupEnt->splashDamage = ent->splashDamage;
                    pickupEnt->splashRadius = ent->splashRadius;
                    pickupEnt->splashMethodOfDeath = ent->splashMethodOfDeath;


                    // Check if a human was caught in the radius.
                    // If so, get rid of this claymore.

                    vec3_t bboxMins, bboxMaxs;

                    VectorAdd(trace->endpos, mins, bboxMins);
                    VectorAdd(trace->endpos, maxs, bboxMaxs);

                    int claymoreTouch[MAX_GENTITIES];

                    int num = trap_EntitiesInBox(bboxMins, bboxMaxs, claymoreTouch,
                        MAX_GENTITIES);

                    // Iterate through caught entities.
                    for (int i = 0; i < num; i++) {
                        gentity_t* hit = &g_entities[claymoreTouch[i]];
                        if (hit->client && hit->client->sess.team == TEAM_RED) {
                            // We hit a human.
                            G_printInfoMessage(ent->parent,
                                "Claymores cannot be thrown on humans!");

                            // Free entity and return.
                            G_FreeEntity(pickupEnt);
                            G_FreeEntity(ent);
                            return;
                        }
                    }
                }
                

                pickupEnt->count = 1;

                pickupEnt->s.eFlags |= EF_ANGLE_OVERRIDE;
                VectorCopy(pickupEnt->s.angles,pickupEnt->r.currentAngles);
                VectorCopy(pickupEnt->s.angles,pickupEnt->s.apos.trBase);
                pickupEnt->s.pos.trType=TR_STATIONARY;
                pickupEnt->s.apos.trTime=level.time;
                pickupEnt->clipmask = ent->clipmask;
                pickupEnt->s.groundEntityNum = trace->entityNum;
                trap_LinkEntity(pickupEnt);

                if (createClaymore) {
                    G_FreeEntity(ent);
                    return;
                }
            }
        }
    }

    ent->freeAfterEvent = qtrue;

    // All grenade explosions are now broadcast to ensure that fire and smoke is always seen
    ent->r.svFlags |= SVF_BROADCAST;

    // change over to a normal entity right at the point of impact
    ent->s.eType = ET_GENERAL;

    SnapVectorTowards( trace->endpos, ent->s.pos.trBase );  // save net bandwidth

    G_SetOrigin( ent, trace->endpos );

    // splash damage (doesn't apply to person directly hit)
    if ( ent->splashDamage )
    {
        if (ent->dflags & DAMAGE_AREA_DAMAGE)
        {
            // do damage over time rather than instantly
            G_CreateDamageArea ( trace->endpos, ent->parent, ent->splashDamage*0.06f,ent->splashRadius*2, 8000,ent->methodOfDeath );

            // do some instant damage
            G_RadiusDamage( trace->endpos, ent->parent, ent->damage, ent->splashRadius, other,
                            1, ent->dflags, ent->splashMethodOfDeath );
        }
        else
        {   // normal radius of effect damage
            G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius,
                            other, 1, ent->dflags, ent->splashMethodOfDeath );
        }
    }

    trap_LinkEntity( ent );
}




/*
================
G_RunMissile
================
*/
void G_RunMissile( gentity_t *ent )
{
    vec3_t      origin;
    trace_t     tr;
    int         passent;

    // get current position
    BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

    // if this missile bounced off an invulnerability sphere
    if ( ent->target_ent )
    {
        passent = ent->target_ent->s.number;
    }
    else
    {
        // ignore interactions with the missile owner
        passent = ent->r.ownerNum;
    }

    // Special case where the grenade has gone up into the sky
    if ( ent->s.eFlags & EF_INSKY )
    {
        // Check to see if its out of the world on the X,Y plane
        // or below it (above is a special case)
        if ( origin[0] < level.worldMins[0] ||
             origin[1] < level.worldMins[1] ||
             origin[0] > level.worldMaxs[0] ||
             origin[1] > level.worldMaxs[1] ||
             origin[2] < level.worldMins[2]    )
        {
            G_FreeEntity( ent );
            return;
        }

        // Above it only kills it if the item has no gravity
        if ( origin[2] > level.worldMaxs[2] && ent->s.pos.trType != TR_GRAVITY && ent->s.pos.trType != TR_LIGHTGRAVITY)
        {
            G_FreeEntity( ent );
            return;
        }

        trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask );

        // Hit another sky, must be reentering
        if ( tr.fraction == 1.0f )
        {
            ent->s.eFlags &= ~EF_INSKY;
            VectorCopy ( origin, ent->r.currentOrigin );
        }

        VectorCopy ( origin, ent->r.currentOrigin );
        trap_LinkEntity ( ent );
    }
    else
    {
        // Run the same test again because the condition may have changed as a result
        // of the greande falling below the sky again
        // Loop this trace so we can break windows
        while ( 1 )
        {
            // trace a line from the previous position to the current position
            trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask );

            // If its glass then redo the trace after breaking the glass
            if ( tr.fraction != 1 && !Q_stricmp ( g_entities[tr.entityNum].classname, "func_glass" ) )
            {
                g_entities[tr.entityNum].use ( &g_entities[tr.entityNum], ent, ent );
                continue;
            }

            break;
        }

        if ( tr.startsolid || tr.allsolid )
        {
            // make sure the tr.entityNum is set to the entity we're stuck in
            trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, passent, ent->clipmask );
            tr.fraction = 0;
        }
        else
        {
            VectorCopy( tr.endpos, ent->r.currentOrigin );
        }

        trap_LinkEntity( ent );

        if ( tr.fraction != 1 )
        {
            // Hit the sky or moving through something
            if ( (tr.surfaceFlags & SURF_NOIMPACT) && (tr.surfaceFlags & SURF_SKY) )
            {
                // Dont kill a missle that hits the sky and has gravity
                ent->s.eFlags |= EF_INSKY;
                ent->r.svFlags |= SVF_BROADCAST;
                VectorCopy ( origin, ent->r.currentOrigin );
                trap_LinkEntity( ent );
            }
            else
            {
                G_MissileImpact( ent, &tr );

                // Is it time to explode
                if ( ent->s.eFlags & EF_EXPLODE )
                {
                    ent->s.eFlags &= (~EF_EXPLODE);
                    G_ExplodeMissile ( ent );
                    return;
                }

                // Exploded
                if ( ent->s.eType != ET_MISSILE )
                {
                    return;
                }
            }
        }
        // Is it time to explode
        else if ( ent->s.pos.trType == TR_STATIONARY && (ent->s.eFlags & EF_EXPLODE) )
        {
            ent->s.eFlags &= (~EF_EXPLODE);
            G_ExplodeMissile ( ent );
            return;
        }
    }

    // If this is a knife then reorient its angles
    if ( ent->s.weapon == WP_KNIFE )
    {
        vec3_t vel;

        BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, vel );
        vectoangles( vel, ent->s.angles );

        ent->s.angles[YAW] += 90;
//      ent->s.angles[ROLL] = ent->s.angles[PITCH];
        ent->s.angles[ROLL] = 0;
        ent->s.angles[PITCH] = 0;
    }

    // check think function after bouncing
    G_RunThink( ent );
}


//=============================================================================




