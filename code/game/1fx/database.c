#include "../g_local.h"
#include "1fxFunctions.h"
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif


void checkDatabaseIntegrity() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int              rc, gameMigrationLevel = 0, logMigrationLevel = 0;
    struct stat st = { 0 };

    if (stat("./1fx/databases", &st) == -1) {
#ifdef _WIN32
#ifdef __GNUC__
        mkdir("./1fx/databases");
#elif _MSC_VER
        mkdir(".\\1fx\\databases");
#endif
#elif __linux__
        mkdir("./1fx/databases", 0755);
#endif
    }

    rc = sqlite3_open("./1fx/databases/game.db", &db);
    
    if (rc) {
        logSystem(LOGLEVEL_FATAL_DB, va("./databases/game.db sqlite3_open failed. RC: %d", rc));
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        Com_Error(ERR_FATAL, "Game dropped due to failing to write game.db file in databases folder.\n");
        return;
    }

    // check for the migration level, use create table if not exists to create migration table.

    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        logSystem(LOGLEVEL_FATAL_DB, va("./databases/gameDb.db sqlite_exec on migrationlevel failed."));
        sqlite3_close(db);
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        Com_Error(ERR_FATAL, "Game dropped due to failing to write migrationlevel table into game.db.\n");
        return;
    }

    sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        gameMigrationLevel = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    Com_Printf("Mod GameDB migration level: %d\n", SQL_GAME_MIGRATION_LEVEL);
    Com_Printf("Current GameDB migration level: %d...\n", gameMigrationLevel);
    migrateGameDatabase(db, gameMigrationLevel);
    Com_Printf("Attaching GameDB to memory...\n");
    sqlite3_open(":memory:", &gameDb);

    sqlite3_exec(db, "BEGIN", NULL, NULL, NULL);
    sqlite3_exec(db, "SELECT sql FROM sqlite_master WHERE sql NOT NULL", &process_ddl_row, gameDb, NULL);
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);

    sqlite3_close(db);

    Q_strncpyz(sqlTempName, "game", sizeof(sqlTempName));

    sqlite3_exec(gameDb, "ATTACH DATABASE './1fx/databases/game.db' as game", NULL, NULL, NULL);
    sqlite3_exec(gameDb, "BEGIN", NULL, NULL, NULL);
    sqlite3_exec(gameDb, "SELECT name FROM country.sqlite_master WHERE type='table'", &process_dml_row, gameDb, NULL);
    sqlite3_exec(gameDb, "COMMIT", NULL, NULL, NULL);

    Com_Printf("GameDB attached.\n");


}

void migrateGameDatabase(sqlite3* db, int migrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically 
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.

    char* migration;

    if (migrationLevel < 1) {
        // fresh database, create all tables.
        migration = "CREATE TABLE IF NOT EXISTS adminlist (adminname VARCHAR(64) COLLATE NOCASE, ip VARCHAR(24), adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP);"
            "CREATE TABLE IF NOT EXISTS banlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0);"
            "CREATE TABLE IF NOT EXISTS subnetbanlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0);"
            "CREATE TABLE IF NOT EXISTS adminpasslist (adminname VARCHAR(64) COLLATE NOCASE, adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, password VARCHAR(80));"
            "CREATE TABLE IF NOT EXISTS aliases (alias VARCHAR(64), ip VARCHAR(24));"
            "CREATE INDEX IF NOT EXISTS idx_aliases_ip ON aliases (ip);"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (1);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            logSystem(LOGLEVEL_FATAL_DB, va("Migration level 1 failed (needed: %d)", migrationLevel));
            sqlite3_close(db);
            Com_Error(ERR_FATAL, "Game dropped due to failing to migrate the database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", migrationLevel, sqlite3_errmsg(gameDb), sqlite3_errcode(gameDb));
            return;
        }
    }

}



// Boe!Man 5/27/13: Misc. SQLite functions.
int process_ddl_row(void* pData, int nColumns,
    char** values, char** columns)
{
    sqlite3* db;

    if (nColumns != 1)
        return 1; // Error

    db = (sqlite3*)pData;
    sqlite3_exec(db, values[0], NULL, NULL, NULL);

    return 0;
}

int process_dml_row(void* pData, int nColumns,
    char** values, char** columns)
{
    sqlite3* db;
    char* stmt;

    if (nColumns != 1)
        return 1; // Error

    db = (sqlite3*)pData;

    stmt = sqlite3_mprintf("insert into main.%q "
        "select * from %s.%q", values[0], sqlTempName, values[0]);
    sqlite3_exec(db, stmt, NULL, NULL, NULL);
    sqlite3_free(stmt);

    return 0;
}
// End Boe!Man 5/27/13



