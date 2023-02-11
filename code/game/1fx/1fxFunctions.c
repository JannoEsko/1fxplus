

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
        trap_Printf(text);
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
       && ent->client->sess.admin && g_entities[clientID].client->sess.admin){
        // Don't allow to execute the command on higher level Admins.
        if(g_entities[clientID].client->sess.admin > ent->client->sess.admin
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

    else if(!otherAdmins && ent && ent->client && !ent->client->sess.admin
            && g_entities[clientID].client->sess.admin)
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

int G_GetChatArgumentCount()
{
    char text[MAX_SAY_TEXT];
    char *text2;
    int argc = 0;

    // Fetch the argument containing the full buffer.
    trap_Argv(1, text, sizeof(text));
    text2 = text;

    // Loop through text, find first character.
    while (text2 != NULL && *text2 == ' ') {
        *text2++;
    }

    if (!text2 || strlen(text2) == 0) {
        // No real argument present.
        return 0;
    }

    while (text2 != NULL && strlen(text2) != 0) {
        text2 = strstr(text2, " ");

        // No more arguments found, return.
        if (text2 == NULL)
            break;

        // Get rid of extra spaces.
        while (text2 != NULL && *text2 == ' ') {
            *text2++;
        }

        if (text2 == NULL || strlen(text2) == 0) {
            // No real argument present.
            break;
        }

        argc++;
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
    char *text2, *end;
    int argc = 0;

    // Reset buffer.
    memset(newArg, 0, sizeof(newArg));

    // Fetch the argument containing the full buffer.
    trap_Argv(1, text, sizeof(text));
    text2 = text;

    // Argument must be present.
    if (G_GetChatArgumentCount() < argNum) {
        return "";
    }

    // Loop through text, find first character.
    while (text2 != NULL && *text2 == ' '){
        *text2++;
    }

    while (argc < argNum) {
        text2 = strstr(text2, " ");

        // Get rid of extra spaces.
        while (text2 && *text2 == ' ') {
            *text2++;
        }

        argc++;
    }

    // Check if there are more arguments after this one, or if it's the last one.
    end = strstr(text2, " ");
    if (end == NULL) {
        Q_strncpyz(newArg, text2, sizeof(newArg));
    }else{
        text2[end - text2] = '\0';
        Q_strncpyz(newArg, text2, sizeof(newArg));
    }

    // Remove colors from arg.
    RemoveColorEscapeSequences(newArg);

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

    if(!shortCmd && !G_GetChatArgumentCount()) {
        argNum++;
        trap_Argv(argNum,arg,sizeof(arg));
    }
    else {
        strncpy(arg,G_GetChatArgument(argNum),sizeof(arg));
    }

    return arg;
}

/*
==============
RemoveColorEscapeSequences
==============
*/

void RemoveColorEscapeSequences(char *text) {
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