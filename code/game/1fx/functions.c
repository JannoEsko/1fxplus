
#include "../g_local.h"

#define MAX_BROADCAST_LINE 48

/*
============
G_GetChatArgumentCount

Get argument count
from chat buffer.
============
*/

int G_GetChatArgumentCount() {

    char arg0[MAX_STRING_TOKENS];
    char arg1[MAX_SAY_TEXT];
    char* token;
    int argc = 0;

    trap_Argv(0, arg0, sizeof(arg0));
    if (!Q_stricmp(arg0, "say") || !Q_stricmp(arg0, "say_team") || !Q_stricmp(arg0, "vsay_team") || !Q_stricmp(arg0, "tell")) {
        trap_Argv(1, arg1, sizeof(arg1));
        if (strlen(arg1) > 0) {
            token = strtok(arg1, " ");

            while (token != NULL) {
                argc++;
                token = strtok(NULL, " ");
            }
        }
    }

    return argc;

}

/*
============
G_GetChatArgument

Get argument from chat
buffer.
============
*/

char* G_GetChatArgument(int argNum)
{
    static char newArg[MAX_SAY_TEXT];
    char text[MAX_SAY_TEXT];
    char* token;

    if (argNum < 0 || argNum >= G_GetChatArgumentCount()) {
        return "";
    }

    memset(newArg, 0, sizeof(newArg));

    trap_Argv(1, text, sizeof(text)); // clientcmd is say "text text text", arg0 = cmd which is checked in getchatargumentcount, arg1 to be parsed here.
    int currentArg = 0;

    if (strlen(text) == 0) {
        return "";
    }

    token = strtok(text, " ");

    while (currentArg < argNum && token != NULL) {
        currentArg++;
        token = strtok(NULL, " ");
    }

    if (token == NULL) {
        return "";
    }

    Q_strncpyz(newArg, token, sizeof(newArg));

    // Remove colors from arg.
    G_RemoveColorEscapeSequences(newArg);

    return newArg;
}

/*
============
G_GetArg

a wrapper for trap_Argv and G_GetChatArgument
============
*/
char* G_GetArg(int argNum, qboolean shortCmd)
{
    static char arg[MAX_STRING_CHARS];
    memset(arg, 0, sizeof(arg));

    if (!shortCmd && G_GetChatArgumentCount() <= 1) {
        //argNum++; if I use it in admin commands, I already have the correct argnum in play. Therefore this will actually mess it up.
        trap_Argv(argNum, arg, sizeof(arg));
    }
    else {
        strncpy(arg, G_GetChatArgument(argNum), sizeof(arg));
    }

    return arg;
}

char* concatArgs(int fromArgNum, qboolean shortCmd) {

    int totalArgs = 0;
    static char output[MAX_STRING_CHARS] = "";
    char* arg;
    int totalLength = 0, argLength = 0;

    memset(output, 0, sizeof(output));

    if (shortCmd) {
        totalArgs = G_GetChatArgumentCount();
    }
    else {
        totalArgs = trap_Argc();
    }

    while (fromArgNum < totalArgs) {
        arg = G_GetArg(fromArgNum, shortCmd);
        argLength = strlen(arg);

        if (totalLength + argLength > MAX_STRING_CHARS) {
            // can't concat more. Chances of it happening are slim.
            break;
        }

        memcpy(output + totalLength, arg, argLength);
        totalLength += argLength;
        fromArgNum++;

        if (fromArgNum != totalArgs) {
            output[totalLength] = ' ';
            totalLength++;
        }
    }

    output[totalLength] = 0;

    return output;

}

/*
==============
G_RemoveColorEscapeSequences
==============
*/

void G_RemoveColorEscapeSequences(char* text) {
    int i, l;

    l = 0;
    for (i = 0; text[i]; i++) {
        if (Q_IsColorString(&text[i])) {
            i++;
            continue;
        }
        if (text[i] > 0x7E)
            continue;
        text[l++] = text[i];
    }
    text[l] = '\0';
}

/*
==============
G_RemoveAdditionalCarets
10/17/15 - 7:22 PM
Removes additional carets from text.
==============
*/

void G_RemoveAdditionalCarets(char* text)
{
    int i, l;

    l = 0;
    for (i = 0; text[i]; i++) {
        if (text[i] == '^' &&
            (text[i + 1] == '^' || i + 1 == strlen(text))) {
            i++;
            continue;
        }

        text[l++] = text[i];
    }

    text[l] = '\0';
}

/*
=============
Boe_ClientSound
=============
*/
void G_ClientSound (gentity_t* ent, int soundIndex) {
    gentity_t* tent;

    tent = G_TempEntity(ent->r.currentOrigin, EV_GLOBAL_SOUND);
    tent->s.eventParm = soundIndex;
    tent->r.svFlags |= SVF_SINGLECLIENT;
    tent->r.singleClient = ent->s.number;
}

/*
=============
Boe_GlobalSound in 1fx. Mod
=============
*/
void G_GlobalSound (int soundIndex) {
    gentity_t* tent;

    tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
    tent->s.eventParm = soundIndex;
    tent->r.svFlags = SVF_BROADCAST;
}

/*
=============
Henk_CloseSound in 1fx. Mod
=============
*/
void G_CloseSound (vec3_t origin, int soundIndex) {
    gentity_t* tent;
    float radius = 100.0f;
    tent = G_TempEntity(origin, EV_GENERAL_SOUND); 
    tent->r.svFlags |= SVF_BROADCAST;
    tent->s.time2 = (int)(radius * 1000.0f);
    G_AddEvent(tent, EV_GENERAL_SOUND, soundIndex);
}

/*
Only performs same character check (e.g. arabic passwords in SoF2).
*/
qboolean checkAdminPassword(char* input) {
    if (!input || !*input) {
        return qfalse;
    }

    char* ptr = input + 1;
    while (*ptr) {
        if (*ptr != *input) {
            return qtrue;
        }
        ptr++;
    }

    return qfalse;
}


int G_ClientNumFromArg(gentity_t* ent, int argNum, const char* action,
    qboolean aliveOnly, qboolean otherAdmins, qboolean higherLvlAdmins,
    qboolean shortCmd)
{
    char arg[16];
    char cleanName[MAX_NETNAME];
    char multipleNamesFound[512];
    int clientID = -1, clientsFound = 0;
    qboolean isName = qfalse;
    int argc = shortCmd ? G_GetChatArgumentCount() : 0;

    // Determine if there are parameters present.
    // Not being present could indicate a short command from the console.
    // Now fetch the argument.
    if (shortCmd && argc > 0) {
        Q_strncpyz(arg, G_GetChatArgument(1), sizeof(arg));
    }
    else {
        trap_Argv(argNum, arg, sizeof(arg));
    }

    // Check if there's a parameter specified at all.
    if (strlen(arg) == 0) {
        G_printInfoMessage(ent, "You haven't entered a valid player ID/player name.");
        return -1;
    }

    // We can continue.
    // Now check whether the client specified an ID or a name.
    isName = qfalse;
    for (int i = 0; arg[i] != '\0'; i++) {
        if (!isdigit(arg[i])) {
            // Letter was found, safe to assume this is a name.
            isName = qtrue;
            break;
        }
    }

    if (isName) {
        // Argument potentially contains a name.

        // Compare using lowercase characters.
        Q_strlwr(arg);

        // Loop through connected clients, and check who the client meant.
        clientsFound = 0;
        memset(multipleNamesFound, 0, sizeof(multipleNamesFound));

        for (int i = 0; i < level.numConnectedClients; i++) {
            gclient_t* cl = g_entities[level.sortedClients[i]].client;
            Q_strncpyz(cleanName, cl->pers.cleanName, sizeof(cleanName));
            Q_strlwr(cleanName);

            if (strstr(cleanName, arg)) {
                // Match. Save the client ID.
                clientID = cl - level.clients;
                clientsFound++;

                // Also fill the "multiple found" buffer.
                // We might need to print this later on.
                snprintf(multipleNamesFound + strlen(multipleNamesFound),
                    sizeof(multipleNamesFound) - strlen(multipleNamesFound),
                    "^1[#%i] ^7%s, ", clientID, cl->pers.cleanName);
            }
        }

        // Multiple clients found?
        if (clientsFound > 1) {
            // Remove the trailing ", ".
            multipleNamesFound[strlen(multipleNamesFound) - 2] = '\0';

            // Print message.
            G_printInfoMessage(ent, "Multiple names found with ^3%s^7: %s", arg, multipleNamesFound);
            return -1;
        }

    }
    else {
        // Argument only contains numbers.
        clientID = atoi(arg);
    }

    // Do some checks, ID must be a valid ID.
    if (clientID < 0 || clientID >= g_maxclients.integer) {
        G_printInfoMessage(ent, "You haven't entered a valid player ID/player name.");
        return -1;
    }

    if (g_entities[clientID].client->pers.connected == CON_DISCONNECTED) {
        G_printInfoMessage(ent, "This client is not connected.");
        return -1;
    }

    // Check if the executing Admin is doing this,
    // and not RCON. If so, perform some extra checks.
    if (ent && ent->client && ent->client->sess.adminLevel && g_entities[clientID].client->sess.adminLevel) {
        // Don't allow to execute the command on higher level Admins.
        if (g_entities[clientID].client->sess.adminLevel > ent->client->sess.adminLevel && !higherLvlAdmins) {
            G_printInfoMessage(ent, "You cannot %s higher level Admins.", action);
            return -1;
        }

        // Don't allow this command on other Admins?
        if (!otherAdmins) {
            G_printInfoMessage(ent, "You cannot %s other Admins.", action);
            return -1;
        }
    }
    // Perform an additional check for when a non-Admin calls this function,
    // and this action is not allowed on Admins.
    else if (!otherAdmins && ent && ent->client && !ent->client->sess.adminLevel
        && g_entities[clientID].client->sess.adminLevel) {
        G_printInfoMessage(ent, "You cannot %s an Admin.", action);
        return -1;
    }

    // Check if the targeted player has to be alive for this command.
    if (aliveOnly) {
        // Are they dead?
        if (G_IsClientDead(g_entities[clientID].client)) {
            G_printInfoMessage(ent, "You cannot %s dead players.", action);
            return -1;
        }

        // Are they spectating?
        if (G_IsClientSpectating(g_entities[clientID].client)) {
            G_printInfoMessage(ent, "You cannot %s a spectator.", action);
            return -1;
        }
    }

    // Everything checked out, return the ID.
    return clientID;
}

char* G_ColorizeMessage(char* broadcast)
{
    int i, newWordLength;
    char* tempBroadcast;
    static char newBroadcast[MAX_STRING_CHARS];
    char line[128], remainingLine[128];
    int lineLength;
    int newWordPosition = 0;
    qboolean parseLine = qtrue;
    char* tempNewline = broadcast;

    // Reset broadcast buffer.
    memset(newBroadcast, 0, sizeof(newBroadcast));

    // Loop through whole broadcast.
    while (parseLine) {
        // Properly reset line to avoid having to NULL terminate it everywhere.
        memset(line, 0, sizeof(line));

        // Check for more lines.
        tempNewline = strstr(broadcast, "\n");
        if (tempNewline == NULL) {
            lineLength = strlen(broadcast);

            // Last line, don't go further after this.
            parseLine = qfalse;
        }
        else {
            lineLength = tempNewline - broadcast + 1;
        }

        // Check if line is too big to copy.
        if (lineLength >= sizeof(line)) {
            lineLength = sizeof(line) - 1;
        }

        // Copy line.
        strncpy(line, broadcast, lineLength);

        // First we check the line. Should colours be applied to it?
        // A backslash can never be applied to any command in-game, like broadcast. Use that to determine what word should be highlighted.
        tempBroadcast = strstr(line, "\\");
        if (tempBroadcast != NULL) {
            // OK, a word is in the line. Find position in actual broadcast.
            tempBroadcast = strstr(broadcast, "\\");

            // A word is found that should be highlighted.
            // First determine the start position of the word in the broadcast.
            newWordPosition = (int)(tempBroadcast - broadcast);

            // Now determine the length of the string.
            strncpy(remainingLine, tempBroadcast, strlen(line) - newWordPosition);
            remainingLine[strlen(line) - newWordPosition] = '\0';
            tempBroadcast = strstr(remainingLine, " ");
            if (tempBroadcast == NULL) {
                // Word is as long as the remaining string.
                newWordLength = strlen(remainingLine) - 1;
            }
            else {
                // Make sure we properly get the address.
                char* c = broadcast;
                c += newWordPosition + 1;

                // Word ends somewhere.
                newWordLength = strstr(c, " ") - (broadcast + newWordPosition) - 1;
            }

            // Now we apply the colors to the broadcast if there are colors to apply it to.
            // Do check the size of the line, if it's getting too big simply cut some colors to make room for the line to fit.
            if (newWordLength > 0 && newWordLength < 64) {
                // Check how many colors there are available to use.
                int availableColors = strlen(g_serverColors.string);
                strncat(newBroadcast, broadcast, newWordPosition);

                if (!availableColors) {
                    // Copy the string to it, excluding backslash.
                    strncat(newBroadcast, broadcast + newWordPosition + 1, strlen(broadcast) - newWordPosition);
                }
                else {
                    // Apply colors.
                    char wordToCopy[64];
                    int color, newLineLen;

                    // Check how many colors we can copy.
                    if (newWordLength >= availableColors) {
                        color = 0;
                    }
                    else {
                        color = availableColors - newWordLength;
                    }

                    // Check if everything fits properly.
                    newLineLen = lineLength - 1 + ((availableColors - color) * 2); // Exclude the \ character and add the colors.

                    while (newLineLen > MAX_BROADCAST_LINE) {
                        color++;
                        if (color >= availableColors)
                            break; // Line will never fit.

                        if ((lineLength - 1 + ((availableColors - color) * 2)) <= MAX_BROADCAST_LINE) {
                            newLineLen = lineLength - 1 + ((availableColors - color) * 2);
                            break;
                        }
                    }

                    // Copy each individual char to the new buffer, plus the color associated to it.
                    strncpy(wordToCopy, broadcast + newWordPosition + 1, newWordLength);
                    for (i = 0; i < newWordLength; i++) {
                        if (color < availableColors) {
                            strcat(newBroadcast, va("^%c%c", g_serverColors.string[color], wordToCopy[i]));
                            color++;
                        }
                        else {
                            strcat(newBroadcast, va("%c", wordToCopy[i]));
                        }
                    }

                    // Copy a trailing ^7 so all text won't be messed up if the colors aren't set to fade out correctly.
                    strcat(newBroadcast, "^7");

                    // Is there a remaining string in the line?
                    if (tempBroadcast) {
                        strncat(newBroadcast, broadcast + newWordPosition + newWordLength + 1, strlen(line) - newWordLength - newWordPosition - 1);
                    }
                }
            }
        }
        else {
            strncat(newBroadcast, line, strlen(line));
        }

        // Advance broadcast.
        broadcast += (int)(tempNewline - broadcast) + 1;
    }

    // Boe!Man 3/13/15: Replace newlines with spaces when the game is paused.
    if (level.paused) {
        for (i = 0; i < strlen(newBroadcast); i++) {
            if (newBroadcast[i] == '\n') {
                newBroadcast[i] = ' ';
            }
        }
    }

    return newBroadcast;
}

void G_Broadcast(int broadcastLevel, gentity_t* to, qboolean playSound, char* broadcast, ...) {
    va_list     argptr;
    char        text[1024];

    va_start(argptr, broadcast);
    Q_vsnprintf(text, sizeof(text), broadcast, argptr);
    va_end(argptr);

    int i;
    char* newBroadcast = G_ColorizeMessage(text);
    // If to is NULL, we're dealing with a global message (equals old way of broadcasting to -1).
    if (to == NULL) {

        if (playSound) {
            G_GlobalSound(level.actionSoundIndex);
        }

        if (!level.paused) {
            for (i = 0; i < level.numConnectedClients; i++) {
                gentity_t* other = &g_entities[level.sortedClients[i]];

                // Skip any client that isn't connected.
                if (other->client->pers.connected != CON_CONNECTED) {
                    continue;
                }
                // Skip any client that received a more important message in the last 5 seconds.
                if (other->client->sess.lastMessagePriority > broadcastLevel && level.time < (other->client->sess.lastMessage + 4000)) {
                    continue;
                }

                trap_SendServerCommand(other - g_entities, va("cp \"@%s\n\"", newBroadcast));
                other->client->sess.lastMessagePriority = broadcastLevel;
                other->client->sess.lastMessage = level.time;
            }
        }
        else {
            // Boe!Man 3/13/15: In order to print messages during pause we need to call a different function.
            trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@%s", level.time + 5000, newBroadcast));
        }
    }
    else if (broadcastLevel >= to->client->sess.lastMessagePriority || level.time > (to->client->sess.lastMessage + 4000)) {
        trap_SendServerCommand(to - g_entities, va("cp \"@%s\n\"", newBroadcast));
        to->client->sess.lastMessagePriority = broadcastLevel;
        to->client->sess.lastMessage = level.time;
    }

}

// Generic function to handle both console and chat messages.
void QDECL G_printMessage(qboolean isChat, qboolean toAll, gentity_t* ent, char* prefix, const char* msg, va_list argptr)
{
    int         len;
    char        text[1024];
    int         remainingSize = sizeof(text) - 4;

    qboolean toConsole = qfalse;

    if (!isChat && !toAll && (!ent || !ent->client)) {
        toConsole = qtrue;
    }

    memset(text, 0, sizeof(text));

    // Construct the prefix (e.g., [Info], [H&S], etc.)
    if (isChat) {
        strncat(text, "chat -1 \"", remainingSize);
        prefix = G_ColorizeMessage(prefix);
    }
    else if (!toConsole){
        strncat(text, "print \"^3[", remainingSize);
    }
    else {
        strncat(text, "^3[", remainingSize);
    }
    strncat(text, prefix, remainingSize - strlen(text));
    if (isChat) {
        strncat(text, ": ^7", remainingSize - strlen(text));
    }
    else {
        strncat(text, "] ^7", remainingSize - strlen(text));
    }
    
    len = vsnprintf(text + strlen(text), remainingSize - strlen(text), msg, argptr);

    if (!toConsole) {
        strncat(text + strlen(text), "\n\"", sizeof(text) - strlen(text) - 1);
    }
    else {
        strncat(text + strlen(text), "\n", sizeof(text) - strlen(text) - 1);
    }

    
    if (ent && ent->client) {
        trap_SendServerCommand(ent - g_entities, text);
    }
    else if (!toConsole) {
        trap_SendServerCommand(-1, text);
    }
    else {
        Com_Printf(text);
    }
}

void QDECL G_printInfoMessage(gentity_t* ent, const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qfalse, qfalse, ent, "Info", msg, argptr);
    va_end(argptr);
}

void QDECL G_printChatInfoMessage(gentity_t* ent, const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qtrue, qfalse, ent, "\\Info", msg, argptr);
    va_end(argptr);
}

void QDECL G_printInfoMessageToAll(const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qfalse, qtrue, NULL, "Info", msg, argptr);
    va_end(argptr);
}

void QDECL G_printChatInfoMessageToAll(const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qtrue, qtrue, NULL, "\\Info", msg, argptr); 
    va_end(argptr);

}

void QDECL G_printCustomChatMessageToAll(const char* prefix, const char* msg, ...) {

    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qtrue, qtrue, NULL, va("\\%s", prefix), msg, argptr);
    va_end(argptr);

}

void QDECL G_printCustomChatMessage(gentity_t* ent, const char* prefix, const char* msg, ...) {

    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qtrue, qfalse, ent, va("\\%s", prefix), msg, argptr);
    va_end(argptr);

}

void QDECL G_printCustomMessage(gentity_t* ent, const char* prefix, const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qfalse, qfalse, ent, prefix, msg, argptr);
    va_end(argptr);
}

void QDECL G_printCustomMessageToAll(const char* prefix, const char* msg, ...)
{
    va_list     argptr;

    va_start(argptr, msg);
    G_printMessage(qfalse, qtrue, NULL, prefix, msg, argptr);
    va_end(argptr);
}

void getSubnet(char* ip, char* output, int outputSize) {

    char origIp[MAX_IP], subnetIp[MAX_IP];

    Q_strncpyz(origIp, ip, sizeof(origIp));
    Com_Memset(subnetIp, 0, sizeof(subnetIp));


    char* token = strtok(origIp, ".");
    int i = 0;

    while (token != NULL && i < g_subnetOctets.integer) {
        if (i > 0) {
            strcat(subnetIp, ".");  // Add period between octets
        }
        strcat(subnetIp, token);  // Append the octet to the subnet IP string
        token = strtok(NULL, ".");
        i++;
    }

    Q_strncpyz(output, subnetIp, outputSize);

}

int swapTeams(qboolean autoSwap) {

    if (!level.gametypeData->teams) {
        return TEAMACTION_INCOMPATIBLE_GAMETYPE;
    }

    if (level.numPlayingClients < 2) {
        return TEAMACTION_NOT_ENOUGH_PLAYERS;
    }

    for (int i = 0; i < level.numConnectedClients; i++) {
        gentity_t* tent = &g_entities[level.sortedClients[i]];

        if (G_IsClientSpectating(tent->client)) {
            continue;
        }

        SetTeam(tent, tent->client->sess.team == TEAM_RED ? "b" : "r", NULL, qtrue);
    }

    if (autoSwap) {
        G_printCustomMessageToAll("Auto Action", "Swapteams!");
    }

    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));

    return TEAMACTION_DONE;

}

int evenTeams(qboolean autoEven) {

    if (!level.gametypeData->teams) {
        return TEAMACTION_INCOMPATIBLE_GAMETYPE;
    }

    if (level.numPlayingClients < 2) {
        return TEAMACTION_NOT_ENOUGH_PLAYERS;
    }

    int redPlayers = TeamCount(-1, TEAM_RED, NULL);
    int bluePlayers = TeamCount(-1, TEAM_BLUE, NULL);

    // JANFIXME H&S / H&Z SPECIFICS
    int redBlueDiff = abs(redPlayers - bluePlayers);
    if (redBlueDiff <= 1) {
        return TEAMACTION_EVEN;
    }

    redBlueDiff /= 2;
    team_t teamToPick = TEAM_RED;

    if (bluePlayers < redPlayers) {
        teamToPick = TEAM_BLUE;
    }

    qboolean havePlayersBeenMoved = qfalse;

    for (int i = 0; i < redBlueDiff; i++) {

        gentity_t* recipient = getLastConnectedClientInTeam(teamToPick == TEAM_RED ? TEAM_BLUE : TEAM_RED, qtrue);

        if (recipient) {
            SetTeam(recipient, teamToPick == TEAM_RED ? "r" : "b", NULL, qtrue);
            havePlayersBeenMoved = qtrue;
        }
        else if (!havePlayersBeenMoved) {
            return TEAMACTION_NOT_ENOUGH_PLAYERS;
        }

    }

    if (autoEven) {
        G_printCustomMessageToAll("Auto Action", "Eventeams!");
    }

    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));

    return TEAMACTION_DONE;

}

/*
Shuffleteams from 1fxmod.
*/
int shuffleTeams(qboolean autoShuffle) {

    if (!level.gametypeData->teams) {
        return TEAMACTION_INCOMPATIBLE_GAMETYPE;
    }

    if (level.numPlayingClients < 2) {
        return TEAMACTION_NOT_ENOUGH_PLAYERS;
    }

    int redTeamCount = TeamCount(-1, TEAM_RED, NULL);
    int blueTeamCount = TeamCount(-1, TEAM_BLUE, NULL);

    int currentRedTeamCount = 0;
    int currentBlueTeamCount = 0;

    team_t newTeam = TEAM_RED;
    char userinfo[MAX_INFO_STRING];

    // shuffle from here.

    for (int i = 0; i < level.numConnectedClients; i++) {

        // Skip clients that are spectating.
        if (g_entities[level.sortedClients[i]].client->sess.team == TEAM_SPECTATOR) {
            continue;
        }

        // Start shuffling using irand, or put them to the team that needs more players when one is on its preserved rate.
        if (currentRedTeamCount == redTeamCount) { // Blimey, we're on max.
            newTeam = TEAM_BLUE;
        }
        else if (currentBlueTeamCount == blueTeamCount) {
            newTeam = TEAM_RED;
        }
        else {
            newTeam = irand(1, 2);
            if (newTeam == TEAM_RED) {
                currentRedTeamCount += 1;
            }
            else {
                currentBlueTeamCount += 1;
            }
        }

        // Drop any gametype items they might have.
        if (g_entities[level.sortedClients[i]].s.gametypeitems > 0) {
            G_DropGametypeItems(&g_entities[level.sortedClients[i]], 0);
        }

        // Remove their weps and set as ghost.
        g_entities[level.sortedClients[i]].client->ps.stats[STAT_WEAPONS] = 0;
        G_StartGhosting(&g_entities[level.sortedClients[i]]);

        // Do the team changing.
        g_entities[level.sortedClients[i]].client->sess.team = (team_t)newTeam;

        // Take care of the bots.
        if (g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT) { // Reset bots to set them to another team
            trap_GetUserinfo(level.sortedClients[i], userinfo, sizeof(userinfo));

            Info_SetValueForKey(userinfo, "team", va("%s", newTeam == TEAM_RED ? "red" : "blue"));
            trap_SetUserinfo(level.sortedClients[i], userinfo);
            g_entities[level.sortedClients[i]].client->sess.team = (team_t)newTeam;
            //if (current_gametype.value != GT_HS) { // JANFIXME - H&S SPECIFICS
                g_entities[level.sortedClients[i]].client->pers.identity = BG_FindTeamIdentity(level.gametypeTeam[newTeam], -1);
            //}
        }

        // Prep. for change & respawn.
        g_entities[level.sortedClients[i]].client->pers.identity = NULL;
        ClientUserinfoChanged(level.sortedClients[i]);
        CalculateRanks();

        G_StopFollowing(&g_entities[level.sortedClients[i]]);
        G_StopGhosting(&g_entities[level.sortedClients[i]]);
        trap_UnlinkEntity(&g_entities[level.sortedClients[i]]);
        ClientSpawn(&g_entities[level.sortedClients[i]]);
    }

    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
    return TEAMACTION_DONE;
}

gentity_t* getLastConnectedClient(qboolean respectGametypeItems) {
    return getLastConnectedClientInTeam(-1, respectGametypeItems);
}

gentity_t* getLastConnectedClientInTeam(int team, qboolean respectGametypeItems) {
    int maxConnectedTime = 0, client = -1;

    for (int i = 0; i < level.numConnectedClients; i++) {
        if (g_entities[level.sortedClients[i]].client->pers.connected == CON_DISCONNECTED) {
            continue;
        }

        if (respectGametypeItems && g_entities[level.sortedClients[i]].s.gametypeitems) {
            continue;
        }

        if (team != -1 && g_entities[level.sortedClients[i]].client->sess.team != team) {
            continue;
        }

        if (g_entities[level.sortedClients[i]].client->pers.enterTime > maxConnectedTime) {
            maxConnectedTime = g_entities[level.sortedClients[i]].client->pers.enterTime;
            client = i;
        }

    }

    if (client != -1) {
        return &g_entities[level.sortedClients[client]];
    }

    return NULL;
}

void runoverPlayer(gentity_t* recipient) {

    vec3_t      direction, fireAngles;

    VectorCopy(recipient->client->ps.viewangles, fireAngles);
    AngleVectors(fireAngles, direction, NULL, NULL);
    direction[0] *= -1.0;
    direction[1] *= -1.0;
    direction[2] = 0.0;
    VectorNormalize(direction);

    G_ClientSound(recipient, G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3"));

    // Do the actual action.
    recipient->client->ps.velocity[2] = 20;
    recipient->client->ps.weaponTime = 3000;
    G_Damage(recipient, NULL, NULL, NULL, NULL, 15, 0, MOD_CRUSH, HL_NONE);
    G_ApplyKnockback(recipient, direction, 400.0f);
}

void uppercutPlayer(gentity_t* recipient, int ucLevel) {

    recipient->client->ps.pm_flags |= PMF_JUMPING;
    recipient->client->ps.groundEntityNum = ENTITYNUM_NONE;

    if (ucLevel) {
        recipient->client->ps.velocity[2] = 200 * ucLevel;
    }
    else {
        recipient->client->ps.velocity[2] = 1000;
    }

    G_ClientSound(recipient, G_SoundIndex("sound/weapons/rpg7/fire01.mp3"));
}

void spinView(gentity_t* recipient) {

    if (recipient->client->sess.lastSpin < level.time) {
        recipient->client->sess.spinViewState = SPINVIEW_NONE;
        return;
    }

    switch (recipient->client->sess.spinViewState) {
    case SPINVIEW_NONE:
        return;
    case SPINVIEW_FAST:
        recipient->client->sess.nextSpin = level.time + 10;
        break;
    case SPINVIEW_SLOW:
        recipient->client->sess.nextSpin = level.time + 250;
        break;
    default:
        recipient->client->sess.spinViewState = SPINVIEW_NONE;
        return;
    }

    vec3_t spin;
    VectorCopy(recipient->client->ps.viewangles, spin);
    spin[0] = (float)Q_irand(0, 360);
    spin[1] = fmod(spin[1] + 20, 360.0);
    SetClientViewAngle(recipient, spin);

    if (recipient->client->sess.nextSpinSound < level.time) {
        G_ClientSound(recipient, G_SoundIndex("sound/npc/air1/guard02/laughs.mp3"));
        recipient->client->sess.nextSpinSound = level.time + 500;
    }
}

void stripClient(gentity_t* recipient, qboolean handsUp) {

    int idle;
    handsUp = qfalse;
    if (recipient->client->sess.team == TEAM_SPECTATOR) {
        return;
    }

    recipient->client->ps.zoomFov = 0;
    recipient->client->ps.pm_flags &= ~(PMF_GOGGLES_ON | PMF_ZOOM_FLAGS);

    recipient->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
    memset(recipient->client->ps.ammo, 0, sizeof(recipient->client->ps.ammo));
    memset(recipient->client->ps.clip, 0, sizeof(recipient->client->ps.clip));
    recipient->client->ps.stats[STAT_WEAPONS] = 0;

    if (handsUp) {
        recipient->client->ps.weapon = WP_NONE;

    }
    else {
        recipient->client->ps.stats[STAT_WEAPONS] |= (1 << WP_KNIFE);
        recipient->client->ps.clip[ATTACK_NORMAL][WP_KNIFE] = weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
        recipient->client->ps.firemode[WP_KNIFE] = BG_FindFireMode(WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO);
        recipient->client->ps.weapon = WP_KNIFE;
        BG_GetInviewAnim(recipient->client->ps.weapon, "idle", &idle);
        recipient->client->ps.weaponAnimId = idle;
    }

    recipient->client->ps.weaponstate = WEAPON_READY;
    recipient->client->ps.weaponTime = 0;
    recipient->client->ps.weaponAnimTime = 0;
    // Boe!Man 6/23/13: Fixed crash bug in Linux, simply because 'bg_outfittingGroups[-1][client->pers.outfitting.items[-1]]' equals 0.
    recipient->client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[0].giTag;

}

void stripTeam(int team, qboolean handsUp) {
    gentity_t* recipient;

    if (team == TEAM_SPECTATOR) {
        return;
    }

    for (int i = 0; i < level.numConnectedClients; i++) {
        recipient = &g_entities[level.sortedClients[i]];

        if (recipient->client->sess.team == team) {
            stripClient(recipient, handsUp);
        }
    }
}

void stripEveryone(qboolean handsUp) {
    gentity_t* recipient;

    for (int i = 0; i < level.numConnectedClients; i++) {
        recipient = &g_entities[level.sortedClients[i]];
        stripClient(recipient, handsUp);
    }
}