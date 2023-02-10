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


void checkDatabaseIntegrity();
void migrateGameDatabase(sqlite3* db, int migrationLevel);
void migrateLogsDatabase(sqlite3* db, int migrationLevel);

int process_ddl_row(void* pData, int nColumns, char** values, char** columns);
int process_dml_row(void* pData, int nColumns, char** values, char** columns);


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



void logGame();
void logSystem(int faultLevel, char* message);
void logAdmin();
void logRcon(char* ip, char* action);
void logDamage();
void logObjective();
void logLogin(char* player, char* ip, int level, int method, char* reference);

// db aliases of above functions.

void dbLogGame();
void dbLogSystem(int faultLevel, char* message);
void dbLogAdmin();
void dbLogRcon(char* ip, char* action);
void dbLogDamage();
void dbLogObjective();
void dbLogLogin(char* player, char* ip, int level, int method, char* reference);

