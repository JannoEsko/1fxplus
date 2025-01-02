#include "../g_local.h"
#define MAX_VOTEOPTION 15


typedef struct voteOption_s {
    char* voteCmd;
    void(*voteFunc) (gentity_t* ent, char* arg1, char* arg2);
    int* voteLvl;
    char* desc;
    char* params;
} voteOption_t;

static void vote_resetVoteInfoOnClients(qboolean removeFromLevelStruct) {

    for (int i = 0; i < level.numConnectedClients; i++) {
        vote_resetVoteInfoOnSingleClient(&g_entities[level.sortedClients[i]], removeFromLevelStruct);
    }

}

void vote_updateConfigString(qboolean onlyTime) {

    if (!level.vote.voteTime) {
        trap_SetConfigstring(CS_VOTE_TIME, "");
    }
    else {
        trap_SetConfigstring(CS_VOTE_TIME, va("%i,%i", level.vote.voteTime, g_voteDuration.integer * 1000));
    }

    if (onlyTime) {
        return;
    }

    trap_SetConfigstring(CS_VOTE_STRING, level.vote.voteDisplayString);
    trap_SetConfigstring(CS_VOTE_YES, va("%i", level.vote.voteYes));
    trap_SetConfigstring(CS_VOTE_NO, va("%i", level.vote.voteNo));

    int votesNeeded = (int)ceil(level.numVotingClients * vote_successThreshold.integer / 100.0);
    level.vote.neededVotes = votesNeeded;
    trap_SetConfigstring(CS_VOTE_NEEDED, va("%i", votesNeeded));
}

static qboolean vote_canCallVote(gentity_t* ent) {

    if (!g_allowVote.integer) {
        G_printInfoMessage(ent, "Voting not allowed here");
        return qfalse;
    }

    if (level.intermissiontime || level.intermissionQueued || level.changemap) {
        G_printInfoMessage(ent, "Voting not allowed during intermission");
        return qfalse;
    }

    if (level.numVotingClients < 2) {
        G_printInfoMessage(ent, "You need at least 2 clients to call a vote.");
        return qfalse;
    }

    if (level.vote.voteTime) {
        G_printInfoMessage(ent, "A vote is already in progress.");
        return qfalse;
    }

    if (ent->client->pers.voteCount >= MAX_VOTE_COUNT) {
        G_printInfoMessage(ent, "You have called the maximum number of votes.");
        return qfalse;
    }

    if (ent->client->sess.team == TEAM_SPECTATOR) {
        G_printInfoMessage(ent, "Not allowed to call a vote as spectator.");
        return qfalse;
    }

    if (ent->client->voteDelayTime > level.time) {
        G_printInfoMessage(ent, "You are not allowed to vote within %d " \
            "minute of a failed vote.", g_failedVoteDelay.integer);

        return qfalse;
    }

    if (level.vote.voteExecuteTime) {
        G_printInfoMessage(ent, "A vote is waiting execution.");
        return qfalse;
    }

    // Seems to be good to go.
    return qtrue;
}

static void vote_startVote(gentity_t* ent, voteAction_t voteAction, char* displayString, qboolean arg1IsInt, char* arg1, int intArg1, qboolean arg2IsInt, char* arg2, int intArg2) {
    Q_strncpyz(level.vote.voteDisplayString, displayString, sizeof(level.vote.voteDisplayString));
    level.vote.voteTime = level.time;
    level.vote.voteYes = 0;
    level.vote.voteNo = 0;
    level.vote.voteClient = ent->s.number;
    level.vote.voteAction = voteAction;
    level.vote.nextPollDisplay = 0;
    level.vote.lastPollDisplay = 0;

    // Theoretiocally based on voteAction, we could determine whether the arguments should be int values.
    // But, expectation is that whatever sanitization, bounds and whatnot will happen on the function side
    // So what is received here is valid arguments, hence why we won't do yet another checkup here.
    // In vote_runFrame, we will check based on voteAction what arguments we expect.
    if (arg1IsInt) {
        level.vote.voteArg1.intVal = intArg1;
    }
    else {
        Q_strncpyz(level.vote.voteArg1.charVal, arg1, sizeof(level.vote.voteArg1.charVal));
    }

    if (arg2IsInt) {
        level.vote.voteArg2.intVal = intArg2;
    }
    else {
        Q_strncpyz(level.vote.voteArg2.charVal, arg2, sizeof(level.vote.voteArg2));
    }

    level.vote.votingClients = level.numVotingClients;

    G_printInfoMessageToAll("%s has started a %s.", ent->client->pers.cleanName, voteAction == VOTEACTION_POLL ? "poll" : "vote");
    vote_resetVoteInfoOnClients(qfalse);
    vote_updateConfigString(qfalse);
}

static void vote_maprestart(gentity_t* ent, char* arg1, char* arg2) {

    // arg1 and arg2 are redundant.
    vote_startVote(ent, VOTEACTION_MAPRESTART, va("%s: Restart the map", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);

}

static void vote_mapcycle(gentity_t* ent, char* arg1, char* arg2) {

    // arg1 and arg2 are redundant.
    // Check if the server runs a mapcycle first.

    if (!strlen(g_mapcycle.string) || !Q_stricmp(g_mapcycle.string, "none")) {
        G_printInfoMessage(ent, "Server is not running a mapcycle.");
        return;
    }

    vote_startVote(ent, VOTEACTION_MAPCYCLE, va("%s: Proceed to next map", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);

}

static void vote_setmap(gentity_t* ent, char* newmap, char* newgametype) {

    if (strlen(newmap) < 3) {
        G_printInfoMessage(ent, "Map string too short. Please specify a valid map name.");
        return;
    }

    // First ensure that we have a map they're trying to vote for.
    char mapOutput[MAX_STRING_CHARS];
    int mapValidate = trap_ValidateMapName(newmap, mapOutput, sizeof(mapOutput));

    if (mapValidate != 1) {
        if (!mapValidate) {
            G_printInfoMessage(ent, "Map '%s' was not found on the server.", newmap);
        }
        else {
            G_printInfoMessage(ent, "Map '%s' was not found as an exact match. Valid options:\n%s", newmap, mapOutput);
        }

        return;
    }

    // We have a valid map. Check the gametype.

    if (!newgametype || !strlen(newgametype) || !G_IsGametypeAValidGametype(newgametype)) {
        G_printInfoMessage(ent, "Gametype was not specified or was an invalid one, defaulting to %s...", g_gametype.string);
        newgametype = g_gametype.string;
    }

    vote_startVote(ent, VOTEACTION_MAP, va("%s: map %s [%s]", ent->client->pers.cleanName, newmap, newgametype), qfalse, newmap, 0, qfalse, newgametype, 0);

}

static void vote_endmap(gentity_t* ent, char* arg1, char* arg2) {
    vote_startVote(ent, VOTEACTION_ENDMAP, va("%s: End the map", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);
}

static void vote_gametype(gentity_t* ent, char* newgametype, char* arg2) {

    if (strlen(newgametype) < 2) {
        G_printInfoMessage(ent, "Gametype string too short. Please specify a valid gametype.");
        return;
    }

    if (!G_IsGametypeAValidGametype(newgametype)) {
        G_printInfoMessage(ent, "Gametype '%s' is not a valid gametype.", newgametype);
        return;
    }

    vote_startVote(ent, VOTEACTION_GAMETYPE, va("%s: gametype %s", ent->client->pers.cleanName, newgametype), qfalse, newgametype, 0, qfalse, arg2, 0);

}

static void vote_clkick(gentity_t* ent, char* client, char* arg2) {

    // Search for the client - we actually ignore the client argument here.
    int idNum = G_ClientNumFromArg(ent, 2, "callvote kick", qfalse, qtrue, qfalse, qfalse);

    if (idNum >= 0) {
        gentity_t* kick = &g_entities[idNum];
        vote_startVote(ent, VOTEACTION_KICK, va("%s: kick %s", ent->client->pers.cleanName, kick->client->pers.cleanName), qtrue, "", idNum, qfalse, arg2, 0);
    }

}

static void vote_settimelimit(gentity_t* ent, char* newtimelimit, char* arg2) {

    if (!strlen(newtimelimit)) {
        G_printInfoMessage(ent, "No timelimit specified.");
        return;
    }

    int newtl = atoi(newtimelimit);

    if (newtl < 0) {
        G_printInfoMessage(ent, "Negative timelimit is not allowed.");
        return;
    }
    else if (newtl > g_timelimit.integer + 60) {
        G_printInfoMessage(ent, "New timelimit exceeds old one by 60 minutes.");
        return;
    }

    vote_startVote(ent, VOTEACTION_TIMELIMIT, va("%s: timelimit %d", ent->client->pers.cleanName, newtl), qtrue, "", newtl, qfalse, arg2, 0);

}

static void vote_setscorelimit(gentity_t* ent, char* newscorelimit, char* arg2) {

    if (!strlen(newscorelimit)) {
        G_printInfoMessage(ent, "No scorelimit specified.");
        return;
    }

    int newsl = atoi(newscorelimit);

    if (newsl < 0) {
        G_printInfoMessage(ent, "Negative scorelimit is not allowed.");
        return;
    }
    else if (newsl > g_scorelimit.integer + 60) {
        G_printInfoMessage(ent, "New scorelimit exceeds old one by 60 points.");
        return;
    }

    vote_startVote(ent, VOTEACTION_SCORELIMIT, va("%s: scorelimit %d", ent->client->pers.cleanName, newsl), qtrue, "", newsl, qfalse, arg2, 0);

}

static void vote_shuffleteams(gentity_t* ent, char* arg1, char* arg2) {
    vote_startVote(ent, VOTEACTION_SHUFFLE, va("%s: Shuffleteams", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);
}

static void vote_swapteams(gentity_t* ent, char* arg1, char* arg2) {
    vote_startVote(ent, VOTEACTION_SWAP, va("%s: Swapteams", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);
}

static void vote_clanvsall(gentity_t* ent, char* arg1, char* arg2) {
    vote_startVote(ent, VOTEACTION_CLANVSALL, va("%s: Clan vs all", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);
}

static void vote_muteclient(gentity_t* ent, char* client, char* duration) {

    // Search for the client - we actually ignore the client argument here.
    int idNum = G_ClientNumFromArg(ent, 2, "callvote mute", qfalse, qtrue, qfalse, qfalse);

    if (idNum >= 0) {

        // Set a valid duration.
        int muteDuration = 5;

        if (strlen(duration) == 0) {
            G_printInfoMessage(ent, "Duration defaulted to 5 minutes.");
        }
        else {
            muteDuration = Com_Clamp(5, 60, atoi(duration));

            if (muteDuration != atoi(duration)) {
                G_printInfoMessage(ent, "Duration %s was changed to %d.", duration, muteDuration);
            }
        }

        gentity_t* mute = &g_entities[idNum];
        vote_startVote(ent, VOTEACTION_MUTE, va("%s: mute %s for %d minutes", ent->client->pers.cleanName, mute->client->pers.cleanName, muteDuration), qtrue, "", idNum, qtrue, "", muteDuration);
    }


}

static void vote_startpoll(gentity_t* ent, char* arg1, char* arg2) {

    char pollquestion[MAX_SAY_TEXT] = { 0 };

    Q_strncpyz(pollquestion, concatArgs(2, qfalse, qfalse), sizeof(pollquestion));

    vote_startVote(ent, VOTEACTION_POLL, va("%s poll: %s", ent->client->pers.cleanName, pollquestion), qfalse, pollquestion, 0, qfalse, arg2, 0);
}

static void vote_extendtl(gentity_t* ent, char* arg1, char* arg2) {
    vote_startVote(ent, VOTEACTION_EXTENDTIMELIMIT, va("%s: Extend timelimit by 5 minutes", ent->client->pers.cleanName), qfalse, arg1, 0, qfalse, arg2, 0);
}

void vote_resetVoteInfoOnSingleClient(gentity_t* ent, qboolean removeFromLevelStruct) {
    ent->client->ps.eFlags &= ~EF_VOTED;

    if (ent->client->sess.voted == VOTE_VOTED_YES && removeFromLevelStruct) {
        level.vote.voteYes--;
    }
    else if (ent->client->sess.voted == VOTE_VOTED_NO && removeFromLevelStruct) {
        level.vote.voteNo--;
    }

    ent->client->sess.voted = VOTE_VOTED_NONE;
}

static voteOption_t voteOptions[] = {

    { "maprestart",        &vote_maprestart,    &vote_map.integer,            "Restart current map",                        ""                    },
    { "mapcycle",        &vote_mapcycle,        &vote_map.integer,            "Go to next map in mapcycle",                ""                    },
    { "map",            &vote_setmap,        &vote_map.integer,            "Switch server to the specified map",        "<mapname> <gt>"    },
    { "endmap",            &vote_endmap,        &vote_map.integer,            "End the current map",                        ""                    },
    { "gametype",        &vote_gametype,        &vote_map.integer,            "Switch server to the specified gametype",    "<gametype>"        },
    { "kick",            &vote_clkick,        &vote_kick.integer,            "Kick a player",                            "<id/name>"            },
    { "timelimit",        &vote_settimelimit,    &vote_timelimit.integer,    "Change the timelimit",                        "<timelimit>"        },
    { "extendtimelimit",&vote_extendtl,        &vote_timelimit.integer,    "Extend the timelimit for 5 minutes",        ""                    },
    { "scorelimit",        &vote_setscorelimit,&vote_scorelimit.integer,    "Change the scorelimit",                    "<scorelimit>"        },
    { "shuffleteams",    &vote_shuffleteams, &vote_teams.integer,        "Shuffle teams",                            ""                    },
    { "swapteams",        &vote_swapteams,    &vote_teams.integer,        "Swap teams",                                ""                    },
    { "clanvsall",        &vote_clanvsall,    &vote_teams.integer,        "Clan vs All",                                ""                    },
    { "mute",            &vote_muteclient,    &vote_mute.integer,            "Mute/unmute a client",                        "<id/name> <time>"    },
    { "poll",            &vote_startpoll,    &vote_poll.integer,            "Start a poll",                                "<question>"        },
};

static int numVoteOptions = sizeof(voteOptions) / sizeof(voteOptions[0]);


void vote_callVote(gentity_t* ent) {

    // We will have at most 2 valid args to question.

    char voteOption[MAX_VOTEOPTION], arg1[MAX_SAY_TEXT], arg2[10];

    trap_Argv(1, voteOption, sizeof(voteOption));
    trap_Argv(2, arg1, sizeof(arg1));
    trap_Argv(3, arg2, sizeof(arg2));

    voteOption_t* voteOpt = NULL;
    qboolean foundVoteCommand = qfalse;

    // In theory, because we removed ; from argument sanitization, we should run a check here that players shouldn't be able to chain potentially malicious commands together.
    // But as we're nevertheless having a completely different approach here with votes, we will just ensure that ; never gets in the execution chain at all.

    for (int i = 0; i < numVoteOptions; i++) {
        voteOpt = &voteOptions[i];

        if (!Q_stricmp(voteOption, voteOpt->voteCmd)) {

            if (ent->client->sess.adminLevel >= *voteOpt->voteLvl) {
                foundVoteCommand = qtrue;
            }
            else {
                G_printInfoMessage(ent, "You're not privileged enough to callvote '%s'.", voteOpt->voteCmd);
                return;
            }

            break;
        }
    }
    

    if (!foundVoteCommand) {
        G_printInfoMessage(ent, "Invalid vote option '%s'. Valid options are:", voteOption);
        trap_SendServerCommand(ent - g_entities, va("print \"\n^3%-13.13s%-8.8s%-40.40s%-16.16s\n^7-----------------------------------------------------------------------------\n\"", "Command", "Lvl", "Description", "Params"));

        for (int i = 0; i < numVoteOptions; i++) {
            voteOpt = &voteOptions[i];

            if (*voteOpt->voteLvl > ent->client->sess.adminLevel) {
                continue;
            }

            char* voteLvl = "Any";
            switch (*voteOpt->voteLvl) {
            case 1:
                voteLvl = "B-Admin";
                break;
            case 2:
                voteLvl = "Admin";
                break;
            case 3:
                voteLvl = "S-Admin";
                break;
            case 4:
                voteLvl = "H-Admin";
                break;
            }

            trap_SendServerCommand(ent - g_entities, va("print \"%-13.13s%-8.8s%-40.40s%-16.16s\n\"", voteOpt->voteCmd, voteLvl, voteOpt->desc, voteOpt->params));
        }

        trap_SendServerCommand(ent - g_entities, "print \"\n\"");
    }
    else {
        if (vote_canCallVote(ent)) {
            voteOpt->voteFunc(ent, arg1, arg2);
        }
    }
}

void vote_castVote(gentity_t* ent) {

    if (!level.vote.voteTime) {
        G_printInfoMessage(ent, "There's no vote currently in progress.");
        return;
    }

    if (ent->client->ps.eFlags & EF_VOTED) {
        G_printInfoMessage(ent, "You have already voted.");
        return;
    }

    if (ent->client->sess.team == TEAM_SPECTATOR) {
        G_printInfoMessage(ent, "Cannot cast a vote as a spectator.");
        return;
    }

    char voteArg[5];
    
    trap_Argv(1, voteArg, sizeof(voteArg));

    if (!strlen(voteArg)) {
        G_printInfoMessage(ent, "Missing vote option. Valid are: yes, y, 1, no, n, 0");
        return;
    }

    qboolean vote = qfalse;
    char voteChar = tolower(voteArg[0]);

    if (voteChar == 'y' || voteChar == '1') {
        vote = qtrue;
    }
    else if (voteChar != 'n' && voteChar != '0') {
        G_printInfoMessage(ent, "Invalid vote option '%s'. Valid are: yes, y, 1, no, n, 0.", voteArg);
        return;
    }

    if (vote) {
        level.vote.voteYes++;
        ent->client->sess.voted = VOTE_VOTED_YES;
    }
    else {
        level.vote.voteNo++;
        ent->client->sess.voted = VOTE_VOTED_NO;
    }

    ent->client->ps.eFlags |= EF_VOTED;
    G_printInfoMessage(ent, "Vote cast.");
    vote_updateConfigString(qfalse);
}

void vote_runFrame() {

    if (level.vote.voteExecuteTime && level.vote.voteExecuteTime <= level.time) {
        level.vote.voteExecuteTime = 0;

        if (level.vote.voteAction == VOTEACTION_POLL) {
            level.vote.nextPollDisplay = level.time;
            level.vote.lastPollDisplay = level.time + 10000;
        }
        else {
            

            switch (level.vote.voteAction) {
            case VOTEACTION_MAPRESTART:
                level.mapAction = MAPACTION_PENDING_RESTART;
                level.runMapAction = level.time;
                break;
            case VOTEACTION_MAPCYCLE:
                level.mapAction = MAPACTION_PENDING_MAPCYCLE;
                level.runMapAction = level.time;
                break;
            case VOTEACTION_MAP:
                level.mapAction = MAPACTION_PENDING_MAPGTCHANGE;
                Q_strncpyz(level.mapActionNewMap, level.vote.voteArg1.charVal, sizeof(level.mapActionNewMap));
                Q_strncpyz(level.mapActionNewGametype, level.vote.voteArg2.charVal, sizeof(level.mapActionNewGametype));
                level.runMapAction = level.time;
                break;
            case VOTEACTION_ENDMAP:
                level.mapAction = MAPACTION_ENDING;
                LogExit("Map ended on a vote.");
                break;
            case VOTEACTION_GAMETYPE:
                level.mapAction = MAPACTION_PENDING_GT;
                Q_strncpyz(level.mapActionNewGametype, level.vote.voteArg1.charVal, sizeof(level.mapActionNewGametype));
                level.runMapAction = level.time;
                break;
            case VOTEACTION_KICK:
            {
                gentity_t* ent = &g_entities[level.vote.voteArg1.intVal];
                if (ent && ent->client && ent->client->pers.connected != CON_DISCONNECTED) {
                    gentity_t* voteStarter = &g_entities[level.vote.voteClient];

                    if (!voteStarter || !voteStarter->client || voteStarter->client->pers.connected != CON_CONNECTED) {
                        voteStarter = NULL;
                    }

                    kickPlayer(ent, voteStarter, "kicked", "Voted off the server.");
                }
                break;
            }
            case VOTEACTION_TIMELIMIT:
                trap_Cvar_Set("timelimit", va("%d", level.vote.voteArg1.intVal));
                trap_Cvar_Update(&g_timelimit);

                G_printCustomMessageToAll("Vote", "Timelimit set to %d", level.vote.voteArg1.intVal);
                break;
            case VOTEACTION_EXTENDTIMELIMIT:
                trap_SendConsoleCommand(EXEC_APPEND, "extendtime 5");

                G_printCustomMessageToAll("Vote", "Timelimit extended by 5 minutes.");
                break;
            case VOTEACTION_SCORELIMIT:
                trap_Cvar_Set("scorelimit", va("%d", level.vote.voteArg1.intVal));
                trap_Cvar_Update(&g_scorelimit);

                G_printCustomMessageToAll("Vote", "Scorelimit set to %d", level.vote.voteArg1.intVal);
                break;
            case VOTEACTION_SHUFFLE:
                shuffleTeams(qfalse);

                G_printCustomMessageToAll("Vote", "Shuffle teams");
                break;
            case VOTEACTION_SWAP:
                swapTeams(qfalse);
                G_printCustomMessageToAll("Vote", "Swap teams");
                break;
            case VOTEACTION_CLANVSALL:
                clanVsAll();
                G_printCustomMessageToAll("Vote", "Clan vs all");
                break;
            case VOTEACTION_MUTE:
            {
                gentity_t* ent = &g_entities[level.vote.voteArg1.intVal];

                if (ent && ent->client && ent->client->pers.connected != CON_DISCONNECTED) {
                    muteClient(ent, level.vote.voteArg2.intVal);
                    G_printCustomMessageToAll("Vote", "%s was muted for %d minutes.", ent->client->pers.cleanName, level.vote.voteArg2.intVal);
                }

                break;
            }
                
            }
        }
    }

    if (level.vote.voteAction == VOTEACTION_POLL && level.vote.nextPollDisplay && level.vote.nextPollDisplay <= level.time) {

        if (level.time > level.vote.lastPollDisplay) {
            level.vote.lastPollDisplay = 0;
            level.vote.nextPollDisplay = 0;
        }
        else {
            level.vote.nextPollDisplay = level.time + 3000;
            G_Broadcast(BROADCAST_GAME_IMPORTANT, NULL, qfalse, "POLL RESULTS\n%s\nYes votes: %d\nNo votes: %d\nMajority %s with the poll.", level.vote.voteDisplayString, level.vote.voteYes, level.vote.voteNo, level.vote.voteYes > level.vote.voteNo ? "agreed" : "disagreed");
        }

    }

    if (level.vote.voteTime) {
        // Means we have a vote going on, check for the duration.

        if ((level.vote.voteTime + g_voteDuration.integer * 1000) < level.time) {
            level.vote.voteTime = 0;
            vote_updateConfigString(qtrue);

            // Check whether the vote passed or not and print back the info.

            if (level.vote.voteAction == VOTEACTION_POLL) {
                // On polls, we treat every vote as a "success".
                level.vote.voteExecuteTime = level.time + 3000;
            }
            else {

                int totalVotes = level.vote.voteYes + level.vote.voteNo;
                
                if (totalVotes > 0 && level.vote.voteYes * 100 / totalVotes >= vote_successThreshold.integer) {
                    G_printInfoMessageToAll("Vote passed.");
                    level.vote.voteExecuteTime = level.time + 3000;
                }
                else {
                    G_printInfoMessageToAll("Vote failed.");
                }

            }
        }
        else {
            if (level.vote.votingClients != level.numVotingClients) {

                // The actual vote removal calls happen in ClientDisconnect and SetTeam.

                level.vote.votingClients = level.numVotingClients;
                vote_updateConfigString(qfalse);

            }

            // STOP THE COUNT
            // ... but only if we should ... 

            int totalVotes = level.vote.voteYes + level.vote.voteNo;

            if (level.vote.voteYes >= level.vote.neededVotes) {
                G_printInfoMessageToAll("Vote passed.");
                level.vote.voteTime = 0;
                level.vote.voteExecuteTime = level.time + 3000;
                vote_updateConfigString(qtrue);
            }
            else if (level.vote.voteNo >= level.vote.neededVotes || totalVotes == level.vote.neededVotes) {
                G_printInfoMessageToAll("Vote failed.");

                level.vote.voteTime = 0;

                if (level.vote.voteAction == VOTEACTION_POLL) {
                    level.vote.voteExecuteTime = level.time + 3000;
                }

                vote_updateConfigString(qtrue);
            } 

        }
    }

}

void vote_forceResult(gentity_t* ent, qboolean result) {

    level.vote.voteTime = 0;

    if (result) {
        level.vote.voteExecuteTime = level.time + 3000;
    }

}

