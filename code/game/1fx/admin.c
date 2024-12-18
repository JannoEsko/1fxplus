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
	{"!mr",     "maprestart",       &minimumAdminLevel,       &adm_mapRestart,                "Restart the current map",          "",                 NULL},
	{"!mr",     "map_restart",      &minimumAdminLevel,       &adm_mapRestart,                "Restart the current map",          "",                 NULL},
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
	{"!fe",    "followenemy",        &minimumAdminLevel,       &adm_followEnemy,                 "Toggles followenemy", "",        NULL },
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
	ent->client->sess.setAdminPassword = qfalse;
}

int adm_adminRemove(int argNum, gentity_t* adm, qboolean shortCmd) {
	// this is removing from the list...

	char removable[64];
	char table[64];
	int rowId;
	admType_t adminType;
	

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(removable, G_GetChatArgument(argNum, qfalse), sizeof(removable));
		Q_strncpyz(table, G_GetChatArgument(argNum + 1, qfalse), sizeof(table));
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

	char arg[64], pageArg[64];
	admType_t adminType = ADMTYPE_IP;
	int page = 0;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
		Q_strncpyz(pageArg, G_GetChatArgument(argNum + 1, qfalse), sizeof(pageArg));
	}
	else {
		trap_Argv(argNum, arg, sizeof(arg));
		trap_Argv(argNum + 1, pageArg, sizeof(pageArg));
	}

	if (!Q_stricmp(arg, "pass")) {
		adminType = ADMTYPE_PASS;
		if (pageArg && strlen(pageArg)) {
			page = atoi(pageArg);
		}
	}
	else if (!Q_stricmp(arg, "guid")) {
		adminType = ADMTYPE_GUID;
		if (pageArg && strlen(pageArg)) {
			page = atoi(pageArg);
		}
	}
	else if (arg && strlen(arg)) {
		page = atoi(arg);
	}
	dbPrintAdminlist(adm, adminType, page);

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
		Q_strncpyz(arg, G_GetChatArgument(argNum + 1, qfalse), sizeof(arg));
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
			G_printInfoMessage(adm, "Client %s (%d) does not have Rox AC running or the verification failed / hasn't passed yet.", ent->client->pers.cleanName, idNum);

			return;
		}
		else if (!g_useSecureRoxVerification.integer) {
			G_printInfoMessage(adm, "You cannot use GUID based admins without secure Rox Verification.");
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
static void adm_toggleCVAR(int argNum, gentity_t* adm, qboolean shortCmd, qboolean isToggle, char* cvarName, vmCvar_t* cvar, qboolean availableInCM, char* cmCvarName, vmCvar_t* cmCvar) {

	char* arg = G_GetArg(argNum, shortCmd, qfalse);
	int newValue = 0;

	if (cm_state.integer == COMPMODE_INITIALIZED) {
		newValue = arg&& strlen(arg) > 0 ? atoi(arg) : (isToggle && cmCvar ? !cmCvar->integer : -1);
		if (newValue < 0) {
			G_printInfoMessage(adm, "%s is %d.", cmCvarName, cmCvar->integer);
		}
		else {
			// means change
			if (cmCvar) {
				G_setTrackedCvarWithoutTrackMessage(cmCvar, newValue);
			}
			else {
				trap_Cvar_Set(cmCvarName, va("%d", newValue));
				trap_Cvar_Update(cmCvar);
			}

			if (isToggle) {
				G_printInfoMessageToAll("%s %s by %s.", cvarName, newValue ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qtrue));
			}
			else {
				G_printInfoMessageToAll("%s was changed to %d by %s.", cvarName, newValue, getNameOrArg(adm, "RCON", qtrue));
			}
			G_GlobalSound(level.actionSoundIndex);
		}
	}
	else {
		newValue = arg && strlen(arg) > 0 ? atoi(arg) : (isToggle && cvar ? !cvar->integer : -1);
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

			if (isToggle) {
				G_printInfoMessageToAll("%s %s by %s.", cvarName, newValue ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qtrue));
				G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\%s %s\nby %s", cvarName, newValue ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qfalse));
				logAdmin(adm, NULL, va("%s %s", cvarName, newValue ? "enabled" : "disabled"), NULL);
			}
			else {
				G_printInfoMessageToAll("%s was changed to %d by %s.", cvarName, newValue, getNameOrArg(adm, "RCON", qtrue));
				G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\%s was changed to %d\nby %s", cvarName, newValue, getNameOrArg(adm, "RCON", qfalse));
				logAdmin(adm, NULL, va("%s %d", cvarName, newValue), NULL);
			}

			// was only sent for rocmod and gold specific, but this shouldn't impact non-ROCMod clients if we send it again over here as well.
			for (int i = 0; i < level.numConnectedClients; i++) {
				DeathmatchScoreboardMessage(&g_entities[level.sortedClients[i]]);
			}
		}
	}
}


int adm_followEnemy(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, qtrue, "Follow Enemy", &g_followEnemy, qtrue, "Follow Enemy in Match", &match_followEnemy);
	return -1;
}

int adm_scoreLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, qfalse, "Scorelimit", &g_scorelimit, qtrue, "match_scorelimit", &match_scorelimit);
	return -1;
}

int adm_timeLimit(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, qfalse, "Timelimit", &g_timelimit, qtrue, "match_timelimit", &match_timelimit);
	return -1;
}

int adm_swapTeams(int argNum, gentity_t* adm, qboolean shortCmd) {
	swapTeams(qfalse);
	logAdmin(adm, NULL, "swapteams", NULL);
	return -1;
}

int adm_Rounds(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (cm_state.integer != COMPMODE_INITIALIZED) {
		G_printInfoMessage(adm, "This command only works during the initialization phase of competition.");
		return -1;
	}

	if (match_doublerounds.integer) {
		trap_Cvar_Set("match_doublerounds", "0");
		trap_Cvar_Update(&match_doublerounds);
		G_printCustomMessageToAll("Competitition Mode", "Double rounds disabled!");
	}
	else {
		trap_Cvar_Set("match_doublerounds", "1");
		trap_Cvar_Update(&match_doublerounds);
		G_printCustomMessageToAll("Competitition Mode", "Double rounds enabled!");
	}

	G_GlobalSound(level.actionSoundIndex);

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
	adm_toggleCVAR(argNum, adm, shortCmd, qfalse, "Round Timelimit", &g_roundtimelimit, qfalse, NULL, NULL);
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
			coasterTimes = atoi(G_GetChatArgument(argNum + 1, qfalse));
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

int adm_mapRestart(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (adm && adm->client && adm->client->sess.adminLevel < a_compmode.integer && cm_state.integer > COMPMODE_NONE) {
		G_printInfoMessage(adm, "You're not privileged enough to run map restart in compmode.");
		return -1;
	}

	if (adm && adm->client && adm->client->sess.adminLevel < a_mapswitch.integer && cm_state.integer == COMPMODE_NONE) {
		G_printInfoMessage(adm, "You're not privileged enough to run map restart.");
		return -1;
	}

	if (cm_state.integer == COMPMODE_INITIALIZED || cm_state.integer == COMPMODE_PRE_ROUND2) {

		level.proceedToNextCompState = qtrue;
		level.nextCompState = level.time + 3000;

		// If we don't play a dual-round game, we push the state further.
		if (!match_doublerounds.integer) {
			trap_Cvar_Set("cm_state", va("%d", COMPMODE_PRE_ROUND2));
			trap_Cvar_Update(&cm_state);
			G_Broadcast(BROADCAST_GAME_IMPORTANT, NULL, qtrue, "Starting the \\game...");
		} else if (cm_state.integer == COMPMODE_INITIALIZED) {
			G_Broadcast(BROADCAST_GAME_IMPORTANT, NULL, qtrue, "Starting the \\game...");
			logAdmin(adm, NULL, "map_restart", "compmode round 1");
		}
		else {
			G_Broadcast(BROADCAST_GAME_IMPORTANT, NULL, qtrue, "Starting \\round 2...");
			logAdmin(adm, NULL, "map_restart", "compmode round 2");
		}
	}
	else if (cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) {

		// When in a round, there are 2 chances to call out a map restart.
		// 1. call it out so you can get to the next compmode state faster.
		// 2. call it out to actually "re-restart" the match.

		if (level.intermissionQueued || level.intermissiontime) {
			level.proceedToNextCompState = qtrue;
			level.nextCompState = level.time + 3000;
			G_printInfoMessageToAll("Competition mode will %s in 3 seconds", cm_state.integer == COMPMODE_ROUND1 ? "proceed to the next stage" : "end");
			// We don't log this action as this is done quite commonly to speed up the transitions (skip awards etc).
			// We also do not broadcast as most likely, another broadcast message is being displayed at this time.
		}
		else if (level.mapAction == MAPACTION_NONE) {
			logAdmin(adm, NULL, "map_restart", "within compmode");
			G_Broadcast(BROADCAST_GAME, NULL, qtrue, "%s\nhas triggered a map \\restart", getNameOrArg(adm, "\\RCON", qfalse));
			level.mapAction = MAPACTION_PENDING_RESTART;
			level.runMapAction = level.time + 3000;
		}
		else {
			printMapActionDenialReason(adm);
		}

	}
	else if (level.mapAction == MAPACTION_NONE) {

		int when = 0;
		char arg[MAX_SAY_TEXT];

		if (shortCmd && G_GetChatArgumentCount()) {
			Q_strncpyz(arg, G_GetChatArgument(argNum + 1, qfalse), sizeof(arg));
		}
		else {
			trap_Argv(argNum + 1, arg, sizeof(arg));
		}

		if (Q_stricmp(arg, "now") && strlen(arg)) {

			when = atoi(arg);

			if (when > 15) {
				G_printInfoMessage(adm, "Maximum delay time is 15 seconds, setting to 15.");
			}
			else if (when < 0) {
				G_printInfoMessage(adm, "Minimum delay time is 0 seconds, setting it to 5.");
				when = 5;
			}

			when = Com_Clamp(0, 15, when);

		}
		else if (!strlen(arg)) {
			when = 5;
		}
		else {
			when = 0;
		}

		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas ordered a map restart in %d \\seconds...", getNameOrArg(adm, "\\RCON", qfalse), when);
		G_printCustomMessageToAll("Admin Action", "%s has ordered a map restart in %d seconds.", getNameOrArg(adm, "RCON", qtrue), when);
		logAdmin(adm, NULL, "map_restart", NULL);
		level.mapAction = MAPACTION_PENDING_RESTART;
		level.runMapAction = level.time + (when * 1000);
	}
	else {
		printMapActionDenialReason(adm);
	}

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
		Q_strncpyz(arg, G_GetChatArgument(argNum + 1, qfalse), sizeof(arg));
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

	if (isCurrentGametype(GT_HNS)) {
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

int adm_blockSeekList(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (isCurrentGametype(GT_HNS)) {

		qboolean isRcon = adm && adm->client ? qfalse : qtrue;

		if (isRcon) {
			Com_Printf("\n[^3Blockseek^7]\n");
			Com_Printf("%-5.5s%-25.25s\n-----------------------------------------------------------------------------\n", "ID", "Name");
		}
		else {
			trap_SendServerCommand(adm - g_entities, "print \"\n[^3Blockseek^7]\n\"");
			trap_SendServerCommand(adm - g_entities, va("print \"%-5.5s%-25.25s\n-----------------------------------------------------------------------------\n\"", "ID", "Name"));

		}

		for (int i = 0; i < level.numConnectedClients; i++) {
			gentity_t* ent = &g_entities[level.sortedClients[i]];

			if (ent->client->sess.blockseek) {

				if (isRcon) {
					Com_Printf("%-5.5s%-25.25s\n", ent->s.number, ent->client->pers.cleanName);
				}
				else {
					trap_SendServerCommand(adm - g_entities, va("print \"%-5.5s%-25.25s\n\"", ent->s.number, ent->client->pers.cleanName));
				}

			}
		}

		if (isRcon) {
			Com_Printf("\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n");
		}
		else {
			trap_SendServerCommand(adm - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n\"");
		}

	}
	else {
		G_printInfoMessage(adm, "This command only works in H&S.");
	}

	return -1;
}

/*
================
adm_toggleSection

Disables or enables a section if an entity of such kind is specified on the current map.
================
*/

static void adm_toggleSection(gentity_t* adm, mapSection_t mapSection) {

	char* sectionName = "Nolower";
	int useSection = g_useNoLower.integer;

	if (mapSection == MAPSECTION_NOMIDDLE) {
		sectionName = "Nomiddle";
		useSection = g_useNoMiddle.integer;
	}
	else if (mapSection == MAPSECTION_NOWHOLE) {
		sectionName = "Nowhole";
		useSection = g_useNoWhole.integer;
	}
	else if (mapSection == MAPSECTION_NOROOF) {
		sectionName = "Noroof";
		useSection = g_useNoRoof.integer;
	}

	char sectionLowercase[10];
	Q_strncpyz(sectionLowercase, sectionName, sizeof(sectionLowercase));
	Q_strlwr(sectionLowercase);

	// Boe!Man 2/27/11: If people don't want to use this section they can specify to disable it.
	if (useSection <= 0) {
		G_printInfoMessage(adm, "%s has been disabled on this server.", sectionName);
		return;
	}
	// Boe!Man 1/8/12: If people want to use nolower but if there's no such entity found, inform the user.
	if (!level.noSectionEntFound[mapSection]) {
		G_printInfoMessage(adm, "No entity found to toggle %s.", sectionLowercase);
		return;
	}

	level.autoSectionActive[mapSection] = !level.autoSectionActive[mapSection];
	qboolean enabled = level.autoSectionActive[mapSection];

	// Custom broadcasts.
	G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\%s %s!", sectionName, enabled ? "enabled" : "disabled");
	logAdmin(adm, NULL, va("%s %s", sectionName, enabled ? "enabled" : "disabled"), NULL);
	G_printCustomMessageToAll("Admin Action", "%s has been %s by %s.", sectionName, enabled ? "enabled" : "disabled", getNameOrArg(adm, "RCON", qtrue));

	// Boe!Man 11/24/13: Also open the section.
	if (g_useAutoSections.integer) {
		if (!level.autoSectionActive[mapSection]) {
			gentity_t* ent = NULL;
			while (NULL != (ent = G_Find(ent, FOFS(classname), sectionLowercase))) {
				sectionAddOrDelInstances(ent, level.autoSectionActive[mapSection]);
				ent->sectionState = MAPSECTIONSTATE_OPENED; // Reset state to Opened.
			}

		}
	}
	else {
		gentity_t* ent = NULL;
		while (NULL != (ent = G_Find(ent, FOFS(classname), sectionLowercase))) {
			sectionAddOrDelInstances(ent, enabled);
			ent->sectionState = enabled ? MAPSECTIONSTATE_CLOSED : MAPSECTIONSTATE_OPENED; // Reset state to Opened.
		}
	}
	
}


/* nosection entity dependent. */
int adm_noLower(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleSection(adm, MAPSECTION_NOLOWER);
	return -1;
}

/* nosection entity dependent. */
int adm_noRoof(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleSection(adm, MAPSECTION_NOROOF);
	return -1;
}

/* nosection entity dependent. */
int adm_noMiddle(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleSection(adm, MAPSECTION_NOMIDDLE);
	return -1;
}

/* nosection entity dependent. */
int adm_noWhole(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleSection(adm, MAPSECTION_NOWHOLE);
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
	adm_toggleCVAR(argNum, adm, shortCmd, qfalse, "g_respawnInterval", &g_respawnInterval, qfalse, NULL, NULL);
	return -1;
}

int adm_realDamage(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_PROP, GT_MM, GT_MAX })) {
		G_printInfoMessage(adm, "You can't use realdamage in this gametype.");
		return -1;
	}

	qboolean result = weaponMod(WEAPONMOD_RD, NULL);

	if (!result) {
		G_printInfoMessage(adm, "Realdamage weapon file not found, please contact the server owner.");
	}
	else {
		G_Broadcast(BROADCAST_GAME, NULL, qtrue, "\\Realdamage\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "Realdamage by %s", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "damagemod", "realdamage");

		// We need to reset outfitting.
		for (int i = 0; i < level.numConnectedClients; i++) {
			level.clients[level.sortedClients[i]].noOutfittingChange = qfalse;
			G_UpdateOutfitting(g_entities[level.sortedClients[i]].s.number);
		}
	}

	return -1;
}

int adm_normalDamage(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_PROP, GT_MM, GT_MAX })) {
		G_printInfoMessage(adm, "You can't use normaldamage in this gametype.");
		return -1;
	}

	qboolean result = weaponMod(WEAPONMOD_ND, NULL);

	if (!result) {
		G_printInfoMessage(adm, "Normaldamage weapon file not found, please contact the server owner.");
	}
	else {
		G_Broadcast(BROADCAST_GAME, NULL, qtrue, "\\Normal damage\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "Normal damage by %s", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "damagemod", "normaldamage");

		// We need to reset outfitting.
		for (int i = 0; i < level.numConnectedClients; i++) {
			level.clients[level.sortedClients[i]].noOutfittingChange = qfalse;
			G_UpdateOutfitting(g_entities[level.sortedClients[i]].s.number);
		}
	}

	return -1;
}

int adm_customDamage(int argNum, gentity_t* adm, qboolean shortCmd) {

	char customDamageMod[64];

	int argc = shortCmd ? G_GetChatArgumentCount() : 0;
	if (shortCmd && argc > 0) {
		Q_strncpyz(customDamageMod, G_GetChatArgument(argNum, qfalse), sizeof(customDamageMod));
	}
	else {
		trap_Argv(argNum, customDamageMod, sizeof(customDamageMod));
	}

	if (strlen(customDamageMod) > 0) {
		qboolean result = weaponMod(WEAPONMOD_CUSTOM, customDamageMod);

		if (!result) {
			G_printInfoMessage(adm, "Custom weapon file not found, please contact the server owner.");
		}
		else {
			G_Broadcast(BROADCAST_GAME, NULL, qtrue, "\\Custom damage\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
			G_printCustomMessageToAll("Admin Action", "Custom damage by %s", getNameOrArg(adm, "RCON", qtrue));
			logAdmin(adm, NULL, "damagemod", va("customdamage %s", customDamageMod));

			// We need to reset outfitting.
			for (int i = 0; i < level.numConnectedClients; i++) {
				level.clients[level.sortedClients[i]].noOutfittingChange = qfalse;
				G_UpdateOutfitting(g_entities[level.sortedClients[i]].s.number);
			}

		}

	}
	else {
		G_printInfoMessage(adm, "Please specify the custom weapon mod file to use.");
	}

	
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

int adm_addClanMember(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "do this to", qfalse, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.clanMember) {
			G_printInfoMessage(adm, "%s is already a clan member.", ent->client->pers.cleanName);
			return -1;
		}

		// Figure out whether an additional arg was given.
		char arg[64];
		clanType_t clanType = CLANTYPE_IP;
		if (shortCmd && G_GetChatArgumentCount()) {
			Q_strncpyz(arg, G_GetChatArgument(argNum + 1, qfalse), sizeof(arg));
		}
		else {
			trap_Argv(argNum + 1, arg, sizeof(arg));
		}

		if (arg && strlen(arg)) {
			if (!Q_stricmp(arg, "pass")) {
				clanType = CLANTYPE_PASS;
			}
			else if (!Q_stricmp(arg, "guid")) {

				if (!ent->client->sess.hasRoxAC) {
					G_printInfoMessage(adm, "%s (%d) is not using Rox AC.", ent->client->pers.cleanName, idNum);
					return -1;
				}

				clanType = CLANTYPE_GUID;

			}
		}

		// Run a check for existence first.

		qboolean isMember = dbGetClan(clanType, ent, NULL);

		if (isMember) {

			if (clanType == CLANTYPE_PASS) {

				ent->client->sess.setClanPassword = qtrue;
				G_printInfoMessage(adm, "%s (%d) is already in the clan-member passlist. They can now change their password.", ent->client->pers.cleanName, idNum);
				G_printChatInfoMessage(ent, "%s has toggled clan-password reset for you.", getNameOrArg(adm, "RCON", qtrue));
				G_printChatInfoMessage(ent, "To do so, issue command /clan pass into console, e.g. /clan pass newpass");
				G_printChatInfoMessage(ent, "That will set your password to newpass");
				logAdmin(adm, ent, "resetclanpassword", NULL);
				return -1;
			}
			else {
				// Theoretically this is not a possibility. GUID or IP based clan, but they do not have it. But...
				G_printInfoMessage(adm, "%s (%d) is already in the clanmember list.", ent->client->pers.cleanName, idNum);
				return -1;
			}
		}

		// good to go.
		Q_strncpyz(ent->client->sess.clanName, ent->client->pers.cleanName, sizeof(ent->client->sess.clanName));
		ent->client->sess.clanType = clanType;
		dbAddClan(clanType, ent, adm, NULL);
		if (clanType == CLANTYPE_PASS) {
			ent->client->sess.setClanPassword = qtrue;
			G_printChatInfoMessage(ent, "%s has added you to the clanlist with password.", getNameOrArg(adm, "RCON", qtrue));
			G_printChatInfoMessage(ent, "To do so, issue command /clan pass into console, e.g. /clan pass newpass");
			G_printChatInfoMessage(ent, "That will set your password to newpass");
		}
		else {
			ent->client->sess.clanMember = qtrue;
		}

		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas been added to the \\clanlist\nby %s", ent->client->pers.netname, getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "%s has been added to the clanlist by %s.", ent->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, ent, "addclan", NULL);

	}

	return -1;
}

int adm_removeClanMember(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "do this to", qfalse, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.clanMember) {
			int rowsAffected = dbRemoveClanByGentity(ent);

			if (rowsAffected) {
				ent->client->sess.clanMember = qfalse;
				ent->client->sess.clanType = CLANTYPE_NONE;
				Com_Memset(ent->client->sess.clanName, 0, sizeof(ent->client->sess.clanName));
				logAdmin(adm, ent, "removeclan", NULL);
				G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\ntheir clan status was \\removed\nby %s", ent->client->pers.netname, getNameOrArg(adm, "\\RCON", qfalse));
				G_printCustomMessageToAll("Admin Action", "%s clan status was removed by %s.", ent->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue));
			}
			else {
				G_printInfoMessage(adm, "Something went wrong with your action.");
				logSystem(LOGLEVEL_WARN, "removeClanMember: !rowsAffected (%d)", rowsAffected);
			}
		}
		else {
			G_printInfoMessage(adm, "%s (%d) does not have clan powers.", ent->client->pers.cleanName, idNum);
		}

	}

	return -1;
}

static void adm_removeIngameClanPowers(clanType_t clanType, char* clanName, gentity_t* adm) {

	for (int i = 0; i < level.numConnectedClients; i++) {
		gentity_t* ent = &g_entities[level.sortedClients[i]];

		if (ent->client->sess.clanType == clanType && !Q_stricmp(ent->client->sess.clanName, clanName)) {
			ent->client->sess.clanType = CLANTYPE_NONE;
			ent->client->sess.clanMember = qfalse;
			Com_Memset(ent->client->sess.clanName, 0, sizeof(ent->client->sess.clanName));

			G_printInfoMessage(ent, "Your clan powers were removed by %s.", getNameOrArg(adm, "RCON", qtrue));
			return;
		}
	}

}

int adm_removeClanMemberFromList(int argNum, gentity_t* adm, qboolean shortCmd) {

	char arg[64];
	int rowId = 0;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
	}
	else {
		trap_Argv(argNum, arg, sizeof(arg));
	}

	if (arg && strlen(arg)) {
		rowId = atoi(arg);
	}

	if (rowId) {

		char clanName[MAX_NETNAME];
		clanType_t clanType = CLANTYPE_NONE;

		qboolean success = dbGetClanDataByRowId(rowId, clanName, sizeof(clanName), &clanType);

		if (success) {
			int rowsAffected = dbRemoveClanByRowId(rowId);

			if (rowsAffected) {

				adm_removeIngameClanPowers(clanType, clanName, adm);

				G_printCustomMessage(adm, "Admin Command", "Row %d was removed.", rowId);
				logAdmin(adm, NULL, "removeclan", NULL);
			}
			else {
				G_printInfoMessage(adm, "Row %d was not found.", rowId);
				logSystem(LOGLEVEL_WARN, "!rowsAffected removeClan but initial call was success?");
			}
		}
		else {
			G_printInfoMessage(adm, "Row %d was not found.", rowId);
		}

		
	}
	else {
		G_printInfoMessage(adm, "Please enter a valid row ID");
	}

	
	return -1;
}

int adm_clanList(int argNum, gentity_t* adm, qboolean shortCmd) {

	char arg[64], pageArg[64];
	clanType_t clanType = CLANTYPE_NONE;
	int page = 0;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
		Q_strncpyz(pageArg, G_GetChatArgument(argNum + 1, qfalse), sizeof(pageArg));
	}
	else {
		trap_Argv(argNum, arg, sizeof(arg));
		trap_Argv(argNum + 1, pageArg, sizeof(pageArg));
	}

	if (!Q_stricmp(arg, "pass")) {
		clanType = CLANTYPE_PASS;
		if (pageArg && strlen(pageArg)) {
			page = atoi(pageArg);
		}
	}
	else if (!Q_stricmp(arg, "guid")) {
		clanType = CLANTYPE_GUID;
		if (pageArg && strlen(pageArg)) {
			page = atoi(pageArg);
		}
	}
	else if (!Q_stricmp(arg, "ip")) {
		clanType = CLANTYPE_IP;
		if (pageArg && strlen(pageArg)) {
			page = atoi(pageArg);
		}
	}
	else if (arg && strlen(arg)) {
		page = atoi(arg);
	}

	dbPrintClanlist(adm, clanType, page);

	return -1;
}

/* Build together with rest of the compmode logic */
int adm_compMode(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (cm_state.integer == COMPMODE_NONE) {

		trap_Cvar_Set("cm_state", va("%d", COMPMODE_INITIALIZED));
		trap_Cvar_Update(&cm_state);

		trap_Cvar_Set("cm_originalsl", va("%d", g_scorelimit.integer));
		trap_Cvar_Set("cm_originaltl", va("%d", g_timelimit.integer));

		trap_Cvar_Update(&cm_originalsl);
		trap_Cvar_Update(&cm_originaltl);

		logAdmin(adm, NULL, "compmode on", NULL);
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\Competition mode enabled!");
		G_printCustomMessageToAll("Admin Action", "Competition mode enabled by %s.", getNameOrArg(adm, "RCON", qtrue));

		level.nextCmInfoDisplay = level.time + 3000;
	}
	else {
		trap_Cvar_Set("cm_state", va("%d", COMPMODE_NONE));
		trap_Cvar_Update(&cm_state);
		logAdmin(adm, NULL, "compmode off", NULL);
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\Competition mode disabled");
		G_printCustomMessageToAll("Admin Action", "Competition mode disabled by %s.", getNameOrArg(adm, "RCON", qtrue));

		// Reset compmode variables.
		resetCompetitionModeVariables();

	}

	return -1;
}

static void adm_printBanlist(int argNum, gentity_t* adm, qboolean shortCmd, qboolean subnet) {

	char pageArg[64];
	int page = 0;

	if (shortCmd && G_GetChatArgumentCount()) {
		Q_strncpyz(pageArg, G_GetChatArgument(argNum, qfalse), sizeof(pageArg));
	}
	else {
		trap_Argv(argNum, pageArg, sizeof(pageArg));
	}

	if (pageArg && strlen(pageArg)) {
		page = atoi(pageArg);
	}

	dbPrintBanlist(adm, subnet, page);

	return -1;

}

int adm_banList(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_printBanlist(argNum, adm, shortCmd, qfalse);

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

static void adm_banPlayer(int argNum, gentity_t* adm, qboolean shortCmd, qboolean subnet) {

	int idNum = G_ClientNumFromArg(adm, argNum, subnet ? "subnetban" : "ban", qfalse, qfalse, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];

		char durationArg[64];

		if (G_GetChatArgumentCount() >= 2 && shortCmd) {
			Q_strncpyz(durationArg, G_GetChatArgument(argNum + 1, qfalse), sizeof(durationArg));
		}
		else {
			trap_Argv(argNum + 1, durationArg, sizeof(durationArg));
		}

		int duration[4] = { 0, 0, 0, 0 };

		getBanDuration(durationArg, duration);

		qboolean durationNotFound = duration[0] == -1 ? qtrue : qfalse;
		qboolean isEom = duration[0] <= 0 ? qtrue : qfalse;

		char reason[MAX_STRING_CHARS];
		Q_strncpyz(reason, concatArgs(argNum + 2 - durationNotFound, shortCmd, qfalse), sizeof(reason));

		dbAddBan(ent, adm, reason, subnet, isEom, duration[1], duration[2], duration[3]);
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\%s\nby %s", ent->client->pers.netname, subnet ? "subnetbanned" : "banned", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Command", "%s was %s by %s.", ent->client->pers.cleanName, subnet ? "subnetbanned" : "banned", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, ent, subnet ? "subnetban" : "ban", reason);
		// and then drop the client as well.

		char kickAction[MAX_SAY_TEXT];
		Q_strncpyz(kickAction, va("%sbanned %s", subnet ? "subnet" : "", isEom ? "until the end of map" : va("for %d days, %d hours, %d minutes", duration[1], duration[2], duration[3])), sizeof(kickAction));

		kickPlayer(ent, adm, kickAction, reason);
	}
}

int adm_Ban(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_banPlayer(argNum, adm, shortCmd, qfalse);
	return -1;

}

static void adm_removeBan(int argNum, gentity_t* adm, qboolean shortCmd, qboolean subnet) {

	char rowArg[64];

	if (G_GetChatArgumentCount() >= 2 && shortCmd) {
		Q_strncpyz(rowArg, G_GetChatArgument(argNum, qfalse), sizeof(rowArg));
	}
	else {
		trap_Argv(argNum, rowArg, sizeof(rowArg));
	}

	if (rowArg && strlen(rowArg)) {
		int rowId = atoi(rowArg);

		if (rowId > 0) {

			// Get ban data by row ID first so that we can log it.
			char bannedPlayer[MAX_NETNAME];
			char bannedIp[MAX_IP];

			qboolean found = dbGetBanDetailsByRowID(subnet, rowId, bannedPlayer, sizeof(bannedPlayer), bannedIp, sizeof(bannedIp));

			if (found) {
				int rowsAffected = dbRemoveBan(subnet, rowId);

				if (rowsAffected) {
					G_printInfoMessage(adm, "Unbanned %s [%s] from row %d.", bannedPlayer, bannedIp, rowId);
					logAdmin(adm, NULL, "unban", va("%s / %s", bannedPlayer, bannedIp));

				}
				else { // this happening is very weird..
					logSystem(LOGLEVEL_WARN, "dbRemoveBan !rowsAffected while a row was found initially...");
					G_printInfoMessage(adm, "Row %d was not found.", rowId);

				}
			}
			else {
				G_printInfoMessage(adm, "Row %d was not found.", rowId);
			}

			
		}
		else {
			G_printInfoMessage(adm, "Please enter a valid row ID");
		}

	}
	else {
		G_printInfoMessage(adm, "Please enter a valid row ID");
	}

}

int adm_Unban(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_removeBan(argNum, adm, shortCmd, qfalse);
	return -1;

}

int adm_Broadcast(int argNum, gentity_t* adm, qboolean shortCmd) {

	char* msg = concatArgs(argNum, shortCmd, qtrue);
	if (!msg || !*msg || !strlen(msg)) {
		G_printInfoMessage(adm, "Please specify a message to broadcast.");
	}
	else {
		G_Broadcast(BROADCAST_CMD, NULL, qfalse, va("Broadcast from %s\n%s", getNameOrArg(adm, "\\RCON", qfalse), msg));
		G_GlobalSound(G_SoundIndex("sound/misc/menus/invalid.wav"));
		logAdmin(adm, NULL, "broadcast", msg);
		G_printCustomMessageToAll("Admin Action", "Broadcast by %s.", getNameOrArg(adm, "RCON", qtrue));
	}

	
	return -1;
}

int adm_subnetbanList(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_printBanlist(argNum, adm, shortCmd, qtrue);

	return -1;
}

int adm_subnetBan(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_banPlayer(argNum, adm, shortCmd, qtrue);
	return -1;

}

int adm_subnetUnban(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_removeBan(argNum, adm, shortCmd, qtrue);
	return -1;

}

int adm_evenTeams(int argNum, gentity_t* adm, qboolean shortCmd) {

	int output = evenTeams(qfalse);

	if (output == TEAMACTION_EVEN) {
		G_printInfoMessage(adm, "Teams are already even.");
	}
	else if (output == TEAMACTION_INCOMPATIBLE_GAMETYPE) {
		G_printInfoMessage(adm, "You cannot even teams in this gametype.");
	}
	else if (output == TEAMACTION_NOT_ENOUGH_PLAYERS) {
		G_printInfoMessage(adm, "There are not enough players to run eventeams.");
	}
	else {
		logAdmin(adm, NULL, "eventeams", NULL);
		G_printCustomMessageToAll("Admin Action", "Eventeams by %s", getNameOrArg(adm, "RCON", qtrue));
		G_Broadcast(BROADCAST_GAME, NULL, qfalse, "Eventeams\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
	}

	return -1;
}

int adm_clanVsAll(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (!level.gametypeData->teams) {
		G_printInfoMessage(adm, "You need to be in a team-based game to run clan vs all.");
	} else if (isCurrentGametypeInList((gameTypes_t[]) { GT_HNS, GT_HNZ, GT_PROP, GT_MAX })) {
		G_printInfoMessage(adm, "Gametype is not supported.");
	} else {
		level.redLocked = qfalse;
		level.blueLocked = qfalse;
		// First find out which team has the most players => move the clan members there.
		team_t moveToTeam = TEAM_BLUE;
		int blueTeamClanPlayers = 0;
		int redTeamClanPlayers = 0;

		for (int i = 0; i < level.numConnectedClients; i++) {
			gentity_t* ent = &g_entities[level.sortedClients[i]];

			if (ent->client->sess.team == TEAM_BLUE && ent->client->sess.clanMember) {
				blueTeamClanPlayers++;
			}
			else if (ent->client->sess.team == TEAM_RED && ent->client->sess.clanMember) {
				redTeamClanPlayers++;
			}
		}

		if (blueTeamClanPlayers < redTeamClanPlayers) {
			moveToTeam = TEAM_RED;
			level.redLocked = qtrue;
		}
		else {
			level.blueLocked = qtrue;
		}

		for (int i = 0; i < level.numConnectedClients; i++) {

			gentity_t* ent = &g_entities[level.sortedClients[i]];

			if (ent->client->sess.team == TEAM_SPECTATOR) {
				continue;
			}

			if (ent->client->sess.team == moveToTeam && ent->client->sess.clanMember) {
				continue;
			}

			// This is a client we need to move.
			team_t newTeam = TEAM_RED;

			if (!ent->client->sess.clanMember) {
				if (moveToTeam == TEAM_RED) {
					newTeam = TEAM_BLUE;
				}
			}
			else {
				newTeam = moveToTeam;
			}

			if (ent->s.gametypeitems > 0) {
				G_DropGametypeItems(ent, 0);
			}


			ent->client->ps.stats[STAT_WEAPONS] = 0;
			TossClientItems(ent);
			G_StartGhosting(ent);

			ent->client->sess.team = newTeam;

			ent->client->pers.identity = NULL;
			ClientUserinfoChanged(ent->s.number);
			CalculateRanks();

			G_StopFollowing(ent);
			G_StopGhosting(ent);
			trap_UnlinkEntity(ent);
			ClientSpawn(ent);

		}

		G_Broadcast(BROADCAST_CMD, NULL, qfalse, "\\Clan vs all");
		G_printCustomMessageToAll("Admin Action", "Clan vs All by %s", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "clanvsall", NULL);
		G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));

	}

	return -1;
}

int adm_lockTeam(int argNum, gentity_t* adm, qboolean shortCmd) {

	char teamArg[64];
	if (G_GetChatArgumentCount() >= 2 && shortCmd) {
		Q_strncpyz(teamArg, G_GetChatArgument(argNum, qfalse), sizeof(teamArg));
	}
	else {
		trap_Argv(argNum, teamArg, sizeof(teamArg));
	}

	if (teamArg && strlen(teamArg) > 0) {
		qboolean isLock = qtrue;
		char* teamName = "";
		char teamArgChar = toupper(teamArg[0]);
		if (teamArgChar == 'A') {

			if (level.blueLocked || level.redLocked || level.specLocked) {
				isLock = qfalse;
			}

			level.blueLocked = isLock;
			level.redLocked = isLock;
			level.specLocked = isLock;

			G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas \\%slocked all teams", getNameOrArg(adm, "\\RCON", qfalse), !isLock ? "un" : "");
			G_printCustomMessageToAll("Admin Action", "%s has %slocked all teams.", getNameOrArg(adm, "RCON", qtrue), !isLock ? "un" : "");
			logAdmin(adm, NULL, va("%slock allteams", !isLock ? "un" : ""), NULL);
		}
		else {

			if (teamArgChar == 'B') {
				if (level.blueLocked) {
					isLock = qfalse;
				}

				level.blueLocked = isLock;
				teamName = "Blue";
			}
			else if (teamArgChar == 'R') {
				if (level.redLocked) {
					isLock = qfalse;
				}

				level.redLocked = isLock;
				teamName = "Red";
			}
			else if (teamArgChar == 'S') {

				if (cm_state.integer == COMPMODE_INITIALIZED) {
					if (match_lockspecs.integer) {
						G_printCustomMessageToAll("Competition Mode", "Spectating will be allowed.");
						trap_Cvar_Set("match_lockspecs", "0");
						trap_Cvar_Update(&match_lockspecs);
					}
					else {
						G_printCustomMessageToAll("Competition Mode", "Spectating will be blocked.");
						trap_Cvar_Set("match_lockspecs", "1");
						trap_Cvar_Update(&match_lockspecs);
					}
					G_GlobalSound(level.actionSoundIndex);
					return -1;
				}
				else {
					if (level.specLocked) {
						isLock = qfalse;
					}

					level.specLocked = isLock;
					teamName = "Spectators";
				}

				
			}
			else {
				G_printInfoMessage(adm, "Wrong team specified. Valid values: blue, red, spec");
				return -1;
			}


			G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas \\%slocked the %s team", getNameOrArg(adm, "\\RCON", qfalse), !isLock ? "un" : "", teamName);
			G_printCustomMessageToAll("Admin Action", "%s has %slocked the %s team.", getNameOrArg(adm, "RCON", qtrue), !isLock ? "un" : "", teamName);
			logAdmin(adm, NULL, va("%slock %s", !isLock ? "un" : "", teamName), NULL);
			
		}

	}
	else {
		G_printInfoMessage(adm, "Please specify a team to lock/unlock.");
	}

	return -1;
}

int adm_Flash(int argNum, gentity_t* adm, qboolean shortCmd) {

	char arg[64];

	if (!shortCmd || shortCmd && !G_GetChatArgumentCount()) {
		trap_Argv(argNum, arg, sizeof(arg));
	}
	else {
		Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
	}

	weapon_t weapon = WP_M84_GRENADE;
	vec3_t dir = { 100, 0, 300 };
	gentity_t* missile;

	if (!Q_stricmp(arg, "all")) {

		for (int i = 0; i < level.numConnectedClients; i++) {
			missile = NV_projectile(&g_entities[level.sortedClients[i]], g_entities[level.sortedClients[i]].r.currentOrigin, dir, weapon, 0);
			missile->nextthink = level.time + 250;
		}

		G_Broadcast(BROADCAST_GAME, NULL, qtrue, "Everyone has been \\flashed\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "Everyone has been flashed by %s.", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "flashall", NULL);
	}
	else {

		int idNum = G_ClientNumFromArg(adm, argNum, "flash", qtrue, qtrue, qtrue, shortCmd);

		if (idNum >= 0) {

			gentity_t* ent = &g_entities[idNum];
			missile = NV_projectile(ent, ent->r.currentOrigin, dir, weapon, 0);
			missile->nextthink = level.time + 250;

		}

		
		return idNum;
	}

	return -1;
}

int adm_Gametype(int argNum, gentity_t* adm, qboolean shortCmd) {

	char        gametype[8];
	char        arg[16] = "\0";
	int         argc = 0;

	if (shortCmd) {
		argc = G_GetChatArgumentCount();
	}

	if (!shortCmd || shortCmd && !argc) {
		trap_Argv(argNum, arg, sizeof(arg));
	}
	else {
		Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
	}

	// Boe!Man 2/4/11: Adding support for uppercase arguments.
	Q_strlwr(arg);

	if (strlen(arg) > 0) {
		if (level.mapAction == MAPACTION_NONE) {

			if (G_IsGametypeAValidGametype(arg)) {
				level.mapAction = MAPACTION_PENDING_GT;
				level.runMapAction = level.time + 3000;
				Q_strncpyz(level.mapActionNewGametype, arg, sizeof(level.mapActionNewGametype));

				G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\Gametype %s!", arg);
				G_printCustomMessageToAll("Admin Action", "Gametype %s by %s", arg, getNameOrArg(adm, "RCON", qtrue));
				logAdmin(adm, NULL, va("gametype %s", arg), NULL);
			}
			else {
				G_printInfoMessage(adm, "Gametype '%s' is not a valid gametype.", arg);
			}

		}
		else {
			printMapActionDenialReason(adm);
		}
	}
	else {
		G_printInfoMessage(adm, "Current gametype: %s", g_gametype.string);
	}

	

	return -1;
}

int adm_Map(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (level.mapAction != MAPACTION_NONE) {
		printMapActionDenialReason(adm);
		return -1;
	}

	char        gametype[12];
	char        mapArg[MAX_QPATH];
	int         argc = 0;

	if (shortCmd) {
		argc = G_GetChatArgumentCount();
	}

	if (!shortCmd || shortCmd && !argc) {
		trap_Argv(argNum, mapArg, sizeof(mapArg));
		trap_Argv(argNum + 1, gametype, sizeof(gametype));

	}
	else {
		Q_strncpyz(mapArg, G_GetChatArgument(argNum, qfalse), sizeof(mapArg));
		Q_strncpyz(gametype, G_GetChatArgument(argNum + 1, qfalse), sizeof(gametype));
	}

	// Boe!Man 2/4/11: Adding support for uppercase arguments.
	Q_strlwr(mapArg);
	Q_strlwr(gametype);

	// Before anything else, validate the map.
	char foundMaps[MAX_STRING_CHARS];
	int mapTrapResponse = trap_ValidateMapName(mapArg, foundMaps, sizeof(foundMaps));

	if (mapTrapResponse != 1) {

		if (mapTrapResponse == 0) {
			G_printInfoMessage(adm, "Map '%s' was not found on the server.", mapArg);
		}
		else {
			G_printInfoMessage(adm, "Didn't find an exact match for map '%s', possible values listed below:", mapArg);

			if (adm && adm->client) {
				trap_SendServerCommand(adm - g_entities, va("print \"%s\"", foundMaps));
			}
			else {
				Com_Printf(foundMaps);
			}
			

		}

		return -1;
	}

	qboolean newGametype = qfalse;

	if (strlen(gametype) && G_IsGametypeAValidGametype(gametype)) {
		Q_strncpyz(level.mapActionNewGametype, gametype, sizeof(level.mapActionNewGametype));
		newGametype = qtrue;
	}

	level.mapAction = newGametype ? MAPACTION_PENDING_MAPGTCHANGE : MAPACTION_PENDING_MAPCHANGE;
	Q_strncpyz(level.mapActionNewMap, mapArg, sizeof(level.mapActionNewMap));
	level.runMapAction = level.time + 3000;

	logAdmin(adm, NULL, va("map %s %s", mapArg, newGametype ? level.mapActionNewGametype : g_gametype.string), NULL);
	G_Broadcast(BROADCAST_GAME, NULL, qtrue, "Map %s [^3%s^7]\nby %s", mapArg, newGametype ? level.mapActionNewGametype : g_gametype.string, getNameOrArg(adm, "\\RCON", qfalse));
	G_printCustomMessageToAll("Admin Action", "Map %s [^3%s^7] by %s", mapArg, newGametype ? level.mapActionNewGametype : g_gametype.string, getNameOrArg(adm, "RCON", qtrue));



	return -1;
}

int adm_mapCycle(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (strlen(g_mapcycle.string) == 0 || !Q_stricmp(g_mapcycle.string, "none")) {
		G_printInfoMessage(adm, "Server does not have a mapcycle.");
		return -1;
	}

	if (level.mapAction == MAPACTION_NONE) {
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "\\Mapcycle\nby %s", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "Mapcycle by %s", getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, NULL, "mapcycle", NULL);

		level.mapAction = MAPACTION_PENDING_MAPCYCLE;
		level.runMapAction = level.time + 3000;
	}
	else {
		printMapActionDenialReason(adm);
	}

	return -1;
}

int adm_passVote(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_cancelVote(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Pause(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (level.intermissiontime || level.intermissionQueued) {
		return -1;
	}
	else if (level.paused) {
		level.mapAction = MAPACTION_UNPAUSE;
		level.runMapAction = level.time + 5000;
		level.unpauseNextNotification = level.time;
		return -1;
	}

	level.paused = qtrue;

	trap_GT_SendEvent(GTEV_PAUSE, level.time, 1, 0, 0, 0, 0);


	for (int i = 0; i < level.numConnectedClients; i++)
	{
		gentity_t* ent = &g_entities[level.sortedClients[i]];
		ent->client->ps.pm_type = PM_INTERMISSION;
	}

	// Send the current scoring to all clients.
	SendScoreboardMessageToAllClients();

	// Tell everyone what just happened.
	G_GlobalSound(G_SoundIndex("sound/misc/events/buzz02.wav"));
	G_Broadcast(BROADCAST_CMD, NULL, qfalse, "\\Paused!");
	logAdmin(adm, NULL, "pause", NULL);
	G_printCustomMessageToAll("Admin Action", "Game paused by %s", getNameOrArg(adm, "RCON", qtrue));

	return -1;
}

int adm_Burn(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "kick", qtrue, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];
		// Create temporary entity for the burn effect.

		gentity_t* tent = G_TempEntity(g_entities[ent->s.number].r.currentOrigin, EV_EXPLOSION_HIT_FLESH);
		tent->s.eventParm = 0;
		tent->s.otherEntityNum2 = idNum;
		tent->s.time = WP_ANM14_GRENADE + ((((int)ent->s.apos.trBase[YAW] & 0x7FFF) % 360) << 16);
		VectorCopy(g_entities[ent->s.number].r.currentOrigin, tent->s.angles);
		SnapVector(tent->s.angles);

		// Set burn seconds and notify the player of what happened with a sound effect.
		ent->client->pers.burnSeconds = 4;
		G_ClientSound(ent, G_SoundIndex("/sound/weapons/incendiary_grenade/incen01.mp3"));
	}

	return idNum;
}

int adm_Kick(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "kick", qfalse, qfalse, qfalse, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		char reason[MAX_SAY_TEXT];
		Q_strncpyz(reason, concatArgs(argNum + 1, shortCmd, qfalse), sizeof(reason));

		logAdmin(adm, ent, "kick", reason);
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas been \\kicked\nby %s", ent->client->pers.netname, getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "%s has been kicked by %s. Reason: %s.", ent->client->pers.cleanName, getNameOrArg(adm, "RCON", qtrue), reason);
		kickPlayer(ent, adm, "kicked", reason);

	}

	return -1;
}

int adm_Mute(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "mute", qfalse, qfalse, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];
		qboolean isMute = ent->client->sess.muted ? qfalse : qtrue;

		if (ent->client->sess.muted) {
			unmuteClient(ent);
		}
		else {

			char muteArg[64];
			int muteDuration = 0;

			if (!shortCmd || shortCmd && !G_GetChatArgumentCount() >= argNum + 1) {
				trap_Argv(argNum + 1, muteArg, sizeof(muteArg));
			}
			else {
				Q_strncpyz(muteArg, G_GetChatArgument(argNum + 1, qfalse), sizeof(muteArg));
			}

			if (muteArg && strlen(muteArg) > 0) {
				muteDuration = atoi(muteArg);
			}

			if (muteDuration > 60) {
				G_printInfoMessage(adm, "Max duration is 60 minutes, changed the duration to 60.");
			}

			muteDuration = Com_Clamp(5, 60, muteDuration);
			muteClient(ent, muteDuration);
		}

		logAdmin(adm, ent, va("%smute", isMute ? "" : "un"), NULL);
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\%smuted\nby %s", ent->client->pers.netname, isMute ? "" : "un", getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "%s was %smuted by %s.", ent->client->pers.cleanName, isMute ? "" : "un", getNameOrArg(adm, "RCON", qtrue));
	}

	return -1;
}

int adm_friendlyFire(int argNum, gentity_t* adm, qboolean shortCmd) {
	adm_toggleCVAR(argNum, adm, shortCmd, qtrue, "Friendlyfire", &g_friendlyFire, qfalse, NULL, NULL);
	return -1;
}

int adm_Rename(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "mute", qfalse, qtrue, qfalse, shortCmd);

	if (idNum >= 0) {
		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.nameChangeBlock == NAMECHANGEBLOCK_RENAME) {
			ent->client->sess.nameChangeBlock = NAMECHANGEBLOCK_NONE;
			G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\ncan \\rename again.", ent->client->pers.netname);
			G_printCustomMessageToAll("Admin Action", "%s can rename again.", ent->client->pers.cleanName);
			ClientUserinfoChanged(idNum);
			return -1;
		}
		else if (ent->client->sess.nameChangeBlock == NAMECHANGEBLOCK_PROFANITY) {
			G_printInfoMessage(adm, "Player needs to change their name themselves to get rid of the profanity.");
			return -1;
		}

		// JANFIXME Profanity filters - don't allow renaming into a profanity.
		char newName[MAX_NETNAME];
		char newCleanname[MAX_NETNAME];

		Q_strncpyz(newName, concatArgs(argNum + 1, shortCmd, qtrue), sizeof(newName));

		if (strlen(newName) < 3) {
			G_printInfoMessage(adm, "New name should be at least the length of 3.");
			return -1;
		}

		G_ClientCleanName(concatArgs(argNum + 1, shortCmd, qtrue), newName, sizeof(newName), qtrue);
		G_ClientCleanName(newName, newCleanname, sizeof(newCleanname), qfalse);

		ent->client->sess.nameChangeBlock = NAMECHANGEBLOCK_RENAME;
		
		// Broadcast before actual change.
		G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nhas been renamed to %s\nby %s", ent->client->pers.netname, newName, getNameOrArg(adm, "\\RCON", qfalse));
		G_printCustomMessageToAll("Admin Action", "%s has been renamed to %s by %s.", ent->client->pers.cleanName, newCleanname, getNameOrArg(adm, "RCON", qtrue));
		logAdmin(adm, ent, va("rename to %s", newCleanname), NULL);

		Q_strncpyz(ent->client->pers.netname, newName, sizeof(ent->client->pers.netname));
		Q_strncpyz(ent->client->pers.cleanName, newCleanname, sizeof(ent->client->pers.cleanName));

		ClientUserinfoChanged(idNum);
	}

	return -1;
}

int adm_Switch(int argNum, gentity_t* adm, qboolean shortCmd) {

	if (!level.gametypeData->teams) {
		G_printInfoMessage(adm, "This command only works in team-based gametypes.");
		return -1;
	}

	int idNum = G_ClientNumFromArg(adm, argNum, "switch", qfalse, qtrue, qtrue, shortCmd);

	if (idNum >= 0) {
		
		gentity_t* ent = &g_entities[idNum];

		if (ent->client->sess.team == TEAM_BLUE) {
			SetTeam(ent, "r", NULL, qtrue);
		}
		else if (ent->client->sess.team == TEAM_RED) {
			SetTeam(ent, "b", NULL, qtrue);
		}
		else {
			G_printInfoMessage(adm, "Player has to be in a team to switch them.");
			return -1;
		}

	}

	return idNum;
}

int adm_Third(int argNum, gentity_t* adm, qboolean shortCmd) {

	adm_toggleCVAR(argNum, adm, shortCmd, qtrue, "Thirdperson", &g_allowThirdPerson, qfalse, NULL, NULL);

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

	if (match_bestOf.integer) {
		trap_Cvar_Set("match_bestof", "0");
		trap_Cvar_Update(&match_bestOf);

		G_GlobalSound(level.actionSoundIndex);
		G_printCustomMessageToAll("Competition Mode", "Best-of logic turned off.");
	}
	else {
		trap_Cvar_Set("match_bestof", "1");
		trap_Cvar_Update(&match_bestOf);

		G_GlobalSound(level.actionSoundIndex);
		G_printCustomMessageToAll("Competition Mode", "Best-of logic turned on.");
	}

	return -1;
}

int adm_profanityList(int argNum, gentity_t* adm, qboolean shortCmd) {
	return -1;
}

int adm_Pop(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "pop", qtrue, qfalse, qfalse, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];
		popPlayer(ent, POPACTION_ADMIN);

	}

	return idNum;
}

int adm_Uppercut(int argNum, gentity_t* adm, qboolean shortCmd) {

	int idNum = G_ClientNumFromArg(adm, argNum, "uppercut", qtrue, qtrue, qtrue, shortCmd);

	if (idNum >= 0) {

		gentity_t* ent = &g_entities[idNum];

		char ucArg[64];
		int ucLevel = 0;

		if (!shortCmd || shortCmd && !G_GetChatArgumentCount() >= argNum + 1) {
			trap_Argv(argNum + 1, ucArg, sizeof(ucArg));
		}
		else {
			Q_strncpyz(ucArg, G_GetChatArgument(argNum + 1, qfalse), sizeof(ucArg));
		}

		if (ucArg && strlen(ucArg) > 0) {
			ucLevel = atoi(ucArg);
		}

		ucLevel = Com_Clamp(0, 10, ucLevel);

		uppercutPlayer(ent, ucLevel);

	}

	return idNum;
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

typedef struct tempAdmcmd_s {
	const admCmd_t* cmd;
	int level;
} tempAdmcmd_t;

/*
There are about 80-90 admin commands. We use tempAdmcmd struct as an array to sort and print out relevant commands to avoid iterations.
*/

static int _compareTempAdmcmds(const void* a, const void* b) {
	const tempAdmcmd_t* cmdA = (const tempAdmcmd_t*)a;
	const tempAdmcmd_t* cmdB = (const tempAdmcmd_t*)b;
	return cmdA->level - cmdB->level; 
}

static int filterAdminCommands(int adminLevel, tempAdmcmd_t* tempAdmCmds) {
	int count = 0;

	for (int i = 0; i < sizeof(adminCommands) / sizeof(adminCommands[0]); i++) {
		const admCmd_t* cmd = &adminCommands[i];
		int cmdLevel = *cmd->adminLevel; 

		// Skip commands above player's level unless they are RCON
		if (adminLevel < ADMLVL_RCON && cmdLevel > adminLevel) {
			continue;
		}

		// Add to the temporary list
		tempAdmCmds[count].cmd = cmd;
		tempAdmCmds[count].level = cmdLevel;
		count++;
	}

	return count;
}

void adm_printAdminCommands(gentity_t* adm) {

	int admLvl = adm && adm->client ? adm->client->sess.adminLevel : ADMLVL_RCON;
	qboolean isRcon = admLvl == ADMLVL_RCON ? qtrue : qfalse;

	tempAdmcmd_t* tempAdminCommands = (tempAdmcmd_t*)malloc(adminCommandsSize * sizeof(tempAdmcmd_t));

	if (!tempAdminCommands) {
		logSystem(LOGLEVEL_WARN, "malloc failed on tempAdminCommands, out of memory?");
		G_printInfoMessage(adm, "Something went wrong, please try again.");
		return;
	}

	int cmdCount = filterAdminCommands(admLvl, tempAdminCommands);

	qsort(tempAdminCommands, cmdCount, sizeof(tempAdmcmd_t), _compareTempAdmcmds);
	char pakBuf[1024];
	Com_Memset(pakBuf, 0, sizeof(pakBuf));

	if (isRcon) {
		Com_Printf("\n^3%-16.16s%-8.8s%-4.4s%-34.34s%-16.16s\n", "Command", "Short", "Lvl", "Description", "Params");
		Com_Printf("-----------------------------------------------------------------------------\n");
	}
	else {
		Q_strncpyz(pakBuf, va("\n^3%-16.16s%-8.8s%-4.4s%-33.33s%-16.16s\n^7-----------------------------------------------------------------------------\n", "Command", "Short", "Lvl", "Description", "Params"), sizeof(pakBuf));
	}
	char maxDesc[1024];
	int maxDescLen = 0;
	// Print the commands
	for (int i = 0; i < cmdCount; i++) {
		const admCmd_t* admCmd = tempAdminCommands[i].cmd;

		char admLvlWithoutColors[MAX_NETNAME];
		Q_strncpyz(admLvlWithoutColors, getAdminNameByAdminLevel(tempAdminCommands[i].level), sizeof(admLvlWithoutColors));
		G_RemoveColorEscapeSequences(admLvlWithoutColors);

		if (strlen(admCmd->desc) > maxDescLen) {
			maxDescLen = strlen(admCmd->desc);
			Q_strncpyz(maxDesc, admCmd->desc, sizeof(maxDesc));
		}

		if (isRcon) {
			Com_Printf("%-16.16s%-8.8s%-4d%-34.34s%-16.16s\n", admCmd->adminCmd, admCmd->shortCmd, *admCmd->adminLevel, admCmd->desc, admCmd->params);
		}
		else {

			if (strlen(pakBuf) + strlen(va("%-16.16s%-8.8s%-4d%-33.33s%-16.16s\n", admCmd->adminCmd, admCmd->shortCmd, *admCmd->adminLevel, admCmd->desc, admCmd->params)) > sizeof(pakBuf) - 155) {
				trap_SendServerCommand(adm - g_entities, va("print \"%s\"", pakBuf));
				Com_Memset(pakBuf, 0, sizeof(pakBuf));
			}

			Q_strcat(pakBuf, sizeof(pakBuf), va("%-16.16s%-8.8s%-4d%-33.33s%-16.16s\n", admCmd->adminCmd, admCmd->shortCmd, *admCmd->adminLevel, admCmd->desc, admCmd->params));

		}
	}

	if (isRcon) {
		Com_Printf("\n^3Short commands are textual representations what you can use in chat\n^3Admin levels: 1 = B-Admin, 2 = Admin, 3 = S-Admin, 4 = H-Admin, 5 = RCON\nUse ^3[Page Up]^7 or ^3[Page Down]^7 to scroll.\n");
	}
	else {
		trap_SendServerCommand(adm - g_entities, va("print \"%s\"", pakBuf));
		trap_SendServerCommand(adm - g_entities, "print \"\n^3Short commands are textual representations what you can use in chat\n^3Admin levels: 1 = B-Admin, 2 = Admin, 3 = S-Admin, 4 = H-Admin, 5 = RCON\n^7Use [^3Page Up^7] or [^3Page Down^7] to scroll.\n\"");
	}
	
	// Free the allocated memory
	free(tempAdminCommands);

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

