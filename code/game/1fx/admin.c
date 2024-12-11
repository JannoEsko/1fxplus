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
	{"!ab",     "addbadmin",        &a_badmin.integer,          &adm_addBadmin,                  "Basic Admin",                      "<i/n>",            NULL},
	{"!aa",     "addadmin",         &a_admin.integer,           &adm_addAdmin,                  "Admin",                            "<i/n>",            NULL},
	{"!as",     "addsadmin",        &a_sadmin.integer,          &adm_addSadmin,                  "Server Admin",                     "<i/n>",            NULL},
	{"!ah",     "addhadmin",        &a_hadmin.integer,          &adm_addHadmin,                  "Head Admin",                     "<i/n>",            NULL},
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

void getCleanAdminNameByAdminLevel(admLevel_t adminLevel, char* output, int sizeOfOutput) {

	char tmp[MAX_NETNAME];

	Q_strncpyz(tmp, getAdminNameByAdminLevel(adminLevel), sizeof(tmp));
	G_RemoveColorEscapeSequences(tmp);
	Q_strncpyz(output, tmp, sizeOfOutput);
}

static qboolean adm_removeIngamePlayerPowers(admType_t adminType, char* cleanName, gentity_t* adm) {

	for (int i = 0; i < level.numConnectedClients; i++) {
		gentity_t* ent = &g_entities[level.sortedClients[i]];

		if (ent->client->sess.adminType == adminType && !Q_stricmp(cleanName, ent->client->sess.adminName)) {
			ent->client->sess.adminType = ADMTYPE_NONE;
			ent->client->sess.adminLevel = ADMLVL_NONE;
			Com_Memset(ent->client->sess.adminName, 0, sizeof(ent->client->sess.adminName));

			G_printInfoMessage(ent, "Your admin powers have been removed by %s.", getNameOrArg(adm, "RCON", qtrue));
			G_Broadcast(BROADCAST_GAME, NULL, qtrue, "%s\n^7their admin powers were \\removed\n^7by %s", ent->client->pers.netname, getNameOrArg(adm, "RCON", qfalse));
			return qtrue;
		}
	}

	return qfalse;

}

void adm_setPassword(gentity_t* ent, char* password) {

	if (!ent->client->sess.setAdminPassword && ent->client->sess.adminType != ADMTYPE_PASS) {
		G_printInfoMessage(ent, "You cannot change your admin-password as you're either not an admin or you don't have pass-admin powers.");
		return;
	}

	// So there are a couple of entrypoints here.
	// 1. setAdminPassword = qtrue, toBeAdminLevel != ADMLVL_NONE => fresh admin setting their first password.
	// 2. setAdminPassword = qtrue, toBeAdminLevel = ADMLVL_NONE => not-logged in admin who can now reset their password.
	// 3. setAdminPassword = qfalse, adminLevel != ADMLVL_NONE and adminType = ADMTYPE_PASS => player wanting to proactively change their own password.

	if (strlen(password) < 6 || !checkAdminPassword(password)) {
		G_printInfoMessage(ent, "Your password is either too short or consists of only the same characters (can happen if you use Arabic passwords).");
		return;
	}

	int rowsAffected = 0;

	if (ent->client->sess.setAdminPassword) {
		rowsAffected = dbUpdateAdminPass(ent->client->sess.adminName, password);

		if (rowsAffected != 1) {
			logSystem(LOGLEVEL_WARN, "rowsAffected on updateadmpass is %s (!= 1)", rowsAffected);
			G_printInfoMessage(ent, "Something went wrong, please try again...");
			return;
		}

		if (ent->client->sess.toBeAdminLevel == ADMLVL_NONE) {
			G_printInfoMessage(ent, "Your password has been updated. Please use /adm login %s to log in.", password);
		}
		else {
			G_printInfoMessage(ent, "Your password is now set.");
			ent->client->sess.adminLevel = ent->client->sess.toBeAdminLevel;
			ent->client->sess.adminType = ADMTYPE_PASS;
			ent->client->sess.toBeAdminLevel = ADMLVL_NONE;
			logLogin(ent);
		}
		ent->client->sess.setAdminPassword = qfalse;
	}
	else if (ent->client->sess.adminLevel > ADMLVL_NONE && ent->client->sess.adminType == ADMTYPE_PASS) {
		rowsAffected = dbUpdateAdminPass(ent->client->sess.adminName, password);

		if (rowsAffected != 1) {
			logSystem(LOGLEVEL_WARN, "rowsAffected on updateadmpass is %s (!= 1)", rowsAffected);
			G_printInfoMessage(ent, "Something went wrong, please try again...");
			return;
		}

		G_printInfoMessage(ent, "Your password has been updated.");

	}
	else {
		G_printInfoMessage(ent, "You cannot change the password because you're either not an admin or do not have pass-admin rights.");
	}

}

void adm_Login(gentity_t* ent, char* password) {

	if (ent->client->sess.adminLevel > ADMLVL_NONE && ent->client->sess.adminLevel <= ADMLVL_HADMIN) {
		G_printInfoMessage(ent, "You already have admin powers.");
		return;
	}

	if (!password || strlen(password) < 6) {
		G_printInfoMessage(ent, "No or too short password entered. Try again.");
		return;
	}

	admLevel_t adminLevel = dbGetAdminLevel(ADMTYPE_PASS, ent, password);
	Com_Printf("check login with admpass on %s psw %s\n", ent->client->pers.cleanName, password);
	if (adminLevel <= ADMLVL_NONE || adminLevel > ADMLVL_HADMIN) {
		G_printInfoMessage(ent, "No admin powers found with this name+password combination.");
		return;
	}

	char colorFreeAdminName[MAX_NETNAME];
	getCleanAdminNameByAdminLevel(adminLevel, colorFreeAdminName, sizeof(colorFreeAdminName));

	G_printInfoMessageToAll("%s has been granted %s.", ent->client->pers.cleanName, colorFreeAdminName);
	ent->client->sess.adminLevel = adminLevel;
	ent->client->sess.adminType = ADMTYPE_PASS;
	Q_strncpyz(ent->client->sess.adminName, ent->client->pers.cleanName, sizeof(ent->client->sess.adminName));
	logLogin(ent);

}

int adm_adminRemove(int argNum, gentity_t* adm, qboolean shortCmd) {
	// this is removing from the list...

	char removable[64];
	char table[64];
	int rowId;
	admType_t adminType;
	

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(removable, G_GetChatArgument(1), sizeof(removable));
		Q_strncpyz(table, G_GetChatArgument(2), sizeof(table));
	}
	else {
		trap_Argv(argNum, removable, sizeof(removable));
		trap_Argv(argNum + 1, table, sizeof(table));
	}

	rowId = atoi(removable);

	if (rowId < 1) {
		G_printCustomMessage(adm, "Admin Action", "Row %d is not a valid row.", rowId);
		return -1;
	}

	if (!Q_stricmp(table, "pass")) {
		adminType = ADMTYPE_PASS;
	}
	else if (!Q_stricmp(table, "guid")) {
		adminType = ADMTYPE_GUID;
	}
	else {
		adminType = ADMTYPE_IP;
	}

	char adminName[MAX_NETNAME];
	int adminLevel = -1;

	qboolean success = dbGetAdminDataByRowId(adminType, rowId, &adminLevel, adminName, sizeof(adminName));

	if (!success) {
		G_printCustomMessage(adm, "Admin Action", "Row %d was not found.", rowId);
		return -1;
	}

	if (adm && adm->client) {
		// Now check whether admin can actually remove it.

		int comparable = -1;

		if (adminLevel == ADMLVL_BADMIN) comparable = a_badmin.integer;
		else if (adminLevel == ADMLVL_ADMIN) comparable = a_admin.integer;
		else if (adminLevel == ADMLVL_SADMIN) comparable = a_sadmin.integer;
		else if (adminLevel == ADMLVL_HADMIN) comparable = a_hadmin.integer;

		if (comparable == -1) {
			return -1; // this cannot happen...
		}

		if (adm->client->sess.adminLevel < comparable) {
			G_printCustomMessage(adm, "Admin Action", "Your admin level is too low to remove this admin.");
			return -1;
		}
	}

	int rowsAffected = dbRemoveAdminByRowId(adminType, rowId);
	logAdmin(adm, NULL, "removeadmin", adminName);

	if (rowsAffected > 0) {
		// as this was a list remove, also try to find the client and remove their powers if they're online.
		adm_removeIngamePlayerPowers(adminType, adminName, adm);

		// feedback to the function caller.
		G_printCustomMessage(adm, "Admin Action", "Row %d was removed from the list.", rowId);
	}
	else {
		logSystem(LOGLEVEL_WARN, "rowsAffected %d on adminRemove while row was valid initially.", rowsAffected);
		G_printCustomMessage(adm, "Admin Action", "Row %d was not found.", rowId);
	}


	return -1;
}

int adm_adminList(int argNum, gentity_t* adm, qboolean shortCmd) {

	char arg[64];
	admType_t adminType = ADMTYPE_IP;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(1), sizeof(arg));
	}
	else {
		trap_Argv(argNum, arg, sizeof(arg));
	}

	if (!Q_stricmp(arg, "pass")) {
		adminType = ADMTYPE_PASS;
	}
	else if (!Q_stricmp(arg, "guid")) {
		adminType = ADMTYPE_GUID;
	}

	dbPrintAdminlist(adm, adminType);

	return -1;
}

static void addAdmin(int argNum, gentity_t* adm, qboolean shortCmd, admLevel_t admlvl) {
	
	int idNum = G_ClientNumFromArg(adm, argNum, "do this to", qfalse, qfalse, qfalse, shortCmd);

	if (idNum < 0) return;
	gentity_t* ent = g_entities + idNum;

	if (ent->client->sess.adminLevel != ADMLVL_NONE) {
		G_printInfoMessage(adm, "Client %s (%d) already has admin powers.", ent->client->pers.cleanName, idNum);
		return;
	}

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

			return;
		}

		adminType = ADMTYPE_GUID;
	}

	// Check for admin existence in the admin tables.
	int existingAdminLevel = dbGetAdminLevel(adminType, ent, NULL);

	if (existingAdminLevel != -1) {

		if (adminType == ADMTYPE_PASS) {
			ent->client->sess.setAdminPassword = qtrue;
			Q_strncpyz(ent->client->sess.adminName, ent->client->pers.cleanName, sizeof(ent->client->sess.adminName));
			G_printInfoMessage(adm, "Client %s (%d) already had admin powers. They can now change their password.", ent->client->pers.cleanName, idNum);
			G_printChatInfoMessage(ent, "%s has toggled admin-password reset for you.", getNameOrArg(adm, "RCON", qtrue));
			G_printChatInfoMessage(ent, "To do so, issue command /adm pass into console, e.g. /adm pass newpass");
			G_printChatInfoMessage(ent, "That will set your password to newpass");

			logAdmin(adm, ent, "resetpassword", NULL);
			return;
		}
		else {
			G_printInfoMessage(adm, "Client %s (%d) already has admin powers. Please remove them before trying to readd.", ent->client->pers.cleanName, idNum);

			return;
		}

		
	}
	// Client doesn't have admin, so lets perform the action.
	Q_strncpyz(ent->client->sess.adminName, ent->client->pers.cleanName, sizeof(ent->client->sess.adminName));
	dbAddAdmin(adminType, admlvl, ent, adm, NULL);

	if (adminType == ADMTYPE_PASS) {
		ent->client->sess.setAdminPassword = qtrue;
		ent->client->sess.toBeAdminLevel = admlvl;
		G_printChatInfoMessage(ent, "%s has added you to admin passlist as %s.", getNameOrArg(adm, "RCON", qtrue), getAdminNameByAdminLevel(admlvl));
		G_printChatInfoMessage(ent, "For that, you do need to set a password.");
		G_printChatInfoMessage(ent, "To do so, issue command /adm pass into console, e.g. /adm pass newpass");
		G_printChatInfoMessage(ent, "That will set your password to newpass");

	}
	else {
		
		ent->client->sess.adminLevel = admlvl;
		ent->client->sess.adminType = adminType;
	}

	logAdmin(adm, ent, va("addadmin %d", admlvl), NULL);
	G_Broadcast(BROADCAST_GAME, NULL, qtrue, "%s\n^7has been added to %s^7list\nby %s", ent->client->pers.netname, getAdminNameByAdminLevel(admlvl), getNameOrArg(adm, "\\RCON", qfalse));

	char colorFreeAdminName[MAX_NETNAME];
	getCleanAdminNameByAdminLevel(admlvl, colorFreeAdminName, sizeof(colorFreeAdminName));

	G_printCustomMessageToAll("Admin Action", "%s was added to %slist by %s.", ent->client->pers.cleanName, colorFreeAdminName, getNameOrArg(adm, "RCON", qtrue));

}

int adm_addBadmin(int argNum, gentity_t* adm, qboolean shortCmd) {
	addAdmin(argNum, adm, shortCmd, ADMLVL_BADMIN);
	return -1;
}

int adm_addAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {
	addAdmin(argNum, adm, shortCmd, ADMLVL_ADMIN);
	return -1;
}

int adm_addSadmin(int argNum, gentity_t* adm, qboolean shortCmd) {
	addAdmin(argNum, adm, shortCmd, ADMLVL_SADMIN);
	return -1;
}

int adm_addHadmin(int argNum, gentity_t* adm, qboolean shortCmd) {
	addAdmin(argNum, adm, shortCmd, ADMLVL_HADMIN);
	return -1;
}

// mostly taken from 1fx. Mod, missing competitive mode which will be added along with the rest of compmode functionality.
static void adm_toggleCVAR(int argNum, gentity_t* adm, qboolean shortCmd, char* cvarName, vmCvar_t* cvar, qboolean availableInCM, char* cmCvarName, vmCvar_t* cmCvar) {

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
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\%s was changed to %d\nby %s", cvarName, newValue, getNameOrArg(adm, "RCON", qfalse));
		logAdmin(adm, NULL, va("%s %d", cvarName, newValue), NULL);

		// was only sent for rocmod and gold specific, but this shouldn't impact non-ROCMod clients if we send it again over here as well.
		for (int i = 0; i < level.numConnectedClients; i++) {
			DeathmatchScoreboardMessage(&g_entities[level.sortedClients[i]]);
		}
	}
}

int adm_scoreLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, "Scorelimit", &g_scorelimit, qtrue, NULL, NULL);
	return -1;
}

int adm_timeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, "Timelimit", &g_timelimit, qtrue, NULL, NULL);
	return -1;
}

int adm_swapTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	swapTeams(qfalse);
	logAdmin(adm, NULL, "swapteams", NULL);
	return -1;
}

// JANFIXME compmode
int adm_Rounds(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

static void adm_unPlant(gentity_t* adm, gentity_t* ent) {

	if (!ent->client->sess.planted) return;

	ent->client->ps.origin[2] += 65;
	VectorCopy(ent->client->ps.origin, ent->s.origin);
	ent->client->sess.planted = qfalse;

	G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\unplanted\nby %s", getNameOrArg(ent, NULL, qfalse), getNameOrArg(adm, "RCON", qfalse));
	G_printCustomMessageToAll("Admin Action", "%s was unplanted by %s", getNameOrArg(ent, NULL, qtrue), getNameOrArg(adm, "RCON", qtrue));

	G_ClientSound(ent, G_SoundIndex("sound/misc/confused/wood_break.mp3"));
}

int adm_Plant(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "plant", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.planted) {
			adm_unPlant(adm, ent);
			return -1;
		}
		ent->client->sess.planted = qtrue;

		if (ent->client->ps.pm_flags & PMF_DUCKED) {
			ent->client->ps.origin[2] -= 40;
		}
		else {
			ent->client->ps.origin[2] -= 65;
		}

		VectorCopy(ent->client->ps.origin, ent->s.origin);
		ent->client->sess.planted = qtrue;

		G_ClientSound(ent, G_SoundIndex("sound/misc/confused/wood_break.mp3"));
	}

	return idNum;
}

int adm_roundTimeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, "Round Timelimit", &g_roundtimelimit, qfalse, NULL, NULL);
	return -1;
}

int adm_Runover(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "runover", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		runoverPlayer(&g_entities[idNum]);
	}

	return idNum;
}

int adm_Rollercoaster(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "rollercoaster", qtrue, qtrue, qfalse, shortCmd);
	
	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];
		int coasterTimes;
		char arg[MAX_SAY_TEXT];

		if (shortCmd && G_GetChatArgumentCount()) {
			coasterTimes = atoi(G_GetChatArgument(argNum + 1));
		}
		else {
			trap_Argv(argNum + 1, arg, sizeof(arg));
			coasterTimes = atoi(arg);
		}

		if (coasterTimes < 2) {
			coasterTimes = 2;
		}
		else if (coasterTimes > 10) {
			coasterTimes = 10;
		}

		ent->client->sess.coaster = coasterTimes * 3;
		ent->client->sess.nextCoasterTime = level.time;

	}

	return idNum;
}

int adm_Respawn(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "rollercoaster", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.team == TEAM_SPECTATOR) {
			G_printInfoMessage(adm, "You cannot respawn a spectator.");
			return -1;
		}

		if (ent->client->sess.ghost) {
			G_StopFollowing(ent);
			ent->client->ps.pm_flags &= ~PMF_GHOST;
			ent->client->ps.pm_type = PM_NORMAL;
			ent->client->sess.ghost = qfalse;
		}
		else {
			TossClientItems(ent);
		}

		ent->client->sess.noTeamChange = qfalse;
		trap_UnlinkEntity(ent);
		ClientSpawn(ent);
		G_ClientSound(ent, G_SoundIndex("sound/ambience/vehicles/telephone_pole.mp3")); 
	}

	return idNum;
}

// JANFIXME check this with compmode.
int adm_mapRestart(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Strip(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "strip", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		stripClient(&g_entities[idNum], qtrue);
	}

	return idNum;
}

int adm_removeAdmin(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "strip", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.adminLevel == ADMLVL_NONE) {
			G_printInfoMessage(adm, "Client %s (%d) is not an admin.", ent->client->pers.cleanName, idNum);
			return -1;
		}

		if (
			(ent->client->sess.adminLevel == ADMLVL_BADMIN && a_badmin.integer > getAdminLevel(adm)) ||
			(ent->client->sess.adminLevel == ADMLVL_ADMIN && a_admin.integer > getAdminLevel(adm)) ||
			(ent->client->sess.adminLevel == ADMLVL_SADMIN && a_sadmin.integer > getAdminLevel(adm)) ||
			(ent->client->sess.adminLevel == ADMLVL_HADMIN && a_hadmin.integer > getAdminLevel(adm))
		) {
			G_printInfoMessage(adm, "You're not privileged enough to remove their admin.");
			return -1;
		}

		// Figure out whether we need to perform a DB removal.
		if (ent->client->sess.adminType == ADMTYPE_IP || ent->client->sess.adminType == ADMTYPE_GUID || ent->client->sess.adminType == ADMTYPE_PASS) {
			dbRemoveAdminByGentity(ent);
		}

		ent->client->sess.adminType = ADMTYPE_NONE;
		ent->client->sess.adminLevel = ADMLVL_NONE;

		G_printCustomMessageToAll("Admin Action", "%s powers were removed by %s", ent->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue));
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\ntheir powers were \\removed\nby %s", ent->client->pers.netname, getNameOrArg(adm, "\\RCON", qfalse));
		logAdmin(adm, ent, "removeadmin", NULL);

	}
	return -1;
}

int adm_forceTeam(int argNum, gentity_t* adm, qboolean shortCmd) {

	char arg[64];
	char* teamChar = "s";
	team_t team;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(2), sizeof(arg));
	}
	else {
		trap_Argv(argNum + 1, arg, sizeof(arg));
	}

	if (strlen(arg) > 0) {
		if (tolower(arg[0]) == 'r') {
			teamChar = "r";
			team = TEAM_RED;
		}
		else if (tolower(arg[0]) == 'b') {
			teamChar = "b";
			team = TEAM_BLUE;
		}
		else if (tolower(arg[0]) == 's') {
			teamChar = "s";
			team = TEAM_SPECTATOR;
		}
		else {
			G_printInfoMessage(adm, "Unknown team specified. Valid values: blue, red, spec");
			return -1;
		}
	}

	

	int idNum = G_ClientNumFromArg(adm, argNum, "forceteam", qfalse, qtrue, qtrue, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		SetTeam(ent, teamChar, NULL, qtrue);
	}

	return idNum;
}

int adm_blockSeek(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (Q_stricmp(g_gametype.string, "h&s")) {
		G_printInfoMessage(adm, "This command only works in H&S gametype.");
	}

	int idNum = G_ClientNumFromArg(adm, argNum, "blockseek", qtrue, qfalse, qfalse, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.blockseek) {
			ent->client->sess.blockseek = qfalse;
			G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\ncan seek again", ent->client->pers.netname);
			G_printCustomMessageToAll("Admin Action", "%s can seek again.", ent->client->pers.cleanName);
		}
		else {
			if (ent->client->sess.team == TEAM_BLUE) {
				SetTeam(ent, "r", NULL, qtrue);
			}

			ent->client->sess.blockseek = qtrue;

			G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\blocked from seeking\nby %s", ent->client->pers.netname, getNameOrArg(adm, "\\RCON", qfalse));
			G_printCustomMessageToAll("Admin Action", "%s was blocked from seeking by %s.", ent->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue));
			logAdmin(adm, ent, "blockseek", NULL);
		}
	}

	return -1;
}

/* JANFIXME build it when it becomes useful (e.g. with H&S) */
int adm_blockSeekList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}
/* nosection entity dependent. */
int adm_noLower(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

/* nosection entity dependent. */
int adm_noRoof(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

/* nosection entity dependent. */
int adm_noMiddle(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

/* nosection entity dependent. */
int adm_noWhole(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}


int adm_shuffleTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	int shuffle = shuffleTeams(qfalse);
	if (shuffle == TEAMACTION_DONE) {
		G_Broadcast(BROADCAST_CMD, NULL, qfalse, "\\Shuffleteams\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "Shuffleteams by %s", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "shuffleteams", NULL);
	}
	else if (shuffle == TEAMACTION_INCOMPATIBLE_GAMETYPE) {
		G_printInfoMessage(adm, "You cannot shuffle teams in this gametype");
	}
	else if (shuffle == TEAMACTION_NOT_ENOUGH_PLAYERS) {
		G_printInfoMessage(adm, "There aren't enough players in the game to have a meaningful shuffle.");
	}
	
	return -1;
}

int adm_noNades(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_respawnInterval(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, "g_respawnInterval", &g_respawnInterval, qfalse, NULL, NULL);
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

	// Cannot restart the gametype in intermission mode.
	if (level.intermissionQueued || level.intermissiontime) {
		G_printInfoMessage(adm, "You cannot restart the gametype while being in intermission.");
		return -1;
	}

	// Cannot restart the gametype while being paused.
	if (level.paused) {
		G_printInfoMessage(adm, "You cannot restart the gametype while the game is paused.");
		return -1;
	}

	// Broadcast the change and restart it.
	G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\Gametype restart!");
	logAdmin(adm, NULL, "gametype restart", NULL);
	G_printCustomMessageToAll("Admin Action", "Gametype restart by %s.", getNameOrArg(adm, "RCON", qtrue));
	trap_SendConsoleCommand(EXEC_APPEND, "gametype_restart\n");
	return -1;
}

/* Do we even want clanmember stuff? */
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

/* Build together with rest of the compmode logic */
int adm_compMode(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_banList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

static void getBanDuration(const char* durationArg, int* duration) {

	if (!durationArg || !*durationArg) {
		duration[0] = -1;
		return;
	}

	if (!Q_stricmp(durationArg, "eom")) {
		duration[0] = 0;
		return;
	}

	int days = 0, hours = 0, minutes = 0;
	const char* ptr = durationArg;
	qboolean parsed = qfalse;

	while (*ptr) {

		while (*ptr && !isdigit(*ptr)) {
			ptr++;
		}

		if (!*ptr) break; 

		// Parse the numeric part
		char* end;
		long value = strtol(ptr, &end, 10);
		if (value < 0 || end == ptr) {
			duration[0] = -1; // Invalid input
			return;
		}

		// Check the unit character
		ptr = end;
		if (*ptr == 'd') {
			days += value;
			parsed = qtrue;
		}
		else if (*ptr == 'h') {
			hours += value;
			parsed = qtrue;
		}
		else if (*ptr == 'm') {
			minutes += value;
			parsed = qtrue;
		}

		if (*ptr) ptr++; // Move to the next character
	}

	if (!parsed) {
		duration[0] = -1; // Invalid input if nothing valid was parsed
		return;
	}

	// If we reach here, the input was valid
	duration[0] = 1;
	duration[1] = days;
	duration[2] = hours;
	duration[3] = minutes;
}

int adm_Ban(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "ban", qtrue, qtrue, qtrue, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];
		char durationArg[64];

		if (G_GetChatArgumentCount() >= 2 && shortCmd) {
			strncpy(durationArg, G_GetChatArgument(argNum + 1), sizeof(durationArg));
		}
		else {
			trap_Argv(argNum + 1, durationArg, sizeof(durationArg));
		}

		int duration[4];

		getBanDuration(durationArg, duration);

		Com_Printf("From durationArg %s I managed to grab out retval %d, %d days, %d hours, %d minutes.\n", durationArg, duration[0], duration[1], duration[2], duration[3]);

	}

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

	gentity_t* ent = &g_entities[idNum];

	G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\%s%s\nby %s", ent->client->pers.netname, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", getNameOrArg(adm, "\\RCON", qfalse));
	G_printCustomMessageToAll("Admin Action", "%s was %s%s by %s", ent->client->pers.cleanName, adminCommands[funcNum].adminCmd, (adminCommands[funcNum].suffix != NULL) ? adminCommands[funcNum].suffix : "", getNameOrArg(adm, "RCON", qtrue));

	logAdmin(adm, ent, adminCommands[funcNum].adminCmd, NULL);
	G_GlobalSound(level.actionSoundIndex);

}

