// header file containing all functions needed to store information in SQLite databases.
#include "../../ext/sqlite/sqlite3.h"

extern sqlite3* gameDb;
extern sqlite3* logDb;
extern char sqlTempName[16];

#define SQL_GAME_MIGRATION_LEVEL 1
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

#define MAX_PACKET_BUF 1000

void loadDatabases();
void migrateGameDatabase(sqlite3* db, int migrationLevel);
void migrateLogsDatabase(sqlite3* db, int migrationLevel);

int processTableStructure(void* pData, int nColumns, char** values, char** columns);
int processTableData(void* pData, int nColumns, char** values, char** columns);

void removeIngameAdminByNameAndType(gentity_t* adm, qboolean passadmin, char* removableName, char* removableIp, int removableLevel);
void removeAdminsFromGame(int adminType);


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
void admGetBanDurationFromArg(qboolean shortCmd, int *duration, char *arg);
void unloadInMemoryDatabases();
void backupInMemoryDatabases(char* dbName, sqlite3* db);
char* getAdminNameByLevel(int adminLevel);
char* getAdminPrefixByLevel(int adminLevel);
void sqlBindTextOrNull(sqlite3_stmt* stmt, int argnum, char* text);
int dbGetPlayerAdminLevel(qboolean passlist, char* ip, char* name, char* password);
qboolean dbIsIpNameInAdminList(qboolean passlist, char* ip, char* name);
void dbUpdatePassAdmin(char* adminname, char* newpass);
int dbGetAdminByRowId(qboolean password, int rowid, char* adminOut, char* ipOut);
qboolean dbDoesRowIDExist(char* table, int rowid);


// admin commands
int admAdminList(int argNum, gentity_t* adm, qboolean shortCmd);
int admRemoveAdminByNameOrId(int argNum, gentity_t* adm, qboolean shortCmd);
int admRemoveAdminByRowId(int argNum, gentity_t* adm, qboolean shortCmd);
int admAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd, int adminLevel);
int admHandleAddBadmin(int argNum, gentity_t* adm, qboolean shortCmd);
int admHandleAddAdmin(int argNum, gentity_t* adm, qboolean shortCmd);
int admHandleAddSadmin(int argNum, gentity_t* adm, qboolean shortCmd);
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
void G_GlobalSound(int soundIndex); // Boe_GlobalSound
void G_CloseSound(vec3_t origin, int soundIndex); // Henk_CloseSound
qboolean checkAdminPassword(char* adminPass);