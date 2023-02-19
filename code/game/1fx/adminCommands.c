
#include "../g_local.h"
#include "1fxFunctions.h"

// Admin commands taken from 1fx. Mod
// commented out as long as they are not functional.

int minimumAdmLevel = (int)LEVEL_BADMIN;

admCmd_t adminCommands[] =
{

    {"!adlr",    "adminlistremove",      &minimumAdmLevel,           &admRemoveAdminByRowId,               "Remove an Admin from the adminlist",    "<line #>",         NULL},
    {"!ra",     "removeadmin",      &minimumAdmLevel,           &admRemoveAdminByNameOrId,            "Remove an Admin who is currently playing", "<i/n>", NULL },
    {"!adr",     "adminremove",      &minimumAdmLevel,           &admRemoveAdminByNameOrId,            "Remove an Admin who is currently playing", "<i/n>", NULL },
    {"!adl",    "adminlist",        &g_adminList.integer,       &admAdminList,                 "Show the Adminlist",               "",                 NULL},
    {"!al",     "adminlist",        &g_adminList.integer,       &admAdminList,                 "Show the Adminlist",               "",                 NULL},
    {"!ab",     "addbadmin",        &g_badmin.integer,          &admHandleAddBadmin,                  "Basic Admin",                      "<i/n>",            NULL},
    {"!aa",     "addadmin",         &g_admin.integer,           &admHandleAddAdmin,                  "Admin",                            "<i/n>",            NULL},
    {"!as",     "addsadmin",        &g_sadmin.integer,          &admHandleAddSadmin,                  "Server Admin",                     "<i/n>",            NULL},
        {"!sl",     "scorelimit",       &g_sl.integer,              &admScoreLimit,                "Change the scorelimit",            "<time>",           NULL},
        {"!tl",     "timelimit",        &g_tl.integer,              &admTimeLimit,                 "Change the timelimit",             "<time>",           NULL},
        {"!sw",     "swapteams",        &g_swapteams.integer,       &admSwapTeams,                 "Swap the players from team",       "",                 NULL},
        {"!ft",     "forceteam",        &g_forceteam.integer,       &admForceTeam,                 "Force a player to join a team",    "<i/n> <team>",     "ed"},
        {"!bl",     "banlist",          &g_ban.integer,             &admBanlist,                   "Shows the current banlist",        "",                 NULL},
        {"!ba",     "ban",              &g_ban.integer,             &admBan,                       "Ban a player",                     "<XdYhZm> <i/n> <rsn>", "ned"},
        {"!ub",     "unban",            &g_ban.integer,             &admUnban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL},
        {"!uba",    "unban",            &g_ban.integer,             &admUnban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL},
        {"!br",     "broadcast",        &g_broadcast.integer,       &admBroadcast,                 "Broadcast a message",              "<message>",        NULL},
        {"!sbl",    "subnetbanlist",    &g_subnetban.integer,       &admSubnetbanList,             "Shows the current subnetbanlist",  "",                 NULL},
        {"!sb",     "subnetban",        &g_subnetban.integer,       &admSubnetBan,                 "Ban a players' subnet",            "<XdYhZm> <i/n> <rsn>",   "ned"},
        {"!sbu",    "subnetunban",      &g_subnetban.integer,       &admSubnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL},
        {"!su",     "subnetunban",      &g_subnetban.integer,       &admSubnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL},
        // Boe!Man 6/2/15: Admin synonyms for Gold.
        // Pop/explode.
        {"!p",      "pop",              &g_pop.integer,             &admPop,                       "Pop/explodes a player",            "<i/n>",            "ped"},
        {"!e",      "explode",          &g_pop.integer,             &admPop,                       "Pop/explodes a player",            "<i/n>",            "ped"},
        // Uppercut/launch.
        {"!u",      "uppercut",         &g_uppercut.integer,        &admUppercut,                  "Launch a player upwards",          "<i/n>",            NULL},
        {"!uc",     "uppercut",         &g_uppercut.integer,        &admUppercut,                  "Launch a player upwards",          "<i/n>",            NULL},
        {"!la",     "launch",           &g_uppercut.integer,        &admUppercut,                  "Launch a player upwards",          "<i/n>",            "ed"},
        {"!swi",    "switch",           &g_forceteam.integer,       &admSwitch,                    "Switch one to the opposite team",  "<i/n>",            "ed"},
        {"!k",      "kick",             &g_kick.integer,            &admKick,                      "Kick a player",                    "<i/n>",            "ed"},
        {"!l",      "lock",             &g_lock.integer,            &admLockTeam,                  "Lock/unlock a team",               "<team>",           NULL},
        {"!r",      "respawn",          &g_respawn.integer,         &admRespawn,                   "Respawn a player",                 "<i/n>",            "ed"},
        {"!rs",     "respawn",          &g_respawn.integer,         &admRespawn,                   "Respawn a player",                 "<i/n>",            "ed"},
        {"!pl",     "plant",            &g_plant.integer,           &admPlant,                     "Plant or unplant a player",        "<i/n>",            "ed"},
    {"!ri",     "respawninterval",  &g_ri.integer,              &admRespawnInterval,           "Change the respawn interval",      "<time>",           NULL},
    {"!rtl",    "roundtimelimit",   &g_rtl.integer,             &admRoundTimelimit,            "Change the round timelimit",       "<time>",           NULL},
    {"!ro",     "runover",          &g_runover.integer,         &admRunover,                   "Push a player backwards",          "<i/n>",            NULL},
    {"!c",      "rollercoaster",    &g_rollercoaster.integer,   &admRollercoaster,             "Uppercut and push a player",       "<i/n>",            "ed"},
    {"!mr",     "maprestart",       &g_mapswitch.integer,       &admMapRestart,                "Restart the current map",          "",                 NULL},
    {"!mr",     "map_restart",      &g_mapswitch.integer,       &admMapRestart,                "Restart the current map",          "",                 NULL},
    {"!st",     "strip",            &g_strip.integer,           &admStrip,                     "Remove weapons from a player",     "<i/n>",            "ped"},

    {"!s",      "strip",            &g_strip.integer,           &admStrip,                     "Remove weapons from a player",     "<i/n>",            "ped"},

    {"!sh",     "shuffleteams",     &g_shuffleteams.integer,    &admShuffleTeams,              "Mix the teams at random",          "",                 NULL},

    {"!gr",     "gametyperestart",  &g_gr.integer,              &admGametypeRestart,           "Restart the current gametype",     "",                 NULL},

    {"!et",     "eventeams",        &g_eventeams.integer,       &admEventeams,                 "Make the teams even",              "",                 NULL},

    {"!g",      "gametype",         &g_mapswitch.integer,       &admGametype,                  "Switch to the given gametype",     "<gametype>",       NULL},
    {"!gt",     "gametype",         &g_mapswitch.integer,       &admGametype,                  "Switch to the given gametype",     "<gametype>",       NULL},

    {"!ff",     "friendlyfire",     &g_ff.integer,              &admFriendlyFire,              "Enables/disables friendly fire",   "",                 NULL},
    {"!rn",     "rename",           &g_rename.integer,          &admRename,                    "Renames a players' name",          "<i/n> <name>",     NULL},

    {"!b",      "burn",             &g_burn.integer,            &admBurn,                      "Burn a player",                    "",                 "ed"},

    {"!mc",     "mapcycle",         &g_mapswitch.integer,       &admMapcycle,                  "Switch to the next-defined map",   "",                 NULL},
        /*{"!rounds", "rounds",           &g_cm.integer,              &adm_Rounds,                    "Set the number of rounds",         "<rounds>",         NULL},
        // Boe!Man 6/2/15: Don't move or modify anything above this comment, the /adm list expects them in that specific order.
        {"!girly",  "girly",            &g_girly.integer,           &adm_Girly,                     "Change player skin to female skin","<i/n> <0-12>",     NULL},

        {"!bs",     "blockseek",        &g_blockseek.integer,       &adm_blockSeek,                 "Block player from joining seek",   "<i/n>",            NULL},
        {"!bsl",    "blockseeklist",    &g_blockseek.integer,       &adm_blockSeekList,             "Show players blocked from seeking","",                 NULL},
        {"!nl",     "nolower",          &g_nosection.integer,       &adm_noLower,                   "Enable/Disable Nolower",           "",                 NULL},
        {"!nr",     "noroof",           &g_nosection.integer,       &adm_noRoof,                    "Enable/Disable Noroof",            "",                 NULL},
        {"!nm",     "nomiddle",         &g_nosection.integer,       &adm_noMiddle,                  "Enable/Disable Nomiddle",          "",                 NULL},
        {"!nw",     "nowhole",          &g_nosection.integer,       &adm_noWhole,                   "Enable/Disable Nowhole",           "",                 NULL},
        {"!nn",     "nonades",          &g_nades.integer,           &adm_noNades,                   "Enable or disable nades",          "",                 NULL},
        {"!rd",     "realdamage",       &g_damage.integer,          &adm_realDamage,                "Toggle Real damage",               "",                 NULL},
        {"!nd",     "normaldamage",     &g_damage.integer,          &adm_normalDamage,              "Toggle Normal damage",             "",                 NULL},
        {"!acl",    "addclan",          &g_clan.integer,            &adm_addClanMember,             "Add a clan member",                "<i/n>",            NULL},
        {"!rc",     "removeclan",       &g_clan.integer,            &adm_removeClanMember,          "Remove a clan member",             "<i/n>",            NULL},
        {"!rcl",    "removeclanlist",   &g_clan.integer,            &adm_removeClanMemberFromList,  "Remove a member from the list",    "<i/line #>",       NULL},
        {"!clr",    "clanlistremove",   &g_clan.integer,            &adm_removeClanMemberFromList,  "Remove a member from the list",    "<i/line #>",       NULL},
        {"!cl",     "clanlist",         &g_clan.integer,            &adm_clanList,                  "Show the clanlist",                "",                 NULL},
        {"!cm",     "compmode",         &g_cm.integer,              &adm_compMode,                  "Toggles Competition Mode",         "",                 NULL},

        {"!cva",    "clanvsall",        &g_clanvsall.integer,       &adm_clanVsAll,                 "Clan versus other players-mode",   "",                 NULL},

        {"!fl",     "flash",            &g_flash.integer,           &adm_Flash,                     "Flash a player",                   "<i/n>",            "ed"},
        {"!map",    "map",              &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified map",      "<map name>",       NULL},
        {"!altmap", "altmap",           &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified altmap",   "<map name>",       NULL},
        {"!devmap", "devmap",           &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified devmap",   "<map name>",       NULL},
        {"!pv",     "passvote",         &g_forcevote.integer,       &adm_passVote,                  "Pass the running vote",            "",                 NULL},
        {"!cv",     "cancelvote",       &g_forcevote.integer,       &adm_cancelVote,                "Cancel the running vote",          "",                 NULL},
        {"!pa",     "pause",            &g_pause.integer,           &adm_Pause,                     "Pause/resume the game",            "",                 NULL},

        {"!m",      "mute",             &g_mute.integer,            &adm_Mute,                      "Mute/unmute a player",             "<i/n> <time>",     NULL},

        {"!3rd",    "3rd",              &g_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL},
        {"!third",  "third",            &g_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL},
        {"!wp",     "weapon",           &g_toggleweapon.integer,    &adm_toggleWeapon,              "Toggles weapon on or off",         "",                 NULL},
        {"!aca",    "anticamp",         &g_anticamp.integer,        &adm_Anticamp,                  "Toggles anticamp on or off",       "",                 NULL},
        {"!em",     "endmap",           &g_endmap.integer,          &adm_endMap,                    "Requests map to end",              "",                 NULL},
        {"!ml",     "maplist",          &g_mapswitch.integer,       &adm_mapList,                   "Lists all available maps",         "",                 NULL},

        */
};

int adminCommandsSize = sizeof(adminCommands) / sizeof(adminCommands[0]);
// End

int admRespawnInterval (int argNum, gentity_t* adm, qboolean shortCmd) {
    admToggleCVAR(argNum, adm, shortCmd, "Respawn interval", &g_respawnInterval);
    return -1;
}

int admRoundTimelimit (int argNum, gentity_t* adm, qboolean shortCmd) {
    admToggleCVAR(argNum, adm, shortCmd, "Round timelimit", &g_roundtimelimit);
    return -1;
}

int admRunover (int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;

    idNum = G_clientNumFromArg(adm, argNum, "runover", qtrue, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        runoverPlayer(recipient);
    }

    return idNum;
}

int admRollercoaster (int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;
    char* coasterCounterArg;
    int coasterCounter = 0;

    idNum = G_clientNumFromArg(adm, argNum, "runover", qtrue, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;

        // check for 2nd arg.
        coasterCounterArg = G_GetArg(argNum + 1, shortCmd);

        if (coasterCounterArg && strlen(coasterCounterArg) > 0) {
            coasterCounter = atoi(coasterCounterArg);
        }

        if (coasterCounter > 0) {
            coasterCounter *= 3;
        } else {
            coasterCounter = 6;
        }

        recipient->client->sess.coasterState = COASTER_UPPERCUT;
        recipient->client->sess.coasterCounter = coasterCounter;
        recipient->client->sess.nextCoaster = level.time;

        G_Broadcast(va("%s\nhas been sent to a \\rollercoaster\nby %s", recipient->client->pers.netname, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been sent to a rollercoaster by %s", recipient->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue));
        logAdmin(adm, recipient, "rollercoaster", NULL);
    }

    return -1;
}

int admMapRestart (int argNum, gentity_t* adm, qboolean shortCmd) {
    level.mapState = MAPSTATE_RESTART;
    level.mapStateTimer = level.time + 5000;

    G_Broadcast(va("\\Map restart by %s", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
    logAdmin(adm, NULL, "map restart", NULL);
    G_printInfoMessageToAll("Map restart by %s", getNameOrArg(adm, "RCON", qtrue));

    return -1;
}

int admStrip (int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;

    idNum = G_clientNumFromArg(adm, argNum, "strip", qtrue, qtrue, qfalse, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        stripClient(recipient, qfalse);
    }

    return idNum;
}

int admShuffleTeams (int argNum, gentity_t* adm, qboolean shortCmd) {

    int initialRedTeamCount = 0, initialBlueTeamCount = 0, shuffleRedTeamCount = 0, shuffleBlueTeamCount = 0;
    gentity_t* recipient;

    if (!level.gametypeData->teams) {
        G_printInfoMessage(adm, "Not in a team game, therefore nothing to shuffle...");
        return -1;
    }

    if (level.blueTeamLocked || level.redTeamLocked) {
        G_printInfoMessage(adm, "Unlock teams prior to shuffling.");
        return -1;
    }

    initialRedTeamCount = TeamCount(-1, TEAM_RED, NULL);
    initialBlueTeamCount = TeamCount(-1, TEAM_BLUE, NULL);

    if (!initialRedTeamCount || !initialBlueTeamCount) {
        G_printInfoMessage(adm, "No players ingame, therefore nothing to shuffle...");
        return -1;
    }

    for (int i = 0; i < level.numConnectedClients; i++) {
        recipient = &g_entities[level.sortedClients[i]];

        if (recipient->client->sess.team == TEAM_SPECTATOR) {
            continue;
        }

        if (initialRedTeamCount == shuffleRedTeamCount) {
            SetTeam(recipient, "b", NULL, TEAMCHANGE_FORCED);
            shuffleBlueTeamCount++;
        } else if (initialBlueTeamCount == shuffleBlueTeamCount) {
            SetTeam(recipient, "r", NULL, TEAMCHANGE_FORCED);
            shuffleRedTeamCount++;
        } else {
            int newTeam = irand(1, 2);
            if (newTeam == TEAM_RED) {
                SetTeam(recipient, "r", NULL, TEAMCHANGE_FORCED);
                shuffleRedTeamCount++;
            } else {
                SetTeam(recipient, "b", NULL, TEAMCHANGE_FORCED);
                shuffleBlueTeamCount++;
            }
        }
    }

    G_Broadcast(va("Teams have been \\shuffled by %s", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qfalse);
    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3", qtrue));
    G_printInfoMessageToAll("Teams have been shuffled by %s", getNameOrArg(adm, "RCON", qtrue));
    logAdmin(adm, NULL, "shuffleteams", NULL);

    return -1;
}

int admGametypeRestart (int argNum, gentity_t* adm, qboolean shortCmd) {
    // Cannot restart the gametype in DM (at least, no visible effect).
    if(!Q_stricmp(g_realGametype.string, "dm")){
        G_printInfoMessage(adm, "You cannot restart this gametype.");
        return -1;
    }

    // Cannot restart the gametype in intermission mode.
    if(level.intermissionQueued || level.intermissiontime){
        G_printInfoMessage(adm, "You cannot restart the gametype while being in intermission.");
        return -1;
    }

    // Cannot restart the gametype while being paused.
    if(level.pause){
        G_printInfoMessage(adm, "You cannot restart the gametype while the game is paused.");
        return -1;
    }

    G_Broadcast("\\Gametype restart", BROADCAST_CMD, NULL, qtrue);
    G_printInfoMessageToAll("Gametype has been restarted by %s.", getNameOrArg(adm, "RCON", qtrue));
    logAdmin(adm, NULL, "gametype restart", NULL);

    trap_SendConsoleCommand(EXEC_APPEND, "gametype_restart\n");

    return -1;
}

int admEventeams (int argNum, gentity_t* adm, qboolean shortCmd) {
    logAdmin(adm, NULL, "eventeams", NULL);
    return -1;
}

int admGametype (int argNum, gentity_t* adm, qboolean shortCmd) {


    char* gametype = G_GetArg(argNum, shortCmd);

    if (!gametype || strlen(gametype) == 0) {
        G_printInfoMessage(adm, "Current gametype is %s.", g_realGametype.string);
    } else if (!G_IsGametypeSupported(gametype)) {
        G_printInfoMessage(adm, "Gametype %s is not supported by 1fxplus.", gametype);
    } else {
        level.mapState = MAPSTATE_CHANGEGT;
        level.mapStateTimer = level.time + 5000;
        G_Broadcast(va("\\Gametype %s by %s", gametype, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        logAdmin(adm, NULL, va("gametype %s", gametype), NULL);
        G_printInfoMessageToAll("Gametype %s by %s", gametype, getNameOrArg(adm, "RCON", qtrue));
        trap_Cvar_Set("g_realGametype", gametype);
        trap_SendConsoleCommand(EXEC_APPEND, va("g_gametype %s\n", gametype));
    }

    return -1;
}

int admFriendlyFire (int argNum, gentity_t* adm, qboolean shortCmd) {

    qboolean enable = !g_friendlyFire.integer;
    G_setTrackedCvarWithoutTrackMessage(&g_friendlyFire, enable);

    G_Broadcast(va("\\Friendlyfire %s by %s", enable ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
    G_printInfoMessageToAll("Friendlyfire %s by %s", enable ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qtrue));
    logAdmin(adm, NULL, va("friendlyfire %s", enable ? "enabled" : "disabled"), NULL);

    return -1;
}

int admRename (int argNum, gentity_t* adm, qboolean shortCmd) {
    return -1;
}

int admBurn (int argNum, gentity_t* adm, qboolean shortCmd) {

    gentity_t       *recipient, *tmp;
    int             idNum = -1;

    idNum = G_clientNumFromArg(adm, argNum, "burn", qtrue, qtrue, qtrue, shortCmd);
    if (idNum >= 0) {
        recipient = g_entities + idNum;

        // Create temporary entity for the burn effect.
        tmp = G_TempEntity(recipient->r.currentOrigin, EV_EXPLOSION_HIT_FLESH);
        tmp->s.eventParm = 0;
        tmp->s.otherEntityNum2 = recipient->s.number;
        tmp->s.time = WP_ANM14_GRENADE + ((((int)recipient->s.apos.trBase[YAW] & 0x7FFF) % 360) << 16);
        VectorCopy(recipient->r.currentOrigin, tmp->s.angles);
        SnapVector(tmp->s.angles);

        // Set burn seconds and notify the player of what happened with a sound effect.
        recipient->client->sess.burnTimer = 4;
        G_ClientSound(recipient, G_SoundIndex("/sound/weapons/incendiary_grenade/incen01.mp3", qtrue));

    }

    return idNum;
}

int admMapcycle (int argNum, gentity_t* adm, qboolean shortCmd) {

    level.mapState = MAPSTATE_MAPCYCLE;
    level.mapStateTimer = level.time + 5000;

    G_Broadcast(va("\\Mapcycle by %s", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
    logAdmin(adm, NULL, "mapcycle", NULL);
    G_printInfoMessageToAll("Mapcycle by %s", getNameOrArg(adm, "RCON", qtrue));

    return -1;
}

void admUnplant(gentity_t* adm, gentity_t* recipient) {

    recipient->client->ps.origin[2] += 65;
    VectorCopy(recipient->client->ps.origin, recipient->s.origin);
    recipient->client->sess.planted = qfalse;

    G_ClientSound(recipient, G_SoundIndex("sound/misc/confused/wood_break.mp3", qtrue));
    G_Broadcast(va("%s\nwas \\unplanted\nby %s", recipient->client->pers.netname, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
    G_printInfoMessageToAll(va("%s was unplanted by %s.", recipient->client->pers.netname, getNameOrArg(adm, "RCON", qtrue)));
    logAdmin(adm, recipient, "unplanted", NULL);

}

int admPlant(int argNum, gentity_t* adm, qboolean shortCmd) {

    gentity_t* recipient;
    int idNum;

    idNum = G_clientNumFromArg(adm, argNum, "plant", qtrue, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;

        if (recipient->client->sess.planted) {
            admUnplant(adm, recipient);
            return -1;
        }

        if (recipient->client->ps.pm_flags & PMF_DUCKED) {
            recipient->client->ps.origin[2] -= 35;
        }
        else {
            recipient->client->ps.origin[2] -= 65;
        }

        VectorCopy(recipient->client->ps.origin, recipient->s.origin);
        recipient->client->sess.planted = qtrue;

        G_ClientSound(recipient, G_SoundIndex("sound/misc/confused/wood_break.mp3", qtrue));
    }

    return idNum;
}

int admAdminList(int argNum, gentity_t* adm, qboolean shortCmd) {

    qboolean passlist = qfalse;

    if (!Q_stricmp(G_GetArg(argNum, shortCmd), "pass")) {
        passlist = qtrue;
    }

    dbGetAdminlist(adm, passlist);

    return -1;
}

// mostly taken from 1fx. Mod, missing competitive mode which will be added along with the rest of compmode functionality.
void admToggleCVAR(int argNum, gentity_t* adm, qboolean shortCmd, char* cvarName, vmCvar_t* cvar) {

    char* arg = G_GetArg(argNum, shortCmd);
    int newValue = arg && strlen(arg) > 0 ? atoi(arg) : -1;

    if (newValue < 0) {
        G_printInfoMessage(adm, "%s is %d.", cvarName, cvar->integer);
    }
    else {
        // means change
        if (cvar) {
            G_setTrackedCvarWithoutTrackMessage(cvar, newValue);
        }
        else {
            trap_Cvar_Set(cvarName, va("%d", newValue));
            trap_Cvar_Update(cvar);
        }

        G_printInfoMessageToAll("%s was changed to %d by %s.", cvarName, newValue, getNameOrArg(adm, "RCON", qtrue));
        G_Broadcast(va("\\%s was changed to %d\nby %s", cvarName, newValue, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        logAdmin(adm, NULL, va("%s changed to %d", cvarName, newValue), NULL);

        // was only sent for rocmod and gold specific, but this shouldn't impact non-ROCMod clients if we send it again over here as well.
        for (int i = 0; i < level.numConnectedClients; i++) {
            DeathmatchScoreboardMessage(&g_entities[level.sortedClients[i]]);
        }
    }
}

int admBanlist(int argNum, gentity_t* adm, qboolean shortCmd) {

    dbGetBanlist(adm, qfalse);

    return -1;
}

void admBanPlayer(int argNum, gentity_t* adm, qboolean shortCmd, qboolean isSubnet) {
    int idNum = -1;
    gentity_t* recipient;
    char* reason;
    char durationString[MAX_STRING_CHARS], dayString[MAX_STRING_CHARS] = "\0", hourString[MAX_STRING_CHARS] = "\0", minuteString[MAX_STRING_CHARS] = "\0";
    char* banDurationArg = G_GetArg(argNum + 1, shortCmd);
    int banDuration[4] = { -1, 0, 0, 0 };
    qboolean isEom = qfalse;

    getBanDurationFromArg(&banDuration, banDurationArg);

    if (banDuration[0] != 1) {
        isEom = qtrue;
        Q_strncpyz(durationString, "until the end of map", sizeof(durationString));
    }
    else {

        if (banDuration[1]) {
            Q_strncpyz(dayString, va("%d days", banDuration[1]), sizeof(dayString));
        }

        if (banDuration[2]) {
            if (banDuration[1]) {
                Q_strncpyz(hourString, va(", %d hours", banDuration[2]), sizeof(hourString));
            } else {
                Q_strncpyz(hourString, va("%d hours", banDuration[2]), sizeof(hourString));
            }
        }

        if (banDuration[3]) {
            if (banDuration[1] || banDuration[2]) {
                Q_strncpyz(minuteString, va(", %d minutes", banDuration[3]), sizeof(minuteString));
            } else {
                Q_strncpyz(minuteString, va("%d minutes", banDuration[3]), sizeof(minuteString));
            }
        }


        Q_strncpyz(durationString, va("for %s%s%s", dayString, hourString, minuteString), sizeof(durationString));
    }

    if (banDuration[0] == -1) {
        reason = concatArgs(argNum + 1, shortCmd);
    }
    else {
        reason = concatArgs(argNum + 2, shortCmd);
    }

    idNum = G_clientNumFromArg(adm, argNum, "do this to", qfalse, qfalse, qfalse, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        G_Broadcast(va("%s\n^7has been \\%sbanned %s\nby %s", recipient->client->pers.netname, isSubnet ? "subnet" : "", durationString, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        logAdmin(adm, recipient, va("%sbanned %s", isSubnet ? "subnet" : "", durationString), reason);
        dbAddBan(isSubnet, recipient->client->pers.cleanName, recipient->client->pers.ip, getNameOrArg(adm, "RCON", qtrue), reason, isEom, banDuration[1], banDuration[2], banDuration[3]);
        trap_DropClient(idNum, va("Banned by %s %s. Reason: %s", getNameOrArg(adm, "RCON", qtrue), durationString, reason));
    }
}

int admBan(int argNum, gentity_t* adm, qboolean shortCmd) {
    admBanPlayer(argNum, adm, shortCmd, qfalse);
    return -1;
}

int admUnban(int argNum, gentity_t* adm, qboolean shortCmd) {
    admUnbanPlayer(argNum, adm, shortCmd, qfalse);
    return -1;
}

int admBroadcast(int argNum, gentity_t* adm, qboolean shortCmd) {

    char* broadcast = concatArgs(argNum, shortCmd);

    if (!broadcast || strlen(broadcast) == 0) {
        G_printInfoMessage(adm, "Enter a message to broadcast.");
        return -1;
    }

    parseTokens(adm, broadcast, SAY_TELL, qfalse);

    G_Broadcast(broadcast, BROADCAST_CMD, NULL, qfalse); // no beep as using different sound.
    G_GlobalSound(G_SoundIndex("sound/misc/confused/small_impact.mp3", qtrue));
    logAdmin(adm, NULL, va("broadcast: %s", broadcast), NULL);
    G_printInfoMessageToAll("Broadcast by %s: %s.", getNameOrArg(adm, "RCON", qtrue), broadcast);

    return -1;
}

int admSubnetbanList(int argNum, gentity_t* adm, qboolean shortCmd) {
    dbGetBanlist(adm, qtrue);
    return -1;
}

int admSubnetBan(int argNum, gentity_t* adm, qboolean shortCmd) {
    admBanPlayer(argNum, adm, shortCmd, qtrue);
    return -1;
}

void admUnbanPlayer(int argNum, gentity_t* adm, qboolean shortCmd, qboolean isSubnet) {
    int rownum = atoi(G_GetArg(argNum, shortCmd));

    if (rownum > 0) {
        // get row data so I can also display it to the player.
        char* bannedName;
        char* bannedIp;

        if (dbGetBanByRow(rownum, &bannedName, &bannedIp, isSubnet)) {
            logAdmin(adm, NULL, va("%s [%d] %s (%s)", isSubnet ? "Subnetunbanned" : "Unbanned", rownum, bannedName, bannedIp), NULL);
            G_printInfoMessage(adm, va("%s [%d] %s (%s)", isSubnet ? "Subnetunbanned" : "Unbanned", rownum, bannedName, bannedIp));
            dbDeleteBanByRowId(rownum, isSubnet);
        }
    }
    else {
        G_printInfoMessage(adm, "No ID specified whom to unban");
    }
}

int admSubnetUnban(int argNum, gentity_t* adm, qboolean shortCmd) {
    admUnbanPlayer(argNum, adm, shortCmd, qtrue);
    return -1;
}

int admPop(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;
    char* reason = G_GetArg(argNum + 1, shortCmd);

    idNum = G_clientNumFromArg(adm, argNum, "pop", qtrue, qfalse, qfalse, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        G_ClientSound(recipient, G_SoundIndex("sound/npc/air1/guard02/laughs.mp3", qtrue));

        // Allow to pop players in godmode.
        recipient->flags &= ~FL_GODMODE;
        G_Damage(recipient, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD | HL_FOOT_RT | HL_FOOT_LT | HL_LEG_UPPER_RT | HL_LEG_UPPER_LT | HL_HAND_RT | HL_HAND_LT | HL_WAIST | HL_CHEST | HL_NECK);

        G_Broadcast(va("%s\n^7has been \\popped\nby %s", recipient->client->pers.netname, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been popped by %s. %s%s", recipient->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue), reason ? "Reason: " : "", reason ? reason : "");
        logAdmin(adm, recipient, "pop", reason);
    }

    return -1; // capture reason.
}

int admUppercut(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;
    int ucLevel = 0;
    char* ucLevelArg;

    idNum = G_clientNumFromArg(adm, argNum, "uppercut", qtrue, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        ucLevelArg = G_GetArg(argNum + 1, shortCmd);

        if (ucLevelArg && strlen(ucLevelArg) > 0) {
            ucLevel = atoi(ucLevelArg);
        }
        uppercutPlayer(recipient, ucLevel);
    }

    return idNum;
}

int admSwitch(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    char playerArg = G_GetArg(argNum, shortCmd);
    gentity_t* recipient;

    idNum = G_clientNumFromArg(adm, argNum, "switch", qfalse, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;

        if (recipient->client->sess.team == TEAM_BLUE) {
            SetTeam(recipient, "r", NULL, TEAMCHANGE_FORCED);
        }
        else if (recipient->client->sess.team == TEAM_RED) {
            SetTeam(recipient, "b", NULL, TEAMCHANGE_FORCED);
        }
        else {
            G_printInfoMessage(adm, "You can only switch players who are in a team.");
            return -1;
        }
    }

    return idNum;
}

int admKick(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;

    char* playerArg = G_GetArg(argNum, shortCmd);
    char* reason = Q_CleanStr(concatArgs(argNum + 1, shortCmd));
    gentity_t* recipient;

    idNum = G_clientNumFromArg(adm, argNum, "kick", qfalse, qfalse, qfalse, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;

        G_Broadcast(va("%s^7\nhas been \\kicked\nby %s", recipient->client->pers.netname, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been kicked by %s. Reason: %s.", recipient->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue), reason);
        logAdmin(adm, recipient, "kick", reason);

        trap_DropClient(idNum, reason);

    }


    return -1; // -1 because I want to also capture the reason.
}

int admLockTeam(int argNum, gentity_t* adm, qboolean shortCmd) {

    char* teamArg = Q_strlwr(G_GetArg(argNum, shortCmd));
    char team[2];

    if (teamArg && strlen(teamArg) > 0) {
        team[0] = teamArg[0];
        team[1] = '\0';
    }
    else {
        G_printInfoMessage(adm, "No team specified.");
    }

    if (!Q_stricmp(team, "b")) {
        level.blueTeamLocked = qtrue;

        G_Broadcast(va("%s has been \\locked!", getTeamPrefixByGametype(TEAM_BLUE)), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been locked!", Q_CleanStr(getTeamPrefixByGametype(TEAM_BLUE)));
        logAdmin(adm, NULL, va("Locked %s team", Q_CleanStr(getTeamPrefixByGametype(TEAM_BLUE))), NULL);
    }
    else if (!Q_stricmp(team, "r")) {
        level.redTeamLocked = qtrue;

        G_Broadcast(va("%s has been \\locked!", getTeamPrefixByGametype(TEAM_RED)), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been locked!", Q_CleanStr(getTeamPrefixByGametype(TEAM_RED)));
        logAdmin(adm, NULL, va("Locked %s team", Q_CleanStr(getTeamPrefixByGametype(TEAM_RED))), NULL);
    }
    else if (!Q_stricmp(team, "s")) {
        level.spectatorsLocked = qtrue;

        G_Broadcast(va("%s has been \\locked!", g_spectatorTeamPrefix.string), BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("%s has been locked!", Q_CleanStr(g_spectatorTeamPrefix.string));
        logAdmin(adm, NULL, "Locked spectators", NULL);
    }
    else if (!Q_stricmp(team, "a")) {
        level.spectatorsLocked = qtrue;
        level.redTeamLocked = qtrue;
        level.blueTeamLocked = qtrue;

        G_Broadcast("All teams have been \\locked!", BROADCAST_CMD, NULL, qtrue);
        G_printInfoMessageToAll("All teams have been locked");
        logAdmin(adm, NULL, "Locked all teams", NULL);
    }
    else {
        G_printInfoMessage(adm, "Incorrect team specified. Valid choices are: blue, red, spec, all.");
    }

    return -1;
}

int admRespawn(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum = -1;
    gentity_t* recipient;

    idNum = G_clientNumFromArg(adm, argNum, "respawn", qfalse, qtrue, qtrue, shortCmd);

    if (idNum >= 0) {
        recipient = g_entities + idNum;
        respawnClient(adm, recipient);
    }

    return idNum;
}

int admForceTeam(int argNum, gentity_t* adm, qboolean shortCmd) {

    gentity_t* recipient;
    char* playerArg = G_GetArg(argNum, shortCmd);
    char* teamArg = Q_strlwr(G_GetArg(argNum + 1, shortCmd));
    int idNum = -1;
    char team[2];

    if (teamArg && strlen(teamArg) > 0) {
        team[0] = teamArg[0];
        team[1] = '\0';
    }
    else {
        G_printInfoMessage(adm, "Please enter a team to force the player(s) to.");
        return -1;
    }

    if (!Q_stricmp(team, "s") && !Q_stricmp(team, "b") && !Q_stricmp(team, "r")) {
        G_printInfoMessage(adm, "Team '%s' not recognized.", teamArg);
        return -1;
    }

    // special cases: forceteam all and forceteam players.
    // forceteam all takes all the connected clients
    // forceteam players takes all the clients who are not spectators.

    if (!Q_stricmp(playerArg, "all") || !Q_stricmp(playerArg, "players")) {

        playerArg = Q_strlwr(playerArg);

        for (int i = 0; i < level.numConnectedClients; i++) {
            recipient = &g_entities[level.sortedClients[i]];

            if (!Q_stricmp(playerArg, "all")) {
                SetTeam(recipient, team, NULL, TEAMCHANGE_FORCED);
            }
            else if (recipient->client->sess.team != TEAM_SPECTATOR) {
                SetTeam(recipient, team, NULL, TEAMCHANGE_FORCED);
            }

        }

        G_Broadcast(va("%s \\forceteamed to %s\nby %s", !Q_stricmp(playerArg, "all") ? "Everyone\nwas" : "All players\nwere", teamArg, getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        logAdmin(adm, NULL, va("forceteam %s to %s", playerArg, teamArg), NULL);

    }
    else {
        idNum = G_clientNumFromArg(adm, argNum, "forceteam", qfalse, qtrue, qtrue, shortCmd);

        if (idNum >= 0) {
            recipient = g_entities + idNum;
            SetTeam(recipient, team, NULL, TEAMCHANGE_FORCED);
            logAdmin(adm, recipient, va("forceteam to %s", teamArg), NULL);
        }

    }

    return idNum;

}

int admScoreLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
    admToggleCVAR(argNum, adm, shortCmd, "Scorelimit", &g_scorelimit);
    return -1;
}

int admTimeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
    admToggleCVAR(argNum, adm, shortCmd, "Timelimit", &g_timelimit);
    return -1;
}

int admSwapTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
    swapTeams(qfalse, adm);
    return -1;
}


int admRemoveAdminByNameOrId(int argNum, gentity_t* adm, qboolean shortCmd) {

    int idNum, removableAdminLevel;
    gentity_t* recipient;
    char* reason;

    idNum = G_clientNumFromArg(adm, argNum, "do this to", qfalse, qtrue, qtrue, shortCmd);

    if (idNum < 0) {
        return -1;
    }

    recipient = g_entities + idNum;

    if (!recipient || !recipient->client) {
        return -1;
    }

    reason = concatArgs(argNum + 1, shortCmd);

    if (recipient->client->sess.adminPassRegistration) {

        removableAdminLevel = recipient->client->sess.adminPassRegistration;

        if (adm && adm->client) {

            qboolean canRemove = qfalse;

            switch (removableAdminLevel) {
            case LEVEL_BADMIN:
                if (g_badmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_ADMIN:
                if (g_admin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_SADMIN:
                if (g_sadmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            default:
                break;
            }

            if (!canRemove) {
                G_printInfoMessage(adm, "You're not permitted to remove %s's.", getAdminNameByLevel(removableAdminLevel));
                return -1;
            }

        }

        recipient->client->sess.adminPassRegistration = 0;

        G_printInfoMessage(adm, "Removed %s %s from adminpass registration list", getAdminNameByLevel(removableAdminLevel), recipient->client->pers.cleanName);

        // notify the player as well, but because they had not yet done /adm pass, just notify them and do not broadcast the change.
        G_printInfoMessage(recipient, "Your pass registration was removed by %s.", getNameOrArg(adm, "RCON", qtrue));

        logAdmin(adm, recipient, va("Removed %s %s from adminpass registrationlist.", getAdminNameByLevel(removableAdminLevel), recipient->client->pers.cleanName), reason);
    }
    else if (recipient->client->sess.adminLevel >= LEVEL_BADMIN) {
        removableAdminLevel = recipient->client->sess.adminLevel;

        if (adm && adm->client) {

            qboolean canRemove = qfalse;

            switch (removableAdminLevel) {
            case LEVEL_BADMIN:
                if (g_badmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_ADMIN:
                if (g_admin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_SADMIN:
                if (g_sadmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            default:
                break;
            }

            if (!canRemove) {
                G_printInfoMessage(adm, "You're not permitted to remove %s's.", getAdminNameByLevel(removableAdminLevel));
                return -1;
            }

        }

        recipient->client->sess.adminLevel = LEVEL_NOADMIN;

        G_Broadcast(va("%s^7\ntheir %s was \\removed\nby %s", recipient->client->pers.netname, getAdminNameByLevel(removableAdminLevel), getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
        logAdmin(adm, recipient, va("Removed %s %s from admin%slist.", getAdminNameByLevel(removableAdminLevel), recipient->client->pers.cleanName, recipient->client->sess.adminType == ADMINTYPE_PASS ? "pass" : ""), reason);
        G_printInfoMessageToAll("%s's %s powers were removed by %s.", recipient->client->pers.cleanName, getAdminNameByLevel(removableAdminLevel), getNameOrArg(adm, "RCON", qtrue));
        int adminRowId = dbGetAdminRowIdByGentity(recipient);

        if (adminRowId == -1) {
            // a case that shouldn't happen, but just in case...
            logSystem(LOGLEVEL_ERROR, va("adminRowId is -1 while %s was removing %s's powers from admin%slist", getNameOrArg(adm, "RCON", qtrue), recipient->client->pers.cleanName, recipient->client->sess.adminType == ADMINTYPE_PASS ? "pass" : ""));
        }
        else {
            if (recipient->client->sess.adminType == ADMINTYPE_PASS) {
                dbDeletePassAdmin(adminRowId);
            }
            else {
                dbDeleteAdmin(adminRowId);
            }
        }
    }
    else {
        G_printInfoMessage(adm, "%s is not an admin.", recipient->client->pers.cleanName);
    }

    return -1;
}

int admRemoveAdminByRowId(int argNum, gentity_t* adm, qboolean shortCmd) {

    int rowid = atoi(G_GetArg(argNum, shortCmd));
    char* pass = G_GetArg(argNum + 1, shortCmd);
    char* removableName;
    char* removableIp;
    int removableAdminLevel = 0;

    qboolean passadmin = qfalse;

    if (!Q_stricmp(pass, "pass")) {
        passadmin = qtrue;
    }

    // first need to figure out whether I can even remove that admin.

    removableAdminLevel = dbGetAdminByRowId(passadmin, rowid, &removableName, &removableIp);

    if (!removableAdminLevel) {
        G_printInfoMessage(adm, "Admin with row %d does not exist.", rowid);
    }
    else {

        qboolean canRemove = qfalse;

        if (adm && adm->client) {

            switch (removableAdminLevel) {
            case LEVEL_BADMIN:
                if (g_badmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_ADMIN:
                if (g_admin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            case LEVEL_SADMIN:
                if (g_sadmin.integer <= adm->client->sess.adminLevel) {
                    canRemove = qtrue;
                }
                break;
            default:
                break;
            }
        }
        else {
            canRemove = qtrue; // RCON can always remove.
        }

        if (!canRemove) {
            G_printInfoMessage(adm, "You're not permitted to remove %s's admin.", removableName);
        }
        else {
            if (passadmin) {
                dbDeletePassAdmin(rowid);
            }
            else {
                dbDeleteAdmin(rowid);
            }

            G_printInfoMessage(adm, "Removed %s %s from admin%slist", getAdminNameByLevel(removableAdminLevel), removableName, passadmin ? "pass" : "");

            removeIngameAdminByNameAndType(adm, passadmin, removableName, removableIp, removableAdminLevel);

            logAdmin(adm, NULL, va("Removed %s %s from admin%slist.", getAdminNameByLevel(removableAdminLevel), removableName, passadmin ? "pass" : ""), NULL);
        }

    }

    return -1;

}

int admHandleAddBadmin(int argNum, gentity_t* adm, qboolean shortCmd) {
    return admAddAdmin(argNum, adm, shortCmd, LEVEL_BADMIN);
}

int admHandleAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {
    return admAddAdmin(argNum, adm, shortCmd, LEVEL_ADMIN);
}

int admHandleAddSadmin(int argNum, gentity_t* adm, qboolean shortCmd) {
    return admAddAdmin(argNum, adm, shortCmd, LEVEL_SADMIN);
}

int admAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd, int adminLevel) {
    int idNum = -1;
    gentity_t* recipient;
    char* arg;

    idNum = G_clientNumFromArg(adm, argNum, "do this to", qfalse, qfalse, qfalse, shortCmd);

    if (idNum < 0) {
        return -1;
    }

    recipient = g_entities + idNum;

    if (recipient->client->sess.adminLevel) {
        G_printInfoMessage(adm, "%s is already an admin.", recipient->client->pers.netname);
        return -1;
    }

    if (recipient->client->sess.adminPassRegistration) {
        G_printInfoMessage(adm, "%s is already added to the passlist, but has not yet registered his password.", recipient->client->pers.netname);
        return -1;
    }


    arg = G_GetArg(argNum + 1, shortCmd);

    if (dbIsIpNameInAdminList(!Q_stricmp(arg, "pass"), recipient->client->pers.ip, recipient->client->pers.cleanName)) {
        G_printInfoMessage(adm, "%s is already in the admin%s list.", recipient->client->pers.netname, arg);
        return -1;
    }

    if (!Q_stricmp(arg, "pass")) {

        // check if we have a player with that name already in the passlist.

        recipient->client->sess.adminPassRegistration = adminLevel;
        // nothing else to do here. User has to run /adm pass password to register himself into the adminlist
        // and after that /adm login to get powers.
        // broadcast over here instead of having ambiguous broadcast at postAdm (as we don't send whether it's a passadmin etc there).
        logAdmin(adm, recipient, va("Add %s with pass", getAdminNameByLevel(adminLevel)), NULL); // no reason for adding admin needed.

        // inform the client what they need to do...

        char* info = G_ColorizeMessage("\\Info:");
        trap_SendServerCommand(recipient->s.number, va("chat -1 \"%s You need to login every time you enter the server.\n\"", info));
        trap_SendServerCommand(recipient->s.number, va("chat -1 \"%s In order to do this, you need to set your own password.\n\"", info));
        trap_SendServerCommand(recipient->s.number, va("chat -1 \"%s Do this by executing the following command: /adm pass yourpassword.\n\"", info));
        trap_SendServerCommand(recipient->s.number, va("chat -1 \"%s Please do not use any arabic letters, as SoF does not understand those.\n\"", info));
        Q_strncpyz(recipient->client->sess.adminPassAddedBy, getNameOrArg(adm, "RCON", qfalse), sizeof(recipient->client->sess.adminPassAddedBy));
    }
    else {
        recipient->client->sess.adminLevel = adminLevel;
        dbAddAdmin(recipient->client->pers.cleanName, recipient->client->pers.ip, adminLevel, getNameOrArg(adm, "RCON", qtrue));
        logAdmin(adm, recipient, va("Add %s", getAdminNameByLevel(adminLevel)), NULL); // no reason for adding admin needed.
        recipient->client->sess.adminType = ADMINTYPE_IP;
        Q_strncpyz(recipient->client->sess.adminName, recipient->client->pers.cleanName, sizeof(recipient->client->sess.adminName));

    }

    G_printInfoMessageToAll("%s was added to the %s's %slist by %s", recipient->client->pers.netname, getAdminNameByLevel(adminLevel), !Q_stricmp(arg, "pass") ? "pass" : "", getNameOrArg(adm, "RCON", qfalse));

    G_Broadcast(va("%s \nwas added to the \\%s's %slist \nby %s", recipient->client->pers.netname, getAdminNameByLevel(adminLevel), !Q_stricmp(arg, "pass") ? "pass" : "", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);

    return -1;
}

// not called by RCON, only in clientcmd.
qboolean canClientRunAdminCommand(gentity_t* adm, int adminCommandId) {
    if (
        adminCommandId >= 0
        && adminCommandId < adminCommandsSize
        && adm
        && adm->client
        && adm->client->sess.adminLevel >= *adminCommands[adminCommandId].adminLevel
        ) {
        return qtrue;
    }

    return qfalse;
}

int cmdIsAdminCmd(char* cmd, qboolean shortCmd) {

    Q_strlwr(cmd);
    Q_CleanStr(cmd);
    for (int i = 0; i < adminCommandsSize; i++) {
        if (shortCmd && (!Q_stricmp(cmd, adminCommands[i].shortCmd) || !Q_stricmp(cmd, va("!%s", adminCommands[i].adminCmd)))) {
            return i;
        }
        else if (!Q_stricmp(cmd, adminCommands[i].adminCmd)) {
            return i;
        }
    }

    return -1;
}

void runAdminCommand(int adminCommandId, int argNum, gentity_t* adm, qboolean shortCmd) {
    postExecuteAdminCommand(
        adminCommandId,
        adminCommands[adminCommandId].Function(argNum, adm, shortCmd),
        adm
    );
}

/*
====================
postExecuteAdminCommand from 1fx. Mod

    a standard admin command which flow here will not be logged / broadcasted in its own function and will not have a reason.
    commands with reasons to be captured will be logged in their respective function.
    hence why idNum check is here. Commands where logs are not needed / need special logging, return -1.
====================
*/
void postExecuteAdminCommand(int funcNum, int idNum, gentity_t* adm) {

    if (idNum < 0) {
        return;
    }


    if (adm && adm->client) {
        G_Broadcast(va("%s\nwas \\%s%s\nby %s", g_entities[idNum].client->pers.netname, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", adm->client->pers.netname), BROADCAST_CMD, NULL, qtrue);
        trap_SendServerCommand(-1, va("print\"^3[Admin Action] ^7%s was %s%s by %s.\n\"", g_entities[idNum].client->pers.cleanName, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", adm->client->pers.cleanName));
    }
    else {
        // rcon command.
        G_Broadcast(va("%s\nwas \\%s%s", g_entities[idNum].client->pers.netname, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : ""), BROADCAST_CMD, NULL, qtrue);
        trap_SendServerCommand(-1, va("print\"^3[Rcon Action] ^7%s was %s%s.\n\"", g_entities[idNum].client->pers.cleanName, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : ""));
    }

    logAdmin(adm, &g_entities[idNum], adminCommands[funcNum].adminCmd, NULL);

}

char* getAdminNameByLevel(int adminLevel) {
    switch (adminLevel) {
    case LEVEL_BADMIN:
        return "B-Admin";
    case LEVEL_ADMIN:
        return "Admin";
    case LEVEL_SADMIN:
        return "S-Admin";
    default:
        return "Not admin";
    }
}

char* getAdminPrefixByLevel(int adminLevel) {
    switch (adminLevel) {
    case LEVEL_BADMIN:
        return g_badminChatPrefix.string;
    case LEVEL_ADMIN:
        return g_adminChatPrefix.string;
    case LEVEL_SADMIN:
        return g_sadminChatPrefix.string;
    default:
        return "";

    }
}
