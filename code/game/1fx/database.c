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
    if (SQL_GAME_MIGRATION_LEVEL != gameMigrationLevel) {
        Com_Printf("Migrating gameDb to be migrated from level %d to level %d.\n", gameMigrationLevel, SQL_GAME_MIGRATION_LEVEL);
        migrateGameDatabase(db, gameMigrationLevel);
    } else {
        Com_Printf("gameDb is up to date!\n");
    }

    Com_Printf("Attaching gameDB to memory...");
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

    Com_Printf(" done.\n");

    sqlite3_exec(gameDb, "DELETE FROM banlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", 0, 0, 0);
    sqlite3_exec(gameDb, "DELETE FROM subnetbanlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", 0, 0, 0);
    sqlite3_exec(gameDb, "VACUUM", NULL, NULL, NULL);

    rc = sqlite3_open("./1fx/databases/logs.db", &db);

    if (rc) {
        logSystem(LOGLEVEL_FATAL_DB, va("./databases/game.db sqlite3_open failed. RC: %d", rc));
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        Com_Error(ERR_FATAL, "Game dropped due to failing to write game.db file in databases folder.\n");
        return;
    }

    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        logSystem(LOGLEVEL_FATAL_DB, va("./databases/gameDb.db sqlite_exec on migrationlevel failed."));
        sqlite3_close(db);
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        Com_Error(ERR_FATAL, "Game dropped due to failing to write migrationlevel table into game.db.\n");
        return;
    }

    sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logMigrationLevel = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (SQL_GAME_MIGRATION_LEVEL != gameMigrationLevel) {
        Com_Printf("Migrating logsDb from level %d to level %d.\n", logMigrationLevel, SQL_LOG_MIGRATION_LEVEL);
        migrateLogsDatabase(db, logMigrationLevel);
    } else {
        Com_Printf("logsDb is up to date!\n");
    }

    sqlite3_close(db); // logs DB can get too large to run just in memory, therefore we're opening / closing it every time when it's needed.
    // can have a performance implication, but haven't seen any lag due to it in 3D which already runs the same logic.
    // FIXME if server gets weird lagouts every second or so, then look here.
    Com_Printf("Database checks done!\n");

}

void migrateGameDatabase(sqlite3* db, int migrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically 
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.



    if (migrationLevel < 1) {
        // fresh database, create all tables.
        char* migration = "CREATE TABLE IF NOT EXISTS adminlist (adminname VARCHAR(64) COLLATE NOCASE, ip VARCHAR(24), adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP);"
                    "CREATE TABLE IF NOT EXISTS banlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0);"
                    "CREATE TABLE IF NOT EXISTS subnetbanlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0);"
                    "CREATE TABLE IF NOT EXISTS adminpasslist (adminname VARCHAR(64) COLLATE NOCASE, adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, password VARCHAR(80));"
                    "CREATE TABLE IF NOT EXISTS aliases (alias VARCHAR(64), ip VARCHAR(24));"
                    "CREATE INDEX IF NOT EXISTS idx_aliases_ip ON aliases (ip);"
                    "DELETE FROM migrationlevel;"
                    "INSERT INTO migrationlevel (migrationlevel) VALUES (1);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            logSystem(LOGLEVEL_FATAL_DB, va("Migration level 1 failed (needed: %d)", SQL_GAME_MIGRATION_LEVEL));
            sqlite3_close(db);
            Com_Error(ERR_FATAL, "Game dropped due to failing to migrate the game database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", migrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

}


void migrateLogsDatabase(sqlite3* db, int migrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.

    if (migrationLevel < 1) {
        // fresh database, create all tables.
        char* migration = "CREATE TABLE IF NOT EXISTS rconlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, ip VARCHAR(50), action VARCHAR(1024));" // ip varchar50 because of IPv6, action 1024 because of a theoretical maximum of the rcon cvar.
                "CREATE TABLE IF NOT EXISTS adminlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), toip VARCHAR(50), toname VARCHAR(64), action VARCHAR(100), reason VARCHAR(1024), adminlevel INTEGER, adminName VARCHAR(64), admintype INTEGER);"
                "CREATE TABLE IF NOT EXISTS loginlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), adminlevel INTEGER, admintype INTEGER);"
                "CREATE TABLE IF NOT EXISTS gameslog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), toip VARCHAR(50), toname VARCHAR(64), action VARCHAR(1024));"
                "DELETE FROM migrationlevel;"
                "INSERT INTO migrationlevel (migrationlevel) VALUES (1);"
            ;

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            logSystem(LOGLEVEL_FATAL_DB, va("Migration level 1 failed (needed: %d)", SQL_LOG_MIGRATION_LEVEL));
            sqlite3_close(db);
            Com_Error(ERR_FATAL, "Game dropped due to failing to migrate the logs database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", migrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
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

void dbLogRcon(char* ip, char* action) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    char* query = "INSERT INTO rconlog (ip, action) VALUES (?, ?)";
    rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        Com_Printf("Failed to open logs.db file to log rcon!\nerr: %s\n", sqlite3_errmsg(db));
        logSystem(LOGLEVEL_WARNING, "Failed to open logs.db file to log rcon!\n");
        return;
    }

    sqlite3_prepare(db, query, -1, &stmt, 0);
    if (ip) {
        sqlite3_bind_text(stmt, 1, ip, strlen(ip), SQLITE_STATIC);
    }
    else {
        sqlite3_bind_null(stmt, 1);
    }

    if (action) {
        sqlite3_bind_text(stmt, 2, action, strlen(action), SQLITE_STATIC);
    } else {
        sqlite3_bind_null(stmt, 2);
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void dbAddAdmin(char* adminname, char* ip, int adminlevel, char* addedby) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    char* query = "INSERT INTO adminlist (adminame, ip, adminlevel, addedby) VALUES (?, ?, ?, ?)";
    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_text(stmt, 1, adminname, strlen(adminname), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, ip, strlen(ip), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, adminlevel);
    sqlite3_bind_text(stmt, 4, addedby, strlen(addedby), SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

}


void dbAddPassAdmin(char* adminname, int adminlevel, char* addedby, char* password) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    char* query = "INSERT INTO adminpasslist (adminname, adminlevel, addedby, password) VALUES (?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_text(stmt, 1, adminname, strlen(adminname), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, adminlevel);
    sqlite3_bind_text(stmt, 3, addedby, strlen(addedby), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, password, strlen(password), SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void truncateGameDbTable(char* tableName) {
    dbDeleteFromGameDbByRowId(va("DELETE FROM %s", tableName), -1);
}

void dbDeleteFromGameDbByRowId(char* query, int rowId) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rowId >= 0) {
        sqlite3_bind_int(stmt, 1, rowId);
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_exec(db, "VACUUM", 0, 0, 0);
}

void dbDeleteAdmin(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM adminlist WHERE ROWID = ?", rowId);
}

void dbDeletePassAdmin(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM adminpasslist WHERE ROWID = ?", rowId);
}

void dbGetAliases(char* ip) {

}

// only pass confirmed additions here.
void dbAddAlias(char* alias, char* ip) {
    char* query = "INSERT INTO aliases (alias, ip) VALUES (?, ?)";
}

void dbDeleteBan(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM banlist WHERE ROWID = ?", rowId);
}

void dbDeleteSubnetBan(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM subnetbanlist WHERE ROWID = ?", rowId);
}

void dbClearOutdatedBans() {
    dbDeleteFromGameDbByRowId("DELETE FROM banlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", -1);
    dbDeleteFromGameDbByRowId("DELETE FROM subnetbanlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", -1);
}

void dbAddBan(char* playername, char* ip, char* adminname, int endofmap, int days, int hours, int minutes) {


    char* query = "INSERT INTO banlist (playername, ip, adminname, banneduntil, endofmap) VALUES (?, ?, ?, ?, ?)";
}

void dbAddSubnetban(char* playername, char* ip, char* adminname, int endofmap, int days, int hours, int minutes) {
    char* query = "INSERT INTO subnetbanlist (playername, ip, adminname, banneduntil, endofmap) VALUES (?, ?, ?, ?, ?)";
}

void dbGetAdminlist(int maxAdminLevel) {
    char* query;
    if (maxAdminLevel < LEVEL_BADMIN) {
        query = "SELECT * FROM adminlist";
    } else {
        query = "SELECT * FROM adminlist WHERE adminlevel <= ?";
    }
}

