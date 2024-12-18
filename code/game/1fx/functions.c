
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

char* G_GetChatArgument(int argNum, qboolean retainColors)
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

    if (!retainColors) {
        // Remove colors from arg.
        G_RemoveColorEscapeSequences(newArg);
    }

    return newArg;
}

/*
============
G_GetArg

a wrapper for trap_Argv and G_GetChatArgument
============
*/
char* G_GetArg(int argNum, qboolean shortCmd, qboolean retainColors)
{
    static char arg[MAX_STRING_CHARS];
    memset(arg, 0, sizeof(arg));

    if (!shortCmd && G_GetChatArgumentCount() <= 1) {
        //argNum++; if I use it in admin commands, I already have the correct argnum in play. Therefore this will actually mess it up.
        trap_Argv(argNum, arg, sizeof(arg));
    }
    else {
        Q_strncpyz(arg, G_GetChatArgument(argNum, retainColors), sizeof(arg));
    }

    return arg;
}

char* concatArgs(int fromArgNum, qboolean shortCmd, qboolean retainColors) {

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
        arg = G_GetArg(fromArgNum, shortCmd, retainColors);
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
        Q_strncpyz(arg, G_GetChatArgument(argNum, qfalse), sizeof(arg));
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

    for (int i = 0; i < level.numConnectedClients; i++) {
        gentity_t* tent = &g_entities[level.sortedClients[i]];

        if (G_IsClientSpectating(tent->client)) {
            continue;
        }

        if (tent->s.gametypeitems > 0) {
            G_DropGametypeItems(tent, 0);
        }


        tent->client->ps.stats[STAT_WEAPONS] = 0;
        TossClientItems(tent);
        G_StartGhosting(tent);

        if (tent->client->sess.team == TEAM_RED) {
            tent->client->sess.team = TEAM_BLUE;
        }
        else if (tent->client->sess.team == TEAM_BLUE) {
            tent->client->sess.team = TEAM_RED;
        }

        tent->client->pers.identity = NULL;
        ClientUserinfoChanged(tent->s.number);
        CalculateRanks();

        G_StopFollowing(tent);
        G_StopGhosting(tent);
        trap_UnlinkEntity(tent);
        ClientSpawn(tent);
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
            havePlayersBeenMoved = qtrue;


            if (recipient->s.gametypeitems > 0) {
                G_DropGametypeItems(recipient, 0);
            }


            recipient->client->ps.stats[STAT_WEAPONS] = 0;
            TossClientItems(recipient);
            G_StartGhosting(recipient);
            recipient->client->sess.team = teamToPick;

            recipient->client->pers.identity = NULL;
            ClientUserinfoChanged(recipient->s.number);
            CalculateRanks();

            G_StopFollowing(recipient);
            G_StopGhosting(recipient);
            trap_UnlinkEntity(recipient);
            ClientSpawn(recipient);
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
    //handsUp = qfalse;
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

/*
==================
G_kickPlayer

Kicks player from the server. Includes who did it and the reason.
==================
*/

void kickPlayer(gentity_t* to, gentity_t* by, char* action, char* reason) {
    trap_DropClient(to - g_entities, va("You got %s.\nBy: %s\nReason: %s", action, getNameOrArg(by, "RCON", qtrue), reason));
}

char* getClanTypeAsText(clanType_t clanType) {

    switch (clanType) {
    case CLANTYPE_GUID:
        return "GUID";
    case CLANTYPE_IP:
        return "IP";
    case CLANTYPE_PASS:
        return "Pass";
    default:
        return "";
    }

}

void clan_setPassword(gentity_t* ent, char* password) {

    if (!ent->client->sess.setClanPassword && ent->client->sess.clanType != CLANTYPE_PASS) {
        G_printInfoMessage(ent, "You cannot change your clan-password as you're either not a clan member or you don't have pass-clan powers.");
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

    if (ent->client->sess.setClanPassword || (ent->client->sess.clanMember && ent->client->sess.clanType == CLANTYPE_PASS)) {
        rowsAffected = dbUpdateClanPass(ent->client->sess.clanName, password);

        if (rowsAffected != 1) {
            logSystem(LOGLEVEL_WARN, "rowsAffected on updateclanpass is %s (!= 1)", rowsAffected);
            G_printInfoMessage(ent, "Something went wrong, please try again...");
            return;
        }

        G_printInfoMessage(ent, "Your password is now set.");
        ent->client->sess.clanMember = qtrue;
        ent->client->sess.setClanPassword = qfalse;
    }
    else {
        G_printInfoMessage(ent, "You cannot change the password because you're either not a clan member or do not have pass-clan rights.");
    }

}

void clan_Login(gentity_t* ent, char* password) {

    if (ent->client->sess.clanMember) {
        G_printInfoMessage(ent, "You already have clan powers.");
        return;
    }

    if (!password || strlen(password) < 6) {
        G_printInfoMessage(ent, "No or too short password entered. Try again.");
        return;
    }

    qboolean clanMember = dbGetClan(CLANTYPE_PASS, ent, password);

    if (!clanMember) {
        G_printInfoMessage(ent, "No clan powers found with this name+password combination.");
        return;
    }

    ent->client->sess.clanMember = qtrue;
    ent->client->sess.clanType = CLANTYPE_PASS;
    Q_strncpyz(ent->client->sess.clanName, ent->client->pers.cleanName, sizeof(ent->client->sess.clanName));
    ent->client->sess.setClanPassword = qfalse;
    G_printInfoMessage(ent, "Your clan powers are set.");
}

gentity_t* NV_projectile(gentity_t* ent, vec3_t start, vec3_t dir, int weapon, int damage) {
    gentity_t* missile;

    missile = G_Spawn();

    if (weapon == WP_M84_GRENADE || weapon == WP_M15_GRENADE) {
        missile->r.singleClient = ent->client->ps.clientNum;
        missile->r.svFlags |= SVF_SINGLECLIENT;
    }
    else {
        missile->r.svFlags |= SVF_BROADCAST;
        missile->damage = 1;
        missile->splashDamage = 1;
    }
    missile->nextthink = level.time + 1000;
    missile->think = G_ExplodeMissile;
    missile->s.eType = ET_MISSILE;
    missile->s.weapon = weapon;
    missile->r.ownerNum = ent->s.number;
    missile->parent = ent;
    missile->classname = "grenade";
    missile->splashRadius = 500;
    missile->s.eFlags = EF_BOUNCE_HALF | EF_BOUNCE;
    missile->clipmask = MASK_SHOT | CONTENTS_MISSILECLIP;
    //missile->s.pos.trType         = TR_INTERPOLATE;
    missile->s.pos.trType = TR_HEAVYGRAVITY;
    missile->s.pos.trTime = level.time - 50;

    if (weapon == WP_ANM14_GRENADE) {
        missile->splashRadius = 150;
        missile->dflags = DAMAGE_AREA_DAMAGE;
        missile->splashDamage = 1;
        missile->methodOfDeath = MOD_ANM14_GRENADE;
    }

    VectorCopy(start, missile->s.pos.trBase);
    VectorCopy(dir, missile->s.pos.trDelta);

    if (ent->client->ps.pm_flags & PMF_JUMPING)
        VectorAdd(missile->s.pos.trDelta, ent->s.pos.trDelta, missile->s.pos.trDelta);

    SnapVector(missile->s.pos.trDelta);           // save net bandwidth

    VectorCopy(start, missile->r.currentOrigin);
    return missile;
}

void popPlayer(gentity_t* ent, popAction_t popAction) {

    G_ClientSound(ent, G_SoundIndex("sound/npc/air1/guard02/laughs.mp3"));

    // Allow to pop players in godmode.
    ent->flags &= ~FL_GODMODE;

    // Do the actual damage.
    G_Damage(ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD | HL_FOOT_RT | HL_FOOT_LT | HL_LEG_UPPER_RT | HL_LEG_UPPER_LT | HL_HAND_RT | HL_HAND_LT | HL_WAIST | HL_CHEST | HL_NECK);

    if (popAction == POPACTION_CAMP) {
        G_Broadcast(BROADCAST_CMD, NULL, qtrue, "%s\nwas \\popped for camping!", ent->client->pers.netname);
        G_printCustomMessageToAll("Auto Action", "%s was popped for camping!", ent->client->pers.cleanName);
    }
}

void muteClient(gentity_t* ent, int duration) {

    ent->client->sess.muted = qtrue;

    for (int i = 0; i < MAX_CLIENTS; i++) {

        mute_t* muteInfo = &level.mutedClients[i];

        if (muteInfo->used) {
            continue;
        }

        Q_strncpyz(muteInfo->ip, ent->client->pers.ip, sizeof(muteInfo->ip));
        muteInfo->time = duration * 60000;
        muteInfo->totalDuration = duration;
        muteInfo->startTime = level.time;
        muteInfo->used = qtrue;
        level.numMutedClients++;

        break;

    }
}

void unmuteClient(gentity_t* ent) {

    ent->client->sess.muted = qfalse;

    for (int i = 0; i < MAX_CLIENTS; i++) {

        mute_t* muteInfo = &level.mutedClients[i];

        if (muteInfo->used && !Q_stricmp(ent->client->pers.ip, muteInfo->ip)) {
            Com_Memset(muteInfo, 0, sizeof(mute_t));
            level.numMutedClients--;
            // We do not stop checking here.
            // Mutes are IP based, if there are multiple clients getting muted with the same IP, we have to remove them. Otherwise we reapply the mute on recon.
            // So, unfortunate side-effect.
        }

        gentity_t* tent = &g_entities[i];

        if (tent && tent->inuse && tent->client) {
            if (!Q_stricmp(tent->client->pers.ip, ent->client->pers.ip)) {
                tent->client->sess.muted = qfalse;
            }
        }

    }

}

void checkMutes() {

    if (level.numMutedClients > 0) {
        for (int i = 0; i < MAX_CLIENTS && level.numMutedClients > 0; i++) {
            mute_t* muteInfo = &level.mutedClients[i];

            // There is a potential edge-case scenario here.
            // If there is more than 1 client on the same IP muted, then they will get unmuted when the first client gets automatically unmuted.
            // But the mute will remain in the array. It will be cleared out (and will not have a matching "muted" client) when its time comes.
            // BUT, when the map changes, the mutes will be reapplied given that they're still valid.
            // FIXME if people complain about mutes.
            if (muteInfo->used) {
                if (level.time > (muteInfo->startTime + muteInfo->time)) {

                    for (int j = 0; j < level.numConnectedClients; j++) {
                        gentity_t* ent = &g_entities[level.sortedClients[j]];

                        if (!Q_stricmp(ent->client->pers.ip, muteInfo->ip) && ent->client->sess.muted) {
                            G_printCustomMessageToAll("Auto Action", "%s has been unmuted.", ent->client->pers.cleanName);
                            ent->client->sess.muted = qfalse;
                        }
                    }

                    Com_Memset(muteInfo, 0, sizeof(mute_t));
                    level.numMutedClients--;
                }
            }
            
        }
    }
}

/*
Run only during clientconnect.
This will re-read mutes into the mutes struct array
*/
void reapplyMuteAfterConnect(gentity_t* ent) {

    for (int i = 0; i < MAX_CLIENTS && level.numMutedClients > 0; i++) {

        mute_t* muteInfo = &level.mutedClients[i];

        if (muteInfo->used && !Q_stricmp(ent->client->pers.ip, muteInfo->ip)) {
            ent->client->sess.muted = qtrue;
            return;
        }

    }
}

qboolean isClientMuted(gentity_t* ent, qboolean printMsg) {

    if (ent->client->sess.muted) {

        if (level.numMutedClients == 0) {
            ent->client->sess.muted = qfalse;
            return qfalse;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {

            mute_t* muteInfo = &level.mutedClients[i];

            if (muteInfo->used && !Q_stricmp(muteInfo->ip, ent->client->pers.ip)) {
                if (printMsg) {
                    int remainingMinutes = (muteInfo->startTime + muteInfo->time - level.time) / 1000 / 60;
                    int remainingSeconds = (muteInfo->startTime + muteInfo->time - level.time) / 1000 % 60;
                    G_printInfoMessage(ent, "You were muted for %d minutes. Time remaining: %d minutes %d seconds.", muteInfo->totalDuration, remainingMinutes, remainingSeconds);
                }

                return qtrue;
            }

        }

    }

    return qfalse;
}

// Section states from 1fxmod.

void blockSection(gentity_t* ent, mapSection_t section) {
    if (section < MAPSECTION_NOMIDDLE) {
        G_SpawnVector("origin", "0", level.noLR[section]);

        // Boe!Man 11/26/13: Not depending on the original state, if this check fails, it's always disabled..
        if (level.noLR[section][2] == 0) {
            level.autoSectionActive[section] = qfalse;
        }
    }

    // Boe!Man 11/21/13: The entity is found.
    level.noSectionEntFound[section] = qtrue;

    // Boe!Man 11/21/13: Is auto nolower enabled?
    if (ent->autoSection && strstr(ent->autoSection, "yes") && ent->min_players > 0) {
        if (!ent->wait) { // There should be a delay. Default is to wait 10 seconds.
            ent->wait = 10;
        }

        // Check if a team is defined.
        if (ent->team && strlen(ent->team) > 0) {
            if (strstr(ent->team, "red")) {
                ent->team2 = TEAM_RED;
            }
            else if (strstr(ent->team, "blue")) {
                ent->team2 = TEAM_BLUE;
            }
            else if (strstr(ent->team, "r")) {
                ent->team2 = TEAM_RED;
            }
            else if (strstr(ent->team, "b")) {
                ent->team2 = TEAM_BLUE;
            }
            else { // All.
                ent->team2 = TEAM_FREE;
            }
        }
        else { // All.
            ent->team2 = TEAM_FREE;
        }

        // The think function needs to know what section is about to be closed/opened.
        ent->section = section;
        ent->sectionState = MAPSECTIONSTATE_INIT;

        // Boe!Man 11/21/13: Create the event.
        ent->think = sectionAutoCheck;
        ent->nextthink = level.time + 1000; // Check every 10 seconds, except the first time (init).
        //on roundtype start, check level.noLRMWEntFound entities, check the current players, perform the actions.
    }
    else { // No auto system.
        G_FreeEntity(ent);
    }
}

void checkSectionState() {
    // Nolower.
    if (g_useNoLower.integer) {
        level.autoSectionActive[MAPSECTION_NOLOWER] = qtrue;
    }

    // Noroof.
    if (g_useNoRoof.integer) {
        level.autoSectionActive[MAPSECTION_NOROOF] = qtrue;
    }

    // Nomiddle.
    if (g_useNoMiddle.integer) {
        level.autoSectionActive[MAPSECTION_NOMIDDLE] = qtrue;
    }

    // Nowhole.
    if (g_useNoWhole.integer) {
        level.autoSectionActive[MAPSECTION_NOWHOLE] = qtrue;
    }
}


void writeGametypeTeamNames(const char* redTeam, const char* blueTeam) {

    trap_Cvar_Set("g_customRedName", redTeam);
    trap_Cvar_Set("g_customBlueName", blueTeam);
    trap_Cvar_Update(&g_customRedName);
    trap_Cvar_Update(&g_customBlueName);

}

void sectionAddOrDelInstances(gentity_t* ent, qboolean add) {
    gentity_t* ent2 = NULL;

    while (NULL != (ent2 = G_Find(ent2, FOFS(target), ent->classname))) {
        if (ent2 != ent) { // Make sure we don't get the parent ent.
            if (!add) { // Upon removal, just make sure they are not drawed and clients can't interact with them.
                trap_UnlinkEntity(ent2);
            }
            else { // Same as removal, but the other way around.
                trap_LinkEntity(ent2);
            }
        }
    }
}


/*
This is an alias function run by think functions.
*/
void sectionAutoCheck(gentity_t* ent) {

    if (g_useAutoSections.integer) {
        realSectionAutoCheck(ent, qfalse);
    }
    else {
        ent->nextthink = level.time + 1000; // Think again afterwards, maybe the state will change...?
    }

}

void realSectionAutoCheck(gentity_t* ent, qboolean override) {

    if (!override && level.gametypeData->respawnType == RT_NONE) {
        // Without override being set, we do not run the section auto checking if it's not a respawn-gametype.
        return;
    }

    // If current gametype runs on 

    if (level.autoSectionActive[ent->section]) {
        // Check what needs to be done depending on its current state.
        switch (ent->sectionState) {
        case MAPSECTIONSTATE_INIT:
            if (((ent->team2 == TEAM_FREE) ? (TeamCount(-1, TEAM_RED, NULL) + TeamCount(-1, TEAM_BLUE, NULL)) : (TeamCount(-1, (team_t)ent->team2, NULL))) >= ent->min_players) {
                // Section should be open, so hide the linking entities.
                ent->sectionState = MAPSECTIONSTATE_OPENED;
                sectionAddOrDelInstances(ent, qfalse);
            }
            else {
                ent->sectionState = MAPSECTIONSTATE_CLOSED;
                sectionAddOrDelInstances(ent, qtrue);
            }
            break;
        case MAPSECTIONSTATE_CLOSED:
            if (((ent->team2 == TEAM_FREE) ? (TeamCount(-1, TEAM_RED, NULL) + TeamCount(-1, TEAM_BLUE, NULL)) : (TeamCount(-1, (team_t)ent->team2, NULL))) >= ent->min_players) {
                // Open the section.
                ent->sectionState = MAPSECTIONSTATE_OPENING;
                G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s^7 will be opened in %0.f seconds!", ent->message, ent->wait);
                G_printInfoMessageToAll("%s will be opened in %0.f seconds.", ent->message2 + 1, ent->wait);
            }
            break;
        case MAPSECTIONSTATE_OPENED:
            if (((ent->team2 == TEAM_FREE) ? (TeamCount(-1, TEAM_RED, NULL) + TeamCount(-1, TEAM_BLUE, NULL)) : (TeamCount(-1, (team_t)ent->team2, NULL))) < ent->min_players) {
                // Close the section.
                ent->sectionState = MAPSECTIONSTATE_CLOSING;
                G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s^7 will be closed in %0.f seconds!", ent->message, ent->wait);
                G_printInfoMessageToAll("%s will be closed in %0.f seconds.", ent->message2 + 1, ent->wait);
            }
            break;
        case MAPSECTIONSTATE_CLOSING:
            // Close it now, the wait has passed.
            ent->sectionState = MAPSECTIONSTATE_CLOSED;
            sectionAddOrDelInstances(ent, qtrue);
            //if (ent->section < NOMIDDLE)
            //    level.noLROpened[ent->section] = qfalse;

            G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s^7 closed!", ent->message);
            G_printInfoMessageToAll("%s is now closed.", ent->message2 + 1);
            break;
        case MAPSECTIONSTATE_OPENING:
            // Open it now, the wait has passed.
            ent->sectionState = MAPSECTIONSTATE_OPENED;
            sectionAddOrDelInstances(ent, qfalse);
            //if (ent->section < NOMIDDLE)
            //    level.noLROpened[ent->section] = qtrue;

            G_Broadcast(BROADCAST_GAME, NULL, qfalse, "%s^7 opened!", ent->message);
            G_printInfoMessageToAll("%s is now opened.", ent->message2 + 1);
            break;
        default:
            break;
        }
    }

    // Boe!Man 11/22/13: When's our next check?
    if (ent->sectionState == MAPSECTIONSTATE_CLOSING || ent->sectionState == MAPSECTIONSTATE_OPENING) {
        ent->nextthink = level.time + (int)ent->wait * 1000;
    }
    else {
        ent->nextthink = level.time + 10000;
    }
}


void checkEnts(gentity_t* ent) {
    if (ent->model && ent->model != NULL && !Q_stricmp(ent->model, "BLOCKED_TRIGGER"))
    {
        if (ent->count) {
            ///Team Games
            if (level.gametypeData->teams) {
                if (ent->count <= (TeamCount(-1, TEAM_RED, NULL)) && ent->count <= (TeamCount(-1, TEAM_BLUE, NULL))) {
                    if (ent->r.linked) {
                        trap_UnlinkEntity(ent);
                        if (ent->message != NULL)
                            G_Broadcast(BROADCAST_GAME, NULL, qfalse, ent->message);
                    }
                }
                else if (!ent->r.linked) {
                    trap_LinkEntity(ent);
                    if (ent->message2 != NULL)
                        G_Broadcast(BROADCAST_GAME, NULL, qfalse, ent->message2);
                }
            }
            ///Non-Team Games
            else if (ent->count >= level.numPlayingClients) {
                if (ent->r.linked) {
                    trap_UnlinkEntity(ent);
                    if (ent->message != NULL)
                        G_Broadcast(BROADCAST_GAME, NULL, qfalse, ent->message);
                }
            }
            else if (!ent->r.linked) {
                trap_LinkEntity(ent);
                if (ent->message2 != NULL)
                    G_Broadcast(BROADCAST_GAME, NULL, qfalse, ent->message2);
            }
        }
        return;
    }
    if (level.numPlayingClients < ent->min_players && ent->min_players != 0) {
        if (ent->r.linked) {
            trap_UnlinkEntity(ent);
            g_entities[ent->effect_index].disabled = qtrue;
        }
    }
    else if (level.numPlayingClients >= ent->min_players && ent->min_players != 0) {
        if (level.numPlayingClients <= ent->max_players) {
            if (!ent->r.linked) {
                trap_LinkEntity(ent);
                g_entities[ent->effect_index].disabled = qfalse;
            }
        }
    }
    if (level.numPlayingClients < ent->max_players && ent->max_players != 0) {
        if (level.numPlayingClients > ent->min_players) {
            if (!ent->r.linked) {
                trap_LinkEntity(ent);
                g_entities[ent->effect_index].disabled = qfalse;
            }
        }
    }
    else if (level.numPlayingClients >= ent->max_players && ent->max_players != 0) {
        if (ent->r.linked) {
            trap_UnlinkEntity(ent);
            g_entities[ent->effect_index].disabled = qtrue;
        }
    }
}

/*
================
Boe_checkRoof
6/3/12 - 3:53 PM
================
*/

void checkRoof(gentity_t* ent)
{
    // Boe!Man 11/22/13: Only proceed if this is true.
    if (!level.autoSectionActive[MAPSECTION_NOROOF] || !level.noSectionEntFound[MAPSECTION_NOROOF]) {
        return;
    }

    // Boe!Man 6/3/12: Do this with an interval. It's a shame to be cocky about something this small, so save resources.
    if (ent->client->sess.noroofCheckTime > level.time) {
        return;
    }

    // Boe!Man 6/3/12: He must be alive.
    if (G_IsClientDead(ent->client)) {
        if (ent->client->sess.isOnRoof) { // Well, since he's dead now, reset this..
            ent->client->sess.isOnRoof = qfalse;
            ent->client->sess.isOnRoofTime = 0;
        }
        ent->client->sess.noroofCheckTime = level.time + 1000;
        return;
    }

    // Boe!Man 6/3/12: Check for the player.
    if (!ent->client->sess.isOnRoof) { // Player ISN'T on roof, last time we checked.
        if (ent->r.currentOrigin[2] >= level.noLR[1][2]) { // Well he is now. Check for the timeout.
            if (!level.noLR[1][1]) { // 0 or less.. Meaning, instant pop. No need for further checks.
                G_Damage(ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_TRIGGER_HURT, 0);
                G_printInfoMessageToAll("%s was killed for being on the roof.", ent->client->pers.cleanName);
            }
            else {
                ent->client->sess.isOnRoof = qtrue; // The server owner specified a timer. So, first, the player initialised this process by being on roof.
                G_Broadcast(BROADCAST_GAME, ent, qfalse, "\\Leave the roof within ^1%.0f^7 seconds!", level.noLR[1][1]);
                ent->client->sess.isOnRoofTime = 1;
            }
        }
    }
    else { // Player IS on roof.
        if (ent->r.currentOrigin[2] < level.noLR[1][2]) { // He left the roof.
            ent->client->sess.isOnRoof = qfalse;
            ent->client->sess.isOnRoofTime = 0;
            G_Broadcast(BROADCAST_GAME, ent, qfalse, "\\You're no longer on the roof!");
        }
        else { // He's still on the roof.
            if (level.noLR[1][1] == ent->client->sess.isOnRoofTime) { // Well, he waited it out. Pop him.
                G_Damage(ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_TRIGGER_HURT, 0);
                G_printInfoMessageToAll("%s was killed for not leaving the roof.", ent->client->pers.cleanName);
                ent->client->sess.isOnRoof = qfalse;
                ent->client->sess.isOnRoofTime = 0;
            }
            else { // Give him another warning.
                G_Broadcast(BROADCAST_GAME, ent, qfalse, "\\Leave the roof within ^1%.0f ^7seconds!", level.noLR[1][1] - ent->client->sess.isOnRoofTime);
                ent->client->sess.isOnRoofTime += 1;
            }
        }
    }

    ent->client->sess.noroofCheckTime = level.time + 1000;

    return;
}

void resetCompetitionModeVariables() {

    trap_Cvar_Set("scorelimit", va("%d", cm_originalsl.integer));
    trap_Cvar_Set("timelimit", va("%d", cm_originaltl.integer));

    trap_Cvar_Set("cm_prevRedTeamScore", NULL);
    trap_Cvar_Set("cm_prevBlueTeamScore", NULL);
    trap_Cvar_Set("cm_bestOf", NULL);
    trap_Cvar_Set("cm_scorelimit", NULL);
    trap_Cvar_Set("cm_timelimit", NULL);
    trap_Cvar_Set("cm_lockspecs", NULL);
    trap_Cvar_Set("cm_doublerounds", NULL);
    trap_Cvar_Set("cm_state", NULL);
    trap_Cvar_Set("cm_originalsl", NULL);
    trap_Cvar_Set("cm_originaltl", NULL);

    trap_Cvar_Update(&cm_prevRedTeamScore);
    trap_Cvar_Update(&cm_prevBlueTeamScore);
    trap_Cvar_Update(&cm_bestOf);
    trap_Cvar_Update(&cm_scorelimit);
    trap_Cvar_Update(&cm_timelimit);
    trap_Cvar_Update(&cm_lockspecs);
    trap_Cvar_Update(&cm_doublerounds);
    trap_Cvar_Update(&cm_state);
    trap_Cvar_Update(&cm_originalsl);
    trap_Cvar_Update(&cm_originaltl);

    trap_Cvar_Update(&g_scorelimit);
    trap_Cvar_Update(&g_timelimit);

}

/*
parseChatTokens - takes in the FULL chat buffer, outputs the tokenized string.
This also takes care of sound spawning (@/!).
*/
void parseChatTokens(gentity_t* ent, chatMode_t chatMode, const char* input, char* output, int sizeOfOutput) {
    char temp[MAX_SAY_TEXT];
    int outIndex = 0;
    qboolean soundParsed = qfalse;

    Q_strncpyz(temp, input, sizeof(temp));

    const char* ptr = temp;
    while (*ptr && outIndex < sizeOfOutput - 1) {
        if (!soundParsed && (*ptr == '@' || *ptr == '!' || *ptr == '&')) {
            // Sound handling
            mvchat_ChatParse_t chatParse = { 0 };
            mvchat_chatDetermineSound(&chatParse, (char*)ptr, ent && ent->client ? ent->client->pers.identity : NULL);

            if (chatParse.shouldSoundPlay) {
                // JANFIXME - add playability based on sound modes + being alive/dead!
                if (chatParse.isCustomSound) {

                    for (int i = 0; i < level.numConnectedClients; i++) {
                        gentity_t* tent = &g_entities[level.sortedClients[i]];

                        if (tent->client->sess.legacyProtocol && !tent->client->sess.hasRoxAC) {
                            G_printInfoMessage(tent, "Custom sound omitted. Please download Rox Anticheat from https://ac.roxmod.net to get easy access to custom sounds.");
                        }
                        else {
                            G_ClientSound(tent, chatParse.soundIndex);
                        }
                    }

                    
                }
                else {
                    G_GlobalSound(chatParse.soundIndex);
                }

                
                
            }

            if (!chatParse.displayNoText && chatParse.text) {
                int len = strlen(chatParse.text);
                if (outIndex + len < sizeOfOutput - 1) {
                    Q_strncpyz(&output[outIndex], chatParse.text, len + 1);
                    outIndex += len;
                }
            }

            ptr += chatParse.stripChars; // Skip processed characters
            soundParsed = qtrue; // Stop processing further sounds
            continue;
        }

        if (*ptr == '#') {
            // Token handling
            const char* start = ptr + 1;
            if (*start >= '0' && *start <= '9') {
                // Client ID handling
                int clientID = 0;
                while (*start >= '0' && *start <= '9') {
                    clientID = clientID * 10 + (*start - '0');
                    start++;
                }
                if (clientID >= 0 && clientID < MAX_CLIENTS) {
                    gentity_t* clent = &g_entities[clientID];

                    if (clent && clent->client) {
                        int len = strlen(clent->client->pers.netname);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], clent->client->pers.netname, len + 1);
                            outIndex += len;
                        }
                    }

                }
                ptr = start;
                continue;
            }
            else {
                // Special tokens
                char token = *start;
                switch (token) {
                case 'd': case 'D': { // Last player who damaged you
                    const char* name = "JANFIXME LASTDMG";
                    if (name) {
                        int len = strlen(name);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], name, len + 1);
                            outIndex += len;
                        }
                    }
                    break;
                }
                case 't': case 'T': { // Last player you damaged
                    const char* name = "JANFIXME LASTATK";
                    if (name) {
                        int len = strlen(name);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], name, len + 1);
                            outIndex += len;
                        }
                    }
                    break;
                }
                case 'h': case 'H': { // Current health
                    int health = ent->client->ps.stats[STAT_HEALTH];
                    char healthStr[16];
                    Com_sprintf(healthStr, sizeof(healthStr), "%d", health);
                    int len = strlen(healthStr);
                    if (outIndex + len < sizeOfOutput - 1) {
                        Q_strncpyz(&output[outIndex], healthStr, len + 1);
                        outIndex += len;
                    }
                    break;
                }
                case 'a': case 'A': { // Current armor
                    int armor = ent->client->ps.stats[STAT_ARMOR];
                    char armorStr[16];
                    Com_sprintf(armorStr, sizeof(armorStr), "%d", armor);
                    int len = strlen(armorStr);
                    if (outIndex + len < sizeOfOutput - 1) {
                        Q_strncpyz(&output[outIndex], armorStr, len + 1);
                        outIndex += len;
                    }
                    break;
                }
                case 'l': case 'L': { // Current location
                    char location[MAX_SAY_TEXT];
                    Com_Memset(location, 0, sizeof(location));
                    if (Team_GetLocationMsg(ent, location, sizeof(location))) {
                        int len = strlen(location);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], location, len + 1);
                            outIndex += len;
                        }
                    }
                    break;
                }
                case 'x': case 'X': { // Taser
                    const char* taserInfo = "JANFIXME TASER";
                    if (taserInfo) {
                        int len = strlen(taserInfo);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], taserInfo, len + 1);
                            outIndex += len;
                        }
                    }
                    break;
                }
                case 'g': case 'G': { // Stun Gun
                    const char* stunGunInfo = "JANFIXME STUNGUN";
                    if (stunGunInfo) {
                        int len = strlen(stunGunInfo);
                        if (outIndex + len < sizeOfOutput - 1) {
                            Q_strncpyz(&output[outIndex], stunGunInfo, len + 1);
                            outIndex += len;
                        }
                    }
                    break;
                }
                }
                ptr = start + 1;
                continue;
            }
        }

        // Regular character, just copy
        output[outIndex++] = *ptr++;
    }

    output[outIndex] = '\0';
}


/*
==============
SortAlpha

Sorts char * array
alphabetically.
==============
*/

int QDECL SortAlpha(const void* a, const void* b)
{
    return strcmp(*(const char**)a, *(const char**)b);
}


void showHnsScores(void)
{
    /*
    char    winner[64];

    Com_sprintf(winner, sizeof(winner), "%s ^7won the round!", level.cagewinner);

    // Boe!Man 9/2/12: Advanced H&S statistics.
    if (hideSeek_ExtendedRoundStats.integer && level.time > level.awardTime + 8000 && level.awardTime) {
        G_Broadcast(va("^3%s\n\n^3Statistics for this map:\n"
            "^_Rounds survived: ^3%i ^_by ^3%s\n"
            "^_MM1 hits taken: ^3%i ^_by ^3%s\n"
            "^_RPG boosts: ^3%i ^_by ^3%s\n"
            "^_Taken RPG: ^3%i ^_by ^3%s\n"
            "^_Taken M4: ^3%i ^_by ^3%s\n"
            "^_Stun attacks: ^3%i ^_by ^3%s\n"
            "^_Seekers caged: ^3%i ^_by ^3%s\n"
            "^_Weapons stolen: ^3%i ^_by ^3%s\n\n"
            "^yPoints: ^3%i ^yby ^3%s\n"
            "^yTaken MM1: ^3%i ^yby ^3%s\n"
            "^yStunned: ^3%i ^yby ^3%s\n"
            "^yTrapped in cage: ^3%i ^yby ^3%s",
            g_motd.string,
            level.advancedHsScores[0].score, level.advancedHsScores[0].name, level.advancedHsScores[1].score, level.advancedHsScores[1].name, level.advancedHsScores[2].score, level.advancedHsScores[2].name, level.advancedHsScores[3].score, level.advancedHsScores[3].name,
            level.advancedHsScores[4].score, level.advancedHsScores[4].name, level.advancedHsScores[5].score, level.advancedHsScores[5].name, level.advancedHsScores[6].score, level.advancedHsScores[6].name, level.advancedHsScores[7].score, level.advancedHsScores[7].name,
            level.advancedHsScores[8].score, level.advancedHsScores[8].name, level.advancedHsScores[9].score, level.advancedHsScores[9].name, level.advancedHsScores[10].score, level.advancedHsScores[10].name, level.advancedHsScores[11].score, level.advancedHsScores[11].name
        ), BROADCAST_AWARDS, NULL);
    }
    else {
        G_Broadcast(va("^3%s\n\n%s\n\n^_ THE 3 BEST HIDERS IN THIS MAP ARE:\n^31st ^7%s with ^3%i ^7wins.\n^+2nd ^7%s with ^+%i ^7wins.\n^@3rd ^7%s with ^@%i ^7wins.\n\n"
            "^y THE 3 BEST SEEKERS IN THIS MAP ARE:\n^31st ^7%s with ^3%i ^7kills.\n^+2nd ^7%s with ^+%i ^7kills.\n^@3rd ^7%s with ^@%i ^7kills.",
            g_motd.string, winner,
            level.top3Hiders[0].name, level.top3Hiders[0].score, level.top3Hiders[1].name, level.top3Hiders[1].score, level.top3Hiders[2].name, level.top3Hiders[2].score,
            level.top3Seekers[0].name, level.top3Seekers[0].score, level.top3Seekers[1].name, level.top3Seekers[1].score, level.top3Seekers[2].name, level.top3Seekers[2].score
        ), BROADCAST_AWARDS, NULL);
    }
    */
}

qboolean isCurrentGametype(gameTypes_t gametype) {
    return currentGametype.integer == gametype;
}

/*
* Function checks whether the current gametype is in the provided list.
* Always add GT_MAX in as the last element - that will be the "stopper".
* Without it, the cycle never ends (and you'll get some funny results).
*/
qboolean isCurrentGametypeInList(gameTypes_t* gametypes) {

    while (gametypes && *gametypes != GT_MAX) {
        if (*gametypes == currentGametype.integer) {
            return qtrue;
        }
        gametypes++;
    }

    return qfalse;
}

void sendClientmodAwards() {
    RPM_Awards();
    ROCmod_sendBestPlayerStats();
}

void notifyPlayersOfTeamScores() {

    if ((cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2)) {

        if (cm_state.integer == COMPMODE_ROUND2) {
            // Find out the winning team.
            int winningTeam = TEAM_NUM_TEAMS;
            int teamRedScores = level.teamScores[TEAM_RED];
            int teamBlueScores = level.teamScores[TEAM_BLUE];
            if (cm_doublerounds.integer) {
                teamRedScores += cm_prevRedTeamScore.integer;
                teamBlueScores += cm_prevBlueTeamScore.integer;
                if (teamRedScores == teamBlueScores) {
                    winningTeam = TEAM_FREE;
                }
                else if (teamRedScores > teamBlueScores) {
                    winningTeam = TEAM_RED;
                }
                else {
                    winningTeam = TEAM_BLUE;
                }
            }
            else {
                if (teamRedScores == teamBlueScores) {
                    winningTeam = TEAM_FREE;
                }
                else if (teamRedScores > teamBlueScores) {
                    winningTeam = TEAM_RED;
                }
                else {
                    winningTeam = TEAM_BLUE;
                }
            }

            if (winningTeam == TEAM_FREE) {

                G_Broadcast(BROADCAST_AWARDS, NULL, qfalse, "%s ^7and %s^7 \nhave \\finished with a tie: %d - %d", g_customRedName.string, g_customBlueName.string, teamRedScores, teamBlueScores);
                G_printInfoMessageToAll("Blue and red team have finished with a tie: %d - %d", teamRedScores, teamBlueScores);
            }
            else {
                G_Broadcast(BROADCAST_AWARDS, NULL, qfalse, "%s^7 \nwon the \\match \nwith a scoreline of %d - %d", winningTeam == TEAM_RED ? g_customRedName.string : g_customBlueName.string, winningTeam == TEAM_RED ? teamRedScores : teamBlueScores, winningTeam == TEAM_RED ? teamBlueScores : teamRedScores);
                G_printInfoMessageToAll("%s won the match with a scoreline of %d - %d", winningTeam == TEAM_RED ? "Red team" : "Blue team", winningTeam == TEAM_RED ? teamRedScores : teamBlueScores, winningTeam == TEAM_RED ? teamBlueScores : teamRedScores);
            }

        }
        else {
            int winningTeam = TEAM_NUM_TEAMS;
            int teamRedScores = level.teamScores[TEAM_RED];
            int teamBlueScores = level.teamScores[TEAM_BLUE];

            if (teamRedScores == teamBlueScores) {
                winningTeam = TEAM_FREE;
            }
            else if (teamRedScores > teamBlueScores) {
                winningTeam = TEAM_RED;
            }
            else {
                winningTeam = TEAM_BLUE;
            }

            if (winningTeam == TEAM_FREE) {
                G_Broadcast(BROADCAST_AWARDS, NULL, qfalse, "%s^7 and %s^7 \nare currently tied: %d - %d", g_customRedName.string, g_customBlueName.string, teamRedScores, teamBlueScores);
                G_printInfoMessageToAll("Blue and red team are currently tied: %d - %d", teamRedScores, teamBlueScores);
            }
            else {
                G_Broadcast(BROADCAST_AWARDS, NULL, qfalse, "%s^7 \nwon the \\first round \nwith a scoreline of %d - %d", winningTeam == TEAM_RED ? g_customRedName.string : g_customBlueName.string, winningTeam == TEAM_RED ? teamRedScores : teamBlueScores, winningTeam == TEAM_RED ? teamBlueScores : teamRedScores);
                G_printInfoMessageToAll("%s won the first round with a scoreline of %d - %d", winningTeam == TEAM_RED ? "Red team" : "Blue team", winningTeam == TEAM_RED ? teamRedScores : teamBlueScores, winningTeam == TEAM_RED ? teamBlueScores : teamRedScores);
            }

            trap_Cvar_Set("cm_prevRedTeamScore", va("%d", level.teamScores[TEAM_BLUE]));
            trap_Cvar_Set("cm_prevBlueTeamScore", va("%d", level.teamScores[TEAM_RED]));

            trap_Cvar_Update(&cm_prevRedTeamScore);
            trap_Cvar_Update(&cm_prevBlueTeamScore);


        }
    }

}


/*
==================
Boe_Motd in 1fxmod.
==================
*/
void showMotd(gentity_t* ent) {
    char    gmotd[1024] = "\0";
    char    motd[1024] = "\0";
    char* s = motd;
    char* gs = gmotd;
    char    name[36];
    char* header1 = va("%s - %s\n", MODNAME_COLORED, MODVERSION);

    strcpy(name, ent->client->pers.netname);

    Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n",
        header1,
        MOD_MOTD_INFO,
        g_motd1.string,
        g_motd2.string,
        g_motd3.string,
        g_motd4.string,
        g_motd5.string);

    gmotd[strlen(gmotd) + 1] = '\0';

    while (*gs)
    {
        if (*gs == '#')
        {
            if (*++gs == 'u')
            {
                strcat(motd, name);
                strcat(motd, "^7");
                s += strlen(name) + 2;
                gs++;
            }
            else
            {
                gs--;
            }
        }

        *s++ = *gs++;
    }

    *s = '\0';
    G_Broadcast(BROADCAST_MOTD, ent, qfalse, motd);
}

qboolean weaponMod(weaponMod_t weaponMod, char* wpnModName) {

    char fileToCheck[MAX_QPATH];

    if (weaponMod == WEAPONMOD_DEFAULT) {
        return BG_InitWeaponStats(level.pickupsDisabled, qfalse, qfalse, NULL);
    }
    else if (weaponMod == WEAPONMOD_ND) {
        Q_strncpyz(fileToCheck, "wpndata/nd.wpn", sizeof(fileToCheck));
    }
    else if (weaponMod == WEAPONMOD_RD) {
        Q_strncpyz(fileToCheck, "wpndata/rd.wpn", sizeof(fileToCheck));
    }
    else {
        if (!wpnModName || !strlen(wpnModName)) {
            return qfalse;
        }

        Q_strncpyz(fileToCheck, va("wpndata/%s.wpn", wpnModName), sizeof(fileToCheck));

    }

    return BG_InitWeaponStats(level.pickupsDisabled, qfalse, qtrue, fileToCheck);

}

void printMapActionDenialReason(gentity_t* adm) {

    if (level.mapAction == MAPACTION_ENDING) {
        G_printInfoMessage(adm, "The map is already ending.");
    }
    else if (level.mapAction == MAPACTION_PENDING_GT) {
        G_printInfoMessage(adm, "Server is already pending a gametype change.");
    }
    else if (level.mapAction == MAPACTION_PENDING_MAPCHANGE) {
        G_printInfoMessage(adm, "Server is already pending a map change.");
    }
    else if (level.mapAction == MAPACTION_PENDING_MAPCYCLE) {
        G_printInfoMessage(adm, "Server is already pending moving to next map.");
    }
    else if (level.mapAction == MAPACTION_PENDING_MAPGTCHANGE) {
        G_printInfoMessage(adm, "Server is already pending a new map and a gametype.");
    }
    else if (level.mapAction == MAPACTION_PENDING_RESTART) {
        G_printInfoMessage(adm, "Server is already pending a map restart.");
    }

}

int getChatModeFromCommand(gentity_t* ent, const char* cmd, chatMode_t mode, int adminCommand) {

    // If the first character is not !, we break out early.
    if (ent && ent->client) {
        if (cmd && *cmd && strlen(cmd) > 0 && cmd[0] == '!' && adminCommand == -1) { // Don't run custom modes if an admin command is used, e.g. chaining together broadcast with !at.

            // Chat modes we support are triggered here.

            if (!Q_stricmp(cmd, "!at")) {
                if (ent->client->sess.adminLevel > ADMLVL_NONE) {
                    return SAY_ADMTALK;
                }
                else {
                    return SAY_CALLADMCHAT;
                }
            }

            if (!Q_stricmp(cmd, "!ca") || !Q_stricmp(cmd, "!ac")) {
                if (ent->client->sess.adminLevel > ADMLVL_NONE) {
                    return SAY_ADMCHAT;
                }
                else {
                    return SAY_CALLADMCHAT;
                }
            }

            if (!Q_stricmp(cmd, "!sc") && ent->client->sess.adminLevel >= ADMLVL_SADMIN) {
                return SAY_SADMCHAT;
            }

            if (!Q_stricmp(cmd, "!hc") && ent->client->sess.adminLevel == ADMLVL_HADMIN) {
                return SAY_HADMCHAT;
            }

        }

        // Wasn't a command, are we in compmode?
        if ((cm_state.integer == COMPMODE_ROUND1 || cm_state.integer == COMPMODE_ROUND2) && ent->client->sess.team == TEAM_SPECTATOR) {
            return SAY_TEAM;
        }
    }
    
    // If none returned first, we retain the original mode.
    return mode;

}

char* getChatAdminPrefixByMode(gentity_t* ent, chatMode_t mode, char* output, int outputSize) {

    Com_Memset(output, 0, outputSize);

    if (ent && ent->client) {
        if (mode <= SAY_ADMTALK) {
            Q_strncpyz(output, getAdminNameByAdminLevel(ent->client->sess.adminLevel), outputSize);
        }
        else if (mode == SAY_ADMCHAT) {
            Q_strncpyz(output, va("%ss Only", g_adminPrefix.string), outputSize);
        }
        else if (mode == SAY_SADMCHAT) {
            Q_strncpyz(output, va("%ss Only", g_sadminPrefix.string), outputSize);
        }
        else if (mode == SAY_HADMCHAT) {
            Q_strncpyz(output, va("%ss Only", g_hadminPrefix.string), outputSize);
        }
        else if (mode == SAY_CALLADMCHAT) {
            Q_strncpyz(output, va("^7Hey %s!", g_adminPrefix.string), outputSize);
        }
    }
}

qboolean shouldChatModeBeep(chatMode_t mode) {
    return
        (
            mode == SAY_ADMTALK ||
            mode == SAY_REFTALK ||
            mode == SAY_CLANTALK ||
            mode == SAY_CALLADMCHAT
        ) ? qtrue : qfalse;
}

void printPlayersInfo(gentity_t* ent) {

    // 1fxmod structure - [id] [name] [ping] [coun], [adm], [cln], [mut], [ver] (ver being clientmod)

    char packetBuf[1000];
    Com_Memset(packetBuf, 0, sizeof(packetBuf));

    Q_strncpyz(packetBuf, va("^7[^3Players^7]\n\n%-5.5s%-33.33s %-9.9s%-6.6s%-5.5s%-4.4s%-4.4s%-4.4s%-6.6s\n-----------------------------------------------------------------------------\n", "ID", "Name", "Protocol", "Ping", "Ctry", "Adm", "Cln", "Mut", "Clmod"), sizeof(packetBuf));

    for (int i = 0; i < MAX_CLIENTS; i++) {
        gentity_t* tent = &g_entities[i];

        if (!tent || !tent->inuse || !tent->client) {
            continue;
        }

        char* adminInitial = "";

        switch (tent->client->sess.adminLevel) {
        case ADMLVL_HADMIN:
            adminInitial = "H";
            break;
        case ADMLVL_SADMIN:
            adminInitial = "S";
            break;
        case ADMLVL_ADMIN:
            adminInitial = "A";
            break;
        case ADMLVL_BADMIN:
            adminInitial = "B";
            break;
        }

        if (strlen(packetBuf) + strlen(va("[^3%2d^7] %-33.33s %-9.9s%-6d%-5.5s%-4.4s%-4.4s%-4.4s%-6.6s\n", i, tent->client->pers.cleanName, tent->client->sess.legacyProtocol ? "Silver" : "Gold", tent->client->ps.ping, tent->client->sess.countryCode, adminInitial, tent->client->sess.clanMember ? "Y" : "", tent->client->sess.muted ? "Y" : "", tent->client->sess.clientMod == CL_ROCMOD ? "ROCmod" : (tent->client->sess.clientMod == CL_RPM && !tent->client->sess.hasRoxAC ? "RPM" : (tent->client->sess.hasRoxAC ? "Rox AC" : "")))) > sizeof(packetBuf)) {
            trap_SendServerCommand(ent - g_entities, va("print \"%s\"", packetBuf));
            Com_Memset(packetBuf, 0, sizeof(packetBuf));
        }

        Q_strcat(packetBuf, sizeof(packetBuf), 
            va("[^3%2d^7] %-33.33s %-9.9s%-6d%-5.5s%-4.4s%-4.4s%-4.4s%-6.6s\n", 
                i, 
                tent->client->pers.cleanName, 
                tent->client->sess.legacyProtocol ? "Silver" : "Gold", 
                tent->client->ps.ping, 
                tent->client->sess.countryCode, 
                adminInitial, 
                tent->client->sess.clanMember ? "Y" : "", 
                tent->client->sess.muted ? "Y" : "", 
                tent->client->sess.clientMod == CL_ROCMOD ? "ROCmod" : 
                    (tent->client->sess.clientMod == CL_RPM && !tent->client->sess.hasRoxAC ? "RPM" : 
                        (tent->client->sess.hasRoxAC ? "Rox AC" : "")
                    )
                ));

    }

    trap_SendServerCommand(ent - g_entities, va("print \"%s\"", packetBuf));
    trap_SendServerCommand(ent - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll.\n\"");
}

void printStatsInfo(gentity_t* ent) {

    int idNum = G_ClientNumFromArg(ent, 1, "view stats", qfalse, qtrue, qtrue, qfalse);

    if (idNum >= 0) {

        gentity_t* tent = &g_entities[idNum];

        trap_SendServerCommand(ent - g_entities, va("print \"\n^3Player statistics for %s\n^7-----------------------------------------------------------------------------\n\"", tent->client->pers.netname));
        char aliases[MAX_STRING_CHARS];
        dbGetAliases(tent, aliases, sizeof(aliases), "\n");

        if (strlen(aliases) > 0) {

            qboolean isFirst = qtrue;

            const char* aliasStart = aliases;
            const char* aliasEnd;

            while ((aliasEnd = strchr(aliasStart, '\n')) != NULL) {

                int aliasLength = aliasEnd - aliasStart;

                if (isFirst) {
                    isFirst = qfalse;
                    trap_SendServerCommand(ent - g_entities, va("print \"%-16.16s%20.*s\n\"", "^7[^3Aliases^7]", aliasLength, aliasStart));
                }
                else {
                    trap_SendServerCommand(ent - g_entities, va("print \"%-12.12s%20.*s\n\"", " ", aliasLength, aliasStart));
                }

                aliasStart = aliasEnd + 1;
            }

            //trap_SendServerCommand(ent - g_entities, "print \"\n\"");
        }

        char* adminLevel = getAdminNameByAdminLevel(tent->client->sess.adminLevel);

        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Admin^7]", adminLevel && *adminLevel ? adminLevel : "No"));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Clan^7]", tent->client->sess.clanMember ? "Yes" : "No"));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Country^7]", tent->client->sess.country));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Protocol^7]", tent->client->sess.legacyProtocol ? "2002 / SoF2 1.00 Silver" : "2004 / SoF2 1.03 Gold"));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Clientmod^7]", tent->client->sess.hasRoxAC ? "Rox Anticheat" : (tent->client->sess.clientMod == CL_ROCMOD ? "ROCmod" : (tent->client->sess.clientMod == CL_RPM ? "RPM" : "None"))));

        char userinfo[MAX_INFO_STRING];
        trap_GetUserinfo(idNum, userinfo, sizeof(userinfo));
        char* rate = Info_ValueForKey(userinfo, "rate");
        char* snaps = Info_ValueForKey(userinfo, "snaps");

        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Rate^7]", rate));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25.25s\n\"", "^7[^3Snaps^7]", snaps));
        trap_SendServerCommand(ent - g_entities, va("print \"%-28.28s%-25d\n\"", "^7[^3Ping^7]", tent->client->ps.ping));

        if (ent->client->sess.muted) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (level.mutedClients[i].used == qtrue) {
                    if (!Q_stricmp(level.mutedClients[i].ip, tent->client->pers.ip)) {
                        int remain = ((level.mutedClients[i].startTime + level.mutedClients[i].time - level.time) / 1000) / 60;
                        int remainS = ((level.mutedClients[i].startTime + level.mutedClients[i].time - level.time) / 1000);
                        trap_SendServerCommand(ent - g_entities, va("print \"%-26.26s%i:%02i minutes remaining\n\n", "[^3Mute^7]", remain, remainS - (remain * 60)));
                        break; // Boe!Man 2/15/13: Duplicate line fix and speed optimize.
                    }
                }
            }
        }
        else {
            trap_SendServerCommand(ent - g_entities, va("print \"\n"));
        }

        // Damage stats as-is from 1fxmod.

        statInfo_t* stat = &tent->client->pers.statInfo;

        // Boe!Man 6/2/10: Tier 1 - Start.
        trap_SendServerCommand(ent - g_entities, va("print \"[^3Total kills^7] [^3Total death^7] [^3Damage done^7] [^3Damage take^7]\n"));
        trap_SendServerCommand(ent - g_entities, va("print \"%7d%14d%14d%14d\n\n", stat->kills, stat->deaths, stat->damageDone, stat->damageTaken));
        // Boe!Man 6/2/10: Tier 1 - End.

        // Boe!Man 6/2/10: Tier 2 - Start.
        trap_SendServerCommand(ent - g_entities, va("print \"[^3Hand^7] [^3Foot^7] [^3Arms^7] [^3Legs^7] [^3Head^7] [^3Neck^7] [^3Tors^7] [^3Wais^7]\n"));
        trap_SendServerCommand(ent - g_entities, va("print \"%4d%7d%7d%7d%7d%7d%7d%7d\n", stat->handhits, stat->foothits, stat->armhits, stat->leghits, stat->headhits, stat->neckhits, stat->torsohits, stat->waisthits));
        // Boe!Man 6/2/10: Tier 2 - End.

        // Boe!Man 6/2/10: Tier 3: Weapon Stats - Start.
        if (stat->shotcount)
        {
            trap_SendServerCommand(ent - g_entities, va("print \"\n%-22s%-13s%-13s%-13s[^3Accu^7]\n\"", "[^3Weapon^7]", "[^3Shot^7]", "[^3Hits^7]", "[^3Head^7]"));
            for (int n = 0; n < WP_NUM_WEAPONS; n++)
            {
                if (stat->weapon_shots[ATTACK_NORMAL * WP_NUM_WEAPONS + n] <= 0 && stat->weapon_shots[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n] <= 0)
                {
                    continue;
                }
                float accuracy = 0;
                if (stat->weapon_shots[ATTACK_NORMAL * WP_NUM_WEAPONS + n])
                {
                    accuracy = (float)stat->weapon_hits[ATTACK_NORMAL * WP_NUM_WEAPONS + n] / (float)stat->weapon_shots[ATTACK_NORMAL * WP_NUM_WEAPONS + n] * 100;
                }
                trap_SendServerCommand(ent - g_entities, va("print \"^3%-14s^7%9d^7%9d^7%9d%7s%3.2f\n\"",
                    bg_weaponNames[n],
                    stat->weapon_shots[ATTACK_NORMAL * WP_NUM_WEAPONS + n],
                    stat->weapon_hits[ATTACK_NORMAL * WP_NUM_WEAPONS + n],
                    stat->weapon_headshots[ATTACK_NORMAL * WP_NUM_WEAPONS + n],
                    "^7",
                    accuracy));

                char* altname = "none";

                if (stat->weapon_shots[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n])
                {
                    switch (n)
                    {
                    case WP_AK74_ASSAULT_RIFLE:
                        altname = "Bayonette";
                        break;

                    case WP_M4_ASSAULT_RIFLE:
                        altname = "M203";
                        break;

                    case WP_M590_SHOTGUN:
                        altname = "Bludgeon";
                        break;

                    case WP_M1911A1_PISTOL:
                    case WP_USSOCOM_PISTOL:
                        altname = "Pistol Whip";
                        break;

                    default:
                        altname = "none";
                        break;
                    }
                    if (Q_stricmp(altname, "none") != 0)
                    {
                        accuracy = 0;
                        if (stat->weapon_hits[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n])
                        {
                            accuracy = (float)stat->weapon_hits[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n] / (float)stat->weapon_shots[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n] * 100;
                        }
                        trap_SendServerCommand(ent - g_entities, va("print \"^3%-14s^7%9d^7%9d^7%9d%7s%3.2f\n\"",
                            altname,
                            stat->weapon_shots[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n],
                            stat->weapon_hits[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n],
                            stat->weapon_headshots[ATTACK_ALTERNATE * WP_NUM_WEAPONS + n],
                            "^7",
                            accuracy));
                    }
                }
            }
        }
        trap_SendServerCommand(ent - g_entities, "print \"\n\nUse [^3Page Up^7] and [^3Page Down^7] to scroll.\n\"");
    }
}

/*
================
altAttack
4/30/14 - 9:25 PM
Convenience function to quickly get the alt attack mod value.
================
*/

int altAttack(int weapon) {
    return weapon + 256;
}

/*
================
normalAttackMod
10/23/15 - 10:13 PM
Convenience function to quickly get normalized WP_* value of a mod.
================
*/

int normalAttackMod(int mod) {
    if (mod > 256) {
        return mod - 256;
    }

    return mod;
}

