#include "../g_local.h"
#include "1fxFunctions.h"
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif


void loadDatabases(void) {
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
    sqlite3_exec(db, "SELECT sql FROM sqlite_master WHERE sql NOT NULL", &processTableStructure, gameDb, NULL);
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);

    sqlite3_close(db);

    Q_strncpyz(sqlTempName, "game", sizeof(sqlTempName));

    sqlite3_exec(gameDb, "ATTACH DATABASE './1fx/databases/game.db' as game", NULL, NULL, NULL);
    sqlite3_exec(gameDb, "BEGIN", NULL, NULL, NULL);
    sqlite3_exec(gameDb, "SELECT name FROM game.sqlite_master WHERE type='table'", &processTableData, gameDb, NULL);
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

    if (SQL_LOG_MIGRATION_LEVEL != logMigrationLevel) {
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
                "CREATE TABLE IF NOT EXISTS adminlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), toip VARCHAR(50), toname VARCHAR(64), action VARCHAR(100), reason VARCHAR(1024), adminlevel INTEGER, adminname VARCHAR(64), admintype INTEGER);"
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
int processTableStructure(void* pData, int nColumns,
    char** values, char** columns)
{
    sqlite3* db;

    if (nColumns != 1)
        return 1; // Error

    db = (sqlite3*)pData;
    sqlite3_exec(db, values[0], NULL, NULL, NULL);

    return 0;
}

int processTableData(void* pData, int nColumns,
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

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, action);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void dbAddAdmin(char* adminname, char* ip, int adminlevel, char* addedby) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    char* query = "INSERT INTO adminlist (adminname, ip, adminlevel, addedby) VALUES (?, ?, ?, ?)";
    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlBindTextOrNull(stmt, 1, adminname);
    sqlBindTextOrNull(stmt, 2, ip);
    sqlite3_bind_int(stmt, 3, adminlevel);
    sqlBindTextOrNull(stmt, 4, addedby);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}


void dbAddPassAdmin(char* adminname, int adminlevel, char* addedby, char* password) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    char* query = "INSERT INTO adminpasslist (adminname, adminlevel, addedby, password) VALUES (?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, adminname);
    sqlite3_bind_int(stmt, 2, adminlevel);
    sqlBindTextOrNull(stmt, 3, addedby);
    sqlBindTextOrNull(stmt, 4, password);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

qboolean dbTruncateGameDbTable(char* tableName) {

    if (!Q_stricmp(tableName, "adminlist")) {
        dbDeleteFromGameDbByRowId("DELETE FROM adminlist", -1);
        removeAdminsFromGame(ADMINTYPE_IP);
    } else if (!Q_stricmp(tableName, "adminpasslist")) {
        dbDeleteFromGameDbByRowId("DELETE FROM adminpasslist", -1);
        removeAdminsFromGame(ADMINTYPE_PASS);
    } else if (!Q_stricmp(tableName, "aliases")) {
        dbDeleteFromGameDbByRowId("DELETE FROM aliases", -1);
    } else if (!Q_stricmp(tableName, "banlist")) {
        dbDeleteFromGameDbByRowId("DELETE FROM banlist", -1);
    } else if (!Q_stricmp(tableName, "subnetbanlist")) {
        dbDeleteFromGameDbByRowId("DELETE FROM subnetbanlist", -1);
    } else {
        // yet again I do not want to printf from here, but it just makes sense to do it here
        // here I can also specify the tables I do have, otherwise jump around in different files when you add 1 more table.
        Com_Printf("^3Info:^7 Invalid choice '%s'. Tables you can clear: adminlist, adminpasslist, aliases, banlist, subnetbanlist.\n", tableName);
        return qfalse;
    }
    Com_Printf("^3Info:^7 Table %s has been successfully cleared.\n", tableName);
    return qtrue;
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

int dbGetAdminRowIdByGentity(gentity_t* removable) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    qboolean passlist = qfalse;
    int returnable = -1;

    db = gameDb;

    if (removable->client->sess.adminType == ADMINTYPE_PASS) {
        passlist = qtrue;
    }

    char* query = va("SELECT ROWID FROM admin%slist WHERE adminname = ? %s", passlist ? "pass" : "", passlist ? "" : "AND ip = ?");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, removable->client->sess.adminName);

    if (!passlist) {
        sqlBindTextOrNull(stmt, 2, removable->client->pers.ip);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        returnable = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return returnable;
}

void dbClearOldAliases(char* ip) {
    // for every alias call, we will check whether the alias list exceeds g_maxAliases.integer
    // if it does, by rowId, clean up the old rows.
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rows = 0, rc = 0;

    db = gameDb;

    char* query = "SELECT ROWID FROM aliases WHERE ip = ? ORDER BY ROWID DESC";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);

    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rows > g_maxAliases.integer) {
            dbDeleteFromGameDbByRowId("DELETE FROM aliases WHERE ROWID = ?", sqlite3_column_int(stmt, 1));
        }
        rows++;
    }

    if (rows > g_maxAliases.integer) {
        sqlite3_exec(db, "VACUUM", 0, 0, 0);
    }

}

void dbGetAliases(char* ip) {
    sqlite3* db;
}

// only pass confirmed additions here.
void dbAddAlias(char* alias, char* ip) {
    sqlite3* db;
    sqlite3_stmt* stmt;


    db = gameDb;

    char* query = "INSERT INTO aliases (alias, ip) VALUES (?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, alias);
    sqlBindTextOrNull(stmt, 2, ip);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    dbClearOldAliases(ip);
}

void dbDeleteBan(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM banlist WHERE ROWID = ?", rowId);
}

void dbDeleteSubnetBan(int rowId) {
    dbDeleteFromGameDbByRowId("DELETE FROM subnetbanlist WHERE ROWID = ?", rowId);
}

void dbClearOutdatedBans(void) {
    dbDeleteFromGameDbByRowId("DELETE FROM banlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", -1);
    dbDeleteFromGameDbByRowId("DELETE FROM subnetbanlist WHERE endofmap = 1 OR banneduntil <= datetime('now')", -1);
}

void dbAddBan(char* playername, char* ip, char* adminname, int endofmap, int days, int hours, int minutes) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    char banDurationString[MAX_STRING_CHARS];

    db = gameDb;

    if (endofmap) {
        strncpy(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", 365, 0, 0), sizeof(banDurationString));
    } else {
        strncpy(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", days, hours, minutes), sizeof(banDurationString));
    }

    char* query = va("INSERT INTO banlist (playername, ip, adminname, banneduntil, endofmap) VALUES (?, ?, ?, %s, ?)", banDurationString);

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, playername);
    sqlBindTextOrNull(stmt, 2, ip);
    sqlBindTextOrNull(stmt, 3, adminname);
    sqlite3_bind_int(stmt, 4, endofmap);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

}

void dbAddSubnetban(char* playername, char* ip, char* adminname, int endofmap, int days, int hours, int minutes) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    char banDurationString[MAX_STRING_CHARS];

    db = gameDb;

    if (endofmap) {
        strncpy(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", 365, 0, 0), sizeof(banDurationString));
    } else {
        strncpy(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", days, hours, minutes), sizeof(banDurationString));
    }

    char* query = va("INSERT INTO subnetbanlist (playername, ip, adminname, banneduntil, endofmap) VALUES (?, ?, ?, %s, ?)", banDurationString);

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, playername);
    sqlBindTextOrNull(stmt, 2, ip);
    sqlBindTextOrNull(stmt, 3, adminname);
    sqlite3_bind_int(stmt, 4, endofmap);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

}

// whilst I would prefer that this function gives you an admin list and another one does the printing,
// it simply does not make sense to do so in C considering that the admin list can quite often be large.
// plus the fact that I would be looping over the contents anyhow to print them, I would just end up creating more of an headache
void dbGetAdminlist(gentity_t* ent, qboolean passlist) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;
    char buf[MAX_PACKET_BUF];

    memset(buf, 0, sizeof(buf));

    db = gameDb;

    char* query = va("SELECT ROWID, adminlevel, %s, adminname, addedby FROM admin%slist", passlist ? "''" : "ip", passlist ? "pass" : "");
    if (ent && ent->client) {
        Q_strcat(buf, sizeof(buf), va("^3 %-6s%-5s%-16s%-22sBy\n^7------------------------------------------------------------------------\n", "#", "Lvl", "IP", "Name"));
    } else {
        Com_Printf("^3 %-6s%-5s%-16s%-22sBy\n", "#", "Lvl", "IP", "Name");
        Com_Printf("^7------------------------------------------------------------------------\n");
    }

    if (sqlite3_prepare(db, query, -1, &stmt, 0) == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
            if (rc == SQLITE_ROW) {
                if(ent && ent->client){
                    // Boe!Man 11/04/11: Put packet through to clients if char size would exceed 1000 and reset buf2.
                    if((strlen(buf) +
                        strlen(
                                va("[^3%-3.3i^7]  [^3%i^7]  %-15.15s %-21.21s %-21.21s\n",
                                   sqlite3_column_int(stmt, 0), // ROWID
                                   sqlite3_column_int(stmt, 1), // adminlevel
                                   sqlite3_column_text(stmt, 2), // ip
                                   sqlite3_column_text(stmt, 3), // adminname
                                   sqlite3_column_text(stmt, 4) // addedby
                                   )
                               )
                       ) > MAX_PACKET_BUF){
                        trap_SendServerCommand( ent-g_entities, va("print \"%s\"", buf));
                        memset(buf, 0, sizeof(buf)); // Boe!Man 11/04/11: Properly empty the buffer.
                    }
                    Q_strcat(buf, sizeof(buf), va("[^3%-3.3i^7]  [^3%i^7]  %-15.15s %-21.21s %-21.21s\n",
                                                  sqlite3_column_int(stmt, 0),
                                                  sqlite3_column_int(stmt, 1),
                                                  sqlite3_column_text(stmt, 2),
                                                  sqlite3_column_text(stmt, 3),
                                                  sqlite3_column_text(stmt, 4)
                                                )
                    );
                }else{
                    Com_Printf("[^3%-3.3i^7%-3s[^3%i^7%-3s%-15.15s %-21.21s %-21.21s\n", sqlite3_column_int(stmt, 0), "]", sqlite3_column_int(stmt, 1), "]", sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3), sqlite3_column_text(stmt, 4));
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    // Boe!Man 11/04/11: Fix for RCON not properly showing footer of banlist.
    if(ent && ent->client){
        trap_SendServerCommand( ent-g_entities, va("print \"%s\nUse ^3[Page Up] ^7and ^3[Page Down] ^7keys to scroll\n\n\"", buf)); // Boe!Man 11/04/11: Also send the last buf2 (that wasn't filled as a whole yet).
    } else {
        Com_Printf("\nUse ^3[Page Up] ^7and ^3[Page Down] ^7keys to scroll\n\n");
    }
}

void unloadInMemoryDatabases(void) {
    backupInMemoryDatabases("game.db", gameDb);
    sqlite3_exec(gameDb, "DETACH DATABASE game", NULL, NULL, NULL);
    sqlite3_close(gameDb);
}

void backupInMemoryDatabases(char* dbName, sqlite3* db) {
    sqlite3_backup  *pBackup;    // Boe!Man 5/27/13: Backup handle used to copy data.
    sqlite3         *pFile;
    int             rc;

    rc = sqlite3_open(va("./1fx/databases/%s", dbName), &pFile);

    if (rc){
        logSystem(LOGLEVEL_ERROR, va("Critical error backing up in-memory database %s: %s\n", dbName, sqlite3_errmsg(pFile)));
        return;
    }

    // Boe!Man 7/1/13: Fixed the backup being very slow when databases are relatively full, can especially be noted when restarting the map.
    sqlite3_exec(pFile, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(pFile, "BEGIN TRANSACTION", NULL, NULL, NULL);

    pBackup = sqlite3_backup_init(pFile, "main", db, "main");

    if(pBackup){
        sqlite3_backup_step(pBackup, -1);
        // Boe!Man 5/27/13: Release resources allocated by backup_init().
        sqlite3_backup_finish(pBackup);
    }

    rc = sqlite3_errcode(pFile);
    if(rc){
        logSystem(LOGLEVEL_ERROR, va("SQLite3 error while backing up data in %s: %s\n", dbName, sqlite3_errmsg(pFile)));
    }

    sqlite3_exec(pFile, "COMMIT", NULL, NULL, NULL);
    sqlite3_close(pFile);
}

void dbLogAdmin(char* byip, char* byname, char* toip, char* toname, char* action, char* reason, int adminlevel, char* adminname, int admintype) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc;

    char* query = "INSERT INTO adminlog (byip, byname, toip, toname, action, reason, adminlevel, adminname, admintype) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        Com_Printf("Failed to open logs.db file to log admin!\nerr: %s\n", sqlite3_errmsg(db));
        logSystem(LOGLEVEL_WARNING, "Failed to open logs.db file to log admin!\n");
        return;
    }

    sqlite3_prepare(db, query, -1, &stmt, 0);
    
    sqlBindTextOrNull(stmt, 1, byip);
    sqlBindTextOrNull(stmt, 2, byname);
    sqlBindTextOrNull(stmt, 3, toip);
    sqlBindTextOrNull(stmt, 4, toname);
    sqlBindTextOrNull(stmt, 5, action);
    sqlBindTextOrNull(stmt, 6, reason);
    sqlite3_bind_int(stmt, 7, adminlevel);
    sqlBindTextOrNull(stmt, 8, adminname);
    sqlite3_bind_int(stmt, 9, admintype);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void sqlBindTextOrNull(sqlite3_stmt* stmt, int argnum, char* text) {

    if (text) {
        sqlite3_bind_text(stmt, argnum, text, strlen(text), SQLITE_STATIC);
    }
    else {
        sqlite3_bind_null(stmt, argnum);
    }

}

void dbLogLogin(char* player, char* ip, int level, int method) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc;

    char* query = "INSERT INTO loginlog (byip, byname, adminlevel, admintype) VALUES (?, ?, ?, ?)";
    rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        Com_Printf("Failed to open logs.db file to log rcon!\nerr: %s\n", sqlite3_errmsg(db));
        logSystem(LOGLEVEL_WARNING, "Failed to open logs.db file to log login!\n");
        return;
    }

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, player);
    sqlite3_bind_int(stmt, 3, level);
    sqlite3_bind_int(stmt, 4, method);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

qboolean dbIsIpNameInAdminList(qboolean passlist, char* ip, char* name) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    qboolean returnable = qfalse;

    db = gameDb;

    char* query = va("SELECT ROWID, adminlevel, %s, adminname, addedby FROM admin%slist WHERE adminname = ? %s", passlist ? "''" : "ip", passlist ? "pass" : "", passlist ? "" : "AND ip = ?");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, name);

    if (!passlist) {
        sqlBindTextOrNull(stmt, 2, ip);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        returnable = qtrue;
    }

    sqlite3_finalize(stmt);

    return returnable;

}

int dbGetPlayerAdminLevel(qboolean passlist, char* ip, char* name, char* password) {
    // password is filled and ip is not filled if passlist = qtrue
    // ip is filled and password is not filled if passlist = qfalse
    sqlite3* db;
    sqlite3_stmt* stmt;
    int returnable = -1;

    db = gameDb;

    char* query = va("SELECT adminlevel, %s FROM admin%slist WHERE adminname = ? AND %s = ?", passlist ? "password" : "ip", passlist ? "pass" : "", passlist ? "password" : "ip");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, name);

    if (passlist) {
        sqlBindTextOrNull(stmt, 2, password);
    }
    else {
        sqlBindTextOrNull(stmt, 2, ip);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // SQLite has already taken care of the checks, so just get the adminlevel back.
        returnable = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return returnable;

}

void dbUpdatePassAdmin(char* adminname, char* newpass) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    db = gameDb;

    char* query = "UPDATE adminpasslist SET password = ? WHERE adminname = ?";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, newpass);
    sqlBindTextOrNull(stmt, 2, adminname);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

}

qboolean dbDoesRowIDExist(char* table, int rowid) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    qboolean returnable = qfalse;

    db = gameDb;

    char* query = va("SELECT ROWID FROM %s WHERE ROWID = ?", table);

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_int(stmt, 1, rowid);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        returnable = qtrue;
    }

    sqlite3_finalize(stmt);

    return returnable;

}

int dbGetAdminByRowId(qboolean password, int rowid, char* adminOut, char* ipOut) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    int adminLevel = 0;

    db = gameDb;

    char* query = va("SELECT adminname, adminlevel, %s FROM admin%slist WHERE ROWID = ?", password ? "''" : "ip", password ? "pass" : "");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_int(stmt, 1, rowid);

    if (sqlite3_step(stmt) == SQLITE_ROW) {

        Q_strncpyz(adminOut, sqlite3_column_text(stmt, 0), sizeof(adminOut));
        adminLevel = sqlite3_column_int(stmt, 1);

        if (!password) {
            Q_strncpyz(ipOut, sqlite3_column_text(stmt, 2), sizeof(ipOut));
        }


    }
    
    return adminLevel;

}
