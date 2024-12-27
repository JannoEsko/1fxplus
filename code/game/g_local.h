// Copyright (C) 2001-2002 Raven Software.
//
// g_local.h -- local definitions for game module

#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "g_public.h"
#include "../gametype/gt_public.h"
#include "../ghoul2/g2_local.h"
#include <curl/curl.h>

//==================================================================

// the "gameversion" client command will print this plus compile date
#define GAMEVERSION "sof2mp"

#define MODNAME "1fxplus"
#define MODNAME_COLORED "^71fx^1plus.^7"
#define MODVERSION "0.01alpha"
#define MOD_MOTD_INFO "Inspired by 1fx. Mod by ^GBoe!Man ^7& ^6Henkie\n^7Running on SoF2Plus, which is a fork of ioquake3\nMade compatible with SoF2 by ^GBoe!Man\n^71fx^1plus^7 Multiprotocol by 1fx^1.^K # ^7Janno\nhttps://github.com/sof2plus\nhttps://github.com/JannoEsko/1fxplus\n\n"

#define BODY_QUEUE_SIZE_MAX         MAX_CLIENTS
#define BODY_QUEUE_SIZE             8

#define GAME_INFINITE               1000000
#define Q3_INFINITE                 16777216

#define FRAMETIME                   100                 // msec

#define INTERMISSION_DELAY_TIME     3000

// gentity->flags
#define FL_GODMODE                  0x00000010
#define FL_NOTARGET                 0x00000020
#define FL_TEAMSLAVE                0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK             0x00000800
#define FL_DROPPED_ITEM             0x00001000
#define FL_NO_BOTS                  0x00002000  // spawn point not for bot use
#define FL_NO_HUMANS                0x00004000  // spawn point just for bots

#define MAX_SPAWNS                  128

#define MAX_CLIENT_MOD 10
#define MAX_AC_GUID 12

#define MAX_NETNAME         36
#define MAX_IDENTITY        64
#define MAX_VOTE_COUNT      3

#define MAX_IP                      40          // ipv6 theoretical max
#define MAX_COUNTRYCODE             10
#define MAX_COUNTRYNAME             50
#define MAX_THREAD_OUTPUT           128

#define TOTAL_SECTIONS              4
#define MAX_CUSTOM_ET_AMOUNT        16

typedef enum {
    CL_NONE,
    CL_RPM,
    CL_ROCMOD,
    CL_1FXROCMOD
} clientMod_t ;

// BoeMan 8/30/14: Broadcast priorities, from low to high.
typedef enum {
    BROADCAST_GAME,             // Regular game messages.
    BROADCAST_CMD,              // Admin commands such as uppercut, broadcast, etc.
    BROADCAST_GAME_IMPORTANT,   // More important gametype messages that should override Admin commands.
    BROADCAST_AWARDS,           // Awards (H&S and regular).
    BROADCAST_MOTD              // Message of the day when entering the server.
} broadcastPrio_t;

typedef enum {
    COMPMODE_NONE,
    COMPMODE_INITIALIZED,
    COMPMODE_ROUND1,
    COMPMODE_PRE_ROUND2,
    COMPMODE_ROUND2,
    COMPMODE_END
} compModeState;

typedef enum {
    TEAMACTION_DONE,
    TEAMACTION_INCOMPATIBLE_GAMETYPE,
    TEAMACTION_EVEN,
    TEAMACTION_NOT_ENOUGH_PLAYERS,
    TEAMACTION_TEAM_LOCKED,
    TEAMACTION_FAILED
} teamAction_t;

typedef enum {
    POPACTION_NONE,
    POPACTION_ADMIN,
    POPACTION_CAMP
} popAction_t;

typedef enum {
    GT_NONE,
    GT_HNS,
    GT_DEM,
    GT_INF,
    GT_CTF,
    GT_DM,
    GT_TDM,
    GT_ELIM,
    GT_HNZ,
    GT_CSINF,
    GT_MM,
    GT_VIP,
    GT_PROP,
    GT_GUNGAME,
    GT_MAX
} gameTypes_t;

typedef enum {
    WEAPONMOD_DEFAULT,
    WEAPONMOD_ND,
    WEAPONMOD_RD,
    WEAPONMOD_CUSTOM
} weaponMod_t;

typedef enum {
    HSEXTRA_MDN11,
    HSEXTRA_F1,
    HSEXTRA_L2A2,
    HSEXTRA_GOGGLES,
    HSEXTRA_BRIEFCASE,
    HSEXTRA_RANDOMGRENADE,
} hideseekExtra_t;

typedef enum {
    HSNADE_FRAG,
    HSNADE_FLASH,
    HSNADE_SMOKE,
    HSNADE_FIRE
} hideseekNades_t;

typedef enum {
    HSWPN_RPG,
    HSWPN_M4,
    HSWPN_MM1,
    HSWPN_TELEGUN,
    HSWPN_TASER
} hideseekWeapons_t;

typedef enum {
    SPEEDALTERATION_NONE,
    SPEEDALTERATION_MM1,
    SPEEDALTERATION_M4,
    SPEEDALTERATION_KNIFE,
    SPEEDALTERATION_FIRENADE,
    SPEEDALTERATION_WATER,
    SPEEDALTERATION_STUNGUN
} speedAlterationReason_t;

typedef struct speedAlteration_s {
    speedAlterationReason_t     speedAlterationReason;
    int                         speedAlterationFrom;
    int                         speedAlterationTo;
    int                         speedAlterationDuration;
} speedAlteration_t;

/*
Mutes from 1fxmod.
*/
typedef struct mute_s {
    char        ip[MAX_IP];     // IP of the muted client.
    int         time;           // Total duration of the mute in msec.
    int         startTime;      // level.time of when the mute started.
    qboolean    used;           // True if this slot is used.
    int         totalDuration;  // Total duration in minutes, so we still have the original value when reading the session data.
} mute_t;

/* Section information from 1fxmod */

typedef enum
{
    MAPSECTION_NOLOWER,
    MAPSECTION_NOROOF,
    MAPSECTION_NOMIDDLE,
    MAPSECTION_NOWHOLE
} mapSection_t;

typedef enum
{
    MAPSECTIONSTATE_INIT,
    MAPSECTIONSTATE_OPENING,
    MAPSECTIONSTATE_OPENED,
    MAPSECTIONSTATE_CLOSING,
    MAPSECTIONSTATE_CLOSED
} mapSectionState_t;

typedef enum {
    MAPACTION_NONE,
    MAPACTION_ENDING,
    MAPACTION_PENDING_RESTART,
    MAPACTION_PENDING_GT,
    MAPACTION_PENDING_MAPCHANGE,
    MAPACTION_PENDING_MAPGTCHANGE,
    MAPACTION_PENDING_MAPCYCLE,
    MAPACTION_UNPAUSE
} mapAction_t;

typedef struct statInfo_s {
    int         weapon_shots[ATTACK_MAX * WP_NUM_WEAPONS];
    int         weapon_hits[ATTACK_MAX * WP_NUM_WEAPONS];
    int         weapon_headshots[ATTACK_MAX * WP_NUM_WEAPONS];
    int         weapon;
    int         attack;
    int         shotcount;
    int         hitcount;
    float       accuracy;
    float       ratio;
    int         handhits;
    int         foothits;
    int         armhits;
    int         leghits;
    int         headhits;
    int         neckhits;
    int         torsohits;
    int         waisthits;
    int         headShotKills;
    int         kills;
    int         killsinarow;
    int         deaths;
    int         damageDone;
    int         damageTaken;
    int         lastclient_hurt;
    int         lasthurtby;
    int         lastKillerHealth;
    int         lastKillerArmor;
    int         overallScore;
    int         explosiveKills;
    int         knifeKills;
    int         bestKillsInARow;
    int         itemCaptures;
    int         itemDefends;
} statInfo_t;

// Flags to determine which extra features the client is willing to accept in ROCmod.
#define ROC_TEAMINFO    0x00000001
#define ROC_SPECLIST    0x00000002

// movers are things like doors, plats, buttons, etc
typedef enum
{
    MOVER_POS1,
    MOVER_POS2,
    MOVER_1TO2,
    MOVER_2TO1

} moverState_t;

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

struct gentity_s
{
    entityState_t   s;              // communicated by server to clients
    entityShared_t  r;              // shared by both the server system and game

    // DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
    // EXPECTS THE FIELDS IN THAT ORDER!
    //================================

    struct gclient_s    *client;            // NULL if not a client

    qboolean    inuse;

    char        *classname;         // set in QuakeEd
    int         spawnflags;         // set in QuakeEd

    qboolean    neverFree;          // if true, FreeEntity will only unlink
                                    // bodyque uses this

    int         flags;              // FL_* variables

    char        *model;
    char        *model2;
    int         freetime;           // level.time when the object was freed

    int         eventTime;          // events will be cleared EVENT_VALID_MSEC after set
    qboolean    freeAfterEvent;
    qboolean    unlinkAfterEvent;

    qboolean    physicsObject;      // if true, it can be pushed by movers and fall off edges
                                    // all game items are physicsObjects,
    float       physicsBounce;      // 1.0 = continuous bounce, 0.0 = no bounce
    int         clipmask;           // brushes with this content value will be collided against
                                    // when moving.  items and corpses do not collide against
                                    // players, for instance

    // movers
    moverState_t moverState;
    int         soundPos1;
    int         sound1to2;
    int         sound2to1;
    int         soundPos2;
    int         soundLoop;
    gentity_t   *parent;
    gentity_t   *nextTrain;
    vec3_t      pos1, pos2;

    char        *message;

    int         timestamp;      // body queue sinking, etc

    float       angle;          // set in editor, -1 = up, -2 = down
    char        *target;
    char        *targetname;
    char        *team;
    char        *targetShaderName;
    char        *targetShaderNewName;
    gentity_t   *target_ent;

    float       speed;
    vec3_t      movedir;
    float       mass;
    int         setTime;

//Think Functions
    int         nextthink;
    void        (*think)(gentity_t *self);
    void        (*reached)(gentity_t *self);    // movers call this when hitting endpoint
    void        (*blocked)(gentity_t *self, gentity_t *other);
    void        (*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
    void        (*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
    void        (*pain)(gentity_t *self, gentity_t *attacker, int damage);
    void        (*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );

    int         pain_debounce_time;
    int         fly_sound_debounce_time;    // wind tunnel
    int         last_move_time;

//Health and damage fields
    int         health;
    qboolean    takedamage;

    int         damage;
    int         dflags;
    int         splashDamage;   // quad will increase this without increasing radius
    int         splashRadius;
    int         methodOfDeath;
    int         splashMethodOfDeath;

    int         count;
    float       bounceScale;

    gentity_t   *chain;
    gentity_t   *enemy;
    gentity_t   *activator;
    gentity_t   *teamchain;     // next entity in team
    gentity_t   *teammaster;    // master of the team

    int         watertype;
    int         waterlevel;

    int         noise_index;

    // timing variables
    float       wait;
    float       random;
    int         delay;

    gitem_t     *item;          // for bonus items


    // Additions from 1fxmod.
    char*       bspmodel;
    int         minimumhiders;
    vec3_t      apos1;
    vec3_t      apos2;
    float       distance;
    char*       message2; // purpose of message2?
    int         up;
    int         forward;
    vec3_t      origin_from;
    vec3_t      origin_to;
    vec3_t      angles_from;
    vec3_t      angles_to;
    char*       both_sides;
    int         max_players;
    int         min_players;
    char*       autoSection;
    char*       invisible;
    int         hideseek;

    char*       endround;
    int         endround2; // needs rename?
    int         score;
    char*       broadcast;
    char*       effect_touch;

    // Boe!Man 5/22/12: Noise for specific entities (e.g. booster).
    char* sound;

    // Boe!Man 6/30/12: Size, for the hideseek_cage.
    char* size;

    // Boe!Man 11/21/13: Used for the auto section block system.
    team_t      team2;
    int         section;
    int         sectionState;

    // Boe!Man 11/26/15: Used for anticamp.
    vec3_t      camperOrigin;

    int         effect_index;
    qboolean    disabled;

};

typedef struct gspawn_s
{
    team_t      team;
    vec3_t      origin;
    vec3_t      angles;

} gspawn_t;

typedef struct gtitem_s
{
    int     id;
    int     useIcon;
    int     useSound;
    int     useTime;

} gtitem_t;

typedef enum
{
    CON_DISCONNECTED,
    CON_CONNECTING,
    CON_CONNECTED

} clientConnected_t;

typedef enum
{
    SPECTATOR_NOT,
    SPECTATOR_FREE,
    SPECTATOR_FOLLOW,
    SPECTATOR_FOLLOWFIRST,
    SPECTATOR_SCOREBOARD

} spectatorState_t;

typedef enum
{
    TEAM_BEGIN,     // Beginning a team game, spawn at base
    TEAM_ACTIVE     // Now actively playing

} playerTeamStateState_t;

typedef struct
{
    playerTeamStateState_t  state;

    int         location;

    int         captures;
    int         basedefense;
    int         carrierdefense;
    int         flagrecovery;
    int         fragcarrier;
    int         assists;

    float       lasthurtcarrier;
    float       lastreturnedflag;
    float       flagsince;
    float       lastfraggedcarrier;

} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define FOLLOW_ACTIVE1  -1
#define FOLLOW_ACTIVE2  -2

// Admin levels.
typedef enum {
    ADMLVL_NONE,
    ADMLVL_BADMIN,
    ADMLVL_ADMIN,
    ADMLVL_SADMIN,
    ADMLVL_HADMIN, // head admin. At least in 3D, we've had challenges in the past as the s-admin part starts from Captain, that we still need RCON (or dev) for some commands.
                  // Theoretically headadmin should give us enough room for it - upper staff = headadmin, sadmin = e.g. captains, editors etc.
    ADMLVL_RCON
} admLevel_t;

typedef enum {
    ADMTYPE_NONE,
    ADMTYPE_IP,
    ADMTYPE_PASS,
    ADMTYPE_OTP,
    ADMTYPE_GUID,
    ADMTYPE_RCON
} admType_t;

typedef enum {
    CLANTYPE_NONE,
    CLANTYPE_IP,
    CLANTYPE_PASS,
    CLANTYPE_GUID
} clanType_t;

typedef enum {
    LEVELSTATE_GAME,
    LEVELSTATE_AWARDS
} levelState_t;

typedef enum {
    COASTERSTATE_NOTHING,
    COASTERSTATE_RUNOVER,
    COASTERSTATE_UPPERCUT,
    COASTERSTATE_SPIN
} coasterState_t;

typedef enum {
    SPINVIEW_NONE,
    SPINVIEW_SLOW,
    SPINVIEW_FAST
} spinView_t;

typedef enum {
    NAMECHANGEBLOCK_NONE,
    NAMECHANGEBLOCK_PROFANITY,
    NAMECHANGEBLOCK_RENAME
} nameChangeBlock_t ;

typedef enum csInfNades_s {
    CSINFNADE_FLASH = 17,
    CSINFNADE_FRAG,
    CSINFNADE_FIRE,
    CSINFNADE_SMOKE
} csInfNades_t ;

#define CSINF_MAX_NADES 4

typedef struct csInfSpecifics_s {
    int cash;
    qboolean boughtUtility[CSINF_MAX_NADES];
    qboolean resetGuns;
} csInfSpecifics_t;

// client data that stays across multiple levels or map restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct
{
    team_t              team;                   // current team
    int                 spectatorTime;          // for determining next-in-line to play
    spectatorState_t    spectatorState;
    qboolean            spectatorFirstPerson;   // First person following?
    int                 spectatorClient;        // for chasecam and follow mode
    int                 score;                  // total score
    int                 kills;                  // number of kills
    int                 deaths;                 // number of deaths
    qboolean            ghost;                  // Whether or not the player is a ghost (sticks past team change)
    int                 teamkillDamage;         // amount of damage death to teammates
    int                 teamkillForgiveTime;    // time when team damage will be forgivin
    qboolean            noTeamChange;           // cant change teams when this is true (rt_none only)
    int                 chatIgnoreClients[2];   // Clients which are ignoring this client. [0] = (0-31)  [1] = (32-63)
    qboolean            muted;

    qboolean            legacyProtocol;
    admLevel_t            adminLevel;
    admType_t             adminType;

    clientMod_t         clientMod;
    char                clientVersion[MAX_CLIENT_MOD];
    qboolean            clanMember;
    qboolean            verifyRoxAC;
    qboolean            hasRoxAC;
    char                roxGuid[MAX_AC_GUID];
    qboolean            roxVerificationFailed;
    int                 nextRoxVerificationMessage;
    int                 roxVerificationAttempts;
    char                roxAcVersion[MAX_AC_GUID];

    qboolean            referee;

    int                 rocExtraFeatures;
    int                 rocClientChecks;
    int                 rocClientCheckTime;

    int                 lastMessagePriority;
    int                 lastMessage;
    qboolean            firstTime;
    int                 motdStartTime;
    int                 motdStopTime;
    int                 clientModCheckTime;
    int                 clientModChecks;
    char                adminName[MAX_NETNAME]; // This holds the name the client had when they got their admin powers.
    qboolean            setAdminPassword;
    admLevel_t          toBeAdminLevel;
    char                countryCode[MAX_COUNTRYCODE];
    char                country[MAX_COUNTRYNAME];
    qboolean            planted;
    int                 coaster;
    int                 nextCoasterTime;
    qboolean            spinView;
    int                 spinViewState;
    int                 nextSpin;
    int                 lastSpin;
    int                 nextSpinSound;
    qboolean            blockseek;

    clanType_t          clanType;
    char                clanName[MAX_NETNAME];
    qboolean            setClanPassword;
    int                 lastmsg;
    int                 lastTele;
    int                 acceleratorCooldown;
    int                 lastjump;
    int                 noroofCheckTime;

    int                 isOnRoofTime;
    qboolean            isOnRoof;
    nameChangeBlock_t   nameChangeBlock;

    qboolean            invisibilityNade;
    int                 invisibleNadeTime;
    qboolean            invisible;
    int                 invisibilityCooldown;
    int                 invisibletime;
    int                 invisibleFxTime; // The effect to play while being invisible.

    speedAlteration_t   speedDecrement;
    speedAlteration_t   speedIncrement;
    int                 rpgAnimation;
    int                 speedAnimation;

    vec3_t              oldvelocity;

    qboolean            transformed;

    int                 transformedEntity;
    int                 transformedEntityBBox;

    qboolean            checkClientAdditions;
    int                 clientAdditionCheckTime;
    qboolean            afkSpec;

    int             voiceFloodTimer;        // Timer used to forgive voice chat flooding
    int             voiceFloodCount;        // Amount of voice chats that need to be forgivin
    int             voiceFloodPenalty;      // Time when a client can voice chat again
    team_t              lastTeam;


    int                 stunAttacks;
    int                 gotStunned;
    int                 takenMM1;
    int                 trappedInCage;
    int                 MM1HitsTaken;
    int                 cageAttempts;
    int                 seekersCaged;
    qboolean            cageFighter;
    int                 mdnAttempts;
    int                 hsTimeOfDeath;

    int                 roundsWonAsHider;
    int                 rpgBoosts;
    int                 rpgTaken;
    int                 m4Taken;

    int                 weaponsStolen;

    int                 lastIdentityChange;
    int                 identityChangeCount;
    int                 lastPM;
    qboolean            privateMessageActive;
} clientSession_t;

//

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
    clientConnected_t   connected;
    usercmd_t           cmd;                        // we would lose angles if not persistant
    qboolean            localClient;                // true if "ip" info key is "localhost"
    qboolean            initialSpawn;               // the first spawn should be at a cool location
    qboolean            predictItemPickup;          // based on cg_predictItems userinfo
    qboolean            pmoveFixed;                 //
    qboolean            antiLag;                    // anti-lag on or off
    qboolean            autoReload;                 // auto rellaod turned on or off
    char                netname[MAX_NETNAME];
    char                deferredname[MAX_NETNAME];
    int                 netnameTime;                // Last time the name was changed
    TIdentity*          identity;                   // Current identity
    goutfitting_t       outfitting;                 // Current outfitting
    int                 enterTime;                  // level.time the client entered the game
    playerTeamState_t   teamState;                  // status in teamplay games
    int                 voteCount;                  // to prevent people from constantly calling votes
    int                 firemode[MAX_WEAPONS];      // weapon firemodes
    char                cleanName[MAX_NETNAME];
    char                ip[MAX_IP];
    char                subnet[MAX_IP];
    int                 burnSeconds;
    int                 oneSecondChecks;
    statInfo_t          statInfo;
    qboolean            seekerAway;
    int                 seekerAwayTime;
    int                 seekerAwayEnt;
    int                 lastpickup;
    int                 deathTime;
    


    qboolean            movingModel;
    qboolean            movingModelStatic;
    int                 movingModelObject;

    qboolean            isVip;
    qboolean            knifeBox;
    int                 killsAsSeekCurrentRound;

    csInfSpecifics_t    csinf;

} clientPersistant_t;

#define MAX_SERVER_FPS      40
#define MAX_ANTILAG         MAX_SERVER_FPS

// Antilag information
typedef struct gantilag_s
{
    vec3_t  rOrigin;                // entity.r.currentOrigin
    vec3_t  rAngles;                // entity.r.currentAngles
    vec3_t  mins;                   // entity.r.mins
    vec3_t  maxs;                   // entity.r.maxs

    vec3_t  legsAngles;             // entity.client.ghoulLegsAngles
    vec3_t  lowerTorsoAngles;       // entity.client.ghoulLowerTorsoAngles
    vec3_t  upperTorsoAngles;       // entity.client.ghoulUpperTorsoAngles
    vec3_t  headAngles;             // entity.client.ghoulHeadAngles

    int     time;                   // time history item was saved
    int     leveltime;

    int     torsoAnim;              // entity.client.ps.torsoAnim
    int     legsAnim;               // entity.client.ps.legsAnim

    float   leanTime;               // entity.client.ps.leanOffset
    int     pm_flags;               // entity.client.ps.pm_flags

} gantilag_t;

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s
{
    // ps MUST be the first element, because the server expects it
    playerState_t   ps;             // communicated by server to clients

    // the rest of the structure is private to game
    clientPersistant_t  pers;
    clientSession_t     sess;

    qboolean    readyToExit;        // wishes to leave the intermission

    qboolean    noclip;

    int         lastCmdTime;        // level.time of last usercmd_t, for EF_CONNECTION
                                    // we can't just use pers.lastCommand.time, because
                                    // of the g_sycronousclients case
    int         buttons;
    int         oldbuttons;
    int         latched_buttons;

    vec3_t      oldOrigin;

    // sum up damage over an entire frame, so
    // shotgun blasts give a single big kick
    int         damage_armor;       // damage absorbed by armor
    int         damage_blood;       // damage taken out of health
    int         damage_knockback;   // impact damage
    vec3_t      damage_from;        // origin for vector calculation
    qboolean    damage_fromWorld;   // if true, don't use the damage_from vector

    int         accurateCount;      // for "impressive" reward sound

    //
    int         lastkilled_client;  // last client that this client killed
    int         lasthurt_client;    // last client that damaged this client
    int         lasthurt_time;      // time this client was last hurt
    int         lasthurt_mod;       // type of damage the client did

    // timers
    int         respawnTime;        // can respawn when time > this, force after g_forcerespwan
    int         invulnerableTime;   // invulnerable time
    int         inactivityTime;     // kick players when time > this
    qboolean    inactivityWarning;  // qtrue if the five seoond warning has been given
    int         rewardTime;         // clear the EF_AWARD_IMPRESSIVE, etc when time > this

    int         airOutTime;

    int         lastKillTime;       // for multiple kill rewards

    qboolean    fireHeld;           // used for hook
    gentity_t   *hook;              // grapple hook if out

    int         switchTeamTime;     // time the player switched teams
    int         voteDelayTime;      // time when this client can vote again

    char        *areabits;

    int             voiceFloodTimer;        // Timer used to forgive voice chat flooding
    int             voiceFloodCount;        // Amount of voice chats that need to be forgivin
    int             voiceFloodPenalty;      // Time when a client can voice chat again

    // Anti-lag information
    gantilag_t      antilag[MAX_ANTILAG];
    gantilag_t      antilagUndo;
    int             antilagHead;

    qboolean        noOutfittingChange;

    animInfo_t      torso;
    animInfo_t      legs;
    vec3_t          ghoulLegsAngles;
    vec3_t          ghoulLowerTorsoAngles;
    vec3_t          ghoulUpperTorsoAngles;
    vec3_t          ghoulHeadAngles;

    gentity_t       *siameseTwin;
    gentity_t       *useEntity;

    vec3_t          maxSave;
    vec3_t          minSave;

    int             sunRespawnTimer;
};

//
// this structure is cleared as each map is entered
//
#define MAX_SPAWN_VARS          64
#define MAX_SPAWN_VARS_CHARS    4096

#define MAX_AUTOKICKLIST        32

typedef struct hnsBestPlayers_s {
    char    playerName[MAX_NETNAME];
    int     playerScore;
} hnsBestPlayers_t;

typedef enum hnsExtendedStats_s {
    HNSEXTSTAT_ROUNDS,
    HNSEXTSTAT_MM1HITS,
    HNSEXTSTAT_RPGBOOSTS,
    HNSEXTSTAT_RPGTAKEN,
    HNSEXTSTAT_M4TAKEN,
    HNSEXTSTAT_STUNS,
    HNSEXTSTAT_SEEKSCAGED,
    HNSEXTSTAT_WPNSTOLEN,
    HNSEXTSTAT_POINTS,
    HNSEXTSTAT_MM1TAKEN,
    HNSEXTSTAT_STUNNED,
    HNSEXTSTAT_TRAPPEDINCAGE,
    HNSEXTSTAT_MAX
} hnsExtendedStats_t;

/*
Moving all H&S level items into hnsLvl struct.
*/
typedef struct hnsLvl_s {
    int     previousRoundBestSeeker;
    int     previousRoundBestSeekerKills;
    vec3_t          hideseek_cage;
    int             hideseek_cageSize;
    qboolean        cageFightLoaded;
    qboolean        cageFightExtras;


    qboolean        MM1Given;
    int             MM1ent;
    qboolean        runMM1Flare;
    qboolean        runRPGFlare;
    qboolean        runM4Flare;
    int             M4ent;
    int             RPGent;
    qboolean        teleGunGiven;
    qboolean        taserGiven;
    qboolean        smokeactive;
    qboolean        cagefightdone;
    qboolean        startCage;
    int             cagefightTimer;


    char            RPGloc[MAX_QPATH];
    char            M4loc[MAX_QPATH];
    char            randomNadeLoc[MAX_QPATH];
    char            MM1loc[MAX_QPATH];


    int             MM1Flare;
    int             RPGFlare;
    int             M4Flare;
    int             lastAliveHiders[2];

    char            cagewinner[MAX_NETNAME];

    int             monkeySpawnCount;
    int             customETHiderAmount[MAX_CUSTOM_ET_AMOUNT];
    qboolean        cagefight;
    qboolean        secondBatchCustomWeaponsDistributed;
    qboolean        roundOver;

    hnsBestPlayers_t    bestHiders[3];
    hnsBestPlayers_t    bestSeekers[3];
    hnsBestPlayers_t    extendedStats[HNSEXTSTAT_MAX];
} hnsLvl_t;

typedef struct csinfLvl_s {
    int losingStreak;
    team_t losingStreakTeam;
} csinfLvl_t;

typedef struct
{
    struct gclient_s    *clients;       // [maxclients]

    struct gentity_s    *gentities;

    int             gentitySize;
    int             num_entities;       // current number, <= MAX_GENTITIES

    int             warmupTime;         // restart match at this time

    int             gametype;           // Current gametype index (gametypeData[gametype])
    gametypeData_t* gametypeData;       // Current gametype data for easier access

    vec3_t          worldMins;          // Absolute min coords of the world
    vec3_t          worldMaxs;          // Absolute max coords of the world

    fileHandle_t    logFile;

    // store latched cvars here that we want to get at often
    int         maxclients;

    int         framenum;
    int         time;                   // in msec
    int         previousTime;           // so movers can back up when blocked
    int         frameStartTime;

    int         startTime;              // level.time the map was started
    int         globalVoiceTime;        // last global voice

    int         teamScores[TEAM_NUM_TEAMS];
    int         teamAliveCount[TEAM_NUM_TEAMS];
    int         lastTeamLocationTime;       // last time of client team location update

    qboolean    newSession;             // don't use any old session data, because
                                        // we changed gametype

    qboolean    restarted;              // waiting for a map_restart to fire

    int         numConnectedClients;
    int         numNonSpectatorClients; // includes connecting clients
    int         numPlayingClients;      // connected, non-spectators
    int         sortedClients[MAX_CLIENTS];     // sorted by score
    int         follow1, follow2;       // clientNums for auto-follow spectators

    int         warmupModificationCount;    // for detecting if g_warmup is changed

    // voting state
    char        voteString[MAX_STRING_CHARS];
    char        voteDisplayString[MAX_STRING_CHARS];
    int         voteTime;               // level.time vote was called
    int         voteExecuteTime;        // time the vote is executed
    int         voteYes;
    int         voteNo;
    int         voteClient;             // client who initiated vote
    int         numVotingClients;       // set by CalculateRanks

    // spawn variables
    qboolean    spawning;               // the G_Spawn*() functions are valid
    int         numSpawnVars;
    char        *spawnVars[MAX_SPAWN_VARS][2];  // key / value pairs
    int         numSpawnVarChars;
    char        spawnVarChars[MAX_SPAWN_VARS_CHARS];

    // intermission state
    int         intermissionQueued;     // intermission was qualified, but
                                        // wait INTERMISSION_DELAY_TIME before
                                        // actually going there so the last
                                        // frag can be watched.  Disable future
                                        // kills during this delay
    int         intermissiontime;       // time the intermission was started
    char        *changemap;
    qboolean    readyToExit;            // at least one client wants to exit
    int         exitTime;
    vec3_t      intermission_origin;    // also used for spectator spawns
    vec3_t      intermission_angle;

    qboolean    locationLinked;         // target_locations get linked
    gentity_t   *locationHead;          // head of the location list

    // body queue
    int         bodyQueIndex;           // dead bodies
    int         bodyQueSize;            // how many dead bodies can there be
    int         bodySinkTime;
    gentity_t   *bodyQue[BODY_QUEUE_SIZE_MAX];

    int         portalSequence;

    int         mNumBSPInstances;
    int         mBSPInstanceDepth;
    vec3_t      mOriginAdjust;
    float       mRotationAdjust;
    char        *mTargetAdjust;
    qboolean    hasBspInstances;
    char        mFilter[MAX_QPATH];
    char        mTeamFilter[MAX_QPATH];

    int             gametypeStartTime;
    int             gametypeJoinTime;
    int             gametypeResetTime;
    int             gametypeRoundTime;
    int             gametypeRespawnTime[TEAM_NUM_TEAMS];
    int             gametypeDelayTime;
    const char*     gametypeTeam[TEAM_NUM_TEAMS];
    gtitem_t        gametypeItems[MAX_GAMETYPE_ITEMS];

    void*           serverGhoul2;
    animation_t     ghoulAnimations[MAX_ANIMATIONS];

    int             ladderCount;

    gspawn_t        spawns[MAX_SPAWNS];
    int             spawnCount;

    qboolean        pickupsDisabled;

    int             timeExtension;

    int             autokickedCount;
    int             autokickedHead;
    char            autokickedName[MAX_AUTOKICKLIST][MAX_NETNAME];
    char            autokickedIP[MAX_AUTOKICKLIST][20];

    qboolean        multiprotocol;
    clientMod_t     legacyMod;
    clientMod_t     goldMod;
    
    qboolean        paused;
    int             lastTMIUpdate;
    int             lastETIUpdate;

    levelState_t    levelState;
    int             actionSoundIndex;

    int             nextSQLBackupTime;

    qboolean        blueLocked;
    qboolean        redLocked;
    qboolean        specLocked;

    mute_t          mutedClients[MAX_CLIENTS];
    int             numMutedClients;

    // Boe!Man 11/21/13: Nolower, Noroof, Nomiddle and Nowhole combined into one system.
    qboolean        noSectionEntFound[TOTAL_SECTIONS];  // If the no* entity was found.
    qboolean        autoSectionActive[TOTAL_SECTIONS];  // True if the auto systems are active.

    vec3_t          noLR[2];            // Location for nolower/noroof.
    int             tempent;

    int             nextCmInfoDisplay;
    qboolean        proceedToNextCompState;
    int             nextCompState;
    int             autoSwapTime;
    int             awardTime;
    int             lastAwardSent;
    qboolean        timelimitHit;
    qboolean        timelimitMsg;
    mapAction_t     mapAction;
    int             runMapAction;
    char            mapActionNewGametype[12];
    char            mapActionNewMap[MAX_QPATH];
    int             unpauseNextNotification;

    qboolean        customGameStarted; // H&S true => seeks released, H&Z true => Shotguns distributed
    qboolean        customGameWeaponsDistributed;

    team_t          vipKilledInTeam;
    int             lowestScore;
    int             mapHighScore;
    qboolean        teamLastAliveSent[TEAM_NUM_TEAMS];
    qboolean        autoEvenTeamsDone;

    hnsLvl_t        hns;                // Level defines for Hide&Seek gametype.

} level_locals_t;

//
// g_spawn.c
//
qboolean    G_SpawnString( const char *key, const char *defaultString, char **out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean    G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean    G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean    G_SpawnVector( const char *key, const char *defaultString, float *out );
qboolean    G_ParseSpawnVars( qboolean inSubBSP );
int         G_SpawnGEntityFromSpawnVars( qboolean inSubBSP );
void        G_SpawnEntitiesFromString( qboolean inSubBSP );
char        *G_NewString( const char *string );
void        AddSpawnField(char *field, char *value);

//
// g_cmds.c
//
void        Cmd_Score_f         ( gentity_t *ent );
void        G_StopFollowing     ( gentity_t *ent );
void        G_StopGhosting      ( gentity_t* ent );
void        G_StartGhosting     ( gentity_t* ent );

void        BroadcastTeamChange( gclient_t *client, int oldTeam );
void        SetTeam( gentity_t *ent, char *s, const char* identity, qboolean forced );
void        Cmd_FollowCycle_f( gentity_t *ent, int dir );
qboolean    CheatsOk                ( gentity_t *ent );
void        G_SpawnDebugCylinder    ( vec3_t origin, float radius, gentity_t* clientent, float viewRadius, int colorIndex );
void        G_Voice                 ( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly );
void        G_VoiceGlobal           ( gentity_t* ent, const char* id, qboolean force );
qboolean    G_CanVoiceGlobal        ( void );

//
// g_items.c
//
void ItemUse_Shield(gentity_t *ent);
void ItemUse_Sentry(gentity_t *ent);
void ItemUse_Seeker(gentity_t *ent);
void ItemUse_MedPack(gentity_t *ent);

void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

void        PrecacheItem        ( gitem_t *it );
gentity_t*  G_DropItem          ( gentity_t *ent, gitem_t *item, float angle );
gentity_t*  LaunchItem          ( gitem_t *item, vec3_t origin, vec3_t velocity );
gentity_t*  G_DropWeapon        ( gentity_t* ent, weapon_t weapon, int pickupDelay );
gentity_t* G_DropItemAtLocation(vec3_t origin, vec3_t angles, gitem_t* item);

void SetRespawn (gentity_t *ent, float delay);
void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon (gentity_t *ent);
int ArmorIndex (gentity_t *ent);
void    Add_Ammo (gentity_t *ent, int weapon, int count);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);

void ClearRegisteredItems( void );
void RegisterItem( gitem_t *item );
void SaveRegisteredItems( void );

//
// g_utils.c
//
int     G_ModelIndex        ( char *name );
int     G_SoundIndex        ( char *name );
int     G_AmbientSoundSetIndex( char *name );
int     G_BSPIndex          ( char *name );
int     G_IconIndex         ( char *name );
int     G_EffectIndex       ( char *name );

void    G_TeamCommand( team_t team, char *cmd );
void    G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
int     G_RadiusList ( vec3_t origin, float radius, gentity_t *ignore, qboolean takeDamage, gentity_t *ent_list[MAX_GENTITIES]);
gentity_t *G_PickTarget (char *targetname);
void    G_UseTargets (gentity_t *ent, gentity_t *activator);
void    G_UseTargetsByName( const char* name, gentity_t *exclude, gentity_t *activator );
void    G_SetMovedir ( vec3_t angles, vec3_t movedir);
void    G_SetAngles( gentity_t *ent, vec3_t angles );

void    G_InitGentity( gentity_t *e );
gentity_t   *G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
void    G_PlayEffect(int fxID, vec3_t org, vec3_t ang);
void    G_SetHUDIcon ( int index, int icon );
void    G_Sound( gentity_t *ent, int channel, int soundIndex );
void    G_SoundAtLoc( vec3_t loc, int channel, int soundIndex );
void    G_EntitySound( gentity_t *ent, int channel, int soundIndex );
void    G_FreeEntity( gentity_t *e );
void    G_FreeEnitityChildren( gentity_t* ent );
qboolean    G_EntitiesFree( void );

void    G_TouchTriggers (gentity_t *ent);
void    G_TouchSolids (gentity_t *ent);

//
// g_object.c
//

extern void G_RunObject         ( gentity_t *ent );


float   *tv (float x, float y, float z);
char    *vtos( const vec3_t v );

float vectoyaw( const vec3_t vec );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig(void);

//=============== Ghoul II functionality ================
void        trap_G2API_ListModelBones       ( void *ghoul2 );
void        trap_G2API_ListModelSurfaces    ( void *ghoul2 );

int         trap_G2API_InitGhoul2Model      ( void **ghoul2, const char *fileName, qhandle_t customSkin, int lodBias );
qboolean    trap_G2API_RemoveGhoul2Model    ( void **ghoul2 );

qboolean    trap_G2API_SetBoneAngles        ( void *ghoul2, const char *boneName, const vec3_t angles, const int flags,
                                              const int up, const int right, const int forward );
qboolean    trap_G2API_SetBoneAnim          ( void *ghoul2, const char *boneName, const int startFrame, const int endFrame,
                                              const int flags, const float animSpeed, const float setFrame );

qboolean    trap_G2API_GetAnimFileName      ( void* ghoul2, char *dest, int destSize );

void        trap_G2API_CollisionDetect      ( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position,
                                              int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod );

qhandle_t   trap_G2API_RegisterSkin         ( const char *skinName, int numPairs, const char *skinPairs );
qboolean    trap_G2API_SetSkin              ( void *ghoul2, qhandle_t customSkin );

//
// g_combat.c
//
qboolean    CanDamage           ( gentity_t *targ, vec3_t origin);
int         G_GetHitLocation    ( gentity_t *target, vec3_t ppoint, vec3_t dir );
int         G_Damage            ( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod, int location );
qboolean    G_RadiusDamage      ( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int power, int dflags, int mod );
void        body_die            ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir );
void        TossClientItems     ( gentity_t *self );
void G_ApplyKnockback(gentity_t* targ, vec3_t newDir, float knockback);

// damage flags
#define DAMAGE_RADIUS               0x00000001  // damage was indirect
#define DAMAGE_NO_ARMOR             0x00000002  // armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK         0x00000004  // do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION        0x00000008  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION   0x00000010  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_DEATH_KNOCKBACK      0x00000080  // only does knockback on death of target
#define DAMAGE_AREA_DAMAGE          0x00000400  // spawn area damage
#define DAMAGE_NO_GORE              0x00000800  // dont spawn gore pieces with this damage
#define DAMAGE_FORCE_GORE           0x00001000  // force something to pop off
#define DAMAGE_NO_TEAMKILL          0x00002000  // does not produce teamkill damage

//
// g_missile.c
//
void        G_RunMissile        ( gentity_t *ent );
gentity_t*  G_CreateMissile     ( vec3_t org, vec3_t dir, float vel, int life, gentity_t *owner, attackType_t attack );
gentity_t*  G_CreateDamageArea  ( vec3_t origin, gentity_t* attacker, float damage, float radius, int duration, int mod );
void        G_BounceProjectile  ( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout );
void        G_ExplodeMissile    ( gentity_t *ent );
void        G_GrenadeThink      ( gentity_t* ent );

//
// g_mover.c
//
void        G_RunMover          ( gentity_t *ent );
void        Touch_DoorTrigger   ( gentity_t *ent, gentity_t *other, trace_t *trace );
void        G_ResetGlass        ( void );
void SP_func_door_rotating(gentity_t* ent);
void Touch_2WayRotDoorTrigger(gentity_t* ent, gentity_t* other, trace_t* trace);
void Think_Spawn2WayRotDoorTrigger(gentity_t* ent);


//
// g_trigger.c
//
void        trigger_teleporter_touch        ( gentity_t *self, gentity_t *other, trace_t *trace );
void        InitTrigger                     ( gentity_t *self );
void NV_blocked_trigger(gentity_t* ent);
void NV_blocked_Teleport(gentity_t* ent);
void SP_accelerator(gentity_t* ent);
void SP_accelerator_delay(gentity_t* self);
void SP_accelerator_touch(gentity_t* self, gentity_t* other, trace_t* trace);
void SP_sun(gentity_t* ent);
void SP_booster(gentity_t* ent);
void SP_teleporter(gentity_t* ent);
void hideseek_cage(gentity_t* ent);
void hideseek_cageextra(gentity_t* ent);
void trigger_NewTeleporter_touch(gentity_t* self, gentity_t* other, trace_t* trace);
void ReachableObject_events(gentity_t* self);
void trigger_ReachableObject_touch(gentity_t* self, gentity_t* other, trace_t* trace);
void trigger_booster_touch(gentity_t* self, gentity_t* other, trace_t* trace);
//
// g_misc.c
//
void        TeleportPlayer                  ( gentity_t *player, vec3_t origin, vec3_t angles, qboolean nojump );


//
// g_weapon.c
//
void        SnapVectorTowards               ( vec3_t v, vec3_t to );
void        G_InitHitModels                 ( void );

//
// g_client.c
//
qboolean    G_IsClientSpectating            ( gclient_t* client );
qboolean    G_IsClientDead                  ( gclient_t* client );
void        G_ClientCleanName               ( const char *in, char *out, int outSize, qboolean colors );
int         TeamCount                       ( int ignoreClientNum, team_t team, int* alive );
int         G_GhostCount                    ( team_t team );
team_t      PickTeam                        ( int ignoreClientNum );
void        SetClientViewAngle              ( gentity_t *ent, vec3_t angle );
void        CopyToBodyQue                   ( gentity_t *ent, int hitLocation, vec3_t hitDirection );
void        respawn                         ( gentity_t *ent);
void        BeginIntermission               ( void);
void        InitClientPersistant            ( gclient_t *client);
void        InitClientResp                  ( gclient_t *client);
void        ClientSpawn                     ( gentity_t *ent );
void        player_die                      ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );
void        G_InitBodyQueue                 ( void);
void        G_AddScore                      ( gentity_t *ent, int score );
void        CalculateRanks                  ( void );
qboolean    G_SpotWouldTelefrag             ( gspawn_t* spawn );
void        G_UpdateClientAnimations        ( gentity_t* ent );
void        G_SetRespawnTimer               ( gentity_t* ent );
gentity_t*  G_FindNearbyClient              ( vec3_t origin, team_t team, float radius, gentity_t* ignore );
void        G_AddClientSpawn                ( gentity_t* ent, team_t team );
qboolean    G_IsClientChatIgnored           ( int ignorer, int ingnoree );
void        G_IgnoreClientChat              ( int ignorer, int ignoree, qboolean ignore );
void        G_UpdateOutfitting              ( int clientNum );
gspawn_t* G_SelectRandomSpawnPoint(team_t team);

//
// g_svcmds.c
//
qboolean    ConsoleCommand                  ( void );

//
// g_weapon.c
//
gentity_t*  G_FireWeapon                    ( gentity_t *ent, attackType_t attack );
gentity_t*  G_FireProjectile                ( gentity_t *ent, weapon_t weapon, attackType_t attack, int projectileLifetime, int flags );

//
// p_hud.c
//
void        MoveClientToIntermission        ( gentity_t *client );
void        DeathmatchScoreboardMessage     ( gentity_t *client );

//
// g_cmds.c
//

//
// g_main.c
//
void        FindIntermissionPoint               ( void );
void        SetLeader                           ( int team, int client );
void        G_RunThink                          ( gentity_t *ent );
void QDECL  G_LogPrintf                         ( const char *fmt, ... );
void        SendScoreboardMessageToAllClients   ( void );
void        CheckGametype                       ( void );
qboolean G_IsGametypeAValidGametype(char* gametype);
void LogExit(const char* string);

//
// g_client.c
//
char*       ClientConnect                       ( int clientNum, qboolean firstTime, qboolean isBot );
void        ClientUserinfoChanged               ( int clientNum );
void        ClientDisconnect                    ( int clientNum );
void        ClientBegin                         ( int clientNum );
void        ClientCommand                       ( int clientNum );
gspawn_t*   G_SelectRandomSpawnPoint            ( team_t team );
intptr_t    G_GametypeCommand                   ( int command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4 );

//
// g_active.c
//
void G_CheckClientTimeouts  ( gentity_t *ent );
void G_CheckClientTeamkill  ( gentity_t *ent );
void ClientThink            ( int clientNum );
void ClientEndFrame         ( gentity_t *ent );
void G_RunClient            ( gentity_t *ent );

//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
void Team_CheckDroppedItem( gentity_t *dropped );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo, qboolean firstTime );

void G_InitWorldSession( void );
void G_WriteSessionData( void );
void writeMutesIntoSession(void);
void readMutesFromSession(void);

//
// g_bot.c
//
void G_InitBots( qboolean restart );
char *G_GetBotInfoByNumber( int num );
char *G_GetBotInfoByName( const char *name );
void G_CheckBotSpawn( void );
void G_RemoveQueuedBotBegin( int clientNum );
qboolean G_BotConnect( int clientNum, qboolean restart );
void Svcmd_AddBot_f( void );
void Svcmd_BotList_f( void );
void BotInterbreedEndMatch( void );
qboolean G_DoesMapSupportGametype   ( const char* gametype );
qboolean G_DoesMapExist             ( const char* mapname );
void G_LoadArenas ( void );

//
// g_gametype.c
//
gentity_t*  G_SelectGametypeSpawnPoint          ( team_t team, vec3_t origin, vec3_t angles );
gentity_t*  G_SpawnGametypeItem                 ( const char* pickup_name, qboolean dropped, vec3_t origin );
gentity_t*  G_SelectRandomGametypeSpawnPoint    ( team_t team );
qboolean    G_ParseGametypeFile                 ( void );
qboolean    G_ExecuteGametypeScript             ( gentity_t* activator, const char* name );
void        G_ResetGametype                     ( qboolean fullRestart, qboolean cagefight );
qboolean    G_CanGametypeTriggerBeUsed          ( gentity_t* self, gentity_t* activator );
void        G_ResetGametypeItem                 ( gitem_t* item );
void        gametype_item_use                   ( gentity_t* self, gentity_t* other );
void        G_DropGametypeItems                 ( gentity_t* self, int delayPickup );
void SP_monkey_player(gentity_t* ent);
void SP_hideseek_cageplayer(gentity_t* ent);
gentity_t* G_RealSpawnGametypeItem(gitem_t* item, vec3_t origin, vec3_t angles, qboolean dropped);

// ai_main.c
#define MAX_FILEPATH            144
int OrgVisible(vec3_t org1, vec3_t org2, int ignore);

//bot settings
typedef struct bot_settings_s
{
    char personalityfile[MAX_FILEPATH];
    float skill;
    char team[MAX_FILEPATH];
} bot_settings_t;

int BotAISetup( int restart );
int BotAIShutdown( int restart );
int BotAILoadMap( int restart );
int BotAISetupClient(int client, struct bot_settings_s *settings, qboolean restart);
int BotAIShutdownClient( int client, qboolean restart );
int BotAIStartFrame( int time );

#include "g_team.h" // teamplay specific stuff


extern  level_locals_t  level;
extern  gentity_t       g_entities[MAX_GENTITIES];

#define FOFS(x) ((intptr_t)&(((gentity_t *)0)->x))

extern  vmCvar_t    g_gametype;
extern  vmCvar_t    g_dedicated;
extern  vmCvar_t    g_cheats;
extern  vmCvar_t    g_maxclients;           // allow this many total, including spectators
extern  vmCvar_t    g_maxGameClients;       // allow this many active
extern  vmCvar_t    g_restarted;

extern  vmCvar_t    g_logHits;

extern  vmCvar_t    g_dmflags;
extern  vmCvar_t    g_scorelimit;
extern  vmCvar_t    g_timelimit;
extern  vmCvar_t    g_friendlyFire;
extern  vmCvar_t    g_password;
extern  vmCvar_t    g_needpass;
extern  vmCvar_t    g_gravity;
extern  vmCvar_t    g_speed;
extern  vmCvar_t    g_knockback;
extern  vmCvar_t    g_forcerespawn;
extern  vmCvar_t    g_inactivity;
extern  vmCvar_t    g_debugMove;
extern  vmCvar_t    g_debugAlloc;
extern  vmCvar_t    g_debugDamage;
extern  vmCvar_t    g_weaponRespawn;
extern  vmCvar_t    g_backpackRespawn;
extern  vmCvar_t    g_synchronousClients;
extern  vmCvar_t    g_motd;
extern  vmCvar_t    g_warmup;
extern  vmCvar_t    g_doWarmup;
extern  vmCvar_t    g_allowVote;
extern  vmCvar_t    g_voteDuration;
extern  vmCvar_t    g_voteKickBanTime;
extern  vmCvar_t    g_failedVoteDelay;
extern  vmCvar_t    g_teamAutoJoin;
extern  vmCvar_t    g_teamForceBalance;
extern  vmCvar_t    g_smoothClients;
extern  vmCvar_t    pmove_fixed;
extern  vmCvar_t    pmove_msec;
extern  vmCvar_t    g_rankings;
extern  vmCvar_t    g_respawnInterval;
extern  vmCvar_t    g_respawnInvulnerability;
extern  vmCvar_t    RMG;
extern  vmCvar_t    g_debugRMG;
extern  vmCvar_t    g_timeouttospec;
extern  vmCvar_t    g_roundtimelimit;
extern  vmCvar_t    g_roundjointime;
extern  vmCvar_t    g_timeextension;
extern  vmCvar_t    g_roundstartdelay;
extern  vmCvar_t    g_availableWeapons;
extern  vmCvar_t    g_forceFollow;
extern  vmCvar_t    g_followEnemy;
extern  vmCvar_t    g_mapcycle;
extern  vmCvar_t    g_pickupsDisabled;
extern  vmCvar_t    g_suicidePenalty;
extern  vmCvar_t    g_teamkillPenalty;
extern  vmCvar_t    g_teamkillDamageMax;
extern  vmCvar_t    g_teamkillDamageForgive;
extern  vmCvar_t    g_teamkillBanTime;
extern  vmCvar_t    g_teamkillNoExcuseTime;
extern  vmCvar_t    g_teamkillNoExcuseMultiplier;
extern  vmCvar_t    g_voiceFloodCount;
extern  vmCvar_t    g_voiceFloodPenalty;
extern  vmCvar_t    g_voiceTalkingGhosts;
extern  vmCvar_t    g_suddenDeath;

extern  vmCvar_t    ac_allowcross;
extern  vmCvar_t    ac_norecoil;
extern  vmCvar_t    rox_support;

extern  vmCvar_t    sv_legacyClientMod;
extern  vmCvar_t    sv_clientMod;

extern  vmCvar_t    g_badminPrefix;
extern  vmCvar_t    g_adminPrefix;
extern  vmCvar_t    g_sadminPrefix;
extern  vmCvar_t    g_hadminPrefix;
extern  vmCvar_t    g_rconPrefix;
extern  vmCvar_t    g_inviewFile;

extern  vmCvar_t    g_weaponFile;

extern vmCvar_t    a_adminlist;
extern vmCvar_t    a_badmin;
extern vmCvar_t    a_admin;
extern vmCvar_t    a_sadmin;
extern vmCvar_t    a_hadmin;
extern vmCvar_t    a_scorelimit;
extern vmCvar_t    a_timelimit;
extern vmCvar_t    a_swapteams;
extern vmCvar_t    a_compmode;
extern vmCvar_t    a_plant;
extern vmCvar_t    a_roundtimelimit;
extern vmCvar_t    a_runover;
extern vmCvar_t    a_rollercoaster;
extern vmCvar_t    a_respawn;
extern vmCvar_t    a_mapswitch;
extern vmCvar_t    a_strip;
extern vmCvar_t    a_forceteam;
extern vmCvar_t    a_blockseek;
extern vmCvar_t    a_nosection;
extern vmCvar_t    a_shuffleteams;
extern vmCvar_t    a_nades;
extern vmCvar_t    a_respawninterval;
extern vmCvar_t    a_damage;
extern vmCvar_t    a_gtrestart;
extern vmCvar_t    a_clan;
extern vmCvar_t    a_ban;
extern vmCvar_t    a_broadcast;
extern vmCvar_t    a_subnetban;
extern vmCvar_t    a_eventeams;
extern vmCvar_t    a_clanvsall;
extern vmCvar_t    a_lock;
extern vmCvar_t    a_flash;
extern vmCvar_t    a_forcevote;
extern vmCvar_t    a_pause;
extern vmCvar_t    a_burn;
extern vmCvar_t    a_kick;
extern vmCvar_t    a_mute;
extern vmCvar_t    a_friendlyFire;
extern vmCvar_t    a_rename;
extern vmCvar_t    a_3rd;
extern vmCvar_t    a_toggleweapon;
extern vmCvar_t    a_anticamp;
extern vmCvar_t    a_pop;
extern vmCvar_t    a_uppercut;
extern  vmCvar_t    a_followEnemy;

extern  vmCvar_t    g_serverColors;
extern  vmCvar_t    g_maxAliases;
extern  vmCvar_t    g_logToFile;
extern  vmCvar_t    g_logToDatabase;
extern  vmCvar_t    g_dbLogRetention;

extern  vmCvar_t    g_iphubAPIKey;
extern  vmCvar_t    g_useCountryAPI;
extern  vmCvar_t    g_useCountryDb;
extern  vmCvar_t    g_countryAging;
extern  vmCvar_t    g_vpnAutoKick;
extern  vmCvar_t    g_subnetOctets;

extern  vmCvar_t    g_allowThirdPerson;
extern  vmCvar_t    g_enforce1fxAdditions;
extern  vmCvar_t    g_recoilRatio;
extern  vmCvar_t    g_inaccuracyRatio;

extern  vmCvar_t    g_allowCustomTeams; 
extern  vmCvar_t    g_customBlueName; // Will be given to us from the gametype module.
extern  vmCvar_t    g_customRedName; // Will be given to us from the gametype module.

extern  vmCvar_t    g_useNoLower;
extern  vmCvar_t    g_useNoRoof;
extern  vmCvar_t    g_useNoMiddle;
extern  vmCvar_t    g_useNoWhole;

extern  vmCvar_t    g_useAutoSections;

extern  vmCvar_t    match_bestOf; // ensures early exit from winning state if one team's score is too far ahead to reach.
extern  vmCvar_t    match_scorelimit;
extern  vmCvar_t    match_timelimit;
extern  vmCvar_t    match_lockspecs;
extern  vmCvar_t    match_doublerounds;

extern  vmCvar_t    cm_prevRedTeamScore;
extern  vmCvar_t    cm_prevBlueTeamScore;
extern  vmCvar_t    cm_state; // init, first round, second round.

extern  vmCvar_t    cm_bestOf;
extern  vmCvar_t    cm_scorelimit;
extern  vmCvar_t    cm_timelimit;
extern  vmCvar_t    cm_lockspecs;
extern  vmCvar_t    cm_doublerounds;

extern  vmCvar_t    cm_originalsl;
extern  vmCvar_t    cm_originaltl;
extern  vmCvar_t    g_mvchatCheckSoundFiles;
extern  vmCvar_t    currentGametype;

extern  vmCvar_t    g_motd1;
extern  vmCvar_t    g_motd2;
extern  vmCvar_t    g_motd3;
extern  vmCvar_t    g_motd4;
extern  vmCvar_t    g_motd5;
extern  vmCvar_t    g_autoEvenTeams;
extern  vmCvar_t    match_followEnemy;
extern  vmCvar_t    g_useSecureRoxVerification;

extern	vmCvar_t    hideSeek_roundstartdelay;
extern	vmCvar_t    hideSeek_availableWeapons;
extern	vmCvar_t    hideSeek_Extra;
extern	vmCvar_t    hideSeek_Nades;
extern	vmCvar_t    hideSeek_randomFireNade;
extern	vmCvar_t    hideSeek_Weapons;
extern	vmCvar_t    g_rpgBoost;
extern	vmCvar_t    g_mm1Style;
extern	vmCvar_t	g_rpgRemove;
extern	vmCvar_t	g_CnRsmokenade;
extern	vmCvar_t	g_smokealert;
extern	vmCvar_t	g_CnRsmokeTime;
extern	vmCvar_t	g_hsgiveknife;
extern	vmCvar_t    g_rpgSpeedDrain;
extern	vmCvar_t    g_rpgSpeedDrainSec;
extern	vmCvar_t    g_waterSpeedDecrement;
extern	vmCvar_t    g_stunSpeedDecrement;
extern	vmCvar_t    g_stunSpeedIncrement;
extern	vmCvar_t    g_fireSpeedDecrement;
extern	vmCvar_t    g_rpgSpeedIncrement;
extern	vmCvar_t    g_waterSpeedTime;
extern	vmCvar_t    g_stunSpeedTime;
extern	vmCvar_t    g_fireSpeedTime;
extern	vmCvar_t    g_hnsWeaponsMinPlayers;
extern	vmCvar_t    g_boxAttempts;
extern	vmCvar_t    g_cageAttempts;
extern	vmCvar_t    g_noHighFps;
extern  vmCvar_t    sv_useLegacyNades;
extern  vmCvar_t    g_hnsStatAging;

extern  vmCvar_t    csinf_suicidePenalty;
extern  vmCvar_t    csinf_maxCash;
extern  vmCvar_t    csinf_minCash;
extern  vmCvar_t    csinf_friendlyKill;

extern  vmCvar_t    csinf_killBonus;
extern  vmCvar_t    csinf_startingCash;

//extern vmCvar_t     g_leanType;

void    trap_Print( const char *text );
void    trap_Error( const char *text ) __attribute__((noreturn));
int     trap_Milliseconds( void );
int     trap_Argc( void );
void    trap_Argv( int n, char *buffer, int bufferLength );
void    trap_Args( char *buffer, int bufferLength );
int     trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void    trap_FS_Read( void *buffer, int len, fileHandle_t f );
void    trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void    trap_FS_FCloseFile( fileHandle_t f );
int     trap_FS_GetFileList ( const char *path, const char *extension, char *listbuf, int bufsize );
void    trap_SendConsoleCommand( int exec_when, const char *text );
void    trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags, float MinValue, float MaxValue );
void    trap_Cvar_Update( vmCvar_t *cvar );
void    trap_Cvar_Set( const char *var_name, const char *value );
int     trap_Cvar_VariableIntegerValue( const char *var_name );
float   trap_Cvar_VariableValue( const char *var_name );
void    trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void    trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void    trap_RMG_Init(int terrainID);
void    trap_DropClient( int clientNum, const char *reason );
void    trap_SendServerCommand( int clientNum, const char *text );
void    trap_SetConfigstring( int num, const char *string );
void    trap_GetConfigstring( int num, char *buffer, int bufferSize );
void    trap_GetUserinfo( int num, char *buffer, int bufferSize );
void    trap_SetUserinfo( int num, const char *buffer );
void    trap_GetServerinfo( char *buffer, int bufferSize );
void    trap_SetBrushModel( gentity_t *ent, const char *name );
void    trap_SetActiveSubBSP(int index);
void    trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int     trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void    trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void    trap_LinkEntity( gentity_t *ent );
void    trap_UnlinkEntity( gentity_t *ent );
int     trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int     trap_BotAllocateClient( void );
void    trap_BotFreeClient( int clientNum );
void    trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean    trap_GetEntityToken( char *buffer, int bufferSize, qboolean inSubBSP );
void        trap_GetWorldBounds ( vec3_t mins, vec3_t maxs );

void    *trap_Mem_Init(int size);

void    *trap_BotGetMemoryGame(int size);
void    trap_BotFreeMemoryGame(void *ptr);

int     trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points);
void    trap_DebugPolygonDelete(int id);

int     trap_BotLibSetup( void );
int     trap_BotLibShutdown( void );
int     trap_BotLibVarSet(char *var_name, char *value);
int     trap_BotLibVarGet(char *var_name, char *value, int size);
int     trap_BotLibDefine(char *string);
int     trap_BotLibStartFrame(float time);
int     trap_BotLibLoadMap(const char *mapname);
int     trap_BotLibUpdateEntity(int ent, void /* struct bot_updateentity_s */ *bue);
int     trap_BotLibTest(int parm0, char *parm1, vec3_t parm2, vec3_t parm3);

int     trap_BotGetSnapshotEntity( int clientNum, int sequence );
int     trap_BotGetServerCommand(int clientNum, char *message, int size);
void    trap_BotUserCommand(int client, usercmd_t *ucmd);

int     trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas);
int     trap_AAS_AreaInfo( int areanum, void /* struct aas_areainfo_s */ *info );
void    trap_AAS_EntityInfo(int entnum, void /* struct aas_entityinfo_s */ *info);

int     trap_AAS_Initialized(void);
void    trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float   trap_AAS_Time(void);

int     trap_AAS_PointAreaNum(vec3_t point);
int     trap_AAS_PointReachabilityAreaIndex(vec3_t point);
int     trap_AAS_TraceAreas(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas);

int     trap_AAS_PointContents(vec3_t point);
int     trap_AAS_NextBSPEntity(int ent);
int     trap_AAS_ValueForBSPEpairKey(int ent, char *key, char *value, int size);
int     trap_AAS_VectorForBSPEpairKey(int ent, char *key, vec3_t v);
int     trap_AAS_FloatForBSPEpairKey(int ent, char *key, float *value);
int     trap_AAS_IntForBSPEpairKey(int ent, char *key, int *value);

int     trap_AAS_AreaReachability(int areanum);

int     trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);
int     trap_AAS_EnableRoutingArea( int areanum, int enable );
int     trap_AAS_PredictRoute(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin,
                            int goalareanum, int travelflags, int maxareas, int maxtime,
                            int stopevent, int stopcontents, int stoptfl, int stopareanum);

int     trap_AAS_AlternativeRouteGoals(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags,
                                        void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals,
                                        int type);
int     trap_AAS_Swimming(vec3_t origin);
int     trap_AAS_PredictClientMovement(void /* aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);


void    trap_EA_Say(int client, char *str);
void    trap_EA_SayTeam(int client, char *str);
void    trap_EA_Command(int client, char *command);

void    trap_EA_Action(int client, int action);
void    trap_EA_Gesture(int client);
void    trap_EA_Talk(int client);
void    trap_EA_Attack(int client);
void    trap_EA_Use(int client);
void    trap_EA_Respawn(int client);
void    trap_EA_Crouch(int client);
void    trap_EA_MoveUp(int client);
void    trap_EA_MoveDown(int client);
void    trap_EA_MoveForward(int client);
void    trap_EA_MoveBack(int client);
void    trap_EA_MoveLeft(int client);
void    trap_EA_MoveRight(int client);
void    trap_EA_SelectWeapon(int client, int weapon);
void    trap_EA_Jump(int client);
void    trap_EA_DelayedJump(int client);
void    trap_EA_Move(int client, vec3_t dir, float speed);
void    trap_EA_View(int client, vec3_t viewangles);
void    trap_EA_Alt_Attack(int client);
void    trap_EA_ForcePower(int client);

void    trap_EA_EndRegular(int client, float thinktime);
void    trap_EA_GetInput(int client, float thinktime, void /* struct bot_input_s */ *input);
void    trap_EA_ResetInput(int client);


int     trap_BotLoadCharacter(char *charfile, float skill);
void    trap_BotFreeCharacter(int character);
float   trap_Characteristic_Float(int character, int index);
float   trap_Characteristic_BFloat(int character, int index, float min, float max);
int     trap_Characteristic_Integer(int character, int index);
int     trap_Characteristic_BInteger(int character, int index, int min, int max);
void    trap_Characteristic_String(int character, int index, char *buf, int size);

int     trap_BotAllocChatState(void);
void    trap_BotFreeChatState(int handle);
void    trap_BotQueueConsoleMessage(int chatstate, int type, char *message);
void    trap_BotRemoveConsoleMessage(int chatstate, int handle);
int     trap_BotNextConsoleMessage(int chatstate, void /* struct bot_consolemessage_s */ *cm);
int     trap_BotNumConsoleMessages(int chatstate);
void    trap_BotInitialChat(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int     trap_BotNumInitialChats(int chatstate, char *type);
int     trap_BotReplyChat(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int     trap_BotChatLength(int chatstate);
void    trap_BotEnterChat(int chatstate, int client, int sendto);
void    trap_BotGetChatMessage(int chatstate, char *buf, int size);
int     trap_StringContains(char *str1, char *str2, int casesensitive);
int     trap_BotFindMatch(char *str, void /* struct bot_match_s */ *match, unsigned long int context);
void    trap_BotMatchVariable(void /* struct bot_match_s */ *match, int variable, char *buf, int size);
void    trap_UnifyWhiteSpaces(char *string);
void    trap_BotReplaceSynonyms(char *string, unsigned long int context);
int     trap_BotLoadChatFile(int chatstate, char *chatfile, char *chatname);
void    trap_BotSetChatGender(int chatstate, int gender);
void    trap_BotSetChatName(int chatstate, char *name, int client);
void    trap_BotResetGoalState(int goalstate);
void    trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void    trap_BotResetAvoidGoals(int goalstate);
void    trap_BotPushGoal(int goalstate, void /* struct bot_goal_s */ *goal);
void    trap_BotPopGoal(int goalstate);
void    trap_BotEmptyGoalStack(int goalstate);
void    trap_BotDumpAvoidGoals(int goalstate);
void    trap_BotDumpGoalStack(int goalstate);
void    trap_BotGoalName(int number, char *name, int size);
int     trap_BotGetTopGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int     trap_BotGetSecondGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int     trap_BotChooseLTGItem(int goalstate, vec3_t origin, int *inventory, int travelflags);
int     trap_BotChooseNBGItem(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime);
int     trap_BotTouchingGoal(vec3_t origin, void /* struct bot_goal_s */ *goal);
int     trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal);
int     trap_BotGetNextCampSpotGoal(int num, void /* struct bot_goal_s */ *goal);
int     trap_BotGetMapLocationGoal(char *name, void /* struct bot_goal_s */ *goal);
int     trap_BotGetLevelItemGoal(int index, char *classname, void /* struct bot_goal_s */ *goal);
float   trap_BotAvoidGoalTime(int goalstate, int number);
void    trap_BotSetAvoidGoalTime(int goalstate, int number, float avoidtime);
void    trap_BotInitLevelItems(void);
void    trap_BotUpdateEntityItems(void);
int     trap_BotLoadItemWeights(int goalstate, char *filename);
void    trap_BotFreeItemWeights(int goalstate);
void    trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void    trap_BotSaveGoalFuzzyLogic(int goalstate, char *filename);
void    trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int     trap_BotAllocGoalState(int state);
void    trap_BotFreeGoalState(int handle);

void    trap_BotResetMoveState(int movestate);
void    trap_BotMoveToGoal(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags);
int     trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void    trap_BotResetAvoidReach(int movestate);
void    trap_BotResetLastAvoidReach(int movestate);
int     trap_BotReachabilityArea(vec3_t origin, int testground);
int     trap_BotMovementViewTarget(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target);
int     trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target);
int     trap_BotAllocMoveState(void);
void    trap_BotFreeMoveState(int handle);
void    trap_BotInitMoveState(int handle, void /* struct bot_initmove_s */ *initmove);
void    trap_BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type);

int     trap_BotChooseBestFightWeapon(int weaponstate, int *inventory);
void    trap_BotGetWeaponInfo(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo);
int     trap_BotLoadWeaponWeights(int weaponstate, char *filename);
int     trap_BotAllocWeaponState(void);
void    trap_BotFreeWeaponState(int weaponstate);
void    trap_BotResetWeaponState(int weaponstate);

int     trap_GeneticParentsAndChildSelection(int numranks, float *ranks, int *parent1, int *parent2, int *child);

void    trap_SnapVector( float *v );

int         trap_CM_RegisterTerrain(const char *config);

// Gametype traps
void        trap_GT_Init        ( const char* gametype, qboolean restart );
void        trap_GT_RunFrame    ( int time );
void        trap_GT_Start       ( int time );
int         trap_GT_SendEvent   ( int event, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
void        trap_GT_Shutdown    ( void );

// custom traps.
qboolean trap_IsClientLegacy(int clientNum);
int trap_TranslateSilverWeaponToGoldWeapon(int weapon);
int trap_TranslateGoldWeaponToSilverWeapon(int weapon);
int trap_ValidateMapName(const char* mapName, char* output, int outputSize);

void G_UpdateClientAntiLag  ( gentity_t* ent );
void G_UndoAntiLag          ( void );
void G_ApplyAntiLag         ( gentity_t* ref, qboolean enlargeHitBox );

// MVChats inclusion.
typedef struct {
    qboolean    shouldSoundPlay;            // If the sound is able to/should play.
    qboolean    displayNoText;              // qtrue if the sound text should not be displayed in chat.

    const char* text;                      // The text belonging to the sound.
    int         soundIndex;                 // The sound index to the actual sound.
    int         stripChars;                 // How many characters should be stripped from the chat text.
    qboolean    isCustomSound;
} mvchat_ChatParse_t;


void        mvchat_parseFiles(void);
void        mvchat_chatDetermineSound(mvchat_ChatParse_t* chatParse, char* chatText, TIdentity* identity);
int         mvchat_chatGetNextSound(TIdentity* identity);
void        mvchat_listSounds(gentity_t* ent, int soundPage);
void        mvchat_printHelp(gentity_t* ent);
void mvchat_findSounds(gentity_t* ent);


#define SQL_GAME_MIGRATION_LEVEL 5
#define SQL_LOG_MIGRATION_LEVEL 1
#define SQL_COUNTRY_MIGRATION_LEVEL 1
#define MAX_SQL_TEMP_NAME 16
#define MAX_PACKET_BUF 900

typedef enum {
    LOGLEVEL_TEXT,
    LOGLEVEL_INFO,
    LOGLEVEL_WARN,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL,
    LOGLEVEL_FATAL_DB // this is used if we have a fatal DB-related error (most likely not usable DB), which means that if db logging is turned on, game will not try to log it into the DB because, well..., the DB part failed...
} loggingLevel_t;

char* getNameOrArg(gentity_t* ent, char* arg, qboolean cleanName);
char* getIpOrArg(gentity_t* ent, char* arg);
admLevel_t getAdminLevel(gentity_t* ent);
char* getAdminName(gentity_t* ent);
admType_t getAdminType(gentity_t* ent);
void loadDatabases(void);
void backupInMemoryDatabases(void);
void unloadInMemoryDatabases(void);

int dbGetAdminLevel(admType_t adminType, gentity_t* ent, char* passguid);
void dbAddAdmin(admType_t adminType, admLevel_t adminLevel, gentity_t* ent, gentity_t* adm, char* password);
qboolean dbGetAdminDataByRowId(admType_t adminType, int rowId, int* adminLevel, char* adminName, int adminNameLength);
int dbRemoveAdminByRowId(admType_t adminType, int rowId);
int dbUpdateAdminPass(char* adminName, char* password);
void dbPrintAdminlist(gentity_t* ent, admType_t adminType, int page);
void dbRunTruncate(char* table);
void dbClearOldAliases(gentity_t* ent);
void dbAddAlias(gentity_t* ent);
void dbGetAliases(gentity_t* ent, char* output, int outputSize, char* separator);
void dbAddBan(gentity_t* ent, gentity_t* adm, char* reason, qboolean subnet, qboolean endofmap, int days, int hours, int minutes);
int dbRemoveBan(qboolean subnet, int rowId);
void dbLogAdmin(char* byIp, char* byName, char* toIp, char* toName, char* action, char* reason, admLevel_t adminLevel, char* adminName, admType_t adminType);
void dbLogGame(char* byIp, char* byName, char* toIp, char* toName, char* action);
void dbLogLogin(char* byIp, char* byName, admLevel_t adminLevel, admType_t adminType);
void dbLogRcon(char* ip, char* action);
qboolean dbCheckBan(char* ip, char* reason, int reasonSize, int* endOfMap, int* banEnd);
void dbLogRetention(void);
qboolean dbGetCountry(char* ip, char* countryCode, int countryCodeSize, char* country, int countrySize, int* blocklevel);
void dbAddCountry(char* ip, char* countryCode, char* country, int blocklevel);
void dbLogSystem(loggingLevel_t logLevel, char* msg);
int dbRemoveAdminByGentity(gentity_t* ent);
void dbAddClan(clanType_t clanType, gentity_t* ent, gentity_t* adm, char* password);
int dbRemoveClanByGentity(gentity_t* ent);
int dbRemoveClanByRowId(int rowId);
int dbUpdateClanPass(char* memberName, char* password);
qboolean dbGetClan(clanType_t clanType, gentity_t* ent, char* password);
qboolean dbGetClanDataByRowId(int rowId, char* memberName, int memberNameSize, int* memberType);
void dbPrintClanlist(gentity_t* ent, clanType_t clanType, int page);
qboolean dbGetBanDetailsByRowID(qboolean subnet, int rowId, char* outputPlayer, int outputPlayerSize, char* outputIp, int outputIpSize);
void dbPrintBanlist(gentity_t* ent, qboolean subnet, int page);
void dbReadSessionDataForClient(gclient_t* client, qboolean gametypeChanged);
void dbWriteSessionDataForClient(gclient_t* client);
void dbRemoveSessionDataById(int clientNum);
void dbReadSessionMutesBackIntoMuteInfo(void);
void dbWriteMuteIntoSession(mute_t* muteInfo);
void dbClearSessionMutes(void);

void logSystem(loggingLevel_t logLevel, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void logRcon(char* ip, char* action);
void logAdmin(gentity_t* by, gentity_t* to, char* action, char* reason);
void logLogin(gentity_t* ent);

const char* getAdminNameByAdminLevel(admLevel_t adminLevel);
void getCleanAdminNameByAdminLevel(admLevel_t adminLevel, char* output, int sizeOfOutput);
void G_SetDisabledWeapons(void);

// struct from 1fxmod
typedef struct
{
    char* shortCmd; // Short admin command, example: !uc, !p(with space)
    char* adminCmd; // Full Admin command for /adm and rcon, and with !.
    int* adminLevel; // The level that the Admin needs to be in order to execute this command.
    int     (*Function)(int argNum, gentity_t* adm, qboolean shortCmd); // Store pointer to the given function so we can call it later.
    char* desc; // Description of the command in /adm.
    char* params; // Description of the command in /adm.
    char* suffix; // Suffix for post processing broadcast, or NULL when function doesn't use it/has no suffix.
} admCmd_t;


// admin.c
extern int adminCommandsSize;
extern admCmd_t adminCommands[];

int adm_adminRemove(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_adminList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_addHadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_addSadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_addAdmin(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_addBadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_scoreLimit(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_timeLimit(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_swapTeams(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Rounds(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Plant(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_roundTimeLimit(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Runover(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Rollercoaster(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Respawn(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_mapRestart(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Strip(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_removeAdmin(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_forceTeam(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_blockSeek(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_blockSeekList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_noLower(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_noRoof(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_noMiddle(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_noWhole(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_shuffleTeams(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_noNades(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_respawnInterval(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_realDamage(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_normalDamage(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_customDamage(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_gametypeRestart(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_addClanMember(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_removeClanMember(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_removeClanMemberFromList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_clanList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_compMode(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_banList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Ban(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Unban(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Broadcast(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_subnetbanList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_subnetBan(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_subnetUnban(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_evenTeams(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_clanVsAll(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_lockTeam(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Flash(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Gametype(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Map(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_mapCycle(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_passVote(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_cancelVote(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Pause(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Burn(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Kick(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Mute(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_friendlyFire(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Rename(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Switch(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Third(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_toggleWeapon(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Anticamp(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_endMap(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_mapList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_matchIsBestOf(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_profanityList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Pop(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Uppercut(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_Punish(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_punishList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_mapCycleList(int argNum, gentity_t* adm, qboolean shortCmd);
int adm_skipToMap(int argNum, gentity_t* adm, qboolean shortCmd);
qboolean canClientRunAdminCommand(gentity_t* adm, int adminCommandId);
int cmdIsAdminCmd(char* cmd, qboolean shortCmd);
void runAdminCommand(int adminCommandId, int argNum, gentity_t* adm, qboolean shortCmd);
void postExecuteAdminCommand(int funcNum, int idNum, gentity_t* adm);
void adm_setPassword(gentity_t* ent, char* password);
void adm_Login(gentity_t* ent, char* password);
void adm_printAdminCommands(gentity_t* adm);
int adm_followEnemy(int argNum, gentity_t* adm, qboolean shortCmd);

// RPM.c
void RPM_UpdateTMI(void);
void RPM_Awards(void);

// ROCmod.c
void ROCmod_verifyClient(gentity_t* ent, int clientNum);
void ROCmod_clientUpdate(gentity_t* ent, int clientNum);
void ROCmod_sendExtraTeamInfo(gentity_t* ent);
void ROCmod_sendBestPlayerStats(void);


void G_Broadcast(int broadcastLevel, gentity_t* to, qboolean playSound, char* broadcast, ...) __attribute__((format(printf, 4, 5)));
char* G_ColorizeMessage(char* broadcast);
int G_ClientNumFromArg(gentity_t* ent, int argNum, const char* action, qboolean aliveOnly, qboolean otherAdmins, qboolean higherLvlAdmins, qboolean shortCmd);

qboolean checkAdminPassword(char* input);
void G_CloseSound(vec3_t origin, int soundIndex);
void G_GlobalSound(int soundIndex);
void G_ClientSound(gentity_t* ent, int soundIndex);
void G_RemoveAdditionalCarets(char* text);
void G_RemoveColorEscapeSequences(char* text);
char* concatArgs(int fromArgNum, qboolean shortCmd, qboolean retainColors);
char* G_GetArg(int argNum, qboolean shortCmd, qboolean retainColors);
char* G_GetChatArgument(int argNum, qboolean retainColors);
int G_GetChatArgumentCount(void);

void QDECL G_printMessage(qboolean isChat, qboolean toAll, gentity_t* ent, char* prefix, const char* msg, va_list argptr);
void QDECL G_printInfoMessage(gentity_t* ent, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void QDECL G_printChatInfoMessage(gentity_t* ent, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void QDECL G_printInfoMessageToAll(const char* msg, ...) __attribute__((format(printf, 1, 2)));
void QDECL G_printChatInfoMessageToAll(const char* msg, ...) __attribute__((format(printf, 1, 2)));
void QDECL G_printGametypeMessage(gentity_t* ent, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void QDECL G_printGametypeMessageToAll(const char* msg, ...) __attribute__((format(printf, 1, 2)));

void QDECL G_printCustomMessageToAll(const char* prefix, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void QDECL G_printCustomMessage(gentity_t* ent, const char* prefix, const char* msg, ...) __attribute__((format(printf, 3, 4)));
void QDECL G_printCustomChatMessage(gentity_t* ent, const char* prefix, const char* msg, ...) __attribute__((format(printf, 3, 4)));
void QDECL G_printCustomChatMessageToAll(const char* prefix, const char* msg, ...) __attribute__((format(printf, 2, 3)));
void getSubnet(char* ip, char* output, int outputSize);
int swapTeams(qboolean autoSwap);
int evenTeams(qboolean autoEven);
int shuffleTeams(qboolean autoShuffle);
gentity_t* getLastConnectedClient(qboolean respectGametypeItems);
gentity_t* getLastConnectedClientInTeam(int team, qboolean respectGametypeItems);
void stripEveryone(qboolean handsUp);
void stripTeam(int team, qboolean handsUp);
void stripClient(gentity_t* recipient, qboolean handsUp);
void spinView(gentity_t* recipient);
void uppercutPlayer(gentity_t* recipient, int ucLevel);
void runoverPlayer(gentity_t* recipient);
void kickPlayer(gentity_t* to, gentity_t* by, char* action, char* reason);
char* getClanTypeAsText(clanType_t clanType);
void clan_setPassword(gentity_t* ent, char* password);
void clan_Login(gentity_t* ent, char* password);
gentity_t* NV_projectile(gentity_t* ent, vec3_t start, vec3_t dir, int weapon, int damage);
void popPlayer(gentity_t* ent, popAction_t popAction);
void muteClient(gentity_t* ent, int duration);
void unmuteClient(gentity_t* ent);
void checkMutes(void);
void reapplyMuteAfterConnect(gentity_t* ent);
qboolean isClientMuted(gentity_t* ent, qboolean printMsg);
void blockSection(gentity_t* ent, mapSection_t section);
void checkSectionState(void);
void writeGametypeTeamNames(const char* redTeam, const char* blueTeam);
void realSectionAutoCheck(gentity_t* ent, qboolean override);
void sectionAutoCheck(gentity_t* ent);
void sectionAddOrDelInstances(gentity_t* ent, qboolean add);
void checkEnts(gentity_t* ent);
void checkRoof(gentity_t* ent);
void resetCompetitionModeVariables(void);
int QDECL SortAlpha(const void* a, const void* b);
void parseChatTokens(gentity_t* ent, chatMode_t chatMode, const char* input, char* output, int sizeOfOutput);
void showHnsScores(void);
qboolean isCurrentGametypeInList(gameTypes_t* gametypes);
qboolean isCurrentGametype(gameTypes_t gametype);
void sendClientmodAwards(void);
void notifyPlayersOfTeamScores(void);
void showMotd(gentity_t* ent);
qboolean weaponMod(weaponMod_t weaponMod, char* wpnModName);
void printMapActionDenialReason(gentity_t* adm);
int getChatModeFromCommand(gentity_t* ent, const char* cmd, chatMode_t mode, int adminCommand);
char* getChatAdminPrefixByMode(gentity_t* ent, chatMode_t mode, char* output, int sizeofOutput);
qboolean shouldChatModeBeep(chatMode_t mode);
void printStatsInfo(gentity_t* ent);
void printPlayersInfo(gentity_t* ent);
int altAttack(int weapon);
int normalAttackMod(int mod);
void parseACCheckGuidMessage(gentity_t* ent);
void giveWeaponToClient(gentity_t* ent, weapon_t wpn, qboolean autoswitch);
void giveWeaponWithCustomAmmoToClient(gentity_t* ent, weapon_t wpn, qboolean autoswitch, int normAmmo, int normClip, int altAmmo, int altClip);
void removeWeaponFromClient(gentity_t* ent, weapon_t wpn, qboolean drop, weapon_t switchTo);
void removeAllWeaponsFromClient(gentity_t* ent);
char* chooseTeam(void);
gentity_t* findLastEnteredPlayer(int highTeam, qboolean scoresAllowed);
int spawnBspModel(const char* bspModel, vec3_t* origin);
void initBspModelSpawns(void);
void transformPlayerBack(gentity_t* self, gentity_t* other, trace_t* trace);
void addSpeedAlteration(gentity_t* ent, qboolean isDecrement, speedAlterationReason_t speedAlterationReason);
void stealWeaponWithAmmo(gentity_t* from, gentity_t* to, weapon_t wpn);
void TeleportPlayerToPlayer(gentity_t* player, gentity_t* toPlayer, qboolean killbox, qboolean nojump);
void spawnCage(vec3_t org, qboolean autoremove, qboolean big);
void resetCages(void);
void initCageFight(void);
void TeleportPlayerNoKillbox(gentity_t* player, vec3_t origin, vec3_t angles, qboolean nojump);
void G_MissileImpact(gentity_t* ent, trace_t* trace);
void runF1Teleport(gentity_t* ent, vec3_t origin);
void spawnBox(vec3_t org);
qboolean isWeaponFullyOutOfAmmo(gentity_t* ent, weapon_t wpn);
int getWeaponAmmoIdx(weapon_t wpn, qboolean alt);
int getWeaponClip(gentity_t* ent, weapon_t wpn, qboolean alt);
int getWeaponAmmo(gentity_t* ent, weapon_t wpn, qboolean alt);
void hnsRunFrame(void);
int spawnEffect(vec3_t origin, char* effect);
void shuffleIntArray(int* input, int sizeOfInput);
void getCurrentGametypeAsString(char* output, int sizeOfOutput, qboolean upperCase);
void giveWeaponWithDirectCustomAmmoToClient(gentity_t* ent, weapon_t wpn, qboolean autoswitch, int normAmmo, int normClip, int altAmmo, int altClip);
gentity_t* findClosestPlayer(gentity_t* ent, team_t team, qboolean bot);
gentity_t* findClosestTeamPlayer(gentity_t* ent, qboolean bot);
gentity_t* findClosestEnemyPlayer(gentity_t* ent, qboolean bot);
void dbWriteHnsBestPlayersIntoHnsStruct(void);
void dbWriteHnsStats(void);
void fillHnsStats(void);
void setIdentityTeamForCustomGametype(TIdentity* ident);
TIdentity* getRandomCustomTeamIdentity(team_t team);
void sendRoxNextSpec(int recipient, int nextSpec);
void sendRoxLastSpec(int recipient, int lastSpec);
void csinf_buyMenu(gentity_t* ent);
void resetCSInfStruct(gentity_t* ent);
void csinf_handleCash(gentity_t* ent, int cash, char* reason, qboolean printChat);

typedef enum
{
    GUNTYPE_NONE
    , GUNTYPE_PISTOL
    , GUNTYPE_SHOTGUN
    , GUNTYPE_SMG
    , GUNTYPE_RIFLE
    , GUNTYPE_NADE // nades
    , GUNTYPE_UTILITY // armor, thermals
    , GUNTYPE_SPECIAL // mm1's, rpg's etc which we do not intend to anyhow gift out, but who knows?

} gunType_t;

typedef enum {
    GUNCLASS_NONE
    , GUNCLASS_SECONDARY
    , GUNCLASS_PRIMARY
    , GUNCLASS_UTILITY
} csInfGunClass_t;

typedef enum {
    GUNPKP_NONE
    , GUNPKP_RED
    , GUNPKP_BLUE
    , GUNPKP_ALL
} csInfGunPkp_t;

typedef enum {
    GUNWEIGHT_NONE
    , GUNWEIGHT_LIGHT
    , GUNWEIGHT_SHOTGUN
    , GUNWEIGHT_SMG
    , GUNWEIGHT_RIFLE
    , GUNWEIGHT_SNIPER
    , GUNWEIGHT_MACHINEGUN
} csInfGunWeight_t;

typedef enum {
    UTILITY_LOWARMOR
    , UTILITY_MAXARMOR
    , UTILITY_THERMALS
} csInfUtility_t;

typedef struct csInfGuns_s {
    int                 gunId;
    gunType_t           gunType;
    char* gunName;
    char* gunAlias1;
    char* gunAlias2;
    int                 price;
    csInfGunClass_t     gunClass;
    csInfGunPkp_t       pickupType;
    csInfGunWeight_t    gunWeight;
} csInfGuns_t;

#define CSINF_GUNTABLE_SIZE 17
extern csInfGuns_t csInfGunsTable[CSINF_GUNTABLE_SIZE];

typedef struct
{
    char* modelName;
    vec3_t originOffset;
    vec3_t angleOffset;

} prophuntObject_t;

#define TOTAL_PROPHUNT_MODELS 116

extern prophuntObject_t propHuntModels[TOTAL_PROPHUNT_MODELS];

void freeProphuntProps(gentity_t* player);
void transformPlayerToObject(gentity_t* ent);
void chooseProp(gentity_t* ent, int prop);
void prop_pickRandomProps(int* result);
void transformPlayerToProp(gentity_t* ent, int propId, qboolean thinkSingle);
void    prop_ThinkMovingModelSingle(gentity_t* ent);
void    prop_ThinkMovingModels(gentity_t* ent);




typedef struct queueNode_s queueNode;

struct queueNode_s {
    int action;
    int playerId;
    char* message; // in outgoing calls, this can be the value we want to request from whatever (e.g. OTP) 
    // in incoming calls, this will be the response structured with \.
    struct queueNode_s* next;
};

typedef enum {
    THREADRESPONSE_SUCCESS,
    THREADRESPONSE_NOTHING_ENQUEUED,
    THREADRESPONSE_ENQUEUE_COULDNT_MALLOC,
    THREADRESPONSE_THREAD_STOPPED
} threadResponse;

typedef enum {
    THREADACTION_IPHUB_DATA_REQUEST,
    THREADACTION_IPHUB_DATA_RESPONSE,
    THREADACTION_RUN_PRINTF
} threadAction;

struct curlProgressData {
    char* prvt;
    size_t size;
};

#define THREAD_CURL_BIGBUF 1024

#ifdef __linux__
#define THREAD_SLEEP_DURATION 50000
#elif defined _WIN32
#define THREAD_SLEEP_DURATION 50
#endif

#define IPHUB_API_ENDPOINT "http://v2.api.iphub.info/ip/"

typedef enum {
    IPHUBBLOCK_SAFE,
    IPHUBBLOCK_VPN,
    IPHUBBLOCK_UNSURE
} ipHubBlock_t;

qboolean performCurlRequest(char* url, struct curl_slist* customHeaders, qboolean verifypeer, char* output);
size_t curlCallbackWriteToChar(void* contents, size_t size, size_t nmemb, void* userp);
void shutdownThread(void);
int dequeueOutbound(int* action, int* playerId, char* message, int sizeOfMessage);
int dequeueInbound(int* action, int* playerId, char* message, int sizeOfMessage);
int enqueueOutbound(int action, int playerId, char* message, int sizeOfMessage);
int enqueueInbound(int action, int playerId, char* message, int sizeOfMessage);

void initMutex(void);
void acquireInboundMutex(void);
void acquireOutboundMutex(void);
void freeInboundMutex(void);
void freeOutboundMutex(void);
void startThread(void);
void closeThread(void);

void G_setTrackedCvarWithoutTrackMessage(vmCvar_t* cvar, int value);