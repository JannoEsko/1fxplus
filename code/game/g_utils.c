// Copyright (C) 2001-2002 Raven Software.
//
// g_utils.c -- misc utility functions for game module

#include "g_local.h"

typedef struct
{
    char    oldShader[MAX_QPATH];
    char    newShader[MAX_QPATH];
    float   timeOffset;

} shaderRemap_t;

#define MAX_SHADER_REMAPS 128

int remapCount = 0;
shaderRemap_t remappedShaders[MAX_SHADER_REMAPS];

#define     CACHE_SOUNDS_NUM_FREE   54      // The amount of sounds to free upon reaching the sound limit.

typedef struct {
    char        name[MAX_QPATH];
    int         index;
    int         usageCount;
    int         lastUsage;

    qboolean    staticSound;
    qboolean    active;
} CS_SOUNDS_Cache_t;

static CS_SOUNDS_Cache_t    soundCache[MAX_SOUNDS - 1] = { 0 };
static int                  soundsUsed = 0;


void AddRemap(const char *oldShader, const char *newShader, float timeOffset)
{
    int i;

    for (i = 0; i < remapCount; i++)
    {
        if (Q_stricmp(oldShader, remappedShaders[i].oldShader) == 0)
        {
            // found it, just update this one
            strcpy(remappedShaders[i].newShader,newShader);
            remappedShaders[i].timeOffset = timeOffset;
            return;
        }
    }

    if (remapCount < MAX_SHADER_REMAPS)
    {
        strcpy(remappedShaders[remapCount].newShader,newShader);
        strcpy(remappedShaders[remapCount].oldShader,oldShader);
        remappedShaders[remapCount].timeOffset = timeOffset;
        remapCount++;
    }
}

const char *BuildShaderStateConfig(void)
{
    static char buff[MAX_STRING_CHARS*4];
    char out[(MAX_QPATH * 2) + 5];
    int i;

    memset(buff, 0, MAX_STRING_CHARS);
    for (i = 0; i < remapCount; i++) {
        Com_sprintf(out, (MAX_QPATH * 2) + 5, "%s=%s:%5.2f@", remappedShaders[i].oldShader, remappedShaders[i].newShader, remappedShaders[i].timeOffset);
        Q_strcat( buff, sizeof( buff ), out);
    }
    return buff;
}

/*
================
G_FindConfigstringIndex
================
*/
int G_FindConfigstringIndex( char *name, int start, int max, qboolean create )
{
    int     i;
    char    s[MAX_STRING_CHARS];

    if ( !name || !name[0] )
    {
        return 0;
    }

    for ( i=1 ; i<max ; i++ )
    {
        trap_GetConfigstring( start + i, s, sizeof( s ) );
        if ( !s[0] )
        {
            break;
        }
        if ( !strcmp( s, name ) )
        {
            return i;
        }
    }

    if ( !create )
    {
        return 0;
    }

    if ( i == max )
    {
        Com_Error( ERR_FATAL, "G_FindConfigstringIndex: overflow" );
    }

    trap_SetConfigstring( start + i, name );

    return i;
}

int G_ModelIndex( char *name )
{
    return G_FindConfigstringIndex (name, CS_MODELS, MAX_MODELS, qtrue);
}

/*
================
G_SoundCacheSort - from 1fx. Mod

Sounds are sorted in cache
as follows (quick sort):
- Static sounds.
- Sounds played very recently (within 100 msec of the request).
- Regular sounds by usage.
================
*/

static int QDECL G_SoundCacheSort(const void* slotPtrA, const void* slotPtrB)
{
    CS_SOUNDS_Cache_t* slotA;
    CS_SOUNDS_Cache_t* slotB;

    slotA = (CS_SOUNDS_Cache_t*)slotPtrA;
    slotB = (CS_SOUNDS_Cache_t*)slotPtrB;

    // Is sound slot A a static sound whilst slot B is not, or vice versa?
    if (slotA->staticSound == qtrue && slotB->staticSound == qfalse) {
        return -1;
    }
    else if (slotB->staticSound == qtrue && slotA->staticSound == qfalse) {
        return 1;
    }

    // Was sound A just requested to be played, unlike sound B, or vice versa?
    if (level.time < slotA->lastUsage + 100 && level.time > slotB->lastUsage + 100) {
        return -1;
    }
    else if (level.time < slotB->lastUsage + 100 && level.time > slotA->lastUsage + 100) {
        return 1;
    }

    // Does this sound have a higher usage count?
    if (slotA->usageCount > slotB->usageCount) {
        return -1;
    }

    // Vice versa.
    return 1;
}

/*
================
G_SoundIndex, 1fx. Mod version

Returns the index to the configstring
of the sound requested to play.

This may either be from cache or a newly
created configstring.
================
*/

int G_SoundIndex(char* name, qboolean staticSound)
{
    int i;

    // Did we on our sound limit?
    if (soundsUsed == MAX_SOUNDS - 1) {
        // We have. Free sounds we have used in the past.
        // Sort the sound cache.
        qsort(soundCache, MAX_SOUNDS - 1, sizeof(CS_SOUNDS_Cache_t), G_SoundCacheSort);

        // Free sound slots.
        for (i = MAX_SOUNDS - 1 - CACHE_SOUNDS_NUM_FREE;
            i < MAX_SOUNDS - 1; i++)
        {
            // Can we reset this slot?
            // Is this a static sound?
            if (soundCache[i].staticSound) {
                continue;
            }
            // Was this sound just requested to play?
            if (level.time < soundCache[i].lastUsage + 100) {
                continue;
            }

            // Mark this slot as inactive to it may be reused.
            soundCache[i].active = qfalse;

            // Decrease sound slots active.
            soundsUsed--;
        }

        // Reset usage count on all slots.
        for (i = 0; i < MAX_SOUNDS - 1; i++) {
            soundCache[i].usageCount = 0;
        }
    }

    // Try to find the sound in our local cache.
    for (i = 0; i < MAX_SOUNDS - 1; i++) {
        // Name may not be NULL.
        if (soundCache[i].name[0] == 0) {
            continue;
        }

        // Does the sound name match?
        if (strcmp(soundCache[i].name, name) != 0) {
            continue;
        }

        // Yes, we found it.
        // Increase usage.
        soundCache[i].usageCount++;

        // Make sure this sound is set to active if it was previously
        // set as inactive.
        if (soundCache[i].active == qfalse) {
            soundCache[i].active = qtrue;
            soundsUsed++;
        }

        // Reset time used.
        soundCache[i].lastUsage = level.time;

        // Return the index to this slot.
        return soundCache[i].index;
    }

    // Not present, find a free slot.
    for (i = 0; i < MAX_SOUNDS - 1; i++) {
        if (soundCache[i].name == NULL || soundCache[i].active == qfalse) {
            // Slot available.
            break;
        }
    }

    // This fatal error should never hit, but is there just in case.
    // If this evaluates true this means we only have static
    // sounds or sounds that have just been played in cache.
    // Neither of which should/could ever happen within the time frame
    // of being cleaned up.
    if (i == MAX_SOUNDS - 1) {
        Com_Error(ERR_FATAL, "Sound cache fully stumped.");
    }

    // Set slot information.
    Q_strncpyz(soundCache[i].name, name, sizeof(soundCache[i].name));
    soundCache[i].active = qtrue;
    soundCache[i].lastUsage = level.time;
    soundCache[i].staticSound = staticSound;

    // Set initial sound index?
    if (soundCache[i].index == 0) {
        soundCache[i].index = i + 1;
    }

    // Set actual configstring.
    trap_SetConfigstring(CS_SOUNDS + soundCache[i].index, name);

    // Increase usage and return the index.
    soundCache[i].usageCount++;
    soundsUsed++;

    // Return the index.
    return soundCache[i].index;

}

int G_AmbientSoundSetIndex( char *name )
{
    return G_FindConfigstringIndex (name, CS_AMBIENT_SOUNDSETS, MAX_AMBIENT_SOUNDSETS, qtrue);
}


int G_IconIndex( char *name )
{
    return G_FindConfigstringIndex (name, CS_ICONS, MAX_ICONS, qtrue);
}

int G_EffectIndex( char *name )
{
    return G_FindConfigstringIndex (name, CS_EFFECTS, MAX_FX, qtrue);
}

int G_BSPIndex( char *name )
{
    return G_FindConfigstringIndex (name, CS_BSP_MODELS, MAX_SUB_BSP, qtrue);
}

/*
================
G_TeamCommand

Broadcasts a command to only a specific team
================
*/
void G_TeamCommand( team_t team, char *cmd )
{
    int     i;

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( level.clients[i].pers.connected == CON_CONNECTED )
        {
            if ( level.clients[i].sess.team == team )
            {
                trap_SendServerCommand( i, va("%s", cmd ));
            }
        }
    }
}

/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the entity after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match)
{
    char    *s;

    if (!from)
        from = g_entities;
    else
        from++;

    for ( ; from < &g_entities[level.num_entities] ; from++)
    {
        if (!from->inuse)
            continue;
        s = *(char **) ((byte *)from + fieldofs);
        if (!s)
            continue;
        if (!Q_stricmp (s, match))
            return from;
    }

    return NULL;
}

/*
=============
G_PickTarget

Selects a random entity from among the targets
=============
*/
#define MAXCHOICES  32

gentity_t *G_PickTarget (char *targetname)
{
    gentity_t   *ent = NULL;
    int     num_choices = 0;
    gentity_t   *choice[MAXCHOICES];

    if (!targetname)
    {
        Com_Printf("G_PickTarget called with NULL targetname\n");
        return NULL;
    }

    while(1)
    {
        ent = G_Find (ent, FOFS(targetname), targetname);
        if (!ent)
        {
            break;
        }

        choice[num_choices++] = ent;
        if (num_choices == MAXCHOICES)
        {
            break;
        }
    }

    if (!num_choices)
    {
        Com_Printf("G_PickTarget: target %s not found\n", targetname);
        return NULL;
    }

    return choice[rand() % num_choices];
}

void G_UseTargetsByName( const char* name, gentity_t* other, gentity_t *activator )
{
    gentity_t* t;

    // Look for an entity
    t = NULL;
    while ( (t = G_Find (t, FOFS(targetname), name)) != NULL )
    {
        if ( t == other )
        {
            Com_Printf ("WARNING: Entity used itself.\n");
        }
        else
        {
            if ( t->use )
            {
                t->use (t, other, activator);
            }
        }

        if ( other && !other->inuse )
        {
            Com_Printf("entity was removed while using targets\n");
            return;
        }
    }
}

/*
==============================
G_UseTargets

"activator" should be set to the entity that initiated the firing.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets( gentity_t *ent, gentity_t *activator )
{
    if ( !ent )
    {
        return;
    }

    if (ent->targetShaderName && ent->targetShaderNewName)
    {
        float f = level.time * 0.001;
        AddRemap(ent->targetShaderName, ent->targetShaderNewName, f);
        trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
    }

    if ( !ent->target )
    {
        return;
    }

    G_UseTargetsByName ( ent->target, ent, activator );
}


/*
=============
TempVector

This is just a convenience function
for making temporary vectors for function calls
=============
*/
float   *tv( float x, float y, float z ) {
    static  int     index;
    static  vec3_t  vecs[8];
    float   *v;

    // use an array so that multiple tempvectors won't collide
    // for a while
    v = vecs[index];
    index = (index + 1)&7;

    v[0] = x;
    v[1] = y;
    v[2] = z;

    return v;
}


/*
=============
VectorToString

This is just a convenience function
for printing vectors
=============
*/
char    *vtos( const vec3_t v ) {
    static  int     index;
    static  char    str[8][32];
    char    *s;

    // use an array so that multiple vtos won't collide
    s = str[index];
    index = (index + 1)&7;

    Com_sprintf (s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

    return s;
}


/*
===============
G_SetMovedir

The editor only specifies a single value for angles (yaw),
but we have special constants to generate an up or down direction.
Angles will be cleared, because it is being used to represent a direction
instead of an orientation.
===============
*/
void G_SetMovedir( vec3_t angles, vec3_t movedir ) {
    static vec3_t VEC_UP        = {0, -1, 0};
    static vec3_t MOVEDIR_UP    = {0, 0, 1};
    static vec3_t VEC_DOWN      = {0, -2, 0};
    static vec3_t MOVEDIR_DOWN  = {0, 0, -1};

    if ( VectorCompare (angles, VEC_UP) ) {
        VectorCopy (MOVEDIR_UP, movedir);
    } else if ( VectorCompare (angles, VEC_DOWN) ) {
        VectorCopy (MOVEDIR_DOWN, movedir);
    } else {
        AngleVectors (angles, movedir, NULL, NULL);
    }
    VectorClear( angles );
}


float vectoyaw( const vec3_t vec ) {
    float   yaw;

    if (vec[YAW] == 0 && vec[PITCH] == 0) {
        yaw = 0;
    } else {
        if (vec[PITCH]) {
            yaw = ( atan2( vec[YAW], vec[PITCH]) * 180 / M_PI );
        } else if (vec[YAW] > 0) {
            yaw = 90;
        } else {
            yaw = 270;
        }
        if (yaw < 0) {
            yaw += 360;
        }
    }

    return yaw;
}

void G_InitGentity( gentity_t *e )
{
    e->inuse = qtrue;
    e->classname = "noclass";
    e->s.number = e - g_entities;
    e->r.ownerNum = ENTITYNUM_NONE;
}

/*
=================
G_Spawn

Either finds a free entity, or allocates a new one.

  The slots from 0 to MAX_CLIENTS-1 are always reserved for clients, and will
never be used by anything else.

Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
gentity_t* G_Spawn( void )
{
    int         i, force;
    gentity_t   *e;

    e = NULL;   // shut up warning
    i = 0;      // shut up warning
    for ( force = 0 ; force < 2 ; force++ )
    {
        // if we go through all entities and can't find one to free,
        // override the normal minimum times before use
        e = &g_entities[MAX_CLIENTS];
        for ( i = MAX_CLIENTS ; i<level.num_entities ; i++, e++)
        {
            if ( e->inuse )
            {
                continue;
            }

            // the first couple seconds of server time can involve a lot of
            // freeing and allocating, so relax the replacement policy
            if ( !force && e->freetime > level.startTime + 2000 && level.time - e->freetime < 1000 )
            {
                continue;
            }

            // reuse this slot
            G_InitGentity( e );
            return e;
        }

        if ( i != MAX_GENTITIES )
        {
            break;
        }
    }

    if ( i == ENTITYNUM_MAX_NORMAL )
    {
        for (i = 0; i < MAX_GENTITIES; i++)
        {
            Com_Printf("%4i: %s\n", i, g_entities[i].classname);
        }

        Com_Error( ERR_FATAL, "G_Spawn: no free entities" );
    }

    // open up a new slot
    level.num_entities++;

    // let the server system know that there are more entities
    trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
                         &level.clients[0].ps, sizeof( level.clients[0] ) );

    G_InitGentity( e );
    return e;
}

/*
=================
G_EntitiesFree
=================
*/
qboolean G_EntitiesFree( void )
{
    int         i;
    gentity_t   *e;

    e = &g_entities[MAX_CLIENTS];
    for ( i = MAX_CLIENTS; i < level.num_entities; i++, e++)
    {
        if ( e->inuse )
        {
            continue;
        }

        // slot available
        return qtrue;
    }
    return qfalse;
}

/*
=================
G_FreeEntity

Marks the entity as free
=================
*/
void G_FreeEntity( gentity_t *ed )
{
    trap_UnlinkEntity (ed);

    if ( ed->neverFree )
    {
        return;
    }

    memset (ed, 0, sizeof(*ed));
    ed->classname = "freed";
    ed->freetime = level.time;
    ed->inuse = qfalse;
}

/*
=================
G_FreeEnitityChildren

Frees any entity that is a child of the given entity
=================
*/
void G_FreeEnitityChildren( gentity_t* ent )
{
    gentity_t* from;

    for ( from = g_entities; from < &g_entities[level.num_entities] ; from++)
    {
        if ( !from->inuse )
        {
            continue;
        }

        if ( from->parent != ent )
        {
            continue;
        }

        G_FreeEntity ( from );
    }
}

/*
=================
G_TempEntity

Spawns an event entity that will be auto-removed
The origin will be snapped to save net bandwidth, so care
must be taken if the origin is right on a surface (snap towards start vector first)
=================
*/
gentity_t *G_TempEntity( vec3_t origin, int event )
{
    gentity_t       *e;
    vec3_t      snapped;

    e = G_Spawn();
    e->s.eType = ET_EVENTS + event;

    e->classname = "tempEntity";
    e->eventTime = level.time;
    e->freeAfterEvent = qtrue;

    VectorCopy( origin, snapped );
    SnapVector( snapped );      // save network bandwidth
    G_SetOrigin( e, snapped );

    // find cluster for PVS
    trap_LinkEntity( e );

    return e;
}



/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
G_KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
void G_KillBox (gentity_t *ent) {
    int         i, num;
    int         touch[MAX_GENTITIES];
    gentity_t   *hit;
    vec3_t      mins, maxs;

    VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
    VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );
    num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

    for ( i = 0; i < num ; i++ )
    {
        hit = &g_entities[touch[i]];
        if ( !hit->client )
        {
            continue;
        }

        // Dont kill yourself if this should ever happen
        if ( hit == ent )
        {
            continue;
        }

        // nail it
        G_Damage ( hit, ent, ent, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG, HL_NONE );
    }

}

//==============================================================================

/*
===============
G_AddPredictableEvent

Use for non-pmove events that would also be predicted on the
client side: jumppads and item pickups
Adds an event+parm and twiddles the event counter
===============
*/
void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm ) {
    if ( !ent->client ) {
        return;
    }
    BG_AddPredictableEventToPlayerstate( event, eventParm, &ent->client->ps );
}


/*
===============
G_AddEvent

Adds an event+parm and twiddles the event counter
===============
*/
void G_AddEvent( gentity_t *ent, int event, int eventParm ) {
    int     bits;

    if ( !event ) {
        Com_Printf( "G_AddEvent: zero event added for entity %i\n", ent->s.number );
        return;
    }

    // clients need to add the event in playerState_t instead of entityState_t
    if ( ent->client ) {
        bits = ent->client->ps.externalEvent & EV_EVENT_BITS;
        bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
        ent->client->ps.externalEvent = event | bits;
        ent->client->ps.externalEventParm = eventParm;
        ent->client->ps.externalEventTime = level.time;
    } else {
        bits = ent->s.event & EV_EVENT_BITS;
        bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
        ent->s.event = event | bits;
        ent->s.eventParm = eventParm;
    }
    ent->eventTime = level.time;
}

/*
=============
G_PlayEffect
=============
*/
void G_PlayEffect(int fxID, vec3_t org, vec3_t ang)
{
    gentity_t   *te;

    te = G_TempEntity( org, EV_PLAY_EFFECT );
    VectorCopy(ang, te->s.angles);
    VectorCopy(org, te->s.origin);
    te->s.eventParm = fxID;
}

/*
=============
G_SetHUDIcon
=============
*/
void G_SetHUDIcon ( int index, int icon )
{
    trap_SetConfigstring ( CS_HUDICONS + index, va("%i", icon ) );
}

/*
=============
G_Sound
=============
*/
void G_Sound( gentity_t *ent, int channel, int soundIndex ) {
    gentity_t   *te;

    te = G_TempEntity( ent->r.currentOrigin, EV_GENERAL_SOUND );
    te->s.eventParm = soundIndex;
    te->s.otherEntityNum = channel;
}

/*
=============
G_SoundAtLoc
=============
*/
void G_SoundAtLoc( vec3_t loc, int channel, int soundIndex ) {
    gentity_t   *te;

    te = G_TempEntity( loc, EV_GENERAL_SOUND );
    te->s.eventParm = soundIndex;
    te->r.svFlags |= SVF_BROADCAST;
}

/*
=============
G_EntitySound
=============
*/
void G_EntitySound( gentity_t *ent, int channel, int soundIndex ) {
    gentity_t   *te;

    te = G_TempEntity( ent->r.currentOrigin, EV_ENTITY_SOUND );
    te->s.eventParm = soundIndex;
    te->s.weapon = ent->s.number;
}


//==============================================================================

qboolean G_PointInBounds( vec3_t point, vec3_t mins, vec3_t maxs )
{
    int i;

    for(i = 0; i < 3; i++ )
    {
        if ( point[i] < mins[i] )
        {
            return qfalse;
        }
        if ( point[i] > maxs[i] )
        {
            return qfalse;
        }
    }

    return qtrue;
}

qboolean G_BoxInBounds( vec3_t point, vec3_t mins, vec3_t maxs, vec3_t boundsMins, vec3_t boundsMaxs )
{
    vec3_t boxMins;
    vec3_t boxMaxs;

    VectorAdd( point, mins, boxMins );
    VectorAdd( point, maxs, boxMaxs );

    if(boxMaxs[0]>boundsMaxs[0])
        return qfalse;

    if(boxMaxs[1]>boundsMaxs[1])
        return qfalse;

    if(boxMaxs[2]>boundsMaxs[2])
        return qfalse;

    if(boxMins[0]<boundsMins[0])
        return qfalse;

    if(boxMins[1]<boundsMins[1])
        return qfalse;

    if(boxMins[2]<boundsMins[2])
        return qfalse;

    //box is completely contained within bounds
    return qtrue;
}


void G_SetAngles( gentity_t *ent, vec3_t angles )
{
    VectorCopy( angles, ent->r.currentAngles );
    VectorCopy( angles, ent->s.angles );
    VectorCopy( angles, ent->s.apos.trBase );
}

qboolean G_ClearTrace( vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int ignore, int clipmask )
{
    static  trace_t tr;

    trap_Trace( &tr, start, mins, maxs, end, ignore, clipmask );

    if ( tr.allsolid || tr.startsolid || tr.fraction < 1.0 )
    {
        return qfalse;
    }

    return qtrue;
}


/*
================
G_SetOrigin

Sets the pos trajectory for a fixed position
================
*/
void G_SetOrigin( gentity_t *ent, vec3_t origin ) {
    VectorCopy( origin, ent->s.pos.trBase );
    ent->s.pos.trType = TR_STATIONARY;
    ent->s.pos.trTime = 0;
    ent->s.pos.trDuration = 0;
    VectorClear( ent->s.pos.trDelta );

    VectorCopy( origin, ent->r.currentOrigin );
}

/*
================
DebugLine

  debug polygons only work when running a local game
  with r_debugSurface set to 2
================
*/
int DebugLine(vec3_t start, vec3_t end, int color) {
    vec3_t points[4], dir, cross, up = {0, 0, 1};
    float dot;

    VectorCopy(start, points[0]);
    VectorCopy(start, points[1]);
    //points[1][2] -= 2;
    VectorCopy(end, points[2]);
    //points[2][2] -= 2;
    VectorCopy(end, points[3]);


    VectorSubtract(end, start, dir);
    VectorNormalize(dir);
    dot = DotProduct(dir, up);
    if (dot > 0.99 || dot < -0.99) VectorSet(cross, 1, 0, 0);
    else CrossProduct(dir, up, cross);

    VectorNormalize(cross);

    VectorMA(points[0], 2, cross, points[0]);
    VectorMA(points[1], -2, cross, points[1]);
    VectorMA(points[2], -2, cross, points[2]);
    VectorMA(points[3], 2, cross, points[3]);

    return trap_DebugPolygonCreate(color, 4, points);
}

