// Copyright (C) 2001-2002 Raven Software.
//

#include "g_local.h"
#include "1fx/1fxFunctions.h"

qboolean G_SpawnString( const char *key, const char *defaultString, char **out )
{
    int     i;

    if ( !level.spawning )
    {
        *out = (char *)defaultString;
    }

    for ( i = 0 ; i < level.numSpawnVars ; i++ )
    {
        if ( !Q_stricmp( key, level.spawnVars[i][0] ) )
        {
            *out = level.spawnVars[i][1];
            return qtrue;
        }
    }

    *out = (char *)defaultString;
    return qfalse;
}

qboolean G_SpawnFloat( const char *key, const char *defaultString, float *out )
{
    char        *s;
    qboolean    present;

    present = G_SpawnString( key, defaultString, &s );
    *out = atof( s );
    return present;
}

qboolean G_SpawnInt( const char *key, const char *defaultString, int *out )
{
    char        *s;
    qboolean    present;

    present = G_SpawnString( key, defaultString, &s );
    *out = atoi( s );
    return present;
}

qboolean G_SpawnVector( const char *key, const char *defaultString, float *out )
{
    char        *s;
    qboolean    present;

    present = G_SpawnString( key, defaultString, &s );
    sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
    return present;
}



//
// fields are needed for spawning from the entity string
//
typedef enum
{
    F_INT,
    F_FLOAT,
    F_LSTRING,          // string on disk, pointer in memory, TAG_LEVEL
    F_GSTRING,          // string on disk, pointer in memory, TAG_GAME
    F_VECTOR,
    F_ANGLEHACK,
    F_ENTITY,           // index on disk, pointer in memory
    F_ITEM,             // index on disk, pointer in memory
    F_CLIENT,           // index on disk, pointer in memory
    F_IGNORE

} fieldtype_t;

typedef struct
{
    char*       name;
    int         ofs;
    fieldtype_t type;
    int         flags;

} field_t;

field_t fields[] =
{
    {"classname",           FOFS(classname),            F_LSTRING},
    {"origin",              FOFS(s.origin),             F_VECTOR},
    {"model",               FOFS(model),                F_LSTRING},
    {"model2",              FOFS(model2),               F_LSTRING},
    {"spawnflags",          FOFS(spawnflags),           F_INT},
    {"speed",               FOFS(speed),                F_FLOAT},
    {"target",              FOFS(target),               F_LSTRING},
    {"targetname",          FOFS(targetname),           F_LSTRING},
    {"message",             FOFS(message),              F_LSTRING},
    {"team",                FOFS(team),                 F_LSTRING},
    {"wait",                FOFS(wait),                 F_FLOAT},
    {"random",              FOFS(random),               F_FLOAT},
    {"count",               FOFS(count),                F_INT},
    {"health",              FOFS(health),               F_INT},
    {"light",               0,                          F_IGNORE},
    {"dmg",                 FOFS(damage),               F_INT},
    {"angles",              FOFS(s.angles),             F_VECTOR},
    {"angle",               FOFS(s.angles),             F_ANGLEHACK},
    {"targetShaderName",    FOFS(targetShaderName),     F_LSTRING},
    {"targetShaderNewName", FOFS(targetShaderNewName),  F_LSTRING},

    {NULL}
};


typedef struct
{
    char    *name;
    void    (*spawn)(gentity_t *ent);

} spawn_t;

void SP_info_player_deathmatch      (gentity_t *ent);
void SP_info_player_intermission    (gentity_t *ent);

void SP_func_plat                   (gentity_t *ent);
void SP_func_static                 (gentity_t *ent);
void SP_func_rotating               (gentity_t *ent);
void SP_func_bobbing                (gentity_t *ent);
void SP_func_pendulum               (gentity_t *ent);
void SP_func_button                 (gentity_t *ent);
void SP_func_door                   (gentity_t *ent);
void SP_func_train                  (gentity_t *ent);
void SP_func_timer                  (gentity_t *ent);
void SP_func_glass                  (gentity_t *ent);
void SP_func_wall                   (gentity_t *ent);

void SP_trigger_always              (gentity_t *ent);
void SP_trigger_multiple            (gentity_t *ent);
void SP_trigger_push                (gentity_t *ent);
void SP_trigger_teleport            (gentity_t *ent);
void SP_trigger_hurt                (gentity_t *ent);
void SP_trigger_ladder              (gentity_t *ent);

void SP_target_give                 (gentity_t *ent);
void SP_target_delay                (gentity_t *ent);
void SP_target_speaker              (gentity_t *ent);
void SP_target_print                (gentity_t *ent);
void SP_target_laser                (gentity_t *ent);
void SP_target_score                (gentity_t *ent);
void SP_target_teleporter           (gentity_t *ent);
void SP_target_relay                (gentity_t *ent);
void SP_target_kill                 (gentity_t *ent);
void SP_target_position             (gentity_t *ent);
void SP_target_location             (gentity_t *ent);
void SP_target_push                 (gentity_t *ent);
void SP_target_effect               (gentity_t *ent);

void SP_info_notnull                (gentity_t *ent);
void SP_info_camp                   (gentity_t *ent);
void SP_path_corner                 (gentity_t *ent);

void SP_misc_teleporter_dest        (gentity_t *ent);
void SP_misc_model                  (gentity_t *ent);
void SP_misc_G2model                (gentity_t *ent);
void SP_misc_portal_camera          (gentity_t *ent);
void SP_misc_portal_surface         (gentity_t *ent);
void SP_misc_bsp                    (gentity_t *ent);
void SP_terrain                     (gentity_t *ent);

void SP_model_static                (gentity_t* ent);

void SP_gametype_item               (gentity_t* ent);
void SP_gametype_trigger            (gentity_t* ent);
void SP_gametype_player             (gentity_t* ent);
void SP_mission_player              (gentity_t* ent);

void SP_fx_play_effect              (gentity_t* ent);

spawn_t spawns[] =
{
    // info entities don't do anything at all, but provide positional
    // information for things controlled by other processes
    {"info_player_deathmatch",      SP_info_player_deathmatch},
    {"info_player_intermission",    SP_info_player_intermission},
    {"info_notnull",                SP_info_notnull},       // use target_position instead

    {"func_plat",                   SP_func_plat},
    {"func_button",                 SP_func_button},
    {"func_door",                   SP_func_door},
    {"func_static",                 SP_func_static},
    {"func_rotating",               SP_func_rotating},
    {"func_bobbing",                SP_func_bobbing},
    {"func_pendulum",               SP_func_pendulum},
    {"func_train",                  SP_func_train},
    {"func_timer",                  SP_func_timer},
    {"func_glass",                  SP_func_glass},
    {"func_wall",                   SP_func_wall},

    // Triggers are brush objects that cause an effect when contacted
    // by a living player, usually involving firing targets.
    // While almost everything could be done with
    // a single trigger class and different targets, triggered effects
    // could not be client side predicted (push and teleport).
    {"trigger_always",              SP_trigger_always},
    {"trigger_multiple",            SP_trigger_multiple},
    {"trigger_push",                SP_trigger_push},
    {"trigger_teleport",            SP_trigger_teleport},
    {"trigger_hurt",                SP_trigger_hurt},
    {"trigger_ladder",              SP_trigger_ladder },

    // targets perform no action by themselves, but must be triggered
    // by another entity
    {"target_give",                 SP_target_give},
    {"target_delay",                SP_target_delay},
    {"target_speaker",              SP_target_speaker},
    {"target_print",                SP_target_print},
    {"target_laser",                SP_target_laser},
    {"target_score",                SP_target_score},
    {"target_teleporter",           SP_target_teleporter},
    {"target_relay",                SP_target_relay},
    {"target_kill",                 SP_target_kill},
    {"target_position",             SP_target_position},
    {"target_location",             SP_target_location},
    {"target_push",                 SP_target_push},
    {"target_effect",               SP_target_effect},

    {"path_corner",                 SP_path_corner},

    {"misc_teleporter_dest",        SP_misc_teleporter_dest},
    {"misc_model",                  SP_misc_model},
    {"client_model",                SP_model_static},
    {"misc_G2model",                SP_misc_G2model},
    {"misc_portal_surface",         SP_misc_portal_surface},
    {"misc_portal_camera",          SP_misc_portal_camera},
    {"misc_bsp",                    SP_misc_bsp},
    {"terrain",                     SP_terrain},

    {"model_static",                SP_model_static },

    {"gametype_item",               SP_gametype_item },
    {"gametype_trigger",            SP_gametype_trigger },
    {"gametype_player",             SP_gametype_player },
    {"mission_player",              SP_mission_player },

    // stuff from SP emulated
    {"func_breakable_brush",        SP_func_static},
    {"fx_play_effect",              SP_fx_play_effect},

    // The following classnames are instantly removed when spawned.  The RMG
    // shares instances with single player which is what causes these things
    // to attempt to spawn
    {"light",                       0},
    {"func_group",                  0},
    {"info_camp",                   0},
    {"info_null",                   0},
    {"door_rotating",               0},
    {"emplaced_wpn",                0},
    {"info_NPC*",                   0},
    {"info_player_start",           0},
    {"NPC_*",                       0},
    {"ce_*",                        0},
    {"pickup_ammo",                 0},
    {"script_runner",               0},
    {"trigger_arioche_objective",   0},
    {"func_brushmodel_child",       0},

    // 1fx additions
    {"seekers",                     parseCustomETHiders},

    {0, 0}
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gentity_t *ent )
{
    spawn_t *s;
    gitem_t *item;

    if ( !ent->classname )
    {
        Com_Printf ("G_CallSpawn: NULL classname\n");
        return qfalse;
    }

    // check item spawn functions
    for ( item=bg_itemlist+1 ; item->classname ; item++ )
    {
        if ( !strcmp(item->classname, ent->classname) )
        {
            // If this is a backpack then handle it specially
            if ( item->giType == IT_BACKPACK )
            {
                if ( !level.gametypeData->backpack )
                {
                    return qfalse;
                }

                G_SpawnItem ( ent, item );
                return qtrue;
            }

            // Make sure pickups arent disabled
            if ( !level.pickupsDisabled )
            {
                G_SpawnItem( ent, item );
                return qtrue;
            }
            else
            {   // Pickups dont spawn when disabled - this avoids the "doesn't have a spawn function" message
                return qfalse;
            }
        }
    }

    // check normal spawn functions
    for ( s=spawns ; s->name ; s++ )
    {
        char* wildcard = strchr ( s->name, '*' );
        int   result;

        if ( wildcard )
        {
            result = Q_strncmp ( s->name, ent->classname, wildcard - s->name );
        }
        else
        {
            result = strcmp(s->name, ent->classname);
        }

        if ( !result )
        {
            if (s->spawn)
            {   // found it
                s->spawn(ent);
                return qtrue;
            }
            else
            {
                return qfalse;
            }
        }
    }

    Com_Printf ("%s doesn't have a spawn function\n", ent->classname);
    return qfalse;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
char *G_NewString( const char *string )
{
    char    *newb, *new_p;
    int     i,l;

    l = strlen(string) + 1;

    newb = G_Alloc(l);

    new_p = newb;

    // turn \n into a real linefeed
    for ( i=0 ; i< l ; i++ ) {
        if (string[i] == '\\' && i < l-1) {
            i++;
            if (string[i] == 'n') {
                *new_p++ = '\n';
            } else {
                *new_p++ = '\\';
            }
        } else {
            *new_p++ = string[i];
        }
    }

    return newb;
}

/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
void G_ParseField( const char *key, const char *value, gentity_t *ent ) {
    field_t *f;
    byte    *b;
    float   v;
    vec3_t  vec;

    for ( f=fields ; f->name ; f++ ) {
        if ( !Q_stricmp(f->name, key) ) {
            // found it
            b = (byte *)ent;

            switch( f->type ) {
            case F_LSTRING:
                *(char **)(b+f->ofs) = G_NewString (value);
                break;
            case F_VECTOR:
                sscanf (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
                ((float *)(b+f->ofs))[0] = vec[0];
                ((float *)(b+f->ofs))[1] = vec[1];
                ((float *)(b+f->ofs))[2] = vec[2];
                break;
            case F_INT:
                *(int *)(b+f->ofs) = atoi(value);
                break;
            case F_FLOAT:
                *(float *)(b+f->ofs) = atof(value);
                break;
            case F_ANGLEHACK:
                v = atof(value);
                ((float *)(b+f->ofs))[0] = 0;
                ((float *)(b+f->ofs))[1] = v;
                ((float *)(b+f->ofs))[2] = 0;
                break;
            default:
            case F_IGNORE:
                break;
            }
            return;
        }
    }
}

/*
===================
G_IsGametypeInList

Determines if the given gametype is in the given list.
===================
*/
qboolean G_IsGametypeInList ( const char* gametype, const char* list )
{
    const char* buf = (char*) list;
    char* token;

    while ( 1 )
    {
        token = COM_Parse ( &buf );
        if ( !token || !token[0] )
        {
            break;
        }

        if ( Q_stricmp ( token, gametype ) == 0 )
        {
            return qtrue;
        }
    }

    return qfalse;
}

/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
void G_SpawnGEntityFromSpawnVars( qboolean inSubBSP )
{
    int         i;
    gentity_t   *ent;
    char        *value;

    if (inSubBSP)
    {
        // filter out the unwanted entities
        G_SpawnString("filter", "", &value);
        if (value[0] && Q_stricmp(level.mFilter, value))
        {
            // we are not matching up to the filter, so no spawney
            return;
        }
    }

    // get the next free entity
    ent = G_Spawn();

    for ( i = 0 ; i < level.numSpawnVars ; i++ )
    {
        G_ParseField( level.spawnVars[i][0], level.spawnVars[i][1], ent );
    }

    // check for "notteam" flag (GT_DM)
    if ( level.gametypeData->teams )
    {
        G_SpawnInt( "notteam", "0", &i );
        if ( i )
        {
            G_FreeEntity( ent );
            return;
        }
    }
    else
    {
        G_SpawnInt( "notfree", "0", &i );
        if ( i )
        {
            G_FreeEntity( ent );
            return;
        }
    }

    // Only spawn this entity in the specified gametype
    if( G_SpawnString( "gametype", NULL, &value ) && value )
    {
        if ( !G_IsGametypeInList ( level.gametypeData->name, value ) )
        {
            if ( level.gametypeData->basegametype )
            {
                if ( !G_IsGametypeInList ( level.gametypeData->basegametype, value ) )
                {
                    G_FreeEntity ( ent );
                    return;
                }
            }
            else
            {
                G_FreeEntity ( ent );
                return;
            }
        }
    }

    // move editor origin to pos
    VectorCopy( ent->s.origin, ent->s.pos.trBase );
    VectorCopy( ent->s.origin, ent->r.currentOrigin );

    // if we didn't get a classname, don't bother spawning anything
    if ( !G_CallSpawn( ent ) )
    {
        G_FreeEntity( ent );
    }
}



/*
====================
G_AddSpawnVarToken
====================
*/
char *G_AddSpawnVarToken( const char *string )
{
    int     l;
    char    *dest;

    l = strlen( string );
    if ( level.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS )
    {
        Com_Error( ERR_FATAL, "G_AddSpawnVarToken: MAX_SPAWN_CHARS" );
    }

    dest = level.spawnVarChars + level.numSpawnVarChars;
    memcpy( dest, string, l+1 );

    level.numSpawnVarChars += l + 1;

    return dest;
}

void AddSpawnField(char *field, char *value)
{
    int i;

    for(i=0;i<level.numSpawnVars;i++)
    {
        if (Q_stricmp(level.spawnVars[i][0], field) == 0)
        {
            level.spawnVars[ i ][1] = G_AddSpawnVarToken( value );
            return;
        }
    }

    level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( field );
    level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( value );
    level.numSpawnVars++;
}

#define NOVALUE "novalue"

static void HandleEntityAdjustment(void)
{
    char        *value;
    vec3_t      origin, newOrigin, angles;
    char        temp[MAX_QPATH];
    float       rotation;

    G_SpawnString("origin", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        sscanf( value, "%f %f %f", &origin[0], &origin[1], &origin[2] );
    }
    else
    {
        origin[0] = origin[1] = origin[2] = 0.0;
    }

    rotation = DEG2RAD(level.mRotationAdjust);
    newOrigin[0] = origin[0]*cos(rotation) - origin[1]*sin(rotation);
    newOrigin[1] = origin[0]*sin(rotation) + origin[1]*cos(rotation);
    newOrigin[2] = origin[2];
    VectorAdd(newOrigin, level.mOriginAdjust, newOrigin);
    // damn VMs don't handle outputing a float that is compatible with sscanf in all cases
    Com_sprintf(temp, MAX_QPATH, "%0.0f %0.0f %0.0f", newOrigin[0], newOrigin[1], newOrigin[2]);
    AddSpawnField("origin", temp);

    G_SpawnString("angles", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        sscanf( value, "%f %f %f", &angles[0], &angles[1], &angles[2] );

        angles[1] = fmod(angles[1] + level.mRotationAdjust, 360.0);
        // damn VMs don't handle outputing a float that is compatible with sscanf in all cases
        Com_sprintf(temp, MAX_QPATH, "%0.0f %0.0f %0.0f", angles[0], angles[1], angles[2]);
        AddSpawnField("angles", temp);
    }
    else
    {
        G_SpawnString("angle", NOVALUE, &value);
        if (Q_stricmp(value, NOVALUE) != 0)
        {
            sscanf( value, "%f", &angles[1] );
        }
        else
        {
            angles[1] = 0.0;
        }
        angles[1] = fmod(angles[1] + level.mRotationAdjust, 360.0);
        Com_sprintf(temp, MAX_QPATH, "%0.0f", angles[1]);
        AddSpawnField("angle", temp);
    }

    // RJR experimental code for handling "direction" field of breakable brushes
    // though direction is rarely ever used.
    G_SpawnString("direction", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        sscanf( value, "%f %f %f", &angles[0], &angles[1], &angles[2] );
    }
    else
    {
        angles[0] = angles[1] = angles[2] = 0.0;
    }
    angles[1] = fmod(angles[1] + level.mRotationAdjust, 360.0);
    Com_sprintf(temp, MAX_QPATH, "%0.0f %0.0f %0.0f", angles[0], angles[1], angles[2]);
    AddSpawnField("direction", temp);


    AddSpawnField("BSPInstanceID", level.mTargetAdjust);

    G_SpawnString("targetname", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("targetname", temp);
    }

    G_SpawnString("target", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("target", temp);
    }

    G_SpawnString("killtarget", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("killtarget", temp);
    }

    G_SpawnString("brushparent", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("brushparent", temp);
    }

    G_SpawnString("brushchild", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("brushchild", temp);
    }

    G_SpawnString("enemy", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("enemy", temp);
    }

    G_SpawnString("ICARUSname", NOVALUE, &value);
    if (Q_stricmp(value, NOVALUE) != 0)
    {
        Com_sprintf(temp, MAX_QPATH, "%s%s", level.mTargetAdjust, value);
        AddSpawnField("ICARUSname", temp);
    }
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( qboolean inSubBSP )
{
    char    keyname[MAX_TOKEN_CHARS];
    char    com_token[MAX_TOKEN_CHARS];

    level.numSpawnVars = 0;
    level.numSpawnVarChars = 0;

    // parse the opening brace
    if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) )
    {
        // end of spawn string
        return qfalse;
    }

    if ( com_token[0] != '{' )
    {
        Com_Error( ERR_FATAL, "G_ParseSpawnVars: found %s when expecting {",com_token );
    }

    // go through all the key / value pairs
    while ( 1 )
    {
        // parse key
        if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) )
        {
            Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );
        }

        if ( keyname[0] == '}' )
        {
            break;
        }

        // parse value
        if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) )
        {
            Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );
        }

        if ( com_token[0] == '}' )
        {
            Com_Error( ERR_FATAL, "G_ParseSpawnVars: closing brace without data" );
        }

        if ( level.numSpawnVars == MAX_SPAWN_VARS )
        {
            Com_Error( ERR_FATAL, "G_ParseSpawnVars: MAX_SPAWN_VARS" );
        }

        AddSpawnField(keyname, com_token);
    }

    if (inSubBSP)
    {
        HandleEntityAdjustment();
    }

    return qtrue;
}

static char *defaultStyles[32][3] =
{
    {   // 0 normal
        "z",
        "z",
        "z"
    },
    {   // 1 FLICKER (first variety)
        "mmnmmommommnonmmonqnmmo",
        "mmnmmommommnonmmonqnmmo",
        "mmnmmommommnonmmonqnmmo"
    },
    {   // 2 SLOW STRONG PULSE
        "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcb",
        "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcb",
        "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcb"
    },
    {   // 3 CANDLE (first variety)
        "mmmmmaaaaammmmmaaaaaabcdefgabcdefg",
        "mmmmmaaaaammmmmaaaaaabcdefgabcdefg",
        "mmmmmaaaaammmmmaaaaaabcdefgabcdefg"
    },
    {   // 4 FAST STROBE
        "mamamamamama",
        "mamamamamama",
        "mamamamamama"
    },
    {   // 5 GENTLE PULSE 1
        "jklmnopqrstuvwxyzyxwvutsrqponmlkj",
        "jklmnopqrstuvwxyzyxwvutsrqponmlkj",
        "jklmnopqrstuvwxyzyxwvutsrqponmlkj"
    },
    {   // 6 FLICKER (second variety)
        "nmonqnmomnmomomno",
        "nmonqnmomnmomomno",
        "nmonqnmomnmomomno"
    },
    {   // 7 CANDLE (second variety)
        "mmmaaaabcdefgmmmmaaaammmaamm",
        "mmmaaaabcdefgmmmmaaaammmaamm",
        "mmmaaaabcdefgmmmmaaaammmaamm"
    },
    {   // 8 CANDLE (third variety)
        "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",
        "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",
        "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa"
    },
    {   // 9 SLOW STROBE (fourth variety)
        "aaaaaaaazzzzzzzz",
        "aaaaaaaazzzzzzzz",
        "aaaaaaaazzzzzzzz"
    },
    {   // 10 FLUORESCENT FLICKER
        "mmamammmmammamamaaamammma",
        "mmamammmmammamamaaamammma",
        "mmamammmmammamamaaamammma"
    },
    {   // 11 SLOW PULSE NOT FADE TO BLACK
        "abcdefghijklmnopqrrqponmlkjihgfedcba",
        "abcdefghijklmnopqrrqponmlkjihgfedcba",
        "abcdefghijklmnopqrrqponmlkjihgfedcba"
    },
    {   // 12 FAST PULSE FOR JEREMY
        "mkigegik",
        "mkigegik",
        "mkigegik"
    },
    {   // 13 Test Blending
        "abcdefghijklmqrstuvwxyz",
        "zyxwvutsrqmlkjihgfedcba",
        "aammbbzzccllcckkffyyggp"
    },
    {   // 14
        "",
        "",
        ""
    },
    {   // 15
        "",
        "",
        ""
    },
    {   // 16
        "",
        "",
        ""
    },
    {   // 17
        "",
        "",
        ""
    },
    {   // 18
        "",
        "",
        ""
    },
    {   // 19
        "",
        "",
        ""
    },
    {   // 20
        "",
        "",
        ""
    },
    {   // 21
        "",
        "",
        ""
    },
    {   // 22
        "",
        "",
        ""
    },
    {   // 23
        "",
        "",
        ""
    },
    {   // 24
        "",
        "",
        ""
    },
    {   // 25
        "",
        "",
        ""
    },
    {   // 26
        "",
        "",
        ""
    },
    {   // 27
        "",
        "",
        ""
    },
    {   // 28
        "",
        "",
        ""
    },
    {   // 29
        "",
        "",
        ""
    },
    {   // 30
        "",
        "",
        ""
    },
    {   // 31
        "",
        "",
        ""
    }
};

qboolean SP_bsp_worldspawn ( void )
{
    return qtrue;
}

/*QUAKED worldspawn (0 0 0) ?

Every map should have exactly one worldspawn.
"music"         music wav file
"soundSet"      soundset name to use (do not combine with 'noise', ignores all other flags)
"gravity"       800 is default gravity
"message"       Text to print during connection process
"mission"       Indicates which mission script file should be used to find the scripts for mission mode
*/
void SP_worldspawn( void )
{
    char        *text, temp[32];
    int         i;
    int         lengthRed, lengthBlue, lengthGreen;

    G_SpawnString( "classname", "", &text );
    if ( Q_stricmp( text, "worldspawn" ) )
    {
        Com_Error( ERR_FATAL, "SP_worldspawn: The first entity isn't 'worldspawn'" );
    }

    // make some data visible to connecting client
    trap_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

    trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );

    G_SpawnString( "music", "", &text );
    trap_SetConfigstring( CS_MUSIC, text );

    if (G_SpawnString( "soundSet", "", &text ) )
    {
        trap_SetConfigstring(CS_AMBIENT_SOUNDSETS, text );
    }

    if ( level.gametypeData->teams )
    {
        G_SpawnString( "redteam", "", &text );
        if ( text && *text )
        {
            level.gametypeTeam[TEAM_RED] = G_StringAlloc ( text );
        }

        G_SpawnString( "blueteam", "", &text );
        if ( text && *text )
        {
            level.gametypeTeam[TEAM_BLUE] = G_StringAlloc ( text );
        }

        if ( !level.gametypeTeam[TEAM_RED]  ||
             !level.gametypeTeam[TEAM_BLUE]    )
        {
            level.gametypeTeam[TEAM_RED] = "marine";
            level.gametypeTeam[TEAM_BLUE] = "thug";
        }

        trap_SetConfigstring( CS_GAMETYPE_REDTEAM, level.gametypeTeam[TEAM_RED] );
        trap_SetConfigstring( CS_GAMETYPE_BLUETEAM, level.gametypeTeam[TEAM_BLUE] );
    }

    G_SpawnString( "message", "", &text );
    trap_SetConfigstring( CS_MESSAGE, text );               // map specific message

    trap_SetConfigstring( CS_MOTD, g_motd.string );     // message of the day

    G_SpawnString( "gravity", va("%d", g_gravity.integer), &text );
    trap_Cvar_Set( "g_gravity", text );

    // Handle all the worldspawn stuff common to both main bsp and sub bsp
    SP_bsp_worldspawn ( );

    g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
    g_entities[ENTITYNUM_WORLD].classname = "worldspawn";

    // see if we want a warmup time
    trap_SetConfigstring( CS_WARMUP, "" );
    if ( g_restarted.integer )
    {
        trap_Cvar_Set( "g_restarted", "0" );
        level.warmupTime = 0;
    }
    else if ( g_doWarmup.integer )
    {
        // Turn it on
        level.warmupTime = -1;
        trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
        G_LogPrintf( "Warmup:\n" );
    }

    trap_SetConfigstring(CS_LIGHT_STYLES+(LS_STYLES_START*3)+0, defaultStyles[0][0]);
    trap_SetConfigstring(CS_LIGHT_STYLES+(LS_STYLES_START*3)+1, defaultStyles[0][1]);
    trap_SetConfigstring(CS_LIGHT_STYLES+(LS_STYLES_START*3)+2, defaultStyles[0][2]);

    for(i=1;i<LS_NUM_STYLES;i++)
    {
        Com_sprintf(temp, sizeof(temp), "ls_%dr", i);
        G_SpawnString(temp, defaultStyles[i][0], &text);
        lengthRed = strlen(text);
        trap_SetConfigstring(CS_LIGHT_STYLES+((i+LS_STYLES_START)*3)+0, text);

        Com_sprintf(temp, sizeof(temp), "ls_%dg", i);
        G_SpawnString(temp, defaultStyles[i][1], &text);
        lengthGreen = strlen(text);
        trap_SetConfigstring(CS_LIGHT_STYLES+((i+LS_STYLES_START)*3)+1, text);

        Com_sprintf(temp, sizeof(temp), "ls_%db", i);
        G_SpawnString(temp, defaultStyles[i][2], &text);
        lengthBlue = strlen(text);
        trap_SetConfigstring(CS_LIGHT_STYLES+((i+LS_STYLES_START)*3)+2, text);

        if (lengthRed != lengthGreen || lengthGreen != lengthBlue)
        {
            Com_Error(ERR_DROP, "Style %d has inconsistent lengths: R %d, G %d, B %d",
                i, lengthRed, lengthGreen, lengthBlue);
        }
    }
}


/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( qboolean inSubBSP )
{
    // allow calls to G_Spawn*()
    level.spawning = qtrue;
    level.numSpawnVars = 0;

    // the worldspawn is not an actual entity, but it still
    // has a "spawn" function to perform any global setup
    // needed by a level (setting configstrings or cvars, etc)
    if ( !G_ParseSpawnVars(inSubBSP) )
    {
        Com_Error( ERR_FATAL, "SpawnEntities: no entities" );
    }

    if (!inSubBSP)
    {
        SP_worldspawn();
    }
    else
    {
        // Skip this guy if its worldspawn fails
        if ( !SP_bsp_worldspawn() )
        {
            return;
        }
    }

    // parse ents
    while( G_ParseSpawnVars(inSubBSP) )
    {
        G_SpawnGEntityFromSpawnVars(inSubBSP);
    }

    if (!inSubBSP)
    {
        level.spawning = qfalse;            // any future calls to G_Spawn*() will be errors
    }
}

/*QUAKED model_static (1 0 0) (-16 -16 -16) (16 16 16) NO_MP
"model"     arbitrary .md3 file to display
*/
void SP_model_static ( gentity_t* ent )
{
    if (ent->spawnflags & 1)
    {   // NO_MULTIPLAYER
        G_FreeEntity( ent );
    }

    G_SetOrigin( ent, ent->s.origin );

    VectorCopy(ent->s.angles, ent->r.currentAngles);
    VectorCopy(ent->s.angles, ent->s.apos.trBase );

    VectorCopy( ent->s.origin, ent->s.pos.trBase );
    VectorCopy( ent->s.origin, ent->r.currentOrigin );

    ent->s.modelindex = G_ModelIndex( ent->model );
    ent->s.pos.trType = TR_STATIONARY;
    ent->s.apos.trTime = level.time;

    if (level.mBSPInstanceDepth)
    {   // this means that this guy will never be updated, moved, changed, etc.
        ent->s.eFlags = EF_PERMANENT;
    }

    trap_LinkEntity ( ent );
}


