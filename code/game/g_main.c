// Copyright (C) 2001-2002 Raven Software.
//

#include "g_local.h"

level_locals_t  level;

typedef struct
{
    vmCvar_t    *vmCvar;
    char        *cvarName;
    char        *defaultString;
    int         cvarFlags;
    float       mMinValue, mMaxValue;
    int         modificationCount;  // for tracking changes
    qboolean    trackChange;        // track this variable, and announce if changed
    qboolean    teamShader;         // track and if changed, update shader state

} cvarTable_t;

gentity_t       g_entities[MAX_GENTITIES];
gclient_t       g_clients[MAX_CLIENTS];

vmCvar_t    g_gametype;
vmCvar_t    g_dmflags;
vmCvar_t    g_scorelimit;
vmCvar_t    g_timelimit;
vmCvar_t    g_friendlyFire;
vmCvar_t    g_password;
vmCvar_t    g_needpass;
vmCvar_t    g_maxclients;
vmCvar_t    g_maxGameClients;
vmCvar_t    g_dedicated;
vmCvar_t    g_speed;
vmCvar_t    g_gravity;
vmCvar_t    g_cheats;
vmCvar_t    g_knockback;
vmCvar_t    g_forcerespawn;
vmCvar_t    g_inactivity;
vmCvar_t    g_debugMove;
vmCvar_t    g_debugDamage;
vmCvar_t    g_debugAlloc;
vmCvar_t    g_weaponRespawn;
vmCvar_t    g_backpackRespawn;
vmCvar_t    g_motd;
vmCvar_t    g_synchronousClients;
vmCvar_t    g_warmup;
vmCvar_t    g_doWarmup;
vmCvar_t    g_restarted;
vmCvar_t    g_log;
vmCvar_t    g_logSync;
vmCvar_t    g_logHits;
vmCvar_t    g_allowVote;
vmCvar_t    g_voteDuration;
vmCvar_t    g_voteKickBanTime;
vmCvar_t    g_failedVoteDelay;
vmCvar_t    g_teamAutoJoin;
vmCvar_t    g_teamForceBalance;
vmCvar_t    g_smoothClients;
vmCvar_t    pmove_fixed;
vmCvar_t    pmove_msec;
vmCvar_t    g_rankings;
vmCvar_t    g_listEntity;
vmCvar_t    g_fps;
vmCvar_t    g_respawnInterval;
vmCvar_t    g_respawnInvulnerability;
vmCvar_t    g_roundtimelimit;
vmCvar_t    g_roundjointime;
vmCvar_t    g_timeextension;
vmCvar_t    g_timeouttospec;
vmCvar_t    g_roundstartdelay;
vmCvar_t    g_availableWeapons;
vmCvar_t    g_forceFollow;
vmCvar_t    g_followEnemy;
vmCvar_t    g_mapcycle;
vmCvar_t    g_pickupsDisabled;              // Whether or not pickups are available in a map (uses outfitting if not)
vmCvar_t    g_suicidePenalty;               // Amount of score added for killing yourself (typically negative)
vmCvar_t    g_teamkillPenalty;              // Amount of score added for killing a teammates (typically negative)
vmCvar_t    g_teamkillDamageMax;            // max damage one can do to teammates before being kicked
vmCvar_t    g_teamkillDamageForgive;        // amount of teamkill damage forgiven each minute
vmCvar_t    g_teamkillBanTime;              // number of minutes to ban someone for after being kicked
vmCvar_t    g_teamkillNoExcuseTime;         // number of seconds into a round where tk damage is inexcusable
vmCvar_t    g_teamkillNoExcuseMultiplier;   // multipier for tk damage that is inexcusable
vmCvar_t    g_voiceFloodCount;              // Number of voice messages in one minute to be concidered flooding
vmCvar_t    g_voiceFloodPenalty;            // Amount of time a void flooder must wait before they can use voice again
vmCvar_t    g_suddenDeath;
vmCvar_t    g_voiceTalkingGhosts;           // Allow ghosts to talk to alive players

vmCvar_t    RMG;
vmCvar_t    g_debugRMG;

vmCvar_t    ac_allowcross;
vmCvar_t    ac_norecoil;
vmCvar_t    rox_support;

vmCvar_t    sv_legacyClientMod;
vmCvar_t    sv_clientMod;

vmCvar_t    g_badminPrefix;
vmCvar_t    g_adminPrefix;
vmCvar_t    g_sadminPrefix;
vmCvar_t    g_hadminPrefix;
vmCvar_t    g_rconPrefix;

vmCvar_t    g_inviewFile;
vmCvar_t    g_weaponFile;

// Admin permissions.
vmCvar_t    a_adminlist;
vmCvar_t    a_badmin;
vmCvar_t    a_admin;
vmCvar_t    a_sadmin;
vmCvar_t    a_hadmin;
vmCvar_t    a_scorelimit;
vmCvar_t    a_timelimit;
vmCvar_t    a_swapteams;
vmCvar_t    a_compmode;
vmCvar_t    a_plant;
vmCvar_t    a_roundtimelimit;
vmCvar_t    a_runover;
vmCvar_t    a_rollercoaster;
vmCvar_t    a_respawn;
vmCvar_t    a_mapswitch;
vmCvar_t    a_strip;
vmCvar_t    a_forceteam;
vmCvar_t    a_blockseek;
vmCvar_t    a_nosection;
vmCvar_t    a_shuffleteams;
vmCvar_t    a_nades;
vmCvar_t    a_respawninterval;
vmCvar_t    a_damage;
vmCvar_t    a_gtrestart;
vmCvar_t    a_clan;
vmCvar_t    a_ban;
vmCvar_t    a_broadcast;
vmCvar_t    a_subnetban;
vmCvar_t    a_eventeams;
vmCvar_t    a_clanvsall;
vmCvar_t    a_lock;
vmCvar_t    a_flash;
vmCvar_t    a_forcevote;
vmCvar_t    a_pause;
vmCvar_t    a_burn;
vmCvar_t    a_kick;
vmCvar_t    a_mute;
vmCvar_t    a_friendlyFire;
vmCvar_t    a_rename;
vmCvar_t    a_3rd;
vmCvar_t    a_toggleweapon;
vmCvar_t    a_anticamp;
vmCvar_t    a_pop;
vmCvar_t    a_uppercut;

vmCvar_t    a_followEnemy;

vmCvar_t    g_leanType;

vmCvar_t    g_serverColors;

vmCvar_t    g_maxAliases;
vmCvar_t    g_logToFile;
vmCvar_t    g_logToDatabase;

vmCvar_t    g_dbLogRetention;

vmCvar_t    g_iphubAPIKey;
vmCvar_t    g_useCountryAPI;
vmCvar_t    g_useCountryDb;

vmCvar_t    g_countryAging;
vmCvar_t    g_vpnAutoKick;
vmCvar_t    g_subnetOctets;

vmCvar_t    g_allowThirdPerson;
vmCvar_t    g_enforce1fxAdditions;
vmCvar_t    g_recoilRatio;
vmCvar_t    g_inaccuracyRatio;

vmCvar_t    g_allowCustomTeams;

// We will be using the ROCmod custom* variables so their scoreboard show the correct team name and we can use it across functions in the game module.
// The gametype module dictates these values.
vmCvar_t    g_customBlueName; // Will be given to us from the gametype module.
vmCvar_t    g_customRedName; // Will be given to us from the gametype module.

vmCvar_t    g_useNoLower;
vmCvar_t    g_useNoRoof;
vmCvar_t    g_useNoMiddle;
vmCvar_t    g_useNoWhole;

vmCvar_t    g_useAutoSections;


// Compmode cvars
// Split into 2, one part is the match_ parts, which can be defaulted by config
// Another part is the internal cm parts, which are used to control the states of the game.

// Archiveable vars.
vmCvar_t    match_bestOf; // ensures early exit from winning state if one team's score is too far ahead to reach.
vmCvar_t    match_scorelimit;
vmCvar_t    match_timelimit;
vmCvar_t    match_lockspecs;
vmCvar_t    match_doublerounds; 
vmCvar_t    match_followEnemy;


// Internal vars

// prevXTeamScores - so when first round is over, we save the variable values to the opposite team - so when we're checking CM states, we will compare blue value to blue value.
vmCvar_t    cm_prevRedTeamScore;
vmCvar_t    cm_prevBlueTeamScore; 
vmCvar_t    cm_bestOf;
vmCvar_t    cm_scorelimit;
vmCvar_t    cm_timelimit;
vmCvar_t    cm_lockspecs;
vmCvar_t    cm_doublerounds;
vmCvar_t    cm_state; // init, first round, second round.

vmCvar_t    cm_originalsl;
vmCvar_t    cm_originaltl;

vmCvar_t    g_mvchatCheckSoundFiles;

vmCvar_t    currentGametype;

// MOTD cvars.
vmCvar_t    g_motd1;
vmCvar_t    g_motd2;
vmCvar_t    g_motd3;
vmCvar_t    g_motd4;
vmCvar_t    g_motd5;

vmCvar_t    g_autoEvenTeams;

vmCvar_t    g_useSecureRoxVerification;

// H&S cvars.
// We take over all of the H&S cvars from 1fxmod as-is, otherwise we got a lot to change.
vmCvar_t    hideSeek_roundstartdelay;
vmCvar_t    hideSeek_availableWeapons;
vmCvar_t    hideSeek_Extra;
vmCvar_t    hideSeek_Nades;
vmCvar_t    hideSeek_randomFireNade;
vmCvar_t    hideSeek_Weapons;
vmCvar_t    g_rpgBoost;
vmCvar_t    g_mm1Style;
vmCvar_t    g_rpgRemove;
vmCvar_t    g_CnRsmokenade;
vmCvar_t    g_smokealert;
vmCvar_t    g_CnRsmokeTime;
vmCvar_t    g_hsgiveknife;
vmCvar_t    g_rpgSpeedDrain;
vmCvar_t    g_rpgSpeedDrainSec;
vmCvar_t    g_waterSpeedDecrement;
vmCvar_t    g_stunSpeedDecrement;
vmCvar_t    g_stunSpeedIncrement;
vmCvar_t    g_fireSpeedDecrement;
vmCvar_t    g_rpgSpeedIncrement;
vmCvar_t    g_waterSpeedTime;
vmCvar_t    g_stunSpeedTime;
vmCvar_t    g_fireSpeedTime;
vmCvar_t    g_hnsWeaponsMinPlayers;
vmCvar_t    g_boxAttempts;
vmCvar_t    g_cageAttempts;
vmCvar_t    g_noHighFps;
vmCvar_t    g_hnsStatAging;

vmCvar_t    sv_useLegacyNades;

// CSInf cvars.

vmCvar_t    csinf_suicidePenalty;
vmCvar_t    csinf_maxCash;
vmCvar_t    csinf_minCash;
vmCvar_t    csinf_friendlyKill;

vmCvar_t    csinf_killBonus;
vmCvar_t    csinf_startingCash;

vmCvar_t    g_anticampType;
vmCvar_t    g_anticamp;
vmCvar_t    g_anticampRadius;
vmCvar_t    g_anticampTime;

// Vote cvars. These control the allowed votes in the server. The integer value defines the admin level required to call this vote (0 being allowed by everyone, 5 being allowed by noone)
vmCvar_t    vote_map;
vmCvar_t    vote_kick;
vmCvar_t    vote_timelimit;
vmCvar_t    vote_scorelimit;
vmCvar_t    vote_teams;
vmCvar_t    vote_mute;
vmCvar_t    vote_poll;

vmCvar_t    vote_successThreshold; // value defines what percentage of votes do we need to call the vote a success.


vmCvar_t    g_sockIp;
vmCvar_t    g_sockPort;
vmCvar_t    g_sockIdentifier;
vmCvar_t    g_logThroughSocket;

static cvarTable_t gameCvarTable[] =
{
    // don't override the cheat state set by the system
    { &g_cheats, "sv_cheats", "", 0, 0.0, 0.0, 0, qfalse },

    // noset vars
    { NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "gamedate", __DATE__ , CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { &g_restarted, "g_restarted", "0", CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "^3Mod Name", MODNAME, CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "^3Mod Version", MODVERSION, CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },

    { &g_fps, "sv_fps", "", CVAR_ROM, 0.0, 0.0, 0, qfalse },

    // latched vars
    { &g_gametype, "g_gametype", "dm", CVAR_SERVERINFO | CVAR_LATCH, 0.0, 0.0, 0, qfalse  },

    { &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
    { &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },

    { &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
    { &g_scorelimit, "scorelimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0.0, 0.0, 0, qtrue },
    { &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0.0, 0.0, 0, qtrue },

    { &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse  },

    { &g_friendlyFire, "g_friendlyFire", "0", CVAR_SERVERINFO|CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },

    { &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE, 0.0, 0.0,   },
    { &g_teamForceBalance, "g_teamForceBalance", "1", CVAR_ARCHIVE, 0.0, 0.0,   },

    { &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
    { &g_doWarmup, "g_doWarmup", "0", 0, 0.0, 0.0, 0, qtrue  },
    { &g_log, "g_log", "games.log", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
    { &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
    { &g_logHits, "g_logHits", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },

    { &g_password, "g_password", "", CVAR_USERINFO, 0.0, 0.0, 0, qfalse  },

    { &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse },

    { &g_dedicated, "dedicated", "0", 0, 0.0, 0.0, 0, qfalse  },

    { &g_speed, "g_speed", "280", 0, 0.0, 0.0, 0, qtrue  },
    { &g_gravity, "g_gravity", "800", 0, 0.0, 0.0, 0, qtrue  },
    { &g_knockback, "g_knockback", "700", 0, 0.0, 0.0, 0, qtrue  },
    { &g_weaponRespawn, "g_weaponrespawn", "15", 0, 0.0, 0.0, 0, qtrue  },
    { &g_backpackRespawn, "g_backpackrespawn", "40", 0, 0.0, 0.0, 0, qtrue  },
    { &g_forcerespawn, "g_forcerespawn", "20", 0, 0.0, 0.0, 0, qtrue },
    { &g_inactivity, "g_inactivity", "180", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
    { &g_debugMove, "g_debugMove", "0", 0, 0.0, 0.0, 0, qfalse },
    { &g_debugDamage, "g_debugDamage", "0", 0, 0.0, 0.0, 0, qfalse },
    { &g_debugAlloc, "g_debugAlloc", "0", 0, 0.0, 0.0, 0, qfalse },
    { &g_motd, "g_motd", "", 0, 0.0, 0.0, 0, qfalse },

    { &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_voteDuration, "g_voteDuration", "60", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_voteKickBanTime, "g_voteKickBanTime", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_failedVoteDelay, "g_failedVoteDelay", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_listEntity, "g_listEntity", "0", 0, 0.0, 0.0, 0, qfalse },

    { &g_smoothClients, "g_smoothClients", "1", 0, 0.0, 0.0, 0, qfalse},
    { &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse},
    { &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse},

    { &g_rankings, "g_rankings", "0", 0, 0.0, 0.0, 0, qfalse},

    { &g_respawnInterval, "g_respawnInterval", "15", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_respawnInvulnerability, "g_respawnInvulnerability", "5", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    { &RMG, "RMG", "0", 0, 0.0, 0.0, },
    { &g_debugRMG, "g_debugRMG", "0", 0, 0.0f, 0.0f },

    { &g_timeouttospec,     "g_timeouttospec",  "15",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_roundtimelimit,    "g_roundtimelimit", "5",        CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_roundjointime,     "g_roundjointime",  "5",        CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_timeextension,     "g_timeextension",  "15",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    { &g_roundstartdelay,   "g_roundstartdelay", "5",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    // 1.03 CHANGE - Rename availableWeapons Cvar which might confuse old map cycles
    { &g_availableWeapons,  "g_available", "0", CVAR_SERVERINFO|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,  "g_availableWeapons", "0", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse },
    { NULL,  "legacy_availableWpns", "0", CVAR_ROM, 0.0, 0.0, 0, qfalse },

    { NULL,                 "disable_weapon_knife",                 "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_US_SOCOM",       "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M19",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_silvertalon",    "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_microuzi",       "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M3A1",           "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_MP5",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_USAS_12",        "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M590",           "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_MSG90A1",        "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M4",             "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_AK_74",          "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_SIG551",         "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M60",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_RPG_7",          "2", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_MM_1",           "2", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M84",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_SMOHG92",        "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_AN_M14",         "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_M15",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },

    // Client additions.
    { NULL,                 "disable_pickup_weapon_M67",            "0", CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_F1",             "0", CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_L2A2",           "0", CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { NULL,                 "disable_pickup_weapon_MDN11",          "0", CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },

    { &g_forceFollow,       "g_forceFollow",     "0",             CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_followEnemy,       "g_followEnemy",     "1",             CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    { &g_mapcycle,          "sv_mapcycle",       "none",        CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    { &g_pickupsDisabled,   "g_pickupsDisabled", "0",                   CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qfalse },

    { &g_suicidePenalty,    "g_suicidePenalty",  "-1",                  CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &g_voiceFloodCount,   "g_voiceFloodCount",    "6",                CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_voiceFloodPenalty, "g_voiceFloodPenalty",  "60",               CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &g_teamkillPenalty,            "g_teamkillPenalty",            "-1",  CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_teamkillDamageMax,          "g_teamkillDamageMax",          "300", CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_teamkillDamageForgive,      "g_teamkillDamageForgive",      "50",  CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_teamkillBanTime,            "g_teamkillBanTime",            "5",   CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_teamkillNoExcuseTime,       "g_teamkillNoExcuseTime",       "8",   CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_teamkillNoExcuseMultiplier, "g_teamkillNoExcuseMultiplier", "3",   CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &g_suddenDeath,           "g_suddenDeath",            "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &g_voiceTalkingGhosts,    "g_voiceTalkingGhosts",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &ac_allowcross,    "ac_allowcross",     "1",        CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO,   0.0f,   0.0f,   0,  qfalse },
    { &ac_norecoil,    "ac_norecoil",     "0",        CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO,   0.0f,   0.0f,   0,  qfalse },
    { &rox_support,    "rox_support",     "1",        CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO,   0.0f,   0.0f,   0,  qfalse },
    { &sv_legacyClientMod,    "sv_legacyClientMod",     "",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &sv_clientMod,    "sv_clientMod",     "",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },    

    { &g_badminPrefix,    "g_badminPrefix",     "^CB^b-^kA^+d^7min",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_adminPrefix,    "g_adminPrefix",     "^CA^bd^km^+i^7n",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_sadminPrefix,    "g_sadminPrefix",     "^CS^b-^kA^+d^7min",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_hadminPrefix,    "g_hadminPrefix",     "^CH^b-^kA^+d^7min",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_rconPrefix,    "g_rconPrefix",     "^CS^be^kr^+v^7er",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &g_inviewFile,    "g_inviewFile",     "ext_data/SOF2.inview",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },

    { &g_weaponFile,    "g_weaponFile",     "ext_data/SOF2.wpn",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },

    // Admin cvars
    { &a_adminlist,    "a_adminlist",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_badmin,    "a_badmin",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_admin,    "a_admin",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_sadmin,    "a_sadmin",     "4",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_hadmin,    "a_hadmin",     "5",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_scorelimit,    "a_scorelimit",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_timelimit,    "a_timelimit",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_swapteams,    "a_swapteams",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_compmode,    "a_compmode",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_plant,    "a_plant",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_roundtimelimit,    "a_roundtimelimit",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_runover,    "a_runover",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_rollercoaster,    "a_rollercoaster",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_respawn,    "a_respawn",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_mapswitch,    "a_mapswitch",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_strip,    "a_strip",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_forceteam,    "a_forceteam",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_blockseek,    "a_blockseek",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_nosection,    "a_nosection",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_shuffleteams,    "a_shuffleteams",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_nades,    "a_nades",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_respawninterval,    "a_respawninterval",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_damage,    "a_damage",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_gtrestart,    "a_gtrestart",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_clan,    "a_clan",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_ban,    "a_ban",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_broadcast,    "a_broadcast",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_subnetban,    "a_subnetban",     "4",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_eventeams,    "a_eventeams",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_clanvsall,    "a_clanvsall",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_lock,    "a_lock",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_flash,    "a_flash",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_forcevote,    "a_forcevote",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_pause,    "a_pause",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_burn,    "a_burn",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_kick,    "a_kick",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_mute,    "a_mute",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_friendlyFire,    "a_friendlyFire",     "4",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_rename,    "a_rename",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_3rd,    "a_3rd",     "4",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_toggleweapon,    "a_toggleweapon",     "4",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_anticamp,    "a_anticamp",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_pop,    "a_pop",     "1",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &a_uppercut,    "a_uppercut",     "2",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },

    { &a_followEnemy,    "a_followEnemy",     "3",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },


    { &g_leanType,    "g_leanType",     "0",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse }, // 0 = gold, 1 = silver, 2 = serve based on client
    
    { &g_serverColors,    "g_serverColors",     "Cbk+7",        CVAR_ARCHIVE,   0.0f,   0.0f,   0,  qfalse },
    { &g_maxAliases,    "g_maxAliases",     "10",        CVAR_ARCHIVE | CVAR_LOCK_RANGE,   0.0f,   15.0f,   0,  qfalse },
    { &g_logToFile,    "g_logToFile",     "0",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_logToDatabase,    "g_logToDatabase",     "1",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_dbLogRetention,    "g_dbLogRetention",     "120",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_iphubAPIKey,    "g_iphubAPIKey",     "",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_useCountryAPI,    "g_useCountryAPI",     "1",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_useCountryDb,    "g_useCountryDb",     "1",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_countryAging,    "g_countryAging",     "120",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },
    { &g_vpnAutoKick,    "g_vpnAutoKick",     "1",        CVAR_ARCHIVE | CVAR_LATCH,   0.0f,   0.0f,   0,  qfalse },

    { &g_subnetOctets,    "g_subnetOctets",     "3",        CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,   1.0f,   3.0f,   0,  qfalse },
    { &g_allowThirdPerson, "g_allowThirdPerson", "1",       CVAR_ARCHIVE | CVAR_SERVERINFO, 0.0f, 0.0f, 0, qfalse },
    { &g_enforce1fxAdditions, "g_enforce1fxAdditions", "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse }, // To run H&S/H&Z gametype. For rest, will be optional.
    { &g_recoilRatio, "g_recoilRatio", "1",       CVAR_ARCHIVE | CVAR_SYSTEMINFO | CVAR_LATCH | CVAR_LOCK_RANGE, 0.0f, 1.0f, 0, qfalse }, 
    { &g_inaccuracyRatio, "g_inaccuracyRatio", "1",       CVAR_ARCHIVE | CVAR_SYSTEMINFO | CVAR_LATCH | CVAR_LOCK_RANGE, 0.0f, 1.0f, 0, qfalse }, // THese 2 will not alter the attack stats any more, but rather will rely on server wpnfile provided values. This is only for 1fx Client Additions support.
    { &g_customRedName, "g_customRedName", "Red Team",       CVAR_ROM | CVAR_SYSTEMINFO, 0.0f, 0.0f, 0, qfalse },
    { &g_customBlueName, "g_customBlueName", "Blue Team",       CVAR_ROM | CVAR_SYSTEMINFO, 0.0f, 0.0f, 0, qfalse },
    { &g_allowCustomTeams, "g_allowCustomTeams", "1",       CVAR_ROM | CVAR_SYSTEMINFO, 0.0f, 0.0f, 0, qfalse },
    { &g_useNoLower, "g_useNoLower", "1",       CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_useNoRoof, "g_useNoRoof", "1",       CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_useNoMiddle, "g_useNoMiddle", "1",       CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_useNoWhole, "g_useNoWhole", "1",       CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_useAutoSections, "g_useAutoSections", "1", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &match_bestOf, "match_bestOf", "1", CVAR_ARCHIVE | CVAR_LOCK_RANGE, 0.0f, 1.0f, 0, qfalse },
    { &match_scorelimit, "match_scorelimit", "3", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &match_timelimit, "match_timelimit", "30", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &match_lockspecs, "match_lockspecs", "1", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &match_doublerounds, "match_doublerounds", "1", CVAR_ARCHIVE | CVAR_LOCK_RANGE, 0.0f, 1.0f, 0, qfalse },

    { &cm_prevRedTeamScore, "cm_prevRedTeamScore", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_prevBlueTeamScore, "cm_prevBlueTeamScore", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_state, "cm_state", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_bestOf, "cm_bestOf", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_scorelimit, "cm_scorelimit", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_timelimit, "cm_timelimit", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_lockspecs, "cm_lockspecs", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_doublerounds, "cm_doublerounds", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_originalsl, "cm_originalsl", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &cm_originaltl, "cm_originaltl", "0", CVAR_ROM, 0.0f, 0.0f, 0, qfalse },
    { &g_mvchatCheckSoundFiles, "g_mvchatCheckSoundFiles", "0", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &currentGametype, "current_gametype", "3", CVAR_SERVERINFO | CVAR_ROM | CVAR_INTERNAL, 0.0f, 0.0f, 0, qfalse },
    { &g_motd1, "g_motd1", "", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_motd2, "g_motd2", "", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_motd3, "g_motd3", "", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_motd4, "g_motd4", "", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_motd5, "g_motd5", "", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },

    { &g_autoEvenTeams, "g_autoEvenTeams", "1", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse},
    { &match_followEnemy, "match_followEnemy", "0", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_useSecureRoxVerification, "g_useSecureRoxVerification", "0", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse }, // do NOT enable this unless you've added the secure version of AC verification.

    // H&S
    { &hideSeek_availableWeapons,       "hideSeek_availableWeapons",    "200000000000000022220000", CVAR_INTERNAL | CVAR_ROM, 0.0, 0.0, 0, qfalse },
    { &hideSeek_Extra,          "hideSeek_Extra",           "110110",   CVAR_ARCHIVE | CVAR_LATCH,    0.0,    0.0,  0, qfalse }, // Boe!Man 3/6/11: So users can change if desired.
    { &hideSeek_Nades,          "hideSeek_Nades",           "1111", CVAR_ARCHIVE | CVAR_LATCH,    0.0,    0.0,  0, qfalse }, // Boe!Man 3/6/11: So users can change if desired.
    { &hideSeek_Weapons,        "hideSeek_Weapons",         "11111",  CVAR_ARCHIVE | CVAR_LATCH,    0.0,    0.0,  0, qfalse }, // Boe!Man 3/6/11: So users can change if desired.

    { &hideSeek_roundstartdelay,     "hideSeek_roundstartdelay",      "30",    CVAR_ARCHIVE | CVAR_LATCH,   0.0,    0.0,  0, qfalse },
    { &g_boxAttempts, "g_boxAttempts", "3", CVAR_ARCHIVE,   0.0,    0.0,  0, qtrue },
    { &g_cageAttempts, "g_cageAttempts", "3",   CVAR_ARCHIVE,   0.0,    0.0,  0, qtrue },
    { &g_noHighFps, "g_noHighFps", "1", CVAR_ARCHIVE | CVAR_INIT,   0.0,    0.0,  0, qtrue },
    { &g_rpgBoost,              "g_rpgBoost",               "450",  CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0, 1500.0, 0, qfalse },
    { &g_mm1Style,              "g_mm1Style",               "0",    CVAR_ARCHIVE | CVAR_LATCH,                                      0.0, 0.0, 0, qfalse },
    { &g_rpgRemove,              "g_rpgRemove",               "0",    CVAR_ARCHIVE | CVAR_LATCH,                                      0.0, 0.0, 0, qfalse },
    { &g_smokealert,              "g_smokealert",               "0",    CVAR_ARCHIVE,                                      0.0, 0.0, 0, qfalse },
    { &g_CnRsmokenade,           "g_CnRsmokenade",       "0",       CVAR_ARCHIVE,                                      0.0, 0.0, 0, qfalse },
    { &g_CnRsmokeTime,              "g_CnRsmokeTime",       "1.5",  CVAR_ARCHIVE | CVAR_LOCK_RANGE,                                 0.1, 3.0, 0, qfalse },
    { &g_hsgiveknife,              "g_hsgiveknife",       "0",  CVAR_ARCHIVE | CVAR_LOCK_RANGE,                                 0.0, 2.0, 0, qfalse },
    { &g_rpgSpeedDrain,         "g_rpgSpeedDrain",          "0",    CVAR_ARCHIVE | CVAR_LATCH,                                      0.0, 0.0, 0, qfalse },
    { &g_rpgSpeedDrainSec,      "g_rpgSpeedDrainSec",       "0.25", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.1, 2.0, 0, qfalse },
    { &hideSeek_randomFireNade, "hideSeek_randomFireNade",  "1",    CVAR_ARCHIVE | CVAR_LATCH,                                      0.0,    0.0,  0, qfalse },
    { &g_waterSpeedDecrement,   "g_waterSpeedDecrement",    "110",  CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    400.0,  0, qfalse },
    { &g_stunSpeedDecrement,    "g_stunSpeedDecrement",     "110",  CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    200.0,  0, qfalse },
    { &g_stunSpeedIncrement,    "g_stunSpeedIncrement",     "70",   CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    200.0,  0, qfalse },
    { &g_fireSpeedDecrement,    "g_fireSpeedDecrement",     "110",  CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    200.0,  0, qfalse },
    { &g_rpgSpeedIncrement,     "g_rpgSpeedIncrement",      "70",   CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    200.0,  0, qfalse },
    { &g_waterSpeedTime,        "g_waterSpeedTime",         "200",  CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    500.0,  0, qfalse },
    { &g_stunSpeedTime,         "g_stunSpeedTime",          "4000", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    8000.0,  0, qfalse },
    { &g_fireSpeedTime,         "g_fireSpeedTime",          "1500", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE,                    0.0,    8000.0,  0, qfalse },
    { &g_hnsStatAging,         "g_hnsStatAging",          "15", CVAR_ARCHIVE,                    0.0,    8000.0,  0, qfalse },
    { &sv_useLegacyNades,      "sv_useLegacyNades",         "",     0, 0, 0, 0, qfalse },

    
    { &csinf_suicidePenalty, "csinf_suicidePenalty", "-500", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, -1000, 0.0, 0, qfalse },
    { &csinf_maxCash, "csinf_maxCash", "12000", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, 8000.0, 16000.0, 0, qfalse },
    { &csinf_minCash, "csinf_minCash", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, -1000.0, 1000.0, 0, qfalse },
    { &csinf_friendlyKill, "csinf_friendlyKill", "-1000", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, -2000.0, 0.0, 0, qfalse },
    { &csinf_killBonus, "csinf_killBonus", "150", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, 100.0, 500.0, 0, qfalse },
    { &csinf_startingCash, "csinf_startingCash", "800", CVAR_ARCHIVE | CVAR_LATCH | CVAR_LOCK_RANGE, 0.0, 1800.0, 0, qfalse },
    { &g_anticampType, "g_anticampType", "1", CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse }, // If anticampType = 1 => we use a hybrid version of extents + radius based. If 0, we use extents from ent if we get it or only radius based.
    { &g_anticamp, "g_anticamp", "0", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_anticampRadius, "g_anticampRadius", "300", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &g_anticampTime, "g_anticampTime", "30", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_map, "vote_map", "3", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_kick, "vote_kick", "2", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_timelimit, "vote_timelimit", "2", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_scorelimit, "vote_scorelimit", "2", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_teams, "vote_teams", "3", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_mute, "vote_mute", "2", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &vote_poll, "vote_poll", "1", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    
    { &vote_successThreshold, "vote_successThreshold", "55", CVAR_ARCHIVE | CVAR_LOCK_RANGE, 51.0f, 100.0f, 0, qfalse },
    { &g_sockIp, "g_sockIp", "127.0.0.1", CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse },
    { &g_sockPort, "g_sockPort", "10000", CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse },
    { &g_sockIdentifier, "g_sockIdentifier", "", CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse },
    { &g_logThroughSocket, "g_logThroughSocket", "0", CVAR_ARCHIVE | CVAR_LATCH, 0.0f, 0.0f, 0, qfalse },
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame                 ( int levelTime, int randomSeed, int restart );
void G_RunFrame                 ( int levelTime );
void G_ShutdownGame             ( int restart );
void CheckExitRules             ( void );
void G_InitGhoul                ( void );
void G_ShutdownGhoul            ( void );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5,
                          intptr_t arg6, intptr_t arg7, intptr_t arg8, intptr_t arg9, intptr_t arg10, intptr_t arg11 )
{
    switch ( command )
    {
        case GAME_GHOUL_INIT:
            G_InitGhoul ( );
            return 0;
        case GAME_GHOUL_SHUTDOWN:
            G_ShutdownGhoul ( );
            return 0;
        case GAME_INIT:
            G_InitGame( arg0, arg1, arg2 );
            return 0;
        case GAME_SHUTDOWN:
            G_ShutdownGame( arg0 );
            return 0;
        case GAME_CLIENT_CONNECT:
            return (intptr_t)ClientConnect( arg0, arg1, arg2 );
        case GAME_CLIENT_THINK:
            ClientThink( arg0 );
            return 0;
        case GAME_CLIENT_USERINFO_CHANGED:
            ClientUserinfoChanged( arg0 );
            return 0;
        case GAME_CLIENT_DISCONNECT:
            ClientDisconnect( arg0 );
            return 0;
        case GAME_CLIENT_BEGIN:
            ClientBegin( arg0, qtrue );
            return 0;
        case GAME_CLIENT_COMMAND:
            ClientCommand( arg0 );
            return 0;
        case GAME_GAMETYPE_COMMAND:
            return G_GametypeCommand ( arg0, arg1, arg2, arg3, arg4, arg5 );
        case GAME_RUN_FRAME:
            G_RunFrame( arg0 );
            return 0;
        case GAME_CONSOLE_COMMAND:
            return ConsoleCommand();
        case BOTAI_START_FRAME:
            return BotAIStartFrame( arg0 );
        case GAME_SPAWN_RMG_ENTITY:
            if (G_ParseSpawnVars(qfalse))
            {
                G_SpawnGEntityFromSpawnVars(qfalse);
            }
            return 0;
        case GAME_RCON_LOG:
            logRcon(arg0, arg1);
            return 0;
    }

    return -1;
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
    gentity_t   *e, *e2;
    int     i, j;
    int     c, c2;

    c = 0;
    c2 = 0;
    for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ){
        if (!e->inuse)
            continue;
        if (!e->team)
            continue;
        if (e->flags & FL_TEAMSLAVE)
            continue;
        e->teammaster = e;
        c++;
        c2++;
        for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
        {
            if (!e2->inuse)
                continue;
            if (!e2->team)
                continue;
            if (e2->flags & FL_TEAMSLAVE)
                continue;
            if (!strcmp(e->team, e2->team))
            {
                c2++;
                e2->teamchain = e->teamchain;
                e->teamchain = e2;
                e2->teammaster = e;
                e2->flags |= FL_TEAMSLAVE;

                // make sure that targets only point at the master
                if ( e2->targetname ) {
                    e->targetname = e2->targetname;
                    e2->targetname = NULL;
                }
            }
        }
    }

    Com_Printf ("%i teams with %i entities\n", c, c2);
}

/*
=================
G_RemapTeamShaders
=================
*/
void G_RemapTeamShaders(void)
{
    trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
}

/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
    int         i;
    cvarTable_t *cv;
    qboolean    remapped = qfalse;

    for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ )
    {
        trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags, cv->mMinValue, cv->mMaxValue );

        if ( cv->vmCvar )
        {
            cv->modificationCount = cv->vmCvar->modificationCount;
        }

        if (cv->teamShader)
        {
            remapped = qtrue;
        }
    }

    if (remapped)
    {
        G_RemapTeamShaders();
    }

    level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
    int         i;
    cvarTable_t *cv;
    qboolean    remapped = qfalse;

    for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ )
    {
        if ( cv->vmCvar )
        {
            trap_Cvar_Update( cv->vmCvar );

            if ( cv->modificationCount != cv->vmCvar->modificationCount )
            {
                // Handle any modified cvar checks
                if ( !Q_stricmp ( cv->cvarName, "sv_fps" ) )
                {
                    if ( cv->vmCvar->integer > 50 )
                    {
                        trap_Cvar_Set ( "sv_fps", "50" );
                        trap_Cvar_Update ( cv->vmCvar );
                    }
                    else if ( cv->vmCvar->integer < 10 )
                    {
                        trap_Cvar_Set ( "sv_fps", "10" );
                        trap_Cvar_Update ( cv->vmCvar );
                    }
                }

                cv->modificationCount = cv->vmCvar->modificationCount;

                if ( cv->trackChange )
                {
                    trap_SendServerCommand( -1, va("print \"^7[^3RCON Action^7]: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
                    logGame(NULL, NULL, cv->cvarName, va("Cvar value changed to %s", cv->vmCvar->string));
                }

                if (cv->teamShader)
                {
                    remapped = qtrue;
                }
            }
        }
    }

    if (remapped)
    {
        G_RemapTeamShaders();
    }
}

void G_SetDisabledWeapons(void) {

    for (int i = WP_KNIFE; i < WP_NUM_WEAPONS; i++) {
        gitem_t* item = BG_FindWeaponItem(i);

        if (isCurrentGametypeInList((gameTypes_t[]) { GT_GUNGAME, GT_CSINF, GT_PROP, GT_HNZ, GT_MAX })) {
            trap_Cvar_Set(va("disable_%s", item->classname), "1");
        }
        else {
            if (g_availableWeapons.string[i - 1] == '0') {
                trap_Cvar_Set(va("disable_%s", item->classname), "1");
            }
            else if (g_availableWeapons.string[i - 1] == '2') {
                trap_Cvar_Set(va("disable_%s", item->classname), "0");
            }
            else {
                trap_Cvar_Set(va("disable_%s", item->classname), "2");
            }
        }
    }

}

static void G_TranslateGoldAvailableWpnsToSilver(const char* input, char* output, int outputSize) {

    memset(output, '0', outputSize);
    output[outputSize - 1] = '\0';

    for (int i = L_WP_KNIFE; i < L_WP_NUM_WEAPONS; i++) {
        int goldWpn = trap_TranslateSilverWeaponToGoldWeapon(i);
        if (goldWpn < WP_KNIFE || goldWpn > WP_NUM_WEAPONS - 1) {
            output[i - 1] = '0';
        }
        else {
            output[i - 1] = input[goldWpn - 1];
        }
    }
}

static void G_SetAvailableWeaponsByHnsCvars(void) {

    char available[WP_NUM_WEAPONS];
    available[0] = '2'; // Knife is always enabled.

    for (weapon_t weapon = WP_KNIFE + 1; weapon < WP_NUM_WEAPONS; weapon++) {
        available[weapon - 1] = '0';

        if (weapon == WP_SMOHG92_GRENADE && hideSeek_Nades.string[HSNADE_FRAG] == '1') {
            available[weapon - 1] = '2';
        }
        else if (weapon == WP_ANM14_GRENADE && hideSeek_Nades.string[HSNADE_FIRE] == '1' && !hideSeek_randomFireNade.integer) {
            available[weapon - 1] = '2';
        }
        else if (weapon == WP_M15_GRENADE && hideSeek_Nades.string[HSNADE_SMOKE] == '1') {
            available[weapon - 1] = '2';
        }
        else if (weapon == WP_M84_GRENADE && hideSeek_Nades.string[HSNADE_FLASH] == '1') {
            available[weapon - 1] = '2';
        }

    }
    available[WP_NUM_WEAPONS - 1] = '\0';

    // Set the availableWeapons string
    trap_Cvar_Set("g_available", available);
    trap_Cvar_Update(&g_availableWeapons);
    logSystem(LOGLEVEL_INFO, "new Avail: %s", available);
}


/*
===============
G_UpdateAvailableWeapons

Updates the g_availableWeapons cvar using the disable cvars.
===============
*/
void G_UpdateAvailableWeapons ( void )
{
    weapon_t weapon;
    char     available[WP_NUM_WEAPONS+1];

    memset ( available, 0, sizeof(available) );

    for ( weapon = WP_KNIFE; weapon < WP_NUM_WEAPONS; weapon ++ )
    {
        gitem_t* item = BG_FindWeaponItem ( weapon );
        if ( !item )
        {
            continue;
        }

        switch ( (int)trap_Cvar_VariableValue ( va("disable_%s", item->classname ) ) )
        {
            case 0:
                available[weapon-1] = '2';
                break;

            case 1:
                available[weapon-1] = '0';
                break;

            case 2:
                available[weapon-1] = '1';
                break;
        }
    }

    // 1.03 CHANGE - Rename availableWeapons Cvar which might confuse old map cycles
    trap_Cvar_Set ( "g_available", available );
    trap_Cvar_Set("g_availableWeapons", available);

    if (level.multiprotocol) {
        char silverAvailableWeapons[L_WP_NUM_WEAPONS];
        G_TranslateGoldAvailableWpnsToSilver(available, silverAvailableWeapons, sizeof(silverAvailableWeapons));
        trap_Cvar_Set("legacy_availableWpns", silverAvailableWeapons);
    }

    trap_Cvar_Update ( &g_availableWeapons );
}

static char* G_TranslateGametype(char* gametype) {

    // We assume a valid gametype has been passed.
    // Therefore, we only replace h&s with hns, h&z with hnz.

    if (!Q_stricmp(gametype, "h&s")) {
        return "hns";
    }

    if (!Q_stricmp(gametype, "h&z")) {
        return "hnz";
    }

    return gametype;

}

qboolean G_IsGametypeAValidGametype(char* gametype) {

    if (!Q_stricmp(gametype, "h&s") || !Q_stricmp(gametype, "hns")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "h&z") || !Q_stricmp(gametype, "hnz")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "gg")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "vip")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "prop")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "mm")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "csinf")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "dem")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "inf")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "ctf")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "dm")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "tdm")) {
        return qtrue;
    }
    else if (!Q_stricmp(gametype, "elim")) {
        return qtrue;
    }
    else {
        return qfalse;
    }
}

/*
===============
G_TranslateGametypeToPublicGametype

Takes the actual gametype (e.g. h&s) and translates it to a public value.
Public value will be used for example in info query and in configstring.
===============
*/
char* G_TranslateGametypeToPublicGametype(char* gametype) {

    if (!Q_stricmp(gametype, "h&s") || !Q_stricmp(gametype, "hns")) {
        return "inf";
    }

    if (!Q_stricmp(gametype, "h&z") || !Q_stricmp(gametype, "hnz")) {
        return "inf";
    }

    if (!Q_stricmp(gametype, "gg")) {
        return "dm";
    }

    if (!Q_stricmp(gametype, "vip")) {
        return "elim";
    }

    if (!Q_stricmp(gametype, "prop")) {
        return "inf";
    }

    if (!Q_stricmp(gametype, "mm")) {
        return "??"; // Devil once started with this gametype, but I've no idea what were his plans. 
                        // Perhaps one day I'll get an idea for it, so far adding it just as a placeholder value.
    }

    if (!Q_stricmp(gametype, "dem")) {
        return "inf";
    }

    if (!Q_stricmp(gametype, "csinf")) {
        return "inf";
    }

    return gametype;
}

 /*
===============
G_TranslateGametypeToPublicGametype

Takes the actual gametype (e.g. h&s) and translates it to a public value.
Public value will be used for example in info query and in configstring.
===============
*/
void G_SetCurrentGametypeValue(char* gametype) {

    if (!Q_stricmp(gametype, "h&s") || !Q_stricmp(gametype, "hns")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_HNS));
    } else if (!Q_stricmp(gametype, "h&z") || !Q_stricmp(gametype, "hnz")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_HNZ));
    } else if (!Q_stricmp(gametype, "gg")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_GUNGAME));
    } else if (!Q_stricmp(gametype, "vip")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_VIP));
    } else if (!Q_stricmp(gametype, "prop")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_PROP));
    } else if (!Q_stricmp(gametype, "mm")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_MM));
    } else if (!Q_stricmp(gametype, "csinf")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_CSINF));
    } else if (!Q_stricmp(gametype, "dem")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_DEM));
    } else if (!Q_stricmp(gametype, "inf")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_INF));
    } else if (!Q_stricmp(gametype, "ctf")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_CTF));
    } else if (!Q_stricmp(gametype, "dm")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_DM));
    } else if (!Q_stricmp(gametype, "tdm")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_TDM));
    } else if (!Q_stricmp(gametype, "elim")) {
        trap_Cvar_Set("current_gametype", va("%d", GT_ELIM));
    } else {
        logSystem(LOGLEVEL_WARN, "Couldn't map gametype %s to a meaningful GT_ enum value...", gametype);
        trap_Cvar_Set("current_gametype", va("%d", GT_NONE));
    }

    trap_Cvar_Update(&currentGametype);
}

/*
===============
G_SetGametype

Sets the current gametype to the given value, if the map doesnt support it then it will
use the first gametype that it does support
===============
*/
void G_SetGametype ( const char* gametype )
{
    // 1.03 CHANGE - (unofficial) Fix for base gametype issue
    // Make sure the gametype is valid, if not default to deathmatch
    level.gametype = BG_FindGametype ( gametype );

    // First make sure its supported on this map
    if ((level.gametype == -1) || (!G_DoesMapSupportGametype (gametype)
     && (!bg_gametypeData[level.gametype].basegametype || !G_DoesMapSupportGametype (bg_gametypeData[level.gametype].basegametype))))
    {
        int i = 0;

        // Find a gametype it does support
        for ( i = 0; i < bg_gametypeCount; i ++ )
        {
            if ( G_DoesMapSupportGametype ( bg_gametypeData[i].name ) )
            {
                break;
            }
        }

        // This is bad, this means the map doesnt support any gametypes
        if ( i >= bg_gametypeCount )
        {
            Com_Error ( ERR_FATAL, "map does not support any of the available gametypes" );
        }

        G_LogPrintf ( "gametype '%s' is not supported on this map and was defaulted to '%s'\n",
                     gametype,
                     bg_gametypeData[i].name );

        gametype = bg_gametypeData[i].name;
        trap_Cvar_Set( "g_gametype", gametype );
        trap_Cvar_Set( "RMG_mission", gametype );
        level.gametype = BG_FindGametype ( gametype );

        trap_Cvar_Update( &g_gametype );
    }

    level.gametypeData = &bg_gametypeData[level.gametype];

    // Copy the backpack percentage over
    bg_itemlist[MODELINDEX_BACKPACK].quantity = level.gametypeData->backpack;

    // Set the pickup state
    if ( RMG.integer || g_pickupsDisabled.integer || level.gametypeData->pickupsDisabled )
    {
        level.pickupsDisabled = qtrue;
        trap_SetConfigstring ( CS_PICKUPSDISABLED, "1" );
    }
    else
    {
        level.pickupsDisabled = qfalse;
        trap_SetConfigstring ( CS_PICKUPSDISABLED, "0" );
    }

    // Update public gametype variable
    trap_Cvar_Set("g_publicGametype", G_TranslateGametypeToPublicGametype(g_gametype.string));

    // And set the currentGametype integer value as well. This is used by e.g. Rox AC to display the actual gametype value.
    G_SetCurrentGametypeValue(g_gametype.string);
}

static void G_InitClientMod(void) {

    if (sv_legacyClientMod.string && strlen(sv_legacyClientMod.string) > 0) {

        if (!Q_stricmp(sv_legacyClientMod.string, "RPM")) {
            trap_Cvar_Register(NULL, "modname", "RPM 2 k 3 v2.00 ^_- ^31fxplus", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0);
            level.legacyMod = CL_RPM;
            Com_PrintInfo("Legacy clientmod ^1R^3P^4M ^7loaded!\n");
        }
        else {
            Com_PrintWarn("Legacy client mod \"%s\" is unknown.\n", sv_legacyClientMod.string);
        }
        
    }

    if (sv_clientMod.string && strlen(sv_clientMod.string) > 0) {
        if (!Q_stricmp(sv_clientMod.string, "rocmod") || !Q_stricmp(sv_clientMod.string, "1fx.rocmod")) {
            // Register ROCmod specific CVARs.
            //if (!g_enforce1fxAdditions.integer) {
                // Our ROCmod client with 1fx. additions properly reads the 1fx. Mod server version.
                trap_Cvar_Register(NULL, "sv_modVersion", "| ^71fx^1plus. 2.1c", CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
            //}

            //trap_Cvar_Register(NULL, "g_allowCustomTeams", "1", CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
            //trap_Cvar_Register(NULL, "g_customRedName", va("%s^7 Team", server_redteamprefix.string), CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
            //trap_Cvar_Register(NULL, "g_customBlueName", va("%s^7 Team", server_blueteamprefix.string), CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);

            // Automatic demo recording in ROCmod.
            trap_Cvar_Register(NULL, "g_autoMatchDemo", "1", CVAR_ARCHIVE, 0.0, 0.0);
            trap_Cvar_Register(NULL, "inMatch", "0", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_TEMP, 0.0, 0.0);

            // Client death messages are handled by client.
            //g_clientDeathMessages.integer = 1;

            level.goldMod = CL_ROCMOD;
            Com_PrintInfo("Clientmod ^1ROCmod ^7loaded!\n");
        }
        else {
            Com_PrintWarn("Client mod \"%s\" is unknown.\n", sv_clientMod.string);
        }
    }

}

/*
============
G_InitGame
============
*/
void G_InitGame( int levelTime, int randomSeed, int restart )
{
    int i;

    Com_Printf ("------- Game Initialization -------\n");
    Com_Printf ("gamename: %s %s\n", PRODUCT_NAME, PRODUCT_VERSION);
    Com_Printf ("gamedate: %s\n", __DATE__);

    // As system logging goes to DB's as well, we need to init the db before we do any activity. Otherwise, funny things are going to happen... :)
    loadDatabases();
    srand( randomSeed );

    // set some level globals
    memset( &level, 0, sizeof( level ) );
    level.time = levelTime;
    level.startTime = levelTime;
    level.nextSQLBackupTime = level.time + 50000;

    level.multiprotocol = trap_Cvar_VariableIntegerValue("net_multiprotocol");

    G_RegisterCvars();

    // Initialize the game memory system.
    G_InitMemory();

    // Load the list of arenas
    G_LoadArenas ( );

    // Build the gametype list so we can verify the given gametype
    BG_BuildGametypeList ( );

    // Set the current gametype

    trap_Cvar_Set("g_gametype", G_TranslateGametype(g_gametype.string));
    trap_Cvar_Update(&g_gametype);

    G_SetGametype(g_gametype.string);

    if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX })) {
        trap_Cvar_Set("g_enforce1fxAdditions", "1");
        trap_Cvar_Update(&g_enforce1fxAdditions);

        trap_Cvar_Set("sv_clientMod", "1fx.rocmod");
        trap_Cvar_Update(&sv_clientMod);

        trap_Cvar_Set("sv_useLegacyNades", "1");
        trap_Cvar_Update(&sv_useLegacyNades);
    }
    else {
        // We do not enforce when it's not needed. Up to the server owner to set the clientmod to 1fx.rocmod.
        // We also do not reset the variable. To be honest, if the server runs h&s / h&z, it should be 1fx.rocmod all the time.
        trap_Cvar_Set("g_enforce1fxAdditions", "0");
        trap_Cvar_Update(&g_enforce1fxAdditions);
        trap_Cvar_Set("sv_useLegacyNades", "0");
        trap_Cvar_Update(&sv_useLegacyNades);
    }

    if (isCurrentGametype(GT_HNS)) {
        G_SetAvailableWeaponsByHnsCvars();
    }

    G_SetDisabledWeapons();

    // Sets the available weapons cvar from the disable_ cvars.
    // ... but first disable the weapons which are not in 1.00 if we're in multiprotocol game.
    if (level.multiprotocol) {
        trap_Cvar_Set("disable_pickup_weapon_silvertalon", "1");
        trap_Cvar_Set("disable_pickup_weapon_MP5", "1");
        trap_Cvar_Set("disable_pickup_weapon_SIG551", "1");

        if (sv_useLegacyNades.integer && g_enforce1fxAdditions.integer) {

            ammoNames[12] = "F1";
            ammoNames[13] = "MDN11";

        }

    }

    G_UpdateAvailableWeapons ( );

    // Set the available outfitting
    BG_SetAvailableOutfitting ( g_availableWeapons.string );

    // Give the game a uniqe id
    trap_SetConfigstring ( CS_GAME_ID, va("%d", randomSeed ) );

    if ( g_log.string[0] )
    {
        if ( g_logSync.integer )
        {
            trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
        }
        else
        {
            trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
        }

        if ( !level.logFile )
        {
            Com_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
        }
        else
        {
            char    serverinfo[MAX_INFO_STRING];

            trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

            G_LogPrintf("------------------------------------------------------------\n" );
            G_LogPrintf("InitGame: %s\n", serverinfo );
        }
    }
    else
    {
        Com_Printf( "Not logging to disk.\n" );
    }

    G_InitWorldSession();
    readMutesFromSession();

    // initialize all entities for this game
    memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
    level.gentities = g_entities;

    // initialize all clients for this game
    level.maxclients = g_maxclients.integer;
    memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
    level.clients = g_clients;

    // set client fields on player ents
    for ( i=0 ; i<level.maxclients ; i++ )
    {
        g_entities[i].client = level.clients + i;
    }

    // always leave room for the max number of clients,
    // even if they aren't all used, so numbers inside that
    // range are NEVER anything but clients
    level.num_entities = MAX_CLIENTS;

    // let the server system know where the entites are
    trap_LocateGameData( level.gentities,
                         level.num_entities,
                         sizeof( gentity_t ),
                         &level.clients[0].ps,
                         sizeof( level.clients[0] ) );

    // Get the boundaries of the world
    trap_GetWorldBounds ( level.worldMins, level.worldMaxs );

    // reserve some spots for dead player bodies
    G_InitBodyQueue();

    ClearRegisteredItems();

    if (isCurrentGametype(GT_HNS)) {
        AddSpawnField("classname", "gametype_item");
        AddSpawnField("targetname", "briefcase");
        AddSpawnField("gametype", "hns");
        AddSpawnField("origin", "9999 9999 9999");
        trap_UnlinkEntity(&g_entities[G_SpawnGEntityFromSpawnVars(qfalse)]);
        Q_strncpyz(level.hns.cagewinner, "none", sizeof(level.hns.cagewinner));
    }

    // parse the key/value pairs and spawn gentities
    G_SpawnEntitiesFromString(qfalse);

    initBspModelSpawns();

    // Now parse the gametype information that we need.  This needs to be
    // done after the entity spawn so that the items and triggers can be
    // linked up properly
    G_ParseGametypeFile ( );

    BG_ParseInviewFile( level.pickupsDisabled );

    // Load in the identities
    BG_ParseNPCFiles ( );

    // general initialization
    G_FindTeams();

    SaveRegisteredItems();

    Com_Printf ("-----------------------------------\n");

    if( trap_Cvar_VariableIntegerValue( "com_buildScript" ) )
    {
        G_SoundIndex( "sound/player/gurp1.wav" );
        G_SoundIndex( "sound/player/gurp2.wav" );
    }

#ifdef _SOF2_BOTS
    if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) )
    {
        BotAISetup( restart );
        BotAILoadMap( restart );
        G_InitBots( restart );
    }
#endif

    G_RemapTeamShaders();
    level.actionSoundIndex = G_SoundIndex("sound/misc/menus/click.wav");
    // Load clientmod specifics
    G_InitClientMod();

    // Boe's magnificent mvchat system.
    mvchat_parseFiles();

    // Initialize the gametype
    trap_GT_Init ( g_gametype.string, restart );

    // Music
    if ( RMG.integer )
    {
        char temp[MAX_INFO_STRING];

        // start the music
        trap_Cvar_VariableStringBuffer("RMG_music", temp, MAX_QPATH);
        trap_SetConfigstring( CS_MUSIC, temp );
    }

    if (g_useAutoSections.integer) {
        if (level.noSectionEntFound[MAPSECTION_NOLOWER]) {
            level.autoSectionActive[MAPSECTION_NOLOWER] = qtrue;
        }

        if (level.noSectionEntFound[MAPSECTION_NOROOF]) {
            level.autoSectionActive[MAPSECTION_NOROOF] = qtrue;
        }

        if (level.noSectionEntFound[MAPSECTION_NOMIDDLE]) {
            level.autoSectionActive[MAPSECTION_NOMIDDLE] = qtrue;
        }

        if (level.noSectionEntFound[MAPSECTION_NOWHOLE]) {
            level.autoSectionActive[MAPSECTION_NOWHOLE] = qtrue;
        }


    }

    trap_SetConfigstring( CS_VOTE_TIME, "" );

    Com_Printf("Starting threads...\n");
    startThread();
    
    if (g_logThroughSocket.integer) {
        char heartbeat[MAX_QPATH];
        Q_strncpyz(heartbeat, va("heartbeat\\%s", g_sockIdentifier.string), sizeof(heartbeat));
        enqueueOutbound(THREADACTION_LOG_VIA_SOCKET, -1, heartbeat, strlen(heartbeat));
    }

    if (cm_state.integer) {

        if (restart) {
            if (cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) {


                if (cm_lockspecs.integer) {
                    G_printCustomMessageToAll("Competition Mode", "Spectators have been locked.");
                    level.specLocked = qtrue;
                }

                level.blueLocked = qtrue;
                level.redLocked = qtrue;

                trap_Cvar_Set("scorelimit", va("%d", cm_scorelimit.integer));
                trap_Cvar_Set("timelimit", va("%d", cm_timelimit.integer));
                trap_Cvar_Update(&g_scorelimit);
                trap_Cvar_Update(&g_timelimit);
            }
            else if (cm_state.integer == COMPMODE_END) {
                // Reset the variables back.
                level.specLocked = qfalse;
                level.blueLocked = qfalse;
                level.redLocked = qfalse;

                resetCompetitionModeVariables();

            }
            else if (cm_state.integer == COMPMODE_PRE_ROUND2) {
                level.nextCmInfoDisplay = level.time;
                level.autoSwapTime = level.time + 1000;
            }
        }
        else {
            // There can be scenarios where cm_ is still set, e.g. changing map during compmode etc.
            // We reset the variables in case it wasn't a map restart.
            resetCompetitionModeVariables();
        }
    }
}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
    Com_Printf ("==== ShutdownGame ====\n");
    logGame(NULL, NULL, "ShutdownGame", "");
    if ( level.logFile )
    {
        G_LogPrintf("ShutdownGame:\n" );
        G_LogPrintf("------------------------------------------------------------\n" );
        trap_FS_FCloseFile( level.logFile );
    }

    if (isCurrentGametype(GT_PROP)) {

        for (int i = 0; i < level.numConnectedClients; i++) {
            gentity_t* ent = &g_entities[level.sortedClients[i]];

            if (ent->client->sess.team == TEAM_BLUE) {
                ent->client->sess.team = TEAM_RED;
            }
        }
    }

    // write all the client session data so we can get it back
    G_WriteSessionData();
    writeMutesIntoSession();

#ifdef _SOF2_BOTS
    if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) )
    {
        BotAIShutdown( restart );
    }
#endif

    Com_PrintInfo("Unloading in-memory databases...\n");
    unloadInMemoryDatabases();
    closeThread();

    // Shutdown the gametype last.
    // Don't make any system calls beyond this point.
    trap_GT_Shutdown();
}


//===================================================================

void QDECL Com_Error ( int level, const char *error, ... ) {
    va_list     argptr;
    char        text[1024];

    va_start (argptr, error);
    Q_vsnprintf (text, sizeof(text), error, argptr);
    va_end (argptr);

    trap_Error( text );
}

void QDECL Com_DPrintf( const char *msg, ... ) {
    va_list     argptr;
    char        text[1024];

    if (trap_Cvar_VariableIntegerValue("developer")) {
        va_start(argptr, msg);
        Q_vsnprintf(text, sizeof(text), msg, argptr);
        va_end(argptr);

        trap_Print(text);
    }
}


void QDECL Com_Printf(const char* msg, ...) {
    va_list     argptr;
    char        text[1024];

    va_start(argptr, msg);
    Q_vsnprintf(text, sizeof(text), msg, argptr);
    va_end(argptr);

    trap_Print(text);
}

void QDECL Com_PrintInfo(const char* msg, ...) {
    va_list argptr;
    char formattedMsg[1012];
    char finalMsg[1012 + 12]; // Reserve extra space for the prefix "^l[INFO] ^7"

    // Format the original message
    va_start(argptr, msg);
    Q_vsnprintf(formattedMsg, sizeof(formattedMsg), msg, argptr);
    va_end(argptr);

    // Prepend the prefix
    snprintf(finalMsg, sizeof(finalMsg), "^l[INFO] ^7%s", formattedMsg);

    // Print the final message
    trap_Print(finalMsg);
}


void QDECL Com_PrintWarn(const char* msg, ...) {
    va_list argptr;
    char formattedMsg[1008];
    char finalMsg[1008 + 16]; // Reserve extra space for the prefix "^l[INFO] ^7"

    // Format the original message
    va_start(argptr, msg);
    Q_vsnprintf(formattedMsg, sizeof(formattedMsg), msg, argptr);
    va_end(argptr);

    // Prepend the prefix
    snprintf(finalMsg, sizeof(finalMsg), "^1[WARNING] ^7%s", formattedMsg);

    // Print the final message
    trap_Print(finalMsg);
}


void QDECL Com_PrintLog(const char* msg, ...) {
    va_list argptr;
    char formattedMsg[1012];
    char finalMsg[1012 + 12]; // Reserve extra space for the prefix "^l[INFO] ^7"

    // Format the original message
    va_start(argptr, msg);
    Q_vsnprintf(formattedMsg, sizeof(formattedMsg), msg, argptr);
    va_end(argptr);

    // Prepend the prefix
    snprintf(finalMsg, sizeof(finalMsg), "^3[LOG] ^7%s", formattedMsg);

    // Print the final message
    trap_Print(finalMsg);
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
SortRanks
=============
*/
int QDECL SortRanks( const void *a, const void *b )
{
    gclient_t   *ca;
    gclient_t   *cb;

    ca = &level.clients[*(int *)a];
    cb = &level.clients[*(int *)b];

    // sort special clients last
    if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD )
    {
        return 1;
    }

    if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD )
    {
        return -1;
    }

    // then connecting clients
    if ( ca->pers.connected == CON_CONNECTING )
    {
        return 1;
    }

    if ( cb->pers.connected == CON_CONNECTING )
    {
        return -1;
    }

    // then spectators
    if ( ca->sess.team == TEAM_SPECTATOR && cb->sess.team == TEAM_SPECTATOR )
    {
        if ( ca->sess.spectatorTime < cb->sess.spectatorTime )
        {
            return -1;
        }
        if ( ca->sess.spectatorTime > cb->sess.spectatorTime )
        {
            return 1;
        }
        return 0;
    }

    if ( ca->sess.team == TEAM_SPECTATOR )
    {
        return 1;
    }

    if ( cb->sess.team == TEAM_SPECTATOR )
    {
        return -1;
    }

    // then sort by score
    if ( ca->sess.score > cb->sess.score )
    {
        return -1;
    }

    if ( ca->sess.score < cb->sess.score )
    {
        return 1;
    }

    // then sort by kills
    if ( ca->sess.kills > cb->sess.kills )
    {
        return -1;
    }

    if ( ca->sess.kills < cb->sess.kills )
    {
        return 1;
    }

    // then sort by deaths
    if ( ca->sess.deaths > cb->sess.deaths )
    {
        return -1;
    }

    if ( ca->sess.deaths < cb->sess.deaths )
    {
        return 1;
    }

    return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void )
{
    int         i;
    int         rank;
    int         score;
    int         newScore;
    gclient_t   *cl;

    level.follow1 = -1;
    level.follow2 = -1;
    level.numConnectedClients = 0;
    level.numNonSpectatorClients = 0;
    level.numPlayingClients = 0;
    level.numVotingClients = 0;     // don't count bots

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( level.clients[i].pers.connected != CON_DISCONNECTED )
        {
            level.sortedClients[level.numConnectedClients] = i;
            level.numConnectedClients++;

            if ( level.clients[i].sess.team != TEAM_SPECTATOR )
            {
                level.numNonSpectatorClients++;

                // decide if this should be auto-followed
                if ( level.clients[i].pers.connected == CON_CONNECTED )
                {
                    level.numPlayingClients++;
                    if ( !(g_entities[i].r.svFlags & SVF_BOT) )
                    {
                        level.numVotingClients++;
                    }
                    if ( level.follow1 == -1 )
                    {
                        level.follow1 = i;
                    }
                    else if ( level.follow2 == -1 )
                    {
                        level.follow2 = i;
                    }
                }
            }
        }
    }

    qsort( level.sortedClients, level.numConnectedClients,
           sizeof(level.sortedClients[0]), SortRanks );

    // set the rank value for all clients that are connected and not spectators
    if ( level.gametypeData->teams )
    {
        int rank;
        if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] )
        {
            rank = 2;
        }
        else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] )
        {
            rank = 0;
        }
        else
        {
            rank = 1;
        }

        // in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
        for ( i = 0;  i < level.numConnectedClients; i++ )
        {
            cl = &level.clients[ level.sortedClients[i] ];
            cl->ps.persistant[PERS_RANK] = rank;
        }
    }
    else
    {
        rank = -1;
        score = 0;
        for ( i = 0;  i < level.numPlayingClients; i++ )
        {
            cl = &level.clients[ level.sortedClients[i] ];
            newScore = cl->sess.score;
            if ( i == 0 || newScore != score )
            {
                rank = i;
                // assume we aren't tied until the next client is checked
                level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
            }
            else
            {
                // we are tied with the previous client
                level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
                level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
            }
            score = newScore;
        }
    }


    // see if it is time to end the level
    CheckExitRules();

    // if we are at the intermission, send the new info to everyone
    if ( level.intermissiontime )
    {
        SendScoreboardMessageToAllClients();
    }
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
    int     i;

    for ( i = 0 ; i < level.maxclients ; i++ ) {
        if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
            DeathmatchScoreboardMessage( g_entities + i );
        }
    }
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent )
{
    // take out of follow mode if needed
    if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    {
        G_StopFollowing( ent );
    }

    FindIntermissionPoint ( );

    // move to the spot
    VectorCopy( level.intermission_origin, ent->s.origin );
    VectorCopy( level.intermission_origin, ent->r.currentOrigin );
    VectorCopy( level.intermission_origin, ent->client->ps.pvsOrigin );
    VectorCopy( level.intermission_origin, ent->client->ps.origin );
    VectorCopy (level.intermission_angle, ent->client->ps.viewangles);

    // Reset some client variables
    ent->client->ps.pm_type       = PM_INTERMISSION;
    ent->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
    ent->client->ps.eFlags        = 0;
    ent->s.eFlags                 = 0;
    ent->s.eType                  = ET_GENERAL;
    ent->s.modelindex             = 0;
    ent->s.loopSound              = 0;
    ent->s.event                  = 0;
    ent->r.contents               = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void )
{
    gentity_t   *ent, *target;
    vec3_t      dir;

    // find the intermission spot
    ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
    if ( !ent )
    {
        gspawn_t* spawn = G_SelectRandomSpawnPoint ( -1 );
        if ( spawn )
        {
            VectorCopy (spawn->origin, level.intermission_origin);
            VectorCopy (spawn->angles, level.intermission_angle);
        }

        return;
    }

    VectorCopy (ent->s.origin, level.intermission_origin);
    VectorCopy (ent->s.angles, level.intermission_angle);

    // if it has a target, look towards it
    if ( ent->target )
    {
        target = G_PickTarget( ent->target );
        if ( target )
        {
            VectorSubtract( target->s.origin, level.intermission_origin, dir );
            vectoangles( dir, level.intermission_angle );
        }
    }
}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void )
{
    int         i;
    gentity_t   *ent;

    if ( level.intermissiontime )
    {
        // already active
        return;
    }

    // Kill any votes
    level.vote.voteTime = 0;
    trap_SetConfigstring( CS_VOTE_TIME, "" );

    level.intermissiontime  = level.time;
    level.gametypeResetTime = level.time;
    FindIntermissionPoint();

    // move all clients to the intermission point
    for (i=0 ; i< level.maxclients ; i++)
    {
        ent = g_entities + i;
        if (!ent->inuse)
        {
            continue;
        }

        // take out of follow mode
        G_StopFollowing( ent );

        // Get rid of ghost state
        G_StopGhosting ( ent );

        // respawn if dead
        if ( G_IsClientDead ( ent->client ) )
        {
            respawn ( ent );
        }

        MoveClientToIntermission( ent );
    }

    // send the current scoring to all clients
    SendScoreboardMessageToAllClients();

    if (level.goldMod == CL_ROCMOD) {
        ROCmod_sendBestPlayerStats();
    }

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel (void)
{
    int         i;
    gclient_t   *cl;


    if (cm_state.integer) {

        trap_Cvar_Set("cm_state", va("%d", cm_state.integer + 1));
        trap_Cvar_Update(&cm_state);

        trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0");
        level.intermissiontime = 0;
        level.teamScores[TEAM_RED] = 0;
        level.teamScores[TEAM_BLUE] = 0;
        return;
    }

    // Next map
    trap_SendConsoleCommand( EXEC_APPEND, "mapcycle\n" );
    level.changemap = NULL;
    level.intermissiontime = 0;

    // reset all the scores so we don't enter the intermission again
    level.teamScores[TEAM_RED] = 0;
    level.teamScores[TEAM_BLUE] = 0;
    for ( i=0 ; i< g_maxclients.integer ; i++ )
    {
        cl = level.clients + i;
        if ( cl->pers.connected != CON_CONNECTED )
        {
            continue;
        }

        cl->sess.score = 0;
        cl->ps.persistant[PERS_SCORE] = 0;
    }

    // we need to do this here before chaning to CON_CONNECTING
    G_WriteSessionData();

    // change all client states to connecting, so the early players into the
    // next level will know the others aren't done reconnecting
    for (i=0 ; i< g_maxclients.integer ; i++)
    {
        if ( level.clients[i].pers.connected == CON_CONNECTED )
        {
            level.clients[i].pers.connected = CON_CONNECTING;
        }
    }

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
    va_list     argptr;
    char        string[1024];
    int         min, tens, sec;

    sec = level.time / 1000;

    min = sec / 60;
    sec -= min * 60;
    tens = sec / 10;
    sec -= tens * 10;

    Com_sprintf( string, sizeof(string), "%4i:%i%i ", min, tens, sec );

    va_start( argptr, fmt );
    vsprintf( string +8 , fmt,argptr );
    va_end( argptr );

#ifdef NDEBUG
    if ( g_dedicated.integer ) {
#endif // NDEBUG
        Com_Printf( "%s", string + 8 );
#ifdef NDEBUG
    }
#endif // NDEBUG

    if ( !level.logFile ) {
        return;
    }

    trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string )
{
    int             i;
    int             numSorted;
    gclient_t       *cl;

    G_LogPrintf( "Exit: %s\n", string );
    logGame(NULL, NULL, "exit", string);

    level.intermissionQueued = level.time;

    // this will keep the clients from playing any voice sounds
    // that will get cut off when the queued intermission starts
    trap_SetConfigstring( CS_INTERMISSION, "1" );

    // don't send more than 32 scores (FIXME?)
    numSorted = level.numConnectedClients;
    if ( numSorted > 32 )
    {
        numSorted = 32;
    }

    if ( level.gametypeData->teams )
    {
        G_LogPrintf( "red:%i  blue:%i\n", level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
    }

    for (i=0 ; i < numSorted ; i++)
    {
        int ping;

        cl = &level.clients[level.sortedClients[i]];

        if ( cl->sess.team == TEAM_SPECTATOR )
        {
            continue;
        }

        if ( cl->pers.connected == CON_CONNECTING )
        {
            continue;
        }

        ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

        G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->sess.score, ping, level.sortedClients[i],  cl->pers.netname );
    }
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void )
{
    //Ryan
    if (!level.exitTime)
    {
        level.exitTime = level.time;
    }

    

    if (level.time < level.exitTime + 5000)
    {
        return;
    }

    if (!level.awardTime)
    {
        if (isCurrentGametype(GT_HNS))
            showHnsScores();
        else
            sendClientmodAwards();
        level.awardTime = level.time;
        level.lastAwardSent = level.time;

        // Boe!Man 10/27/14: Make sure that in H&Z players are all forceteamed to spec.
        if (isCurrentGametype(GT_HNZ)) {
            int i;
            gentity_t* ent;

            for (i = 0; i < level.maxclients; i++) {
                ent = g_entities + i;
                if (ent->client->pers.connected != CON_CONNECTED)
                    continue;

                if (ent->client->sess.team == TEAM_SPECTATOR)
                    continue;

                SetTeam(ent, "spectator", NULL, qtrue);
            }
        }
        return;
    }

    if (level.awardTime && (level.time > level.lastAwardSent + 3000))
    {
        if (isCurrentGametype(GT_HNS))
            showHnsScores();
        else
            sendClientmodAwards();
        level.lastAwardSent = level.time;
    }

    if (level.time < level.awardTime + 15000)
    {
        return;
    }
    //RPM_LogAwards();


    ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied(void)
{
    int a;
    int b;

    if (level.numPlayingClients < 2)
    {
        return qfalse;
    }

    if (level.gametypeData->teams)
    {
        return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
    }

    a = level.clients[level.sortedClients[0]].sess.score;
    b = level.clients[level.sortedClients[1]].sess.score;

    return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules(void)
{
    int         i;
    gclient_t* cl;

    // if at the intermission, wait for all non-bots to
    // signal ready, then go to next level
    if (level.intermissiontime || level.changemap)
    {
        CheckIntermissionExit();
        return;
    }

    if (level.intermissionQueued)
    {
        int time = INTERMISSION_DELAY_TIME;
        if (level.time - level.intermissionQueued >= time)
        {
            level.intermissionQueued = 0;
            BeginIntermission();
        }

        return;
    }

    // check for sudden death
    if (g_suddenDeath.integer && ScoreIsTied() && !isCurrentGametypeInList((gameTypes_t[]){GT_HNS, GT_HNZ, GT_MAX})) // Don't check suddenDeath in HNS and HNZ, as there it has no meaning.
    {
        // always wait for sudden death
        return;
    }

    // Check to see if the timelimit was hit
    if (g_timelimit.integer && !level.warmupTime && !level.timelimitHit && !level.hns.cagefight)
    {

        if (level.time - level.startTime >= (g_timelimit.integer + level.timeExtension) * 60000)
        {

            if (isCurrentGametypeInList((gameTypes_t[]){ GT_INF, GT_CSINF, GT_VIP, GT_PROP, GT_ELIM, GT_DEM, GT_HNS, GT_HNZ, GT_MAX })) {
                G_printInfoMessageToAll("Timelimit hit, waiting for round to finish.");
                level.timelimitHit = qtrue;
            }
            else {

                if ((cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) && isCurrentGametype(GT_CTF)) {
                    if (level.teamScores[TEAM_RED] + cm_prevRedTeamScore.integer == level.teamScores[TEAM_BLUE] + cm_prevBlueTeamScore.integer) {
                        if (!level.timelimitMsg) {
                            G_printInfoMessageToAll("Timelimit hit, waiting for the final flag to be captured.");
                            level.timelimitMsg = qtrue;
                        }
                        return;
                    }
                }

                gentity_t* tent;
                tent = G_TempEntity(vec3_origin, EV_GAME_OVER);
                if (cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) {
                    tent->s.eventParm = LEEG;
                }
                else {
                    tent->s.eventParm = GAME_OVER_TIMELIMIT;
                }
                tent->r.svFlags = SVF_BROADCAST;
                notifyPlayersOfTeamScores();

                LogExit("Timelimit hit.");
            }

            return;
        }
    }

    // Check to see if the score was hit
    if ( g_scorelimit.integer )
    {
        if ( level.gametypeData->teams )
        {
            qboolean exitGame = qfalse;
            if ( level.teamScores[TEAM_RED] >= g_scorelimit.integer )
            {
                exitGame = qtrue;
                gentity_t* tent;
                tent = G_TempEntity( vec3_origin, EV_GAME_OVER );

                if (cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) {
                    tent->s.eventParm = LEEG;
                }
                else {
                    tent->s.eventParm = GAME_OVER_SCORELIMIT;
                }

                tent->r.svFlags = SVF_BROADCAST;
                tent->s.otherEntityNum = TEAM_RED;



                LogExit( "Red team hit the score limit." );
                //return;
            }

            if ( level.teamScores[TEAM_BLUE] >= g_scorelimit.integer )
            {
                exitGame = qtrue;
                gentity_t* tent;
                tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
                if (cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) {
                    tent->s.eventParm = LEEG;
                }
                else {
                    tent->s.eventParm = GAME_OVER_SCORELIMIT;
                }
                tent->r.svFlags = SVF_BROADCAST;
                tent->s.otherEntityNum = TEAM_BLUE;

                LogExit( "Blue team hit the score limit." );
                //return;
            }

            if (exitGame) {
                notifyPlayersOfTeamScores();
                return;
            }
        }
        else
        {
            // Check to see if any of the clients scores have crossed the scorelimit
            for ( i = 0 ; i < level.numConnectedClients ; i++ )
            {
                cl = g_entities[level.sortedClients[i]].client;

                if ( cl->pers.connected != CON_CONNECTED )
                {
                    continue;
                }

                if ( cl->sess.team != TEAM_FREE )
                {
                    continue;
                }

                if ( cl->sess.score >= g_scorelimit.integer )
                {
                    gentity_t* tent;
                    tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
                    tent->s.eventParm = GAME_OVER_SCORELIMIT;
                    tent->r.svFlags = SVF_BROADCAST;
                    tent->s.otherEntityNum = level.sortedClients[i];

                    LogExit( "Scorelimit hit." );
                    return;
                }
            }
        }
    }
}

/*
=============
CheckWarmup
=============
*/
void CheckWarmup ( void )
{
    int         counts[TEAM_NUM_TEAMS];
    qboolean    notEnough = qfalse;

    // check because we run 3 game frames before calling Connect and/or ClientBegin
    // for clients on a map_restart
    if ( level.numPlayingClients == 0 )
    {
        return;
    }

    if ( !level.warmupTime  )
    {
        return;
    }

    if ( level.gametypeData->teams )
    {
        counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE, NULL );
        counts[TEAM_RED] = TeamCount( -1, TEAM_RED, NULL );

        if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1)
        {
            notEnough = qtrue;
        }
    }
    else if ( level.numPlayingClients < 2 )
    {
        notEnough = qtrue;
    }

    if ( notEnough )
    {
        if ( level.warmupTime != -1 )
        {
            level.warmupTime = -1;
            trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
            G_LogPrintf( "Warmup:\n" );
        }

        return; // still waiting for team members
    }

    if ( level.warmupTime == 0 )
    {
        return;
    }

    // if the warmup is changed at the console, restart it
    if ( g_warmup.modificationCount != level.warmupModificationCount )
    {
        level.warmupModificationCount = g_warmup.modificationCount;
        level.warmupTime = -1;
    }

    // if all players have arrived, start the countdown
    if ( level.warmupTime < 0 )
    {
        // fudge by -1 to account for extra delays
        level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
        trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
        return;
    }

    // if the warmup time has counted down, restart
    if ( level.time > level.warmupTime )
    {
//      level.warmupTime += 10000;
//      trap_Cvar_Set( "g_restarted", "1" );

        G_ResetGametype ( qtrue, qfalse );
//      trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
//      level.restarted = qtrue;
        return;
    }
}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char *message)
{
    int i;

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if (level.clients[i].sess.team != team)
        {
            continue;
        }

        trap_SendServerCommand( i, message );
    }
}

/*
==================
CheckCvars
==================
*/
void CheckCvars( void )
{
    static int lastMod = -1;

    if ( g_password.modificationCount != lastMod )
    {
        lastMod = g_password.modificationCount;
        if ( *g_password.string && Q_stricmp( g_password.string, "none" ) )
        {
            trap_Cvar_Set( "g_needpass", "1" );
        } else
        {
            trap_Cvar_Set( "g_needpass", "0" );
        }
    }
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent)
{
    float   thinktime;

    thinktime = ent->nextthink;

    if (thinktime <= 0)
    {
        return;
    }
    if (thinktime > level.time)
    {
        return;
    }

    ent->nextthink = 0;
    if (!ent->think)
    {
        Com_Error ( ERR_FATAL, "NULL ent->think");
    }
    ent->think (ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime )
{
    int         i;
    gentity_t   *ent;
    int         msec;

    //NT - store the time the frame started
    level.frameStartTime = trap_Milliseconds();

    // if we are waiting for the level to restart, do nothing
    if ( level.restarted )
    {
        return;
    }

    level.framenum++;
    level.previousTime = level.time;
    level.time = levelTime;
    msec = level.time - level.previousTime;

    if (level.paused)
    {
        ///RxCxW - 08.30.06 - 03:33pm #paused - add to level.startTime so pausing wont take away from timelimit
        level.startTime += msec;
        if (level.gametypeRoundTime)
            level.gametypeRoundTime += msec;
        ///End  - 08.30.06 - 03:34pm
    }

    // get any cvar changes
    G_UpdateCvars();

    if (level.proceedToNextCompState && level.nextCompState < level.time) {
        level.proceedToNextCompState = qfalse;

        // Apply all of the "pre-cached" values.
        if (cm_state.integer == COMPMODE_INITIALIZED || (cm_state.integer == COMPMODE_PRE_ROUND2 && !match_doublerounds.integer)) {
            // Write the match_ vars into the CVAR_ROM vars so they cannot be altered.
            trap_Cvar_Set("cm_bestOf", va("%d", match_bestOf.integer));
            trap_Cvar_Set("cm_scorelimit", va("%d", match_scorelimit.integer));
            trap_Cvar_Set("cm_timelimit", va("%d", match_timelimit.integer));
            trap_Cvar_Set("cm_lockspecs", va("%d", match_lockspecs.integer));
            trap_Cvar_Set("cm_doublerounds", va("%d", match_doublerounds.integer));

            trap_Cvar_Update(&cm_bestOf);
            trap_Cvar_Update(&cm_scorelimit);
            trap_Cvar_Update(&cm_timelimit);
            trap_Cvar_Update(&cm_lockspecs);
            trap_Cvar_Update(&cm_doublerounds);

        }

        trap_Cvar_Set("cm_state", va("%d", cm_state.integer + 1));
        trap_Cvar_Update(&cm_state);

        trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0");
        return;
    }

    if (level.mapAction == MAPACTION_UNPAUSE && level.runMapAction > level.time && level.unpauseNextNotification) {

        if (level.time > level.unpauseNextNotification) {
            
            int timeRemaining = (((level.runMapAction - level.time) + 999) / 1000);
            
            if (timeRemaining > 0) {
                G_Broadcast(BROADCAST_CMD, NULL, qfalse, "\\Unpausing in %d...", timeRemaining);
                G_GlobalSound(G_SoundIndex("sound/misc/events/buzz02.wav"));
                level.unpauseNextNotification = level.time + 1000;

                if (timeRemaining == 1) {
                    // Correct the time ever so slightly to not have "out of synch" sound queue.
                    // There is a small drift every time because we run "rounding" above. So, during the final buzz, we correct the state
                    // so that the final buzz with the GO message doesn't sound like out of place.
                    // As it seemed, the time drift was actually quite horrible, but this solves it from the player's perspective so all's good.
                    level.runMapAction = level.time + 1000;
                    level.unpauseNextNotification = 0;
                }
            }
            
        }

    }

    if (level.mapAction > MAPACTION_NONE && level.runMapAction < level.time && level.runMapAction) {

        level.runMapAction = 0;

        if (level.mapAction == MAPACTION_PENDING_GT || level.mapAction == MAPACTION_PENDING_MAPGTCHANGE) {
            trap_Cvar_Set("g_gametype", G_TranslateGametype(level.mapActionNewGametype));
        }

        if (level.mapAction == MAPACTION_PENDING_MAPCHANGE || level.mapAction == MAPACTION_PENDING_MAPGTCHANGE) {
            trap_SendConsoleCommand(EXEC_APPEND, va("map %s", level.mapActionNewMap));
        }
        else if (level.mapAction == MAPACTION_PENDING_MAPCYCLE) {
            trap_SendConsoleCommand(EXEC_APPEND, "mapcycle");
        }
        else if (level.mapAction == MAPACTION_UNPAUSE) {
            G_Broadcast(BROADCAST_CMD, NULL, qfalse, "GO GO GO!");
            G_GlobalSound(G_SoundIndex("sound/misc/events/buzz02.wav"));
            level.paused = qfalse;
            trap_GT_SendEvent(GTEV_PAUSE, level.time, 0, 0, 0, 0, 0);
        } else if (level.mapAction == MAPACTION_SKIPTOMAP) {
            trap_SkipToMap(level.mapActionSkipTo);
        } else {
            trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0");
        }

        level.mapAction = MAPACTION_NONE;
        return;
    }

    // go through all allocated objects
    ent = &g_entities[0];
    for (i=0 ; i<level.num_entities ; i++, ent++)
    {
        if ( !ent->inuse )
        {
            continue;
        }

        // clear events that are too old
        if ( level.time - ent->eventTime > EVENT_VALID_MSEC )
        {
            if ( ent->s.event )
            {
                ent->s.event = 0;   // &= EV_EVENT_BITS;
                if ( ent->client )
                {
                    ent->client->ps.externalEvent = 0;
                    // predicted events should never be set to zero
                    //ent->client->ps.events[0] = 0;
                    //ent->client->ps.events[1] = 0;
                }
            }

            if ( ent->freeAfterEvent )
            {
                // tempEntities or dropped items completely go away after their event
                G_FreeEntity( ent );
                continue;
            }
            else if ( ent->unlinkAfterEvent )
            {
                // items that will respawn will hide themselves after their pickup event
                ent->unlinkAfterEvent = qfalse;
                trap_UnlinkEntity( ent );
            }
        }
        checkEnts(ent);
        // temporary entities don't think
        if ( ent->freeAfterEvent )
        {
            continue;
        }

        if ( !ent->r.linked && ent->neverFree )
        {
            continue;
        }

        if ( ent->s.eType == ET_MISSILE )
        {
            G_RunMissile( ent );
            continue;
        }

        if ( ent->s.eType == ET_ITEM || ent->physicsObject )
        {
            G_RunItem( ent );
            continue;
        }

        if ( ent->s.eType == ET_MOVER )
        {
            G_RunMover( ent );
            continue;
        }

        if ( i < MAX_CLIENTS )
        {
            G_CheckClientTimeouts ( ent );
            G_RunClient( ent );
            G_CheckClientTeamkill ( ent );
            continue;
        }

        G_RunThink( ent );
    }

    // perform final fixups on the players
    ent = &g_entities[0];
    for (i=0 ; i < level.maxclients ; i++, ent++ )
    {
        
        if (ent && ent->inuse && ent->client) {
            
            if (ent->client->pers.connected == CON_CONNECTED) {

                // Run an anticamp check. The function checks the cvars, gametypes etc by itself.
                checkAnticamp(ent);

                // ROX verification.
                // NB - this is the simplified version - do NOT enable GUID based admins with this.
                // To have GUID based admins, please contact 1fx. # Shoke for information how to set up a secure verification method.

                if (!ent->client->sess.hasRoxAC && ent->client->sess.verifyRoxAC && !ent->client->sess.roxVerificationFailed && level.time > ent->client->sess.nextRoxVerificationMessage) {

                    ent->client->sess.nextRoxVerificationMessage = level.time + 3000;
                    ent->client->sess.roxVerificationAttempts++;

                    if (ent->client->sess.roxVerificationAttempts > 20) {
                        ent->client->sess.roxVerificationFailed = qtrue;
                        G_printCustomMessage(ent, "Anticheat Verification", "Rox Anticheat verification failed.");
                    }
                    else {
                        trap_SendServerCommand(ent - g_entities, "acverify");
                    }

                }


                if (!G_IsClientSpectating(ent->client) && !G_IsClientDead(ent->client)) {
                    
                    
                    if (ent->client->sess.coaster > 0 && level.time > ent->client->sess.nextCoasterTime) {
                        
                        if (ent->client->sess.coaster % COASTERSTATE_SPIN == 0) {
                            ent->client->sess.spinView = qtrue;
                            ent->client->sess.spinViewState = SPINVIEW_FAST;
                            ent->client->sess.lastSpin = level.time + 500;
                        }
                        else if (ent->client->sess.coaster % COASTERSTATE_UPPERCUT) {
                            //ent->client->sess.spinView = qfalse;
                            uppercutPlayer(ent, 0);
                        }
                        else {
                            //ent->client->sess.spinView = qfalse;
                            runoverPlayer(ent);
                        }

                        ent->client->sess.coaster--;
                        if (ent->client->sess.coaster == 0) {
                            ent->client->sess.nextCoasterTime = 0;
                            ent->client->sess.spinView = qfalse;
                            ent->client->sess.spinViewState = SPINVIEW_NONE;
                        }
                        else if (ent->client->sess.spinView) {
                            ent->client->sess.nextCoasterTime += 50;
                        } else {
                            ent->client->sess.nextCoasterTime += 500;
                        }
                        
                    }


                    if (ent->client->sess.spinView) {
                        spinView(ent);

                        if (level.time > ent->client->sess.lastSpin) {
                            ent->client->sess.spinView = qfalse;
                            ent->client->sess.spinViewState = SPINVIEW_NONE;
                        }

                    }

                }

                if (G_IsClientDead(ent->client) && ent->client->ps.stats[STAT_FROZEN] && isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_MAX })) {
                    ent->client->ps.stats[STAT_FROZEN] = 0;
                }

                if (isCurrentGametype(GT_HNS)) {

                    

                    if (!level.hns.cagefight && !level.hns.cagefightdone && ent->client->sess.team != TEAM_SPECTATOR && G_IsClientDead(ent->client)) {

                        if (ent->client->sess.team == TEAM_BLUE || (level.time < (level.gametypeStartTime + hideSeek_roundstartdelay.integer * 1000) && ent->client->sess.team == TEAM_RED)) {

                            if (ent->client->sess.ghost) {
                                // Clean up any following monkey business
                                G_StopFollowing(ent);

                                // Disable being a ghost
                                ent->client->ps.pm_flags &= ~PMF_GHOST;
                                ent->client->ps.pm_type = PM_NORMAL;
                                ent->client->sess.ghost = qfalse;
                            }

                            ent->client->sess.noTeamChange = qfalse;

                            trap_UnlinkEntity(ent);
                            ClientSpawn(ent);

                        }

                    }

                }
                else if (isCurrentGametype(GT_HNZ)) {
                    if (ent->client->pers.hnz.zombieBody >= MAX_CLIENTS && ent->client->sess.team == TEAM_RED) {

                        gentity_t* zombieBody = &g_entities[ent->client->pers.hnz.zombieBody];

                        if (!zombieBody || zombieBody->s.pos.trType == TR_STATIONARY || level.time > zombieBody->timestamp + 5000) {

                            SetTeam(ent, "blue", NULL, qtrue);
                            G_StopFollowing(ent);
                            ent->client->ps.pm_flags &= ~PMF_GHOST;
                            ent->client->ps.pm_type = PM_NORMAL;
                            ent->client->sess.ghost = qfalse;
                            trap_UnlinkEntity(ent);
                            ClientSpawn(ent);

                            // Boe!Man 11/16/15: Only teleport if their new location was stationary.
                            if (zombieBody->s.pos.trType == TR_STATIONARY) {
                                TeleportPlayer(ent, zombieBody->r.currentOrigin, ent->client->pers.hnz.spawnAngles, qtrue);
                                SetClientViewAngle(ent, ent->client->pers.hnz.spawnAngles);
                            }

                            zombieBody->nextthink = level.time + 1000;
                            zombieBody->think = G_FreeEntity;
                            ent->client->pers.hnz.zombieBody = -1;

                        }



                    }

                    int teamCountBlue = TeamCount(-1, TEAM_BLUE, NULL);

                    if (G_IsClientDead(ent->client)) {
                        if (ent->client->sess.team == TEAM_BLUE || (teamCountBlue == 0 && ent->client->sess.team == TEAM_RED)) {
                            if (ent->client->sess.ghost) {
                                // Clean up any following monkey business
                                G_StopFollowing(ent);

                                // Disable being a ghost
                                ent->client->ps.pm_flags &= ~PMF_GHOST;
                                ent->client->ps.pm_type = PM_NORMAL;
                                ent->client->sess.ghost = qfalse;
                            }

                            ent->client->sess.noTeamChange = qtrue;

                            trap_UnlinkEntity(ent);
                            ClientSpawn(ent);
                        }
                    }
                    else if (ent->client->pers.hnz.healthRegen && ent->client->sess.team == TEAM_BLUE && level.time >= ent->client->pers.hnz.healthRegen) {
                        ent->client->ps.stats[STAT_HEALTH]++;
                        ent->health++;

                        if (ent->client->ps.stats[STAT_HEALTH] >= MAX_HEALTH) {
                            ent->client->pers.hnz.healthRegen = 0;
                        }
                        else {
                            ent->client->pers.hnz.healthRegen = level.time + 50 + (150 / level.numPlayingClients * teamCountBlue);
                        }
                    }

                }
                else if (isCurrentGametype(GT_PROP)) {

                    if (G_IsClientDead(ent->client) && level.customGameStarted) {
                        freeProphuntProps(ent);

                        if (ent->client->sess.team == TEAM_RED) {
                            SetTeam(ent, "blue", NULL, qtrue);
                        }

                        G_StopFollowing(ent);
                        ent->client->ps.pm_flags &= ~PMF_GHOST;
                        ent->client->ps.pm_type = PM_NORMAL;
                        ent->client->sess.ghost = qfalse;
                        trap_UnlinkEntity(ent);
                        ClientSpawn(ent);
                    }

                }

            }

        }


        if (ent->inuse)
        {
            ClientEndFrame(ent);
        }
    }

    if (level.legacyMod == CL_RPM) {
        // Henk 06/04/10 -> Update tmi every x sec
        RPM_UpdateTMI();
    }

    if (level.goldMod == CL_ROCMOD && level.time > level.lastETIUpdate) {
        ROCmod_sendExtraTeamInfo(NULL);

        level.lastETIUpdate = level.time + 1000;
    }

    if (isCurrentGametype(GT_HNS)) {
        hnsRunFrame();
    }
    else if (isCurrentGametype(GT_HNZ)) {
        hnzRunFrame();
    }
    else if (isCurrentGametype(GT_PROP)) {
        propRunFrame();
    }

    vote_runFrame();

    // Check warmup rules
    CheckWarmup();
 
    // see if it is time to end the level
    CheckExitRules();

    // Update gametype stuff
    CheckGametype ();

    trap_GT_RunFrame ( level.time );

    // for tracking changes
    CheckCvars();

    if (level.nextSQLBackupTime <= level.time) {
        backupInMemoryDatabases();
    }

    if (g_useCountryAPI.integer) {

        int threadAction, threadPlayerId;
        char threadMsg[MAX_THREAD_OUTPUT];

        int threadResponse = dequeueInbound(&threadAction, &threadPlayerId, threadMsg, sizeof(threadMsg));

        if (threadResponse == THREADRESPONSE_SUCCESS) {

            int blockLevel = 0;
            gentity_t* tent = &g_entities[threadPlayerId];

            if (tent && tent->client) {

                switch (threadAction) {
                case THREADACTION_IPHUB_DATA_RESPONSE:

                    Q_strncpyz(tent->client->sess.countryCode, Info_ValueForKey(threadMsg, "countryCode"), sizeof(tent->client->sess.countryCode));
                    Q_strncpyz(tent->client->sess.country, Info_ValueForKey(threadMsg, "countryName"), sizeof(tent->client->sess.country));
                    blockLevel = atoi(Info_ValueForKey(threadMsg, "blockLevel"));
                    dbAddCountry(tent->client->pers.ip, tent->client->sess.countryCode, tent->client->sess.country, blockLevel);

                    if (g_vpnAutoKick.integer && blockLevel == IPHUBBLOCK_VPN) {
                        trap_DropClient(threadPlayerId, "VPN Detected");
                    }

                    break;

                case THREADACTION_RUN_PRINTF:
                    Com_Printf(threadMsg);
                    break;
                }
            }
        }
    }

    if (level.autoSwapTime && level.autoSwapTime < level.time) {
        // Swap the teams.
        swapTeams(qtrue);
        level.autoSwapTime = 0;
    }

    if (level.nextCmInfoDisplay < level.time) {

        if (cm_state.integer == COMPMODE_INITIALIZED) {
            G_Broadcast(BROADCAST_GAME, NULL, qfalse, "^7[^3Competition mode^7]\n"
            "\n"
            "[^3Scorelimit^7]: %d\n"
            "[^3Timelimit^7]: %d\n"
            "[^3Lock specs^7]: %s\n"
            "[^3Rounds^7]: %s\n"
            "[^3Best-of logic^7]: %s\n"
            "[^3Follow enemy^7]: %s\n"
            "\n"
            "To start the match, do !mr"
            ,
                match_scorelimit.integer,
                match_timelimit.integer,
                match_lockspecs.integer ? "Yes" : "No",
                match_doublerounds.integer ? "Two rounds" : "One round",
                match_bestOf.integer ? "Yes" : "No",
                match_followEnemy.integer ? "Yes" : "No"
            );

            level.nextCmInfoDisplay = level.time + 3000;
        }
        else if (cm_state.integer == COMPMODE_PRE_ROUND2) {

            G_Broadcast(BROADCAST_GAME, NULL, qfalse, "^7[^3Competition mode^7]\n"
                "\n"
                "Previous round was won by %s\n"
                "^7Scores: %d - %d\n\n"
                "To start the second round, do !mr"
                ,
                cm_prevRedTeamScore.integer > cm_prevBlueTeamScore.integer ? g_customRedName.string : g_customBlueName.string,
                cm_prevRedTeamScore.integer > cm_prevBlueTeamScore.integer ? cm_prevRedTeamScore.integer : cm_prevBlueTeamScore.integer,
                cm_prevRedTeamScore.integer > cm_prevBlueTeamScore.integer ? cm_prevBlueTeamScore.integer : cm_prevRedTeamScore.integer
            );

            level.nextCmInfoDisplay = level.time + 3000;
        }

    }

    if (g_listEntity.integer)
    {
        for (i = 0; i < MAX_GENTITIES; i++)
        {
            Com_Printf("%4i: %s\n", i, g_entities[i].classname);
        }
        trap_Cvar_Set("g_listEntity", "0");
    }


    if (level.intermissiontime && level.intermissiontime + 5000 < level.time && 0) {

        if (cm_state.integer) {

            if (cm_state.integer == COMPMODE_ROUND1) {
                // Blue score is intentionally set as red team score - after swap, they'll be in red team.
                trap_Cvar_Set("cm_prevRedTeamScore", va("%d", level.teamScores[TEAM_BLUE]));
                trap_Cvar_Set("cm_prevBlueTeamScore", va("%d", level.teamScores[TEAM_RED]));
                trap_Cvar_Update(&cm_prevRedTeamScore);
                trap_Cvar_Update(&cm_prevBlueTeamScore);
            }

            trap_Cvar_Set("cm_state", va("%d", cm_state.integer + 1));
            trap_Cvar_Update(&cm_state);
            trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0");
        }
        else {
            // Try to figure out if we're in a mapcycle game. If not, restart the map.

            if (!Q_stricmp(g_mapcycle.string, "none") || strlen(g_mapcycle.string) == 0) {
                trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0");
                
            }
            else {
                trap_SendConsoleCommand(EXEC_APPEND, "mapcycle");
            }

        }

    }

}

void G_InitGhoul ( void )
{
    G_InitHitModels ( );
}

void G_ShutdownGhoul ( void )
{
    if ( !level.serverGhoul2 )
    {
        return;
    }

    trap_G2API_RemoveGhoul2Model ( &level.serverGhoul2 );
    level.serverGhoul2 = NULL;
}


/*
===============
Boe_setTrackedCvar
===============
*/
void G_setTrackedCvarWithoutTrackMessage(vmCvar_t* cvar, int value) {
    cvarTable_t* cv = gameCvarTable;
    int i;
    qboolean found = qfalse;

    for (i = 0; i < gameCvarTableSize; i++) {
        if (cv[i].vmCvar == cvar) {
            found = qtrue;
            break;
        }
    }

    if (found) {
        cv[i].trackChange = qfalse;
        trap_Cvar_Set(cv[i].cvarName, va("%d", value));
        G_UpdateCvars();
        cv[i].trackChange = qtrue;
    }

}
