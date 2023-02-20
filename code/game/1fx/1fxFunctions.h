// header file containing all functions needed to store information in SQLite databases.
#include "../../ext/sqlite/sqlite3.h"

extern sqlite3* gameDb;
extern sqlite3* logDb;
extern char sqlTempName[16];

#define SQL_GAME_MIGRATION_LEVEL 2
#define SQL_LOG_MIGRATION_LEVEL 1

#define LOGLEVEL_INFO 0
#define LOGLEVEL_WARNING 1
#define LOGLEVEL_ERROR 2
#define LOGLEVEL_FATAL 3
#define LOGLEVEL_FATAL_DB 4 // this is used if we have a fatal DB-related error (most likely not usable DB), which means that if db logging is turned on, game will not try to log it into the DB because, well..., the DB part failed...

typedef enum {
    ADMINTYPE_RCON,
    ADMINTYPE_IP,
    ADMINTYPE_PASS
} admTypes;

typedef enum {
    COASTER_RUNOVER,
    COASTER_UPPERCUT,
    COASTER_SPIN
} coasterState;

typedef enum {
    CL_NONE,
    CL_ROCMOD
} clientMod;

#define MAX_PACKET_BUF 1000

#define COREUI_DEFAULT_PAKNAME "1fx.additions/1fx_coreUI_1.21"
#define COREUI_DEFAULT_PAKNUM -1896782216

#define COREUI_ROCMOD_PAKNAME "1fx.rocmod/ROCmod_1fx_coreUI_1.21"
#define COREUI_ROCMOD_PAKNUM 1917941726

void loadDatabases(void);
void migrateGameDatabase(sqlite3* db, int migrationLevel);
void migrateLogsDatabase(sqlite3* db, int migrationLevel);

int processTableStructure(void* pData, int nColumns, char** values, char** columns);
int processTableData(void* pData, int nColumns, char** values, char** columns);

void removeIngameAdminByNameAndType(gentity_t* adm, qboolean passadmin, char* removableName, char* removableIp, int removableLevel);
void removeAdminsFromGame(int adminType);
char* concatArgs(int fromArgNum, qboolean shortCmd);
void swapTeams(qboolean autoSwap, gentity_t* adm);
char* getNameOrArg(gentity_t* ent, char* arg, qboolean cleanName);
void parseTokens(gentity_t* ent, char* chatText, int mode, qboolean checkSounds);
char* getTeamPrefixByGametype(int team);
void respawnClient(gentity_t* requestor, gentity_t* ent);
void runMapStateEvents(void);
void uppercutPlayer(gentity_t* recipient, int ucLevel);
void runoverPlayer(gentity_t* recipient);
void spinView(gentity_t* recipient);
void stripTeam(int team, qboolean handsUp);
void stripClient(gentity_t* recipient, qboolean handsUp);
void stripEveryone(qboolean handsUp);
void resetSession(gentity_t* ent);
void G_FreeStatsMemory(gentity_t* ent);
void G_AllocateStatsMemory(gentity_t* ent);
void G_EmptyStatsMemory(gentity_t* ent);
void refreshStats(gentity_t* ent);
int normalAttackMod(int mod);
int altAttack(int weapon);



// struct from 1fxmod

typedef struct
{
    char* shortCmd; // Short admin command, example: !uc, !p(with space)
    char* adminCmd; // Full Admin command for /adm and rcon, and with !.
    int* adminLevel; // The level that the Admin needs to be in order to execute this command.
    int     (*Function)(int argNum, gentity_t* adm, qboolean shortCmd); // Store pointer to the given function so we can call it later.
    char* desc; // Description of the command in /adm.
    char* params; // Description of the command in /adm.
    char* suffix; // Suffix for post processing broadcast, or NULL when function doesn't use it/has no suffix.
}admCmd_t;

typedef enum {
    MAPSTATE_NOTHING, // default, no action defined.
    MAPSTATE_RESTART,
    MAPSTATE_CHANGEMAP,
    MAPSTATE_CHANGEGT,
    MAPSTATE_MAPCYCLE
} mapStates;

typedef enum {
    SPINVIEW_NONE,
    SPINVIEW_SLOW,
    SPINVIEW_FAST
};

extern int adminCommandsSize;
extern admCmd_t adminCommands[];

// BoeMan 8/30/14: Broadcast priorities, from low to high.
typedef enum {
    BROADCAST_GAME,             // Regular game messages.
    BROADCAST_CMD,              // Admin commands such as uppercut, broadcast, etc.
    BROADCAST_GAME_IMPORTANT,   // More important gametype messages that should override Admin commands.
    BROADCAST_AWARDS,           // Awards (H&S and regular).
    BROADCAST_MOTD              // Message of the day when entering the server.
} broadcastPrio_t;



void logGame();
void logSystem(int faultLevel, char* message);
void logAdmin(gentity_t* by, gentity_t* to, char* action, char* reason);
void logRcon(char* ip, char* action);
void logDamage();
void logObjective();
void logLogin(char* player, char* ip, int level, int method);

// db aliases of above functions.

void dbLogGame();
void dbLogSystem(int faultLevel, char* message);
void dbLogAdmin(char* byip, char* byname, char* toip, char* toname, char* action, char* reason, int adminlevel, char* adminname, int admintype);
void dbLogRcon(char* ip, char* action);
void dbLogDamage();
void dbLogObjective();
void dbLogLogin(char* player, char* ip, int level, int method);
void dbAddAdmin(char* adminname, char* ip, int adminlevel, char* addedby);
void dbAddPassAdmin(char* adminname, int adminlevel, char* addedby, char* password);
qboolean dbTruncateGameDbTable(char* tableName);
void dbDeleteFromGameDbByRowId(char* query, int rowId);
void dbDeleteAdmin(int rowId);
void dbDeletePassAdmin(int rowId);
void dbGetAdminlist(gentity_t* ent, qboolean passlist);
void getSubnet(char* ipIn, char* out);
void getBanDurationFromArg(int *duration, char *arg);
void unloadInMemoryDatabases(void);
void backupInMemoryDatabases(char* dbName, sqlite3* db);
char* getAdminNameByLevel(int adminLevel);
char* getAdminPrefixByLevel(int adminLevel);
void sqlBindTextOrNull(sqlite3_stmt* stmt, int argnum, char* text);
int dbGetPlayerAdminLevel(qboolean passlist, char* ip, char* name, char* password);
qboolean dbIsIpNameInAdminList(qboolean passlist, char* ip, char* name);
void dbUpdatePassAdmin(char* adminname, char* newpass);
int dbGetAdminByRowId(qboolean password, int rowid, char** adminOut, char** ipOut);
qboolean dbDoesRowIDExist(char* table, int rowid);
int dbGetAdminRowIdByGentity(gentity_t* removable);
void dbClearOutdatedBans(qboolean includeEom);
void dbAddBan(qboolean isSubnet, char* playername, char* ip, char* adminname, char* reason, int endofmap, int days, int hours, int minutes);
qboolean checkBanReason(char* ip, qboolean isSubnet, char** output);
void dbGetBanlist(gentity_t* ent, qboolean isSubnet);
qboolean dbGetBanByRow(int rownum, char** bannedName, char** bannedIp, qboolean isSubnet);
void dbDeleteBanByRowId(int rownum, qboolean isSubnet);
void admUnbanPlayer(int argNum, gentity_t* adm, qboolean shortCmd, qboolean isSubnet);


// admin commands
int admAdminList(int argNum, gentity_t* adm, qboolean shortCmd);
int admRemoveAdminByNameOrId(int argNum, gentity_t* adm, qboolean shortCmd);
int admRemoveAdminByRowId(int argNum, gentity_t* adm, qboolean shortCmd);
int admAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd, int adminLevel);
int admHandleAddBadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int admHandleAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd);
int admHandleAddSadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int admScoreLimit(int argNum, gentity_t* adm, qboolean shortCmd);
int admTimeLimit(int argNum, gentity_t* adm, qboolean shortCmd);
int admSwapTeams(int argNum, gentity_t* adm, qboolean shortCmd);
void admToggleCVAR(int argNum, gentity_t* adm, qboolean shortCmd, char* cvarName, vmCvar_t* cvar);
int admForceTeam(int argNum, gentity_t* adm, qboolean shortCmd);
int admBanlist(int argNum, gentity_t* adm, qboolean shortCmd);
int admBan(int argNum, gentity_t* adm, qboolean shortCmd);
int admUnban(int argNum, gentity_t* adm, qboolean shortCmd);
int admBroadcast(int argNum, gentity_t* adm, qboolean shortCmd);
int admSubnetbanList(int argNum, gentity_t* adm, qboolean shortCmd);
int admSubnetBan(int argNum, gentity_t* adm, qboolean shortCmd);
int admSubnetUnban(int argNum, gentity_t* adm, qboolean shortCmd);
int admPop(int argNum, gentity_t* adm, qboolean shortCmd);
int admUppercut(int argNum, gentity_t* adm, qboolean shortCmd);
int admSwitch(int argNum, gentity_t* adm, qboolean shortCmd);
int admKick(int argNum, gentity_t* adm, qboolean shortCmd);
int admLockTeam(int argNum, gentity_t* adm, qboolean shortCmd);
int admRespawn(int argNum, gentity_t* adm, qboolean shortCmd);
int admPlant(int argNum, gentity_t* adm, qboolean shortCmd);
void admUnplant(gentity_t* adm, gentity_t* recipient);
int admRespawnInterval (int argNum, gentity_t* adm, qboolean shortCmd);
int admRoundTimelimit (int argNum, gentity_t* adm, qboolean shortCmd);
int admRunover (int argNum, gentity_t* adm, qboolean shortCmd);
int admRollercoaster (int argNum, gentity_t* adm, qboolean shortCmd);
int admMapRestart (int argNum, gentity_t* adm, qboolean shortCmd);
int admStrip (int argNum, gentity_t* adm, qboolean shortCmd);
int admShuffleTeams (int argNum, gentity_t* adm, qboolean shortCmd);
int admGametypeRestart (int argNum, gentity_t* adm, qboolean shortCmd);
int admEventeams (int argNum, gentity_t* adm, qboolean shortCmd);
int admGametype (int argNum, gentity_t* adm, qboolean shortCmd);
int admFriendlyFire (int argNum, gentity_t* adm, qboolean shortCmd);
int admRename (int argNum, gentity_t* adm, qboolean shortCmd);
int admBurn (int argNum, gentity_t* adm, qboolean shortCmd);
int admMapcycle (int argNum, gentity_t* adm, qboolean shortCmd);

int cmdIsAdminCmd(char* cmd, qboolean shortCmd);
void runAdminCommand(int adminCommandId, int argNum, gentity_t* adm, qboolean shortCmd);
void postExecuteAdminCommand(int funcNum, int idNum, gentity_t *adm);
qboolean canClientRunAdminCommand(gentity_t* adm, int adminCommandId);

void QDECL G_printInfoMessage(gentity_t *ent, const char *msg, ...) __attribute__ ((format (printf, 2, 3)));
void QDECL G_printInfoMessageToAll(const char *msg, ...) __attribute__ ((format (printf, 1, 2)));
int G_clientNumFromArg(gentity_t *ent, int argNum, const char *action, qboolean aliveOnly, qboolean otherAdmins, qboolean higherLvlAdmins, qboolean shortCmd);

char *G_GetArg(int argNum,qboolean shortCmd);
char *G_GetChatArgument(int argNum);
int G_GetChatArgumentCount();
void G_RemoveAdditionalCarets(char *text);
void G_RemoveColorEscapeSequences(char *text);
char* G_ColorizeMessage(char* broadcast);
void G_Broadcast(char* broadcast, int broadcastLevel, gentity_t* to, qboolean playSound);
void G_ClientSound(gentity_t* ent, int soundIndex); // Boe_ClientSound
void G_GlobalSound(int soundIndex); // Boe_GlobalSound
void G_CloseSound(vec3_t origin, int soundIndex); // Henk_CloseSound
qboolean checkAdminPassword(char* adminPass);