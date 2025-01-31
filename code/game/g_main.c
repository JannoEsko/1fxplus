// Copyright (C) 2001-2002 Raven Software.
//

#include "g_local.h"
#include "1fx/1fxFunctions.h"
#include "../ext/rocmod/rocmod.h"
#include "1fx/threadedFunctions.h"

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

vmCvar_t    g_realGametype;
vmCvar_t    g_logToDatabase;

vmCvar_t    g_adminList;
vmCvar_t    g_badmin;
vmCvar_t    g_admin;
vmCvar_t    g_sadmin;
vmCvar_t    g_maxBanDuration;
vmCvar_t    g_maxAliases;

vmCvar_t    g_rconChatPrefix;
vmCvar_t    g_badminChatPrefix;
vmCvar_t    g_adminChatPrefix;
vmCvar_t    g_sadminChatPrefix;
vmCvar_t    g_serverColors;
vmCvar_t    g_sl;
vmCvar_t    g_tl;
vmCvar_t    g_swapteams;
vmCvar_t    g_forceteam;
vmCvar_t    g_ban;
vmCvar_t    g_broadcast;
vmCvar_t    g_subnetban;
vmCvar_t    g_pop;
vmCvar_t    g_uppercut;
vmCvar_t    g_kick;
vmCvar_t    g_lock;
vmCvar_t    g_respawn;
vmCvar_t    g_plant;

vmCvar_t    g_redTeamPrefix;
vmCvar_t    g_blueTeamPrefix;
vmCvar_t    g_spectatorTeamPrefix;

vmCvar_t    g_ri;
vmCvar_t    g_rtl;
vmCvar_t    g_runover;
vmCvar_t    g_rollercoaster;
vmCvar_t    g_mapswitch;
vmCvar_t    g_strip;
vmCvar_t    g_shuffleteams;
vmCvar_t    g_gr;
vmCvar_t    g_eventeams;
vmCvar_t    g_ff;
vmCvar_t    g_rename;
vmCvar_t    g_burn;
vmCvar_t    g_clientHandlesDeathMessages;
vmCvar_t    g_timeextension;
vmCvar_t    g_useCountryAPI;
vmCvar_t    g_iphubAPIKey;
vmCvar_t    g_ipcacheAgeing;
vmCvar_t    g_dontAllowVPN;
vmCvar_t    g_useThreads;
vmCvar_t    g_motd1;
vmCvar_t    g_motd2;
vmCvar_t    g_motd3;
vmCvar_t    g_motd4;
vmCvar_t    g_motd5;
vmCvar_t    g_customWeaponFile;
vmCvar_t    g_enforce1fxAdditions;
vmCvar_t    g_instaGib;
vmCvar_t    g_damage;
vmCvar_t    g_inviewFile;
vmCvar_t    g_nosection;
vmCvar_t    g_compCurrentRound;
vmCvar_t    g_compModeState;
vmCvar_t    g_cm;
vmCvar_t    g_loadEntityFromFile;
vmCvar_t    g_entityFile;
vmCvar_t    g_altmap;

// SQLite3 tables.
sqlite3* gameDb; // will hold anything related to the game itself: admins, bans, aliases and so on.
sqlite3* logDb; // will hold all the log tables (RCON, admin, game etc)
char sqlTempName[16];

static cvarTable_t gameCvarTable[] =
{
    // don't override the cheat state set by the system
    { &g_cheats, "sv_cheats", "", 0, 0.0, 0.0, 0, qfalse },

    // noset vars
    { NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "gamedate", __DATE__ , CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { &g_restarted, "g_restarted", "0", CVAR_ROM, 0.0, 0.0, 0, qfalse  },
    { NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },

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
    { &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE, 0.0, 0.0,   },

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
    { &g_roundtimelimit,    "g_roundtimelimit", "5",        CVAR_ARCHIVE, 1.0, 0.0, 0, qfalse },
    { &g_roundjointime,     "g_roundjointime",  "5",        CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_timeextension,     "g_timeextension",  "15",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    { &g_roundstartdelay,   "g_roundstartdelay", "5",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

    // 1.03 CHANGE - Rename availableWeapons Cvar which might confuse old map cycles
    { &g_availableWeapons,  "g_available", "0", CVAR_SERVERINFO|CVAR_ROM|CVAR_LATCH, 0.0, 0.0, 0, qfalse },

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
    { &g_realGametype,          "g_realGametype",           "dm",       CVAR_ROM | CVAR_LATCH,  0.0f,   0.0f,   0,  qfalse },
    { &g_logToDatabase,         "g_logToDatabase",          "1",        CVAR_ARCHIVE | CVAR_LATCH,  0.0f,   0.0f,   0, qfalse },
    { &g_adminList,             "g_adminList",              "3",        CVAR_ARCHIVE,               0.0f,   0.0f,   0, qfalse },
    { &g_badmin,             "g_badmin",              "3",        CVAR_ARCHIVE,               0.0f,   0.0f,   0, qfalse },
    { &g_admin,             "g_admin",              "3",        CVAR_ARCHIVE,               0.0f,   0.0f,   0, qfalse },
    { &g_sadmin,             "g_sadmin",              "4",        CVAR_ARCHIVE,               0.0f,   0.0f,   0, qfalse },
    { &g_maxBanDuration,           "g_maxBanDuration",  "365",      CVAR_ARCHIVE,               0.0f,   0.0f,   0, qfalse },
    { &g_maxAliases,            "g_maxAliases",                 "10",                   CVAR_ARCHIVE,                       0.0f,   0.0f,   0,  qfalse},
    { &g_rconChatPrefix,        "g_rconChatPrefix",                 "^CS^be^kr^+v^7er",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_badminChatPrefix,        "g_badminChatPrefix",                 "^GB-^gA^Kdm^7in",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_adminChatPrefix,        "g_adminChatPrefix",                 "^GA^gd^Km^7in",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_sadminChatPrefix,        "g_sadminChatPrefix",                 "^GS-^gA^Kdm^7in",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_serverColors,        "g_serverColors",                 "GgKk+7",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_sl,        "g_sl",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_tl,        "g_tl",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_swapteams,        "g_swapTeams",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_forceteam,        "g_forceTeam",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_ban,        "g_ban",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_broadcast,        "g_broadcast",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_subnetban,        "g_subnetban",                 "3",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_pop,        "g_pop",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_uppercut,        "g_uppercut",                 "3",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_kick,        "g_kick",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_lock,        "g_lock",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_respawn,        "g_respawn",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_plant,        "g_plant",                 "3",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_blueTeamPrefix,        "g_blueTeamPrefix",                 "^yB^Il^fu^+e",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_redTeamPrefix,        "g_redTeamPrefix",                 "^$R^Te^Hd",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_spectatorTeamPrefix,        "g_spectatorTeamPrefix",                 "^CS^bp^ke^+c^7tators",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_ri,        "g_ri",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_rtl,        "g_rtl",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_runover,        "g_runover",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_rollercoaster,        "g_rollercoaster",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_mapswitch,        "g_mapswitch",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_strip,        "g_strip",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_shuffleteams,        "g_shuffleteams",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_gr,        "g_gr",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_eventeams,        "g_eventeams",                 "1",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_ff,        "g_ff",                 "3",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_rename,        "g_rename",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_burn,        "g_burn",                 "2",       CVAR_ARCHIVE,   0.0f,   0.0f,   0 ,  qfalse },
    { &g_timeextension,     "g_timeextension",  "15",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_useCountryAPI,     "g_useCountryAPI",  "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_iphubAPIKey,     "g_iphubAPIKey",  "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_ipcacheAgeing,     "g_ipcacheAgeing",      "30",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_dontAllowVPN,     "g_dontAllowVPN",      "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_useThreads,     "g_useThreads",      "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_motd1,     "g_motd1",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_motd2,     "g_motd2",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_motd3,     "g_motd3",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_motd4,     "g_motd4",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_motd5,     "g_motd5",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_customWeaponFile,     "g_customWeaponFile",      "",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_enforce1fxAdditions,     "g_enforce1fxAdditions",      "0",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_instaGib,     "g_instaGib",      "0",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_damage,     "g_damage",      "2",       CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
    { &g_inviewFile,     "g_inviewFile",      "",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_nosection,     "g_nosection",      "2",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },
    { &g_compCurrentRound,     "g_compCurrentRound",      "2",       CVAR_ROM, 0.0, 0.0, 0, qfalse },
    { &g_compModeState,     "g_compModeState",      "2",       CVAR_ROM, 0.0, 0.0, 0, qfalse },
    { &g_cm,     "g_cm",      "2",       CVAR_ARCHIVE | CVAR_LATCH, 0.0, 0.0, 0, qfalse },

    { &g_loadEntityFromFile,     "g_loadEntityFromFile",      "0",       CVAR_ROM, 0.0, 0.0, 0, qfalse },

    { &g_entityFile,     "g_entityFile",      "",       CVAR_ROM, 0.0, 0.0, 0, qfalse },
    { &g_altmap,     "g_altmap",      "0",       CVAR_ROM, 0.0, 1.0, 0, qfalse },


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
            ClientBegin( arg0 );
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
            //Com_Printf("Ip: %s\nArg1: %s\n", arg0, arg1);
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
                    //trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
                    G_printInfoMessageToAll("%s changed to %s", cv->cvarName, cv->vmCvar->string);
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
    trap_Cvar_Update ( &g_availableWeapons );
}

qboolean G_IsGametypeSupported(char* input) {
    // only add the DLL/SO's being built over here.
    return
        Q_stricmp(input ? input : g_gametype.string, "h&s") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "h&z") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "hns") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "hnz") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "inf") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "dm") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "elim") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "dem") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "tdm") == 0 ? qtrue : qfalse ||
        Q_stricmp(input ? input : g_gametype.string, "ctf") == 0 ? qtrue : qfalse

        ;

}

void G_SetRealGametype() {

    // check whether the g_gametype is a supported gametype.
    if (!G_IsGametypeSupported(NULL)) {
        Com_Printf("Gametype %s is not supported by 1fxplus. Gametype defaulted to deathmatch.\n", g_gametype.string);
        trap_Cvar_Set("g_gametype", "dm");
        trap_Cvar_Update(&g_gametype);

    }

    if (!Q_stricmp(g_gametype.string, "inf")) {
        trap_Cvar_Set("g_gametype", "inf");
        trap_Cvar_Set("g_realGametype", "inf");
        trap_Cvar_Update(&g_gametype);
    } else if (!Q_stricmp(g_gametype.string, "h&s") || !Q_stricmp(g_gametype.string, "hns")) {
        trap_Cvar_Set("g_gametype", "inf");
        trap_Cvar_Set("g_realGametype", "hns");
        trap_Cvar_Update(&g_gametype);
        // Boe!Man 10/4/12: Reset g_gametype to set the gt latched, so it will remain effective upon the next /rcon map switch..
        trap_SendConsoleCommand(EXEC_APPEND, "g_gametype hns\n");
    } else if (!Q_stricmp(g_gametype.string, "h&z") || !Q_stricmp(g_gametype.string, "hnz")) {
        trap_Cvar_Set("g_gametype", "inf");
        trap_Cvar_Set("g_realGametype", "hnz");
        trap_Cvar_Update(&g_gametype);
        // Boe!Man 10/4/12: Reset g_gametype to set the gt latched, so it will remain effective upon the next /rcon map switch..
        trap_SendConsoleCommand(EXEC_APPEND, "g_gametype hnz\n");
    } else {
        trap_Cvar_Set("g_realGametype", Q_strlwr(g_gametype.string));
    }

    trap_Cvar_Update(&g_realGametype);
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
        trap_Cvar_Set( "g_realGametype", gametype );
        trap_Cvar_Set( "RMG_mission", gametype );
        level.gametype = BG_FindGametype ( gametype );

        trap_Cvar_Update( &g_gametype );
        G_SetRealGametype();
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
    Com_Printf("Mod: %s\nMod version: %s\n", MOD_NAME, PRODUCT_VERSION);

    srand( randomSeed );

    // set some level globals
    memset( &level, 0, sizeof( level ) );
    level.time = levelTime;
    level.startTime = levelTime;

    G_RegisterCvars();

    // Initialize the game memory system.
    G_InitMemory();

    // Load the list of arenas
    G_LoadArenas ( );

    // Build the gametype list so we can verify the given gametype
    BG_BuildGametypeList ( );

    // Set the current gametype
    G_SetRealGametype();
    G_SetGametype(g_gametype.string);

    // Sets the available weapons cvar from the disable_ cvars.
    G_UpdateAvailableWeapons ( );

    // Set the available outfitting
    BG_SetAvailableOutfitting ( g_availableWeapons.string );

    // Give the game a uniqe id
    trap_SetConfigstring ( CS_GAME_ID, va("%d", randomSeed ) );
    char    serverinfo[MAX_INFO_STRING];
    trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

    Q_strncpyz(level.mapname, Info_ValueForKey(serverinfo, "mapname"), sizeof(level.mapname));

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
            

            G_LogPrintf("------------------------------------------------------------\n" );
            G_LogPrintf("InitGame: %s\n", serverinfo );
        }
    }
    else
    {
        Com_Printf( "Not logging to disk.\n" );
    }

    G_InitWorldSession();

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

    // parse the key/value pairs and spawn gentities
    G_SpawnEntitiesFromString(qfalse);

    // Now parse the gametype information that we need.  This needs to be
    // done after the entity spawn so that the items and triggers can be
    // linked up properly
    G_ParseGametypeFile ( );


    // Boe!Man 7/27/15: Initialize weapon and ammo globals.
    level.grenadeMin = 17;

    if (g_enforce1fxAdditions.integer) {
        level.wpNumWeapons = 25;
        level.ammoMax = 18;
        level.grenadeMax = 24;
    }else{
        level.wpNumWeapons = 21;
        level.ammoMax = 14;
        level.grenadeMax = 20;
    }
    BG_InitializeWeaponsAndAmmo();
    BG_ParseInviewFile( level.pickupsDisabled );

    if (strlen(g_customWeaponFile.string) > 0) {
        weaponMod(WEAPONMOD_CUSTOM, qtrue);
    } else {
        weaponMod(WEAPONMOD_GT, qtrue); // this is actually redundant because WEAPONMOD_CUSTOM has a fallback, but in case that changes...
    }

    // Load in the identities
    BG_ParseNPCFiles ( );

    // general initialization
    G_FindTeams();

    SaveRegisteredItems();

    // check databases.
    Com_Printf ("Loading databases...\n");
    loadDatabases();
    dbClearOutdatedBans(qtrue);

    Com_Printf ("-----------------------------------\n");

    if( trap_Cvar_VariableIntegerValue( "com_buildScript" ) )
    {
        G_SoundIndex( "sound/player/gurp1.wav", qtrue );
        G_SoundIndex( "sound/player/gurp2.wav", qtrue );
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

    // set the client mod variable in level struct.
    char clientMod[MAX_CVAR_VALUE_STRING];
    level.clientMod = CL_NONE;

    trap_Cvar_VariableStringBuffer("sv_clientmod", clientMod, sizeof(clientMod));

    if (!Q_stricmp(clientMod, "rocmod")) {
        level.clientMod = CL_ROCMOD;
        trap_Cvar_Register(NULL, "sv_modVersion", "| ^71fx^1plus 2.1c", CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
        trap_Cvar_Register(NULL, "g_allowCustomTeams", "1", CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
        trap_Cvar_Register(NULL, "g_customRedName", va("%s^7 Team", getTeamPrefixByGametype(TEAM_RED)), CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);
        trap_Cvar_Register(NULL, "g_customBlueName", va("%s^7 Team", getTeamPrefixByGametype(TEAM_BLUE)), CVAR_SYSTEMINFO | CVAR_ROM, 0.0, 0.0);

        // Automatic demo recording in ROCmod.
        trap_Cvar_Register(NULL, "g_autoMatchDemo", "1", CVAR_ARCHIVE, 0.0, 0.0);
        trap_Cvar_Register(NULL, "inMatch", "0", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_TEMP, 0.0, 0.0);

        // Client death messages are handled by client.
        g_clientHandlesDeathMessages.integer = 1;
    }
    else {
        g_clientHandlesDeathMessages.integer = 0;
    }

    G_AllocateStatsMemory(NULL);

    // Initialize the gametype
    // as we're now able to load gametype DLL's, separate h&s/h&z/....? from other gametypes,
    // load the gametype from realGametype string.
    trap_GT_Init ( g_realGametype.string, restart );

    // Boe!Man 1/30/14: If we're not in a game that has rounds and the user wishes to have automatic backups (enabled by default), do this every 5 minutes.
    if(level.gametypeData->respawnType == RT_INTERVAL){
        level.sqlBackupTime = level.time + 50000;
    }

    if (g_useThreads.integer) {
        startThread();
        Com_Printf("Initializing threads...");
    }

    level.actionSoundIndex = G_SoundIndex("sound/misc/menus/click.wav", qtrue);

    // Music
    if ( RMG.integer )
    {
        char temp[MAX_INFO_STRING];

        // start the music
        trap_Cvar_VariableStringBuffer("RMG_music", temp, MAX_QPATH);
        trap_SetConfigstring( CS_MUSIC, temp );
    }

    trap_SetConfigstring( CS_VOTE_TIME, "" );
}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
    Com_Printf ("==== ShutdownGame ====\n");

    if ( level.logFile )
    {
        G_LogPrintf("ShutdownGame:\n" );
        G_LogPrintf("------------------------------------------------------------\n" );
        trap_FS_FCloseFile( level.logFile );
    }
    // write all the client session data so we can get it back
    G_WriteSessionData();
    Com_Printf("Unloading and backing up in-memory databases.\n");

    unloadInMemoryDatabases();
    // Boe!Man 7/27/15: Free statinfo memory of all clients.
    G_FreeStatsMemory(NULL);
    

    if (g_useThreads.integer) {
        closeThread();
    }

#ifdef _SOF2_BOTS
    if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) )
    {
        BotAIShutdown( restart );
    }
#endif

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

void QDECL Com_Printf( const char *msg, ... ) {
    va_list     argptr;
    char        text[1024];

    va_start (argptr, msg);
    Q_vsnprintf (text, sizeof(text), msg, argptr);
    va_end (argptr);

    trap_Print( text );
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
    level.voteTime = 0;
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

    if (level.clientMod == CL_ROCMOD) {
        // Send the Awards to the players.
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
    int         ready, notReady;
    int         i;
    gclient_t   *cl;
    int         readyMask;

    // see which players are ready
    ready = 0;
    notReady = 0;
    readyMask = 0;
    for (i=0 ; i< g_maxclients.integer ; i++) {
        cl = level.clients + i;
        if ( cl->pers.connected != CON_CONNECTED ) {
            continue;
        }
        if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
            continue;
        }

        if ( cl->readyToExit ) {
            ready++;
            if ( i < 16 ) {
                readyMask |= 1 << i;
            }
        } else {
            notReady++;
        }
    }

    // copy the readyMask to each player's stats so
    // it can be displayed on the scoreboard
    for (i=0 ; i< g_maxclients.integer ; i++) {
        cl = level.clients + i;
        if ( cl->pers.connected != CON_CONNECTED ) {
            continue;
        }
        cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
    }

    // never exit in less than five seconds
    if ( level.time < level.intermissiontime + 5000 ) {
        return;
    }

    // if nobody wants to go, clear timer
    if ( !ready ) {
        level.readyToExit = qfalse;
        return;
    }

    // if everyone wants to go, go now
    if ( !notReady ) {
        ExitLevel();
        return;
    }

    // the first person to ready starts the ten second timeout
    if ( !level.readyToExit ) {
        level.readyToExit = qtrue;
        level.exitTime = level.time;
    }

    // if we have waited ten seconds since at least one player
    // wanted to exit, go ahead
    if ( level.time < level.exitTime + 10000 ) {
        return;
    }

    ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void )
{
    int a;
    int b;

    if ( level.numPlayingClients < 2 )
    {
        return qfalse;
    }

    if ( level.gametypeData->teams )
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
void CheckExitRules( void )
{
    int         i;
    gclient_t   *cl;

    // if at the intermission, wait for all non-bots to
    // signal ready, then go to next level
    if ( level.intermissiontime )
    {
        CheckIntermissionExit ();
        return;
    }

    if ( level.intermissionQueued )
    {
        int time = INTERMISSION_DELAY_TIME;
        if ( level.time - level.intermissionQueued >= time )
        {
            level.intermissionQueued = 0;
            BeginIntermission();
        }

        return;
    }

    // check for sudden death
    if ( g_suddenDeath.integer && ScoreIsTied() )
    {
        // always wait for sudden death
        return;
    }

    // Check to see if the timelimit was hit
    if ( g_timelimit.integer && !level.warmupTime )
    {
        if ( level.gametypeData->respawnType != RT_NONE || level.gametypeResetTime )
        {
            if ( level.time - level.startTime >= (g_timelimit.integer + level.timeExtension)*60000 )
            {
                gentity_t* tent;
                tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
                tent->s.eventParm = GAME_OVER_TIMELIMIT;
                tent->r.svFlags = SVF_BROADCAST;

                LogExit( "Timelimit hit." );
                return;
            }
        }
    }

    // Check to see if the score was hit
    if ( g_scorelimit.integer )
    {
        if ( level.gametypeData->teams )
        {
            if ( level.teamScores[TEAM_RED] >= g_scorelimit.integer )
            {
                gentity_t* tent;
                tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
                tent->s.eventParm = GAME_OVER_SCORELIMIT;
                tent->r.svFlags = SVF_BROADCAST;
                tent->s.otherEntityNum = TEAM_RED;

                LogExit( "Red team hit the score limit." );
                return;
            }

            if ( level.teamScores[TEAM_BLUE] >= g_scorelimit.integer )
            {
                gentity_t* tent;
                tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
                tent->s.eventParm = GAME_OVER_SCORELIMIT;
                tent->r.svFlags = SVF_BROADCAST;
                tent->s.otherEntityNum = TEAM_BLUE;

                LogExit( "Blue team hit the score limit." );
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

        G_ResetGametype ( qtrue );
//      trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
//      level.restarted = qtrue;
        return;
    }
}

/*
==================
CheckVote
==================
*/
void CheckVote( void )
{
    if ( level.voteExecuteTime && level.voteExecuteTime < level.time )
    {
        level.voteExecuteTime = 0;
        trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
    }

    if ( !level.voteTime )
    {
        return;
    }

    // Update the needed clients
    trap_SetConfigstring ( CS_VOTE_NEEDED, va("%i", (level.numVotingClients / 2) + 1 ) );

    if ( level.time - level.voteTime >= g_voteDuration.integer*1000 )
    {
        trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
        level.clients[level.voteClient].voteDelayTime = level.time + g_failedVoteDelay.integer * 60000;
    }
    else
    {
        if ( level.voteYes > level.numVotingClients/2 )
        {
            // execute the command, then remove the vote
            trap_SendServerCommand( -1, "print \"Vote passed.\n\"" );
            level.voteExecuteTime = level.time + 3000;
        }
        else if ( level.voteNo >= level.numVotingClients/2 )
        {
            // same behavior as a timeout
            trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
            level.clients[level.voteClient].voteDelayTime = level.time + g_failedVoteDelay.integer * 60000;
        }
        else
        {
            // still waiting for a majority
            return;
        }
    }

    level.voteTime = 0;
    trap_SetConfigstring( CS_VOTE_TIME, "" );
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

    // get any cvar changes
    G_UpdateCvars();

    if (level.clientMod == CL_ROCMOD && level.time > level.lastETIupdate) {
        ROCmod_sendExtraTeamInfo(NULL);

        level.lastETIupdate = level.time + 1000;
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
        if ( ent->inuse )
        {
            ClientEndFrame( ent );
        }
    }

    // Boe!Man 5/27/13: The automatic in-memory to disk backup. Note this only happens when playing a game which doesn't have a round limit.
    if(level.sqlBackupTime && level.time >= level.sqlBackupTime){
        backupInMemoryDatabases("game.db", gameDb);
        // Do this again in the next 5 minutes.
        level.sqlBackupTime = level.time + 50000;
    }

    if (level.mapState != MAPSTATE_NOTHING && level.time >= level.mapStateTimer) {
        runMapStateEvents();
    }

    // Check warmup rules
    CheckWarmup();

    // see if it is time to end the level
    CheckExitRules();

    // Update gametype stuff
    CheckGametype ();

    trap_GT_RunFrame ( level.time );

    // cancel vote if timed out
    CheckVote();

    // for tracking changes
    CheckCvars();

    if (g_listEntity.integer)
    {
        for (i = 0; i < MAX_GENTITIES; i++)
        {
            Com_Printf("%4i: %s\n", i, g_entities[i].classname);
        }
        trap_Cvar_Set("g_listEntity", "0");
    }

    // check for any activities from the thread.
    checkThreadInboundMessages();

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
