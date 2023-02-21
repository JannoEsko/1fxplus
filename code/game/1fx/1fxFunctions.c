#include "../g_local.h"
#include "1fxFunctions.h"


/*
==============
G_printInfoMessage
10/30/16 - 6:06 PM

Prints [Info] message to the console of a player
or regular message to server console (or RCON).
==============
*/

void QDECL G_printInfoMessage(gentity_t *ent, const char *msg, ...)
{
    va_list     argptr;
    int         len;
    char        text[1024];

    memset(text, 0, sizeof(text));

    // If we're printing this message to a client,
    // ensure we prepend the [Info] part.
    if(ent && ent->client){
        // Prepend the [Info].
        strncat(text, "print \"^3[Info] ^7", sizeof(text));

        // Concatenate the message.
        va_start(argptr, msg);
        len = vsnprintf(text + 18, sizeof(text) - 20, msg, argptr) + 18;
        va_end(argptr);

        // Append the newline.
        strncat(text + len, "\n\"", sizeof(text) - len);
    }else{
        // Concatenate the message.
        va_start(argptr, msg);
        len = vsnprintf(text, sizeof(text) - 1, msg, argptr);
        va_end(argptr);

        // Append the newline.
        strncat(text + len, "\n", sizeof(text) - len);
    }

    // Print the info message.
    if(ent && ent->client){
        trap_SendServerCommand(ent-g_entities, text);
    }else{
        trap_Print(text);
    }
}

/*
==============
G_printInfoMessageToAll
10/30/16 - 6:06 PM

Prints [Info] message to the console
of all players.
==============
*/

void QDECL G_printInfoMessageToAll(const char *msg, ...)
{
    va_list     argptr;
    int         len;
    char        text[1024];

    memset(text, 0, sizeof(text));

    // If we're printing this message to a client,
    // ensure we prepend the [Info] part.
    strncat(text, "print \"^3[Info] ^7", sizeof(text));

    // Concatenate the message.
    va_start(argptr, msg);
    len = vsnprintf(text + 18, sizeof(text) - 20, msg, argptr) + 18;
    va_end(argptr);

    // Append the newline.
    strncat(text + len, "\n\"", sizeof(text) - len);

    // Print the info message.
    trap_SendServerCommand(-1, text);
}


/*
==============
G_clientNumFromArg
10/30/16 - 5:29 PM
Gets client number from the argument.

Has support for both regular Admin commands (console)
and the short commands (! in chat).
Can fail on request depending on supplied boolean parameters.
==============
*/

int G_clientNumFromArg(gentity_t *ent, int argNum, const char *action,
                       qboolean aliveOnly, qboolean otherAdmins, qboolean higherLvlAdmins,
                       qboolean shortCmd)
{
    char        arg[16];
    char        cleanName[MAX_NETNAME];
    int         argc, i, clientsFound;
    int         clientID = -1;
    qboolean    name;

    // Determine if there are parameters present.
    // Not being present could indicate a short command from the console.
    if (shortCmd) {
        argc = G_GetChatArgumentCount();
    }

    // Now fetch the argument.
    if (shortCmd && argc){
        Q_strncpyz(arg, G_GetChatArgument(1), sizeof(arg));
    }else{
        trap_Argv(argNum, arg, sizeof(arg));
    }

    // Check if there's a parameter specified at all.
    if(!strlen(arg)){
        G_printInfoMessage(ent,
                           "You haven't entered a valid player ID/player name.");
        return -1;
    }

    // We can continue.
    // Now check whether the client specified an ID or a name.
    name = qfalse;
    for(i = 0; i < strlen(arg); i++){
        if(!isdigit(arg[i])){
            // Letter was found, safe to assume this is a name.
            name = qtrue;
            break;
        }
    }

    if(name){ // Argument potentially contains a name.
        char multipleNamesFound[512];

        // Compare using lowercase characters.
        Q_strlwr(arg);

        // Loop through connected clients, and check who the client meant.
        clientsFound = 0;
        memset(multipleNamesFound, 0, sizeof(multipleNamesFound));
        for(i = 0; i < level.numConnectedClients; i++){
            gclient_t *cl = g_entities[level.sortedClients[i]].client;
            Q_strncpyz(cleanName, cl->pers.cleanName, sizeof(cleanName));

            if(strstr(Q_strlwr(cleanName), arg)){
                // Match. Save the client ID.
                clientID = cl - &level.clients[0];
                clientsFound++;

                // Also fill the "multiple found" buffer.
                // We might need to print this later on.
#ifdef __GNUC__
                snprintf(multipleNamesFound + strlen(multipleNamesFound),
                    sizeof(multipleNamesFound) - strlen(multipleNamesFound),
                    "^1[#%i] ^7%s, ", clientID, cl->pers.cleanName);
#elif _MSC_VER_
                _snprintf_s(multipleNamesFound + strlen(multipleNamesFound),
                    sizeof(multipleNamesFound) - strlen(multipleNamesFound),
                    "^1[#%i] ^7%s, ", clientID, cl->pers.cleanName);
#endif // __GNUC__
            }
        }

        // Multiple clients found?
        if(clientsFound > 1){
            // Remove the trailing ", ".
            multipleNamesFound[strlen(multipleNamesFound) -2] = '\0';

            // Print message.
            G_printInfoMessage(ent,
                               "Multiple names found with ^3%s^7: %s",
                               arg, multipleNamesFound);

            return -1;
        }
    }else{ // Argument only contains numbers.
        clientID = atoi(arg);
    }

    // Do some checks, ID must be a valid ID.
    if(clientID < 0 || clientID >= g_maxclients.integer){
        G_printInfoMessage(ent,
                           "You haven't entered a valid player ID/player name.");
        return -1;
    }

    if (g_entities[clientID].client->pers.connected == CON_DISCONNECTED){
        G_printInfoMessage(ent, "This client is not connected.");
        return -1;
    }

    // Check if the executing Admin is doing this,
    // and not RCON. If so, perform some extra checks.
    if(ent && ent->client
       && ent->client->sess.adminLevel && g_entities[clientID].client->sess.adminLevel){
        // Don't allow to execute the command on higher level Admins.
        if(g_entities[clientID].client->sess.adminLevel > ent->client->sess.adminLevel
           && !higherLvlAdmins)
        {
            G_printInfoMessage(ent,
                               "You cannot %s higher level Admins.", action);
            return -1;
        }

        // Don't allow this command on other Admins?
        if(!otherAdmins){
            G_printInfoMessage(ent,
                               "You cannot %s other Admins.", action);
            return -1;
        }
    }
        // Perform an additional check for when a non-Admin calls this function,
        // and this action is not allowed on Admins.

    else if(!otherAdmins && ent && ent->client && !ent->client->sess.adminLevel
            && g_entities[clientID].client->sess.adminLevel)
    {
        G_printInfoMessage(ent, "You cannot %s an Admin.", action);
        return -1;
    }

    // Check if the targeted player has to be alive for this command.
    if(aliveOnly){
        // Are they dead?
        if(G_IsClientDead (g_entities[clientID].client))
        {
            G_printInfoMessage(ent,
                               "You cannot %s dead players.", action);
            return -1;
        }

        // Are they spectating?
        if (G_IsClientSpectating(g_entities[clientID].client))
        {
            G_printInfoMessage(ent,
                               "You cannot %s a spectator.", action);
            return -1;
        }
    }

    // Everything checked out, return the ID.
    return clientID;
}

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

char *G_GetChatArgument(int argNum)
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
char *G_GetArg(int argNum,qboolean shortCmd)
{
    static char arg[MAX_STRING_CHARS];
    memset(arg, 0, sizeof(arg));

    if(!shortCmd && G_GetChatArgumentCount() <= 1) {
        //argNum++; if I use it in admin commands, I already have the correct argnum in play. Therefore this will actually mess it up.
        trap_Argv(argNum,arg,sizeof(arg));
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

        if (fromArgNum != totalArgs) {
            output[totalLength] = ' ';
            totalLength++;
        }

        fromArgNum++;

    }

    output[totalLength] = 0;

    return output;

}

/*
==============
G_RemoveColorEscapeSequences
==============
*/

void G_RemoveColorEscapeSequences(char *text) {
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

void G_RemoveAdditionalCarets(char *text)
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


void getBanDurationFromArg(int *duration, char *arg) {

    if (strlen(arg)) {
        if(Q_stricmp(arg,"eom") != 0) {
            int d = 0, h = 0, m = 0, tmp = 0, totalDays = 0;
            for (int i = 0; i < strlen(arg); i++) {
                if (arg[i] == 'd' || arg[i] == 'D') {
                    d += tmp;
                    tmp = 0;
                } else if (arg[i] == 'h' || arg[i] == 'H') {
                    h += tmp;
                    tmp = 0;
                } else if (arg[i] == 'm' || arg[i] == 'M') {
                    m += tmp;
                    tmp = 0;
                } else if (isdigit(arg[i])) {
                    tmp = tmp * 10;
                    tmp += arg[i] - '0';
                } else {
                    tmp = 0;
                }
            }

            h += m / 60;
            d += h / 24;
            h = h % 24;
            m = m % 60;
            totalDays = d + h / 24 + m / (60 * 24);

            if ((h == 0 && d == 0 && m == 0)) {
                duration[0] = -1;
            } 
            else if (totalDays > g_maxBanDuration.integer) {
                duration[0] = 2;
            }
            else {
                duration[0] = 1;
                duration[1] = d;
                duration[2] = h;
                duration[3] = m;
            }
        }
        else
        {
            duration[0] = 0;
        }

    }
    else {
        duration[0] = -1;
    }
}

void getSubnet(char* ipIn, char* out) {
    char ip[MAX_IP];
    char* token;
    int oct1 = 0, oct2 = 0;

    Q_strncpyz(ip, ipIn, sizeof(ip));

    // FIXME add ipv6 support when iosof2mp becomes a reality.

    token = strtok(ip, ".");

    // I only want 2 tokens.
    if (token != NULL) {
        oct1 = atoi(token);
    }
    token = strtok(NULL, ".");
    if (token != NULL) {
        oct2 = atoi(token);
    }

    snprintf(out, sizeof(out), "%d.%d", oct1, oct2);

}

/*
==================
G_ColorizeMessage
Adds server colors into a broadcast message (if required).
==================
*/
#define MAX_BROADCAST_LINE 48

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
    if (level.pause) {
        for (i = 0; i < strlen(newBroadcast); i++) {
            if (newBroadcast[i] == '\n') {
                newBroadcast[i] = ' ';
            }
        }
    }

    return newBroadcast;
}

/*
==================
G_Broadcast

Broadcasts a message to clients that are supposed to receive it.
Additional param playSound which also sends G_GlobalSound with level.actionSoundIndex only if param to is null.
==================
*/
void G_Broadcast(char* broadcast, int broadcastLevel, gentity_t* to, qboolean playSound)
{
    int i;
    char* newBroadcast;

    newBroadcast = G_ColorizeMessage(broadcast);

    // If to is NULL, we're dealing with a global message (equals old way of broadcasting to -1).
    if (to == NULL) {

        if (playSound) {
            G_GlobalSound(level.actionSoundIndex);
        }

        if (!level.pause) {
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

qboolean checkAdminPassword(char* adminPass) {
    // currently just check that it does not consist of the exact same characters.
    int passwordLength;
    char* sameChar;

    passwordLength = strlen(adminPass);
    sameChar = malloc(passwordLength + 1);
    memset(sameChar, adminPass[0], passwordLength);
    sameChar[passwordLength] = '\0';

    return Q_stricmp(sameChar, adminPass);
}

/*
============
removeIngameAdminByNameAndType
Tries to find whether the just removed admin was in game and if they were, then removes their admin powers and broadcasts the change.
============
*/
void removeIngameAdminByNameAndType(gentity_t* adm, qboolean passadmin, char* removableName, char* removableIp, int removableLevel) {

    gentity_t* tmp;

    for (int i = 0; i < level.numConnectedClients; i++) {
        tmp = &g_entities[level.sortedClients[i]];

        if (
            tmp && tmp->client
            && (tmp->client->pers.connected == CON_CONNECTED || tmp->client->pers.connected == CON_CONNECTING)
            && tmp->client->sess.adminLevel >= LEVEL_BADMIN
            && !Q_stricmp(removableName, tmp->client->sess.adminName)
            && (
                (
                    passadmin && tmp->client->sess.adminType == ADMINTYPE_PASS
                )
                ||
                (
                    !passadmin && removableIp && !Q_stricmp(removableIp, tmp->client->pers.ip) && tmp->client->sess.adminType == ADMINTYPE_IP
                )
            )
        ) {
            // mammoth if clause but if above states true, then the player's admin should be removed.

            G_Broadcast(va("%s\n^7their %s powers were \\removed\nby %s", tmp->client->pers.netname, getAdminNameByLevel(removableLevel), getNameOrArg(adm, "RCON", qfalse)), BROADCAST_CMD, NULL, qtrue);
            tmp->client->sess.adminLevel = LEVEL_NOADMIN;
            break;
        }
    }

}

void removeAdminsFromGame(int adminType) {
    gentity_t* tmp;

    for (int i = 0; i < level.numConnectedClients; i++) {
        tmp = &g_entities[level.sortedClients[i]];

        if (tmp && tmp->client && tmp->client->sess.adminLevel >= LEVEL_BADMIN && tmp->client->sess.adminType == adminType) {
            tmp->client->sess.adminLevel = 0;
        }
    }

    G_Broadcast(va("All %sadmins were \\removed", adminType == ADMINTYPE_IP ? "" : "pass"), BROADCAST_GAME, NULL, qtrue);

}

void swapTeams(qboolean autoSwap, gentity_t* adm) {

    int redScore = level.teamScores[TEAM_RED];
    int blueScore = level.teamScores[TEAM_BLUE];
    qboolean redLocked = level.redTeamLocked;
    qboolean blueLocked = level.blueTeamLocked;
    gentity_t* tmp;

    if (!level.gametypeData->teams) {
        G_printInfoMessage(adm, "Currently not playing a team game.");
        return;
    }

    if (!Q_stricmp(g_realGametype.string, "h&s") || !Q_stricmp(g_realGametype.string, "h&z")) {
        G_printInfoMessage(adm, "This gametype does not support swapping teams.");
        return;
    }

    for (int i = 0; i < level.numConnectedClients; i++) {
        tmp = &g_entities[level.sortedClients[i]];

        if (tmp->client->pers.connected != CON_CONNECTED) {
            continue;
        }

        if (tmp->client->sess.team == TEAM_SPECTATOR) {
            continue;
        }

        SetTeam(tmp, tmp->client->sess.team == TEAM_RED ? "blue" : "red", NULL, TEAMCHANGE_SWAP);
    }

    level.teamScores[TEAM_BLUE] = redScore;
    level.teamScores[TEAM_RED] = blueScore;

    level.redTeamLocked = blueLocked;
    level.blueTeamLocked = redLocked;

    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3", qtrue));

    if (!autoSwap) {
        G_printInfoMessageToAll("Swap teams by %s.", getNameOrArg(adm, "RCON", qtrue));
        G_Broadcast(va("%s^7 has \\swapped the teams!", getNameOrArg(adm, "RCON", qfalse)), BROADCAST_GAME, NULL, qtrue);
        logAdmin(adm, NULL, "Swapteams", NULL);
    }
    else {
        G_printInfoMessageToAll("Teams have been swapped automatically.");
    }
}

/*
=============
Wrapper function not to call ent && ent->client ? on functions requiring name or an arg (e.g. RCON) to be displayed.
=============
*/
char* getNameOrArg(gentity_t* ent, char* arg, qboolean cleanName) {

    if (cleanName) {
        return ent && ent->client ? ent->client->pers.cleanName : arg;
    }

    return ent && ent->client ? ent->client->pers.netname : arg;

}

/*
===========
Boe_Tokens in 1fx. Mod
===========
*/
void parseTokens(gentity_t* ent, char* chatText, int mode, qboolean checkSounds) {

}

char* getTeamPrefixByGametype(int team) {
    
    if (team == TEAM_BLUE) {
        if (!Q_stricmp(g_realGametype.string, "h&s")) {
            return "seekers";
        }
        else {
            return g_blueTeamPrefix.string;
        }
    }
    else {
        if (!Q_stricmp(g_realGametype.string, "h&s")) {
            return "hiders";
        }
        else {
            return g_redTeamPrefix.string;
        }
    }

}

void respawnClient(gentity_t* requestor, gentity_t* recipient) {

    if (recipient->client->sess.team == TEAM_SPECTATOR){
        G_printInfoMessage(requestor, "You cannot respawn a spectator.");
        return -1;
    }

    if (recipient->client->sess.ghost) {
        G_StopFollowing(recipient);
        recipient->client->ps.pm_flags &= ~PMF_GHOST;
        recipient->client->ps.pm_type = PM_NORMAL;
        recipient->client->sess.ghost = qfalse;
    }
    else {
        TossClientItems(recipient);
    }
    recipient->client->sess.noTeamChange = qfalse;
    trap_UnlinkEntity(recipient);
    ClientSpawn(recipient);

    G_ClientSound(recipient, G_SoundIndex("sound/ambience/vehicles/telephone_pole.mp3", qtrue)); // Let the client know something happened.

}

void runMapStateEvents(void) {

    int mapState = level.mapState;
    level.mapState = MAPSTATE_NOTHING;

    if (mapState == MAPSTATE_RESTART || mapState == MAPSTATE_CHANGEGT) {
        trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
    } else if (mapState == MAPSTATE_CHANGEMAP) {
        trap_SendConsoleCommand(EXEC_APPEND, va("map %s\n", level.newMap));
    } else if (mapState == MAPSTATE_MAPCYCLE) {
        trap_SendConsoleCommand(EXEC_APPEND, "mapcycle\n");
    }

}

void runoverPlayer(gentity_t* recipient) {

    vec3_t      direction, fireAngles;

    VectorCopy(recipient->client->ps.viewangles, fireAngles);
    AngleVectors(fireAngles, direction, NULL, NULL);
    direction[0] *= -1.0;
    direction[1] *= -1.0;
    direction[2] = 0.0;
    VectorNormalize(direction);

    G_ClientSound(recipient, G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3", qtrue));

    // Do the actual action.
    recipient->client->ps.velocity[2] = 20;
    recipient->client->ps.weaponTime = 3000;
    G_Damage(recipient, NULL, NULL, NULL, NULL, 15, 0, MOD_CAR, HL_NONE);
    G_ApplyKnockback(recipient, direction, 400.0f);
}

void uppercutPlayer(gentity_t* recipient, int ucLevel) {

    recipient->client->ps.pm_flags |= PMF_JUMPING;
    recipient->client->ps.groundEntityNum = ENTITYNUM_NONE;

    if (ucLevel) {
        recipient->client->ps.velocity[2] = 200 * ucLevel;
    } else {
        recipient->client->ps.velocity[2] = 1000;
    }

    G_ClientSound(recipient, G_SoundIndex("sound/weapons/rpg7/fire01.mp3", qtrue));
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
    spin[0] = (float) Q_irand(0, 360);
    spin[1] = fmod(spin[1] + 20, 360.0);
    SetClientViewAngle(recipient, spin);
    G_ClientSound(recipient, G_SoundIndex("sound/npc/air1/guard02/laughs.mp3", qtrue));
}

void stripClient(gentity_t* recipient, qboolean handsUp) {

    int idle;

    if (recipient->client->sess.team == TEAM_SPECTATOR) {
        return;
    }

    recipient->client->ps.zoomFov = 0;
    recipient->client->ps.pm_flags &= ~(PMF_GOGGLES_ON | PMF_ZOOM_FLAGS);

    recipient->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
    memset(recipient->client->ps.ammo, 0, sizeof(recipient->client->ps.ammo));
    memset(recipient->client->ps.clip, 0, sizeof(recipient->client->ps.clip));

    if (handsUp) {
        recipient->client->ps.stats[STAT_WEAPONS] = 0;
        recipient->client->ps.weapon = WP_NONE;

    } else {
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

void resetSession(gentity_t* ent) {

    clientSession_t* sess = &ent->client->sess;
    sess->adminLevel = LEVEL_NOADMIN;
    sess->adminPassRegistration = LEVEL_NOADMIN;
    memset(sess->adminName, 0, sizeof(sess->adminName));
    memset(sess->adminPassAddedBy, 0, sizeof(sess->adminPassAddedBy));
    sess->playerDbChecksDone = qfalse;
    sess->planted = qfalse;
    sess->lastSpin = 0;
    sess->clientMod = CL_NONE;
    memset(sess->clientModVersion, 0, sizeof(sess->clientModVersion));
    sess->clientCheckCount = 0;

}

void G_FreeStatsMemory(gentity_t* ent)
{
    int i;
    gclient_t* client;

    if (ent == NULL) {
        for (i = 0; i < level.maxclients; i++)
        {
            client = &level.clients[i];

            if (client->pers.statinfo.weapon_shots != NULL)
                free(client->pers.statinfo.weapon_shots);
            if (client->pers.statinfo.weapon_hits != NULL)
                free(client->pers.statinfo.weapon_hits);
            if (client->pers.statinfo.weapon_headshots != NULL)
                free(client->pers.statinfo.weapon_headshots);

            client->pers.statinfo.weapon_shots = NULL;
            client->pers.statinfo.weapon_hits = NULL;
            client->pers.statinfo.weapon_headshots = NULL;
        }
    }
    else {
        client = ent->client;

        if (client->pers.statinfo.weapon_shots != NULL)
            free(client->pers.statinfo.weapon_shots);
        if (client->pers.statinfo.weapon_hits != NULL)
            free(client->pers.statinfo.weapon_hits);
        if (client->pers.statinfo.weapon_headshots != NULL)
            free(client->pers.statinfo.weapon_headshots);

        client->pers.statinfo.weapon_shots = NULL;
        client->pers.statinfo.weapon_hits = NULL;
        client->pers.statinfo.weapon_headshots = NULL;
    }
}

void G_EmptyStatsMemory(gentity_t* ent)
{
    // Free the stats memory before memsetting the struct.
    G_FreeStatsMemory(ent);

    // Memset it now.
    memset(&ent->client->pers.statinfo, 0, sizeof(ent->client->pers.statinfo));

    // Re-allocate the memory.
    G_AllocateStatsMemory(ent);
}

void G_AllocateStatsMemory(gentity_t* ent)
{
    int i;
    gclient_t* client;

    if (ent == NULL) {
        for (i = 0; i < level.maxclients; i++)
        {
            client = &level.clients[i];

            client->pers.statinfo.weapon_shots = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
            client->pers.statinfo.weapon_hits = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
            client->pers.statinfo.weapon_headshots = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
            if (!client->pers.statinfo.weapon_shots || !client->pers.statinfo.weapon_hits || !client->pers.statinfo.weapon_headshots) {
                Com_Error(ERR_FATAL, "Unable to initialize memory for weapon stats! Out of memory?");
            }
        }
    }
    else {
        client = ent->client;

        client->pers.statinfo.weapon_shots = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
        client->pers.statinfo.weapon_hits = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
        client->pers.statinfo.weapon_headshots = calloc(ATTACK_MAX * WP_NUM_WEAPONS, sizeof(int));
        if (!client->pers.statinfo.weapon_shots || !client->pers.statinfo.weapon_hits || !client->pers.statinfo.weapon_headshots) {
            Com_Error(ERR_FATAL, "Unable to initialize memory for weapon stats! Out of memory?");
        }
    }
}

void refreshStats(gentity_t* ent) {

    if (level.time <= level.gametypeStartTime + 5000) {
        G_printInfoMessage(ent, "You shouldn't refresh at the start of a new round.");
        return;
    }

    if (!G_IsClientSpectating(ent->client))
    {
        ent->flags &= ~FL_GODMODE;
        ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
        player_die(ent, ent, ent, 100000, MOD_REFRESH, HL_NONE, vec3_origin);
    }

    G_EmptyStatsMemory(ent);

    ent->client->pers.enterTime = level.time;
    ent->client->ps.persistant[PERS_SCORE] = 0;
    ent->client->sess.score = 0;
    ent->client->sess.deaths = 0;
    ent->client->sess.kills = 0;
    ent->client->sess.teamkillDamage = 0;
    ent->client->sess.teamkillForgiveTime = 0;
    ent->client->pers.statinfo.lastclient_hurt = -1;
    ent->client->pers.statinfo.lasthurtby = -1;
    ent->client->pers.statinfo.lastKillerHealth = -1;
    ent->client->pers.statinfo.lastKillerArmor = -1;
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

int evenTeams(qboolean automatic) {

    int redTeamCount, blueTeamCount;
    gentity_t* recipient;
    int maxConnectedTime = 0, team = 0, playersToMove = 0;
    qboolean havePlayersBeenMoved = qfalse;

    if (level.redTeamLocked || level.blueTeamLocked) {
        return EVENTEAMS_TEAM_LOCKED;
    }

    if (!Q_stricmp(g_realGametype.string, "hnz") || !level.gametypeData->teams) {
        return EVENTEAMS_GT_INCOMPATIBLE;
    }

    redTeamCount = TeamCount(-1, TEAM_RED, NULL);
    blueTeamCount = TeamCount(-1, TEAM_BLUE, NULL);

    if (redTeamCount <= 1 && blueTeamCount <= 1) {
        return EVENTEAMS_EVEN;
    }

    if (!Q_stricmp(g_realGametype.string, "hns")) {
        int totalPlayers = redTeamCount + blueTeamCount;
        int seekers = 0, maxhiders = 0;
        if (level.customETHiderAmount[0]) {

            for (int i = 0; i < sizeof(level.customETHiderAmount); i++) {
                if (level.customETHiderAmount[i + 1] == -1) {
                    seekers = i + 1;
                    break;
                }

                if (totalPlayers >= level.customETHiderAmount[i] && totalPlayers <= level.customETHiderAmount[i + 1]) {
                    seekers = i + 1;
                    break;
                }
            }

        }
        else {
            if (redTeamCount < 4) {
                seekers = 1;
            }
            else {
                seekers = (totalPlayers + 1) / 5;
            }
        }

        maxhiders = totalPlayers - seekers;

        team = TEAM_RED;
        playersToMove = blueTeamCount - seekers;

        if (blueTeamCount < seekers) {
            team = TEAM_BLUE;
            playersToMove = seekers - blueTeamCount;
        }

        if (playersToMove <= 0) {
            return EVENTEAMS_EVEN;
        }

        for (int i = 0; i < playersToMove; i++) {
            recipient = getLastConnectedClientInTeam(team == TEAM_RED ? TEAM_BLUE : TEAM_RED, qfalse);

            if (recipient) {
                SetTeam(recipient, team == TEAM_RED ? "r" : "b", NULL, TEAMCHANGE_FORCED);
                havePlayersBeenMoved = qtrue;
            }
            else if (!havePlayersBeenMoved) {
                return EVENTEAMS_NORECIPIENTS;
            }
        }

    }
    else {
        playersToMove = abs(redTeamCount - blueTeamCount);

        if (playersToMove <= 1) {
            return 0;
        }

        playersToMove /= 2;

        team = TEAM_RED;


        if (redTeamCount > blueTeamCount) {
            team = TEAM_BLUE;
        }

        for (int i = 0; i < playersToMove; i++) {

            recipient = getLastConnectedClientInTeam(team == TEAM_RED ? TEAM_BLUE : TEAM_RED, qtrue);

            if (recipient) {
                SetTeam(recipient, team == TEAM_RED ? "r" : "b", NULL, TEAMCHANGE_FORCED);
                havePlayersBeenMoved = qtrue;
            }
            else if (!havePlayersBeenMoved) {
                return EVENTEAMS_NORECIPIENTS;
            }

        }


    }

    if (automatic) { // if not then most likely this is from an admin call and the broadcasting will come from that function instead.
        G_Broadcast("Teams have been \\evened!", BROADCAST_GAME, NULL, qtrue);
        G_printInfoMessageToAll("Teams have been evened!");
        logGame(); // currently a placeholder before the game logging is done just to track through func references.
    }

    G_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3", qtrue));

    return EVENTEAMS_DONE;

}

void parseCustomETHiders(gentity_t* ent) {

    int tmp;
    for (int i = 1; i <= MAX_CLIENTS; i++) {

        G_SpawnInt(va("%i", i), -1, &tmp);

        if (tmp) {
            level.customETHiderAmount[i - 1] = tmp;
        }
    }
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