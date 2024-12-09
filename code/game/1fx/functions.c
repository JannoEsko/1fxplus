
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
    int newWordPosition;
    qboolean parseLine = qtrue;

    // Reset broadcast buffer.
    memset(newBroadcast, 0, sizeof(newBroadcast));

    // Loop through the entire broadcast, processing one line at a time.
    while (parseLine) {
        // Check for newlines to determine line boundaries.
        char* tempNewline = strstr(broadcast, "\n");
        int lineLength = tempNewline ? (tempNewline - broadcast) : strlen(broadcast);

        // Stop processing after the last line.
        parseLine = (tempNewline != NULL);

        // First we check the line. Should colors be applied to it?
        // A backslash can never be applied to any command in-game, like broadcast.
        // Use that to determine what word should be highlighted.
        tempBroadcast = strstr(broadcast, "\\");
        if (tempBroadcast && tempBroadcast < broadcast + lineLength) {
            // OK, a word is in the line. Find position in actual broadcast.
            newWordPosition = (int)(tempBroadcast - broadcast);

            // Skip the backslash character.
            tempBroadcast++;

            // Determine the length of the word to highlight.
            char* wordEnd = strstr(tempBroadcast, " ");
            newWordLength = wordEnd ? (wordEnd - tempBroadcast) : strlen(tempBroadcast);

            // Now we apply the colors to the broadcast if there are colors to apply it to.
            if (newWordLength > 0 && newWordLength < 64) {
                // Add the text before the highlighted word to the new broadcast.
                strncat(newBroadcast, broadcast, newWordPosition);

                // Check how many colors there are available to use.
                int availableColors = strlen(g_serverColors.string);
                char wordToCopy[64];
                strncpy(wordToCopy, tempBroadcast, newWordLength);
                wordToCopy[newWordLength] = '\0';

                // Copy each individual char to the new buffer, adding the color associated with it.
                for (i = 0; i < newWordLength; i++) {
                    if (i < availableColors) {
                        strcat(newBroadcast, va("^%c%c", g_serverColors.string[i], wordToCopy[i]));
                    }
                    else {
                        strncat(newBroadcast, &wordToCopy[i], 1);
                    }
                }

                // Add a trailing ^7 to reset the colors for the rest of the text.
                strcat(newBroadcast, "^7");

                // Is there a remaining string after the highlighted word in the line?
                if (wordEnd) {
                    strncat(newBroadcast, wordEnd, strlen(wordEnd));
                }
            }

            // Move the broadcast pointer forward to process the remaining text.
            broadcast += newWordPosition + newWordLength + 1;
        }
        else {
            // No backslash found in this line, copy it as-is.
            strncat(newBroadcast, broadcast, lineLength);
            broadcast += lineLength;
        }

        // If a newline was found, copy it as well and advance the pointer.
        if (tempNewline) {
            strncat(newBroadcast, "\n", 1);
            broadcast++;
        }
    }

    // Boe!Man 3/13/15: Replace newlines with spaces when the game is paused.
    if (level.paused) {
        for (i = 0; newBroadcast[i] != '\0'; i++) {
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
    char* newBroadcast;
    Com_Printf("TXT: %s\n", text);
    newBroadcast = G_ColorizeMessage(text);
    Com_Printf("nB: %s\n", newBroadcast);

    // If to is NULL, we're dealing with a global message (equals old way of broadcasting to -1).
    if (to == NULL) {

        if (playSound) {
            //G_GlobalSound(level.actionSoundIndex);
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
