
#include "../g_local.h"
#include "1fxFunctions.h"

// Admin commands taken from 1fx. Mod
// commented out as long as they are not functional.

int minimumAdmLevel = (int) LEVEL_BADMIN;

static admCmd_t adminCommands[] =
{

    {"!adr",    "adminremove",      &minimumAdmLevel,           &admRemoveAdmin,               "Remove an Admin from the list",    "<line #>",         NULL,qtrue},
    {"!adl",    "adminlist",        &g_adminList.integer,       &admAdminList,                 "Show the Adminlist",               "",                 NULL,qtrue},
    {"!al",     "adminlist",        &g_adminList.integer,       &admAdminList,                 "Show the Adminlist",               "",                 NULL,qtrue},
    {"!ab",     "addbadmin",        &g_badmin.integer,          &admAddAdmin,                  "Basic Admin",                      "<i/n>",            NULL,qtrue},
    {"!aa",     "addadmin",         &g_admin.integer,           &admAddAdmin,                  "Admin",                            "<i/n>",            NULL,qtrue},
    {"!as",     "addsadmin",        &g_sadmin.integer,          &admAddAdmin,                  "Server Admin",                     "<i/n>",            NULL,qtrue},
        /*{"!sl",     "scorelimit",       &g_sl.integer,              &adm_scoreLimit,                "Change the scorelimit",            "<time>",           NULL,qtrue},
        {"!tl",     "timelimit",        &g_tl.integer,              &adm_timeLimit,                 "Change the timelimit",             "<time>",           NULL,qtrue},
        {"!sw",     "swapteams",        &g_swapteams.integer,       &adm_swapTeams,                 "Swap the players from team",       "",                 NULL,qfalse},
        {"!rounds", "rounds",           &g_cm.integer,              &adm_Rounds,                    "Set the number of rounds",         "<rounds>",         NULL,qtrue},
        // Boe!Man 6/2/15: Don't move or modify anything above this comment, the /adm list expects them in that specific order.
        {"!pl",     "plant",            &g_plant.integer,           &adm_Plant,                     "Plant or unplant a player",        "<i/n>",            "ed",qfalse},
        {"!rtl",    "roundtimelimit",   &g_rtl.integer,             &adm_roundTimeLimit,            "Change the round timelimit",       "<time>",           NULL,qtrue},
        {"!ro",     "runover",          &g_runover.integer,         &adm_Runover,                   "Push a player backwards",          "<i/n>",            NULL,qfalse},
        {"!c",      "rollercoaster",    &g_rollercoaster.integer,   &adm_Rollercoaster,             "Uppercut and push a player",       "<i/n>",            "ed",qfalse},
        {"!girly",  "girly",            &g_girly.integer,           &adm_Girly,                     "Change player skin to female skin","<i/n> <0-12>",     NULL,qtrue},
        {"!r",      "respawn",          &g_respawn.integer,         &adm_Respawn,                   "Respawn a player",                 "<i/n>",            "ed",qfalse},
        {"!rs",     "respawn",          &g_respawn.integer,         &adm_Respawn,                   "Respawn a player",                 "<i/n>",            "ed",qfalse},
        {"!mr",     "maprestart",       &g_mapswitch.integer,       &adm_mapRestart,                "Restart the current map",          "",                 NULL,qtrue},
        {"!mr",     "map_restart",      &g_mapswitch.integer,       &adm_mapRestart,                "Restart the current map",          "",                 NULL,qtrue},
        {"!st",     "strip",            &g_strip.integer,           &adm_Strip,                     "Remove weapons from a player",     "<i/n>",            "ped",qfalse},
        {"!ra",     "removeadmin",      &minimumAdmLevel,           &adm_removeAdmin,               "Remove an Admin",                  "<i/n>",            NULL,qtrue},
        {"!ft",     "forceteam",        &g_forceteam.integer,       &adm_forceTeam,                 "Force a player to join a team",    "<i/n> <team>",     "ed",qtrue},
        {"!bs",     "blockseek",        &g_blockseek.integer,       &adm_blockSeek,                 "Block player from joining seek",   "<i/n>",            NULL,qfalse},
        {"!bsl",    "blockseeklist",    &g_blockseek.integer,       &adm_blockSeekList,             "Show players blocked from seeking","",                 NULL,qtrue},
        {"!nl",     "nolower",          &g_nosection.integer,       &adm_noLower,                   "Enable/Disable Nolower",           "",                 NULL,qtrue},
        {"!nr",     "noroof",           &g_nosection.integer,       &adm_noRoof,                    "Enable/Disable Noroof",            "",                 NULL,qtrue},
        {"!nm",     "nomiddle",         &g_nosection.integer,       &adm_noMiddle,                  "Enable/Disable Nomiddle",          "",                 NULL,qtrue},
        {"!nw",     "nowhole",          &g_nosection.integer,       &adm_noWhole,                   "Enable/Disable Nowhole",           "",                 NULL,qtrue},
        {"!sh",     "shuffleteams",     &g_shuffleteams.integer,    &adm_shuffleTeams,              "Mix the teams at random",          "",                 NULL,qtrue},
        {"!nn",     "nonades",          &g_nades.integer,           &adm_noNades,                   "Enable or disable nades",          "",                 NULL,qtrue},
        {"!ri",     "respawninterval",  &g_ri.integer,              &adm_respawnInterval,           "Change the respawn interval",      "<time>",           NULL,qtrue},
        {"!rd",     "realdamage",       &g_damage.integer,          &adm_realDamage,                "Toggle Real damage",               "",                 NULL,qtrue},
        {"!nd",     "normaldamage",     &g_damage.integer,          &adm_normalDamage,              "Toggle Normal damage",             "",                 NULL,qtrue},
        {"!gr",     "gametyperestart",  &g_gr.integer,              &adm_gametypeRestart,           "Restart the current gametype",     "",                 NULL,qtrue},
        {"!acl",    "addclan",          &g_clan.integer,            &adm_addClanMember,             "Add a clan member",                "<i/n>",            NULL,qtrue},
        {"!rc",     "removeclan",       &g_clan.integer,            &adm_removeClanMember,          "Remove a clan member",             "<i/n>",            NULL,qtrue},
        {"!rcl",    "removeclanlist",   &g_clan.integer,            &adm_removeClanMemberFromList,  "Remove a member from the list",    "<i/line #>",       NULL,qtrue},
        {"!clr",    "clanlistremove",   &g_clan.integer,            &adm_removeClanMemberFromList,  "Remove a member from the list",    "<i/line #>",       NULL,qtrue},
        {"!cl",     "clanlist",         &g_clan.integer,            &adm_clanList,                  "Show the clanlist",                "",                 NULL,qtrue},
        {"!cm",     "compmode",         &g_cm.integer,              &adm_compMode,                  "Toggles Competition Mode",         "",                 NULL,qtrue},
        {"!bl",     "banlist",          &g_ban.integer,             &adm_banList,                   "Shows the current banlist",        "",                 NULL,qtrue},
        {"!ba",     "ban",              &g_ban.integer,             &adm_Ban,                       "Ban a player",                     "<XdYhZm> <i/n> <rsn>", "ned",qtrue},
        {"!ub",     "unban",            &g_ban.integer,             &adm_Unban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL,qtrue},
        {"!uba",    "unban",            &g_ban.integer,             &adm_Unban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL,qtrue},
        {"!br",     "broadcast",        &g_broadcast.integer,       &adm_Broadcast,                 "Broadcast a message",              "<message>",        NULL,qtrue},
        {"!sbl",    "subnetbanlist",    &g_subnetban.integer,       &adm_subnetbanList,             "Shows the current subnetbanlist",  "",                 NULL,qtrue},
        {"!sb",     "subnetban",        &g_subnetban.integer,       &adm_subnetBan,                 "Ban a players' subnet",            "<XdYhZm> <i/n> <rsn>",   "ned",qtrue},
        {"!sbu",    "subnetunban",      &g_subnetban.integer,       &adm_subnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL,qtrue},
        {"!su",     "subnetunban",      &g_subnetban.integer,       &adm_subnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL,qtrue},
        {"!et",     "eventeams",        &g_eventeams.integer,       &adm_evenTeams,                 "Make the teams even",              "",                 NULL,qtrue},
        {"!cva",    "clanvsall",        &g_clanvsall.integer,       &adm_clanVsAll,                 "Clan versus other players-mode",   "",                 NULL,qtrue},
        {"!l",      "lock",             &g_lock.integer,            &adm_lockTeam,                  "Lock/unlock a team",               "<team>",           NULL,qtrue},
        {"!fl",     "flash",            &g_flash.integer,           &adm_Flash,                     "Flash a player",                   "<i/n>",            "ed",qfalse},
        {"!g",      "gametype",         &g_mapswitch.integer,       &adm_Gametype,                  "Switch to the given gametype",     "<gametype>",       NULL,qtrue},
        {"!gt",     "gametype",         &g_mapswitch.integer,       &adm_Gametype,                  "Switch to the given gametype",     "<gametype>",       NULL,qtrue},
        {"!map",    "map",              &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified map",      "<map name>",       NULL,qtrue},
        {"!altmap", "altmap",           &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified altmap",   "<map name>",       NULL,qtrue},
        {"!devmap", "devmap",           &g_mapswitch.integer,       &adm_Map,                       "Switch to the specified devmap",   "<map name>",       NULL,qtrue},
        {"!mc",     "mapcycle",         &g_mapswitch.integer,       &adm_mapCycle,                  "Switch to the next-defined map",   "",                 NULL,qtrue},
        {"!pv",     "passvote",         &g_forcevote.integer,       &adm_passVote,                  "Pass the running vote",            "",                 NULL,qtrue},
        {"!cv",     "cancelvote",       &g_forcevote.integer,       &adm_cancelVote,                "Cancel the running vote",          "",                 NULL,qtrue},
        {"!pa",     "pause",            &g_pause.integer,           &adm_Pause,                     "Pause/resume the game",            "",                 NULL,qtrue},
        {"!b",      "burn",             &g_burn.integer,            &adm_Burn,                      "Burn a player",                    "",                 "ed",qfalse},
        {"!k",      "kick",             &g_kick.integer,            &adm_Kick,                      "Kick a player",                    "<i/n>",            "ed",qfalse},
        {"!m",      "mute",             &g_mute.integer,            &adm_Mute,                      "Mute/unmute a player",             "<i/n> <time>",     NULL,qtrue},
        {"!s",      "strip",            &g_strip.integer,           &adm_Strip,                     "Remove weapons from a player",     "<i/n>",            "ped",qfalse},
        {"!ff",     "friendlyfire",     &g_ff.integer,              &adm_friendlyFire,              "Enables/disables friendly fire",   "",                 NULL,qtrue},
        {"!rn",     "rename",           &g_rename.integer,          &adm_Rename,                    "Renames a players' name",          "<i/n> <name>",     NULL,qtrue},
        {"!swi",    "switch",           &g_forceteam.integer,       &adm_Switch,                    "Switch one to the opposite team",  "<i/n>",            "ed",qfalse},
        #ifndef _DEMO
        {"!3rd",    "3rd",              &g_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL,qtrue},
        {"!third",  "third",            &g_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL,qtrue},
        #endif // not _DEMO
        {"!wp",     "weapon",           &g_toggleweapon.integer,    &adm_toggleWeapon,              "Toggles weapon on or off",         "",                 NULL,qtrue},
        {"!aca",    "anticamp",         &g_anticamp.integer,        &adm_Anticamp,                  "Toggles anticamp on or off",       "",                 NULL,qtrue},
        {"!em",     "endmap",           &g_endmap.integer,          &adm_endMap,                    "Requests map to end",              "",                 NULL,qtrue},
        {"!ml",     "maplist",          &g_mapswitch.integer,       &adm_mapList,                   "Lists all available maps",         "",                 NULL,qtrue},
        // Boe!Man 6/2/15: Admin synonyms for Gold.
        // Pop/explode.
        {"!p",      "pop",              &g_pop.integer,             &adm_Pop,                       "Pop/explodes a player",            "<i/n>",            "ped",qfalse},
        #ifdef _GOLD
        {"!e",      "explode",          &g_pop.integer,             &adm_Pop,                       "Pop/explodes a player",            "<i/n>",            "ped",qfalse},
        #endif // _GOLD
        // Uppercut/launch.
        {"!u",      "uppercut",         &g_uppercut.integer,        &adm_Uppercut,                  "Launch a player upwards",          "<i/n>",            NULL,qtrue},
        {"!uc",     "uppercut",         &g_uppercut.integer,        &adm_Uppercut,                  "Launch a player upwards",          "<i/n>",            NULL,qtrue},
        #ifdef _GOLD
        {"!la",     "launch",           &g_uppercut.integer,        &adm_Uppercut,                  "Launch a player upwards",          "<i/n>",            "ed",qtrue},
        #endif // _GOLD
        */
};

static int adminCommandsSize = sizeof(adminCommands) / sizeof(adminCommands[0]);
// End

int admAdminList(int argNum, gentity_t* adm, qboolean shortCmd) {

}

int admRemoveAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {

}

int admAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {

}

// not called by RCON, only in clientcmd.
qboolean canClientRunAdminCommand(gentity_t* adm, int adminCommandId) {
    if (
            adminCommandId >= 0
            && adminCommandId < adminCommandsSize
            && adm
            && adm->client
            && (int) adm->client->sess.adminLevel >= *adminCommands[adminCommandId].adminLevel
    ) {
        return qtrue;
    }

    return qfalse;
}

int cmdIsAdminCmd(char* cmd, qboolean shortCmd) {

    for (int i = 0; i < adminCommandsSize; i++) {
        if (shortCmd && !Q_stricmp(cmd, adminCommands[i].shortCmd)) {

        } else if (!Q_stricmp(cmd, adminCommands[i].adminCmd)) {
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

void postExecuteAdminCommand(int funcNum, int idNum, gentity_t *adm) {
    if (idNum < 0) {
        return;
    }
}