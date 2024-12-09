#include "../g_local.h"

int minimumAdminLevel = ADMLVL_BADMIN;

/*
==================
Admincommand list mostly as-is from current 3D-1fxmod.
==================
*/
admCmd_t adminCommands[] = {
	{"!adr",    "adminremove",      &minimumAdminLevel,         &adm_adminRemove,               "Remove an Admin from the list",    "<line #>",         NULL},
	{"!adl",    "adminlist",        &a_adminlist.integer,       &adm_adminList,                 "Show the Adminlist",               "",                 NULL},
	{"!al",     "adminlist",        &a_adminlist.integer,       &adm_adminList,                 "Show the Adminlist",               "",                 NULL},
	{"!ab",     "addbadmin",        &a_badmin.integer,          &adm_addAdmin,                  "Basic Admin",                      "<i/n>",            NULL},
	{"!aa",     "addadmin",         &a_admin.integer,           &adm_addAdmin,                  "Admin",                            "<i/n>",            NULL},
	{"!as",     "addsadmin",        &a_sadmin.integer,          &adm_addAdmin,                  "Server Admin",                     "<i/n>",            NULL},
	{"!ah",     "addhadmin",        &a_hadmin.integer,          &adm_addAdmin,                  "Head Admin",                     "<i/n>",            NULL},
	{"!sl",     "scorelimit",       &a_scorelimit.integer,              &adm_scoreLimit,                "Change the scorelimit",            "<time>",           NULL},
	{"!tl",     "timelimit",        &a_timelimit.integer,              &adm_timeLimit,                 "Change the timelimit",             "<time>",           NULL},
	{"!sw",     "swapteams",        &a_swapteams.integer,       &adm_swapTeams,                 "Swap the players from team",       "",                 NULL},
	{"!rounds", "rounds",           &a_compmode.integer,              &adm_Rounds,                    "Set the number of rounds",         "<rounds>",         NULL},
	// Boe!Man 6/2/15: Don't move or modify anything above this comment, the /adm list expects them in that specific order.
	{"!pl",     "plant",            &a_plant.integer,           &adm_Plant,                     "Plant or unplant a player",        "<i/n>",            "ed"},
	{"!rtl",    "roundtimelimit",   &a_roundtimelimit.integer,             &adm_roundTimeLimit,            "Change the round timelimit",       "<time>",           NULL},
	{"!ro",     "runover",          &a_runover.integer,         &adm_Runover,                   "Push a player backwards",          "<i/n>",            NULL},
	{"!c",      "rollercoaster",    &a_rollercoaster.integer,   &adm_Rollercoaster,             "Uppercut and push a player",       "<i/n>",            "ed"},
	{"!r",      "respawn",          &a_respawn.integer,         &adm_Respawn,                   "Respawn a player",                 "<i/n>",            "ed"},
	{"!rs",     "respawn",          &a_respawn.integer,         &adm_Respawn,                   "Respawn a player",                 "<i/n>",            "ed"},
	{"!mr",     "maprestart",       &a_mapswitch.integer,       &adm_mapRestart,                "Restart the current map",          "",                 NULL},
	{"!mr",     "map_restart",      &a_mapswitch.integer,       &adm_mapRestart,                "Restart the current map",          "",                 NULL},
	{"!st",     "strip",            &a_strip.integer,           &adm_Strip,                     "Remove weapons from a player",     "<i/n>",            "ped"},
	{"!ra",     "removeadmin",      &minimumAdminLevel,           &adm_removeAdmin,               "Remove an Admin",                  "<i/n>",            NULL},
	{"!ft",     "forceteam",        &a_forceteam.integer,       &adm_forceTeam,                 "Force a player to join a team",    "<i/n> <team>",     "ed"},
	{"!bs",     "blockseek",        &a_blockseek.integer,       &adm_blockSeek,                 "Block player from joining seek",   "<i/n>",            NULL},
	{"!bsl",    "blockseeklist",    &a_blockseek.integer,       &adm_blockSeekList,             "Show players blocked from seeking","",                 NULL},
	{"!nl",     "nolower",          &a_nosection.integer,       &adm_noLower,                   "Enable/Disable Nolower",           "",                 NULL},
	{"!nr",     "noroof",           &a_nosection.integer,       &adm_noRoof,                    "Enable/Disable Noroof",            "",                 NULL},
	{"!nm",     "nomiddle",         &a_nosection.integer,       &adm_noMiddle,                  "Enable/Disable Nomiddle",          "",                 NULL},
	{"!nw",     "nowhole",          &a_nosection.integer,       &adm_noWhole,                   "Enable/Disable Nowhole",           "",                 NULL},
	{"!sh",     "shuffleteams",     &a_shuffleteams.integer,    &adm_shuffleTeams,              "Mix the teams at random",          "",                 NULL},
	{"!nn",     "nonades",          &a_nades.integer,           &adm_noNades,                   "Enable or disable nades",          "",                 NULL},
	{"!ri",     "respawninterval",  &a_respawninterval.integer,              &adm_respawnInterval,           "Change the respawn interval",      "<time>",           NULL},
	{"!rd",     "realdamage",       &a_damage.integer,          &adm_realDamage,                "Toggle Real damage",               "",                 NULL},
	{"!nd",     "normaldamage",     &a_damage.integer,          &adm_normalDamage,              "Toggle Normal damage",             "",                 NULL},
	{"!cd",     "customdamage",     &a_damage.integer,          &adm_customDamage,              "Toggle Custom damage",             "<filename>",                 NULL},
	{"!gr",     "gametyperestart",  &a_gtrestart.integer,              &adm_gametypeRestart,           "Restart the current gametype",     "",                 NULL},
	{"!acl",    "addclan",          &a_clan.integer,            &adm_addClanMember,             "Add a clan member",                "<i/n>",            NULL},
	{"!rc",     "removeclan",       &a_clan.integer,            &adm_removeClanMember,          "Remove a clan member",             "<i/n>",            NULL},
	{"!rcl",    "removeclanlist",   &a_clan.integer,            &adm_removeClanMemberFromList,  "Remove a member from the list",    "<i/line #>",       NULL},
	{"!cl",     "clanlist",         &a_clan.integer,            &adm_clanList,                  "Show the clanlist",                "",                 NULL},
	{"!cm",     "compmode",         &a_compmode.integer,              &adm_compMode,                  "Toggles Competition Mode",         "",                 NULL},
	{"!bl",     "banlist",          &a_ban.integer,             &adm_banList,                   "Shows the current banlist",        "",                 NULL},
	{"!ba",     "ban",              &a_ban.integer,             &adm_Ban,                       "Ban a player",                     "<XdYhZm> <i/n> <rsn>", "ned"},
	{"!ub",     "unban",            &a_ban.integer,             &adm_Unban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL},
	{"!uba",    "unban",            &a_ban.integer,             &adm_Unban,                     "Unban a banned IP (player)",       "<ip/line #>",      NULL},
	{"!br",     "broadcast",        &a_broadcast.integer,       &adm_Broadcast,                 "Broadcast a message",              "<message>",        NULL},
	{"!sbl",    "subnetbanlist",    &a_subnetban.integer,       &adm_subnetbanList,             "Shows the current subnetbanlist",  "",                 NULL},
	{"!sb",     "subnetban",        &a_subnetban.integer,       &adm_subnetBan,                 "Ban a players' subnet",            "<XdYhZm> <i/n> <rsn>",   "ned"},
	{"!sbu",    "subnetunban",      &a_subnetban.integer,       &adm_subnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL},
	{"!su",     "subnetunban",      &a_subnetban.integer,       &adm_subnetUnban,               "Unban a banned subnet",            "<ip/line #>",      NULL},
	{"!et",     "eventeams",        &a_eventeams.integer,       &adm_evenTeams,                 "Make the teams even",              "",                 NULL},
	{"!cva",    "clanvsall",        &a_clanvsall.integer,       &adm_clanVsAll,                 "Clan versus other players-mode",   "",                 NULL},
	{"!l",      "lock",             &a_lock.integer,            &adm_lockTeam,                  "Lock/unlock a team",               "<team>",           NULL},
	{"!fl",     "flash",            &a_flash.integer,           &adm_Flash,                     "Flash a player",                   "<i/n>",            "ed"},
	{"!g",      "gametype",         &a_mapswitch.integer,       &adm_Gametype,                  "Switch to the given gametype",     "<gametype>",       NULL},
	{"!gt",     "gametype",         &a_mapswitch.integer,       &adm_Gametype,                  "Switch to the given gametype",     "<gametype>",       NULL},
	{"!map",    "map",              &a_mapswitch.integer,       &adm_Map,                       "Switch to the specified map",      "<map name>",       NULL},
	{"!altmap", "altmap",           &a_mapswitch.integer,       &adm_Map,                       "Switch to the specified altmap",   "<map name>",       NULL},
	{"!devmap", "devmap",           &a_mapswitch.integer,       &adm_Map,                       "Switch to the specified devmap",   "<map name>",       NULL},
	{"!mc",     "mapcycle",         &a_mapswitch.integer,       &adm_mapCycle,                  "Switch to the next-defined map",   "",                 NULL},
	{"!pv",     "passvote",         &a_forcevote.integer,       &adm_passVote,                  "Pass the running vote",            "",                 NULL},
	{"!cv",     "cancelvote",       &a_forcevote.integer,       &adm_cancelVote,                "Cancel the running vote",          "",                 NULL},
	{"!pa",     "pause",            &a_pause.integer,           &adm_Pause,                     "Pause/resume the game",            "",                 NULL},
	{"!b",      "burn",             &a_burn.integer,            &adm_Burn,                      "Burn a player",                    "",                 "ed"},
	{"!k",      "kick",             &a_kick.integer,            &adm_Kick,                      "Kick a player",                    "<i/n>",            "ed"},
	{"!m",      "mute",             &a_mute.integer,            &adm_Mute,                      "Mute/unmute a player",             "<i/n> <time>",     NULL},
	{"!s",      "strip",            &a_strip.integer,           &adm_Strip,                     "Remove weapons from a player",     "<i/n>",            "ped"},
	{"!ff",     "friendlyfire",     &a_friendlyFire.integer,              &adm_friendlyFire,              "Enables/disables friendly fire",   "",                 NULL},
	{"!rn",     "rename",           &a_rename.integer,          &adm_Rename,                    "Renames a players' name",          "<i/n> <name>",     NULL},
	{"!swi",    "switch",           &a_forceteam.integer,       &adm_Switch,                    "Switch one to the opposite team",  "<i/n>",            "ed"},
	{"!3rd",    "3rd",              &a_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL},
	{"!third",  "third",            &a_3rd.integer,             &adm_Third,                     "Toggles Thirdperson on or off",    "",                 NULL},
	{"!wp",     "weapon",           &a_toggleweapon.integer,    &adm_toggleWeapon,              "Toggles weapon on or off",         "",                 NULL},
	{"!aca",    "anticamp",         &a_anticamp.integer,        &adm_Anticamp,                  "Toggles anticamp on or off",       "",                 NULL},
	{"!em",     "endmap",           &a_mapswitch.integer,          &adm_endMap,                    "Requests map to end",              "",                 NULL},
	{"!ml",     "maplist",          &a_mapswitch.integer,       &adm_mapList,                   "Lists all available maps",         "",                 NULL},

	{"!bo",     "bestof",           &a_compmode.integer,     			&adm_matchIsBestOf,             "Toggles Best-of system",  	   		"",        	NULL},
	{"!pfl",    "profanitylist",	&minimumAdminLevel,     		&adm_profanityList,             "Show profanity list",  	   		"",        	NULL},
	{"!p",      "pop",              &a_pop.integer,             &adm_Pop,                       "Pop/explodes a player",            "<i/n>",            "ped"},
	{"!e",      "explode",          &a_pop.integer,             &adm_Pop,                       "Pop/explodes a player",            "<i/n>",            "ped"},
	{"!u",		"uppercut",         &a_uppercut.integer,        &adm_Uppercut,                  "Launch a player upwards",          "<i/n>",            NULL},
	{"!pu",     "punish",           &a_pop.integer,             &adm_Punish,                    "Punishes a player",                "<i/n>",            "ed"},
	{"!psl",    "punishlist",       &a_pop.integer,             &adm_punishList,                "Show punished players",            "",                 NULL},
	{"!mcl",    "mapcyclelist",     &a_mapswitch.integer,       &adm_mapCycleList,              "Shows the current mapcycle",       "",                 NULL},
	{"!stm",    "skiptomap",        &a_mapswitch.integer,       &adm_skipToMap,                 "Skips to the specified map index", "<map num>",        NULL },
};

int adminCommandsSize = sizeof(adminCommands) / sizeof(adminCommands[0]);

const char* getAdminNameByAdminLevel(admLevel_t adminLevel) {

	if (adminLevel <= ADMLVL_NONE || adminLevel > ADMLVL_HADMIN) {
		return "";
	}

	switch (adminLevel) {
	case ADMLVL_BADMIN:
		return g_badminPrefix.string;
	case ADMLVL_ADMIN:
		return g_adminPrefix.string;
	case ADMLVL_SADMIN:
		return g_sadminPrefix.string;
	case ADMLVL_HADMIN:
		return g_hadminPrefix.string;
	default:
		return "";
	}

}

int adm_adminRemove(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_adminList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

static void addAdmin(int argNum, gentity_t* adm, qboolean shortCmd, admLevel_t admlvl) {
	
	int idNum = G_ClientNumFromArg(adm, argNum, "do this to", qfalse, qtrue, qtrue, shortCmd);

	if (idNum < 0) return;
	gentity_t* ent = g_entities + idNum;
	char arg[64];
	admType_t adminType = ADMTYPE_IP;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(2), sizeof(arg));
	}
	else {
		trap_Argv(argNum + 1, arg, sizeof(arg));
	}

	if (!Q_stricmp(arg, "pass")) {
		adminType = ADMTYPE_PASS;
	}
	else if (!Q_stricmp(arg, "guid")) {

		if (!ent->client->sess.legacyProtocol) {

			G_printInfoMessage(adm, "You can only add GUID-admins for legacy / 1.00 clients.");

			return;
		}
		else if (!ent->client->sess.hasRoxAC) {
			G_printInfoMessage(adm, "Client %s (%d) does not have Rox AC running or the verification failed.", ent->client->pers.cleanName, idNum);
		}

		adminType = ADMTYPE_GUID;
	}

	// Check for admin existence in the admin tables.
	Com_Printf("GOT HERE");
	int existingAdminLevel = dbGetAdminLevel(adminType, ent, NULL);
	Com_Printf("GOT THERE");
	if (existingAdminLevel != -1) {

		if (adminType == ADMTYPE_PASS) {
			ent->client->sess.setAdminPassword = qtrue;
			G_printInfoMessage(adm, "Client %s (%d) already had admin powers. They can now change their password.", ent->client->pers.cleanName, idNum);
			G_printChatInfoMessage(ent, "%s has toggled admin-password reset for you.", getNameOrArg(adm, "RCON", qtrue));
			G_printChatInfoMessage(ent, "To do so, issue command \"/adm pass\" into console, e.g. \"/adm pass newpass\"");
			G_printChatInfoMessage(ent, "That will set your password to \"newpass\"");

			logAdmin();
			return;
		}
		else {
			G_printInfoMessage(adm, "Client %s (%d) already has admin powers. Please remove them before trying to readd.", ent->client->pers.cleanName, idNum);

			return;
		}

		
	}
	Com_Printf("GOT 123");
	// Client doesn't have admin, so lets perform the action.
	Com_Printf("\ndbAddAdmin, params: %d, %d, %s, %s\n", adminType, admlvl, getNameOrArg(ent, "WTF", qtrue), getNameOrArg(adm, "RCON?", qtrue));
	dbAddAdmin(adminType, admlvl, ent, adm, NULL);
	Com_Printf("GOT 11222333HERE");
	if (adminType == ADMTYPE_PASS) {
		ent->client->sess.setAdminPassword = qtrue;
		G_printChatInfoMessage(ent, "%s has added you to admin passlist as %s.", getNameOrArg(adm, "RCON", qtrue), getAdminNameByAdminLevel(admlvl));
		G_printChatInfoMessage(ent, "For that, you do need to set a password.");
		G_printChatInfoMessage(ent, "To do so, issue command \"/adm pass\" into console, e.g. \"/adm pass newpass\"");
		G_printChatInfoMessage(ent, "That will set your password to \"newpass\"");

		
	}
	else {
		ent->client->sess.adminLevel = admlvl;
		ent->client->sess.adminLogonMethod = adminType;
	}

	logAdmin();
	G_Broadcast(BROADCAST_GAME, NULL, qtrue, "%s\n^7has been added to %s^7list\nby %s", ent->client->pers.netname, getAdminNameByAdminLevel(admlvl), getNameOrArg(adm, "\\RCON", qtrue));



}

int adm_addAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {

	char            command[64] = "\0";

	if (adm && adm->client) {
		trap_Argv(1, command, sizeof(command));
	}
	else {
		trap_Argv(0, command, sizeof(command));
	}
	Q_strlwr(command); // Boe!Man 2/16/13: Fix capitalized Admin commands resulting in adding S-Admin by converting the command to lower case.

	if (strstr(command, "!ab") || strstr(command, "addbadmin")) {
		addAdmin(argNum, adm, shortCmd, ADMLVL_BADMIN, "addbadmin");
	}
	else if (strstr(command, "!aa") || strstr(command, "addadmin")) {
		addAdmin(argNum, adm, shortCmd, ADMLVL_ADMIN, "addadmin");
	}
	else if (strstr(command, "!as") || strstr(command, "addsadmin")) { 
		addAdmin(argNum, adm, shortCmd, ADMLVL_SADMIN, "addsadmin");
	}
	else if (strstr(command, "!ah") || strstr(command, "addhadmin")) { 
		addAdmin(argNum, adm, shortCmd, ADMLVL_HADMIN, "addsadmin");
	}

	return -1;
}

int adm_scoreLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_timeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_swapTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Rounds(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Plant(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_roundTimeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Runover(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Rollercoaster(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Respawn(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_mapRestart(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Strip(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_removeAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_forceTeam(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_blockSeek(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_blockSeekList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_noLower(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_noRoof(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_noMiddle(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_noWhole(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_shuffleTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_noNades(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_respawnInterval(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_realDamage(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_normalDamage(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_customDamage(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_gametypeRestart(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_addClanMember(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_removeClanMember(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_removeClanMemberFromList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_clanList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_compMode(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_banList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Ban(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Unban(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Broadcast(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_subnetbanList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_subnetBan(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_subnetUnban(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_evenTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_clanVsAll(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_lockTeam(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Flash(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Gametype(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Map(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_mapCycle(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_passVote(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_cancelVote(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Pause(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Burn(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Kick(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Mute(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_friendlyFire(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Rename(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Switch(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Third(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_toggleWeapon(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Anticamp(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_endMap(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_mapList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_matchIsBestOf(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_profanityList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Pop(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Uppercut(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Punish(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_punishList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_mapCycleList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_skipToMap(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

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
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\%s%s\nby %s", g_entities[idNum].client->pers.netname, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", adm->client->pers.netname);
		trap_SendServerCommand(-1, va("print\"^3[Admin Action] ^7%s was %s%s by %s.\n\"", g_entities[idNum].client->pers.cleanName, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", adm->client->pers.cleanName));
	}
	else {
		// rcon command.
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\%s%s", g_entities[idNum].client->pers.netname, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "");
		trap_SendServerCommand(-1, va("print\"^3[Rcon Action] ^7%s was %s%s.\n\"", g_entities[idNum].client->pers.cleanName, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : ""));
	}

	logAdmin();

}

