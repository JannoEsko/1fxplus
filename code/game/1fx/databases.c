
#include "../../ext/sqlite/sqlite3.h"
#include "../g_local.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <direct.h>
#include <unistd.h>
#endif

char sqlTempName[MAX_SQL_TEMP_NAME];
sqlite3* gameDb;
sqlite3* logsDb;

static void sqlBindTextOrNull(sqlite3_stmt* stmt, int argnum, char* text) {

    if (text) {
        sqlite3_bind_text(stmt, argnum, text, strlen(text), SQLITE_STATIC);
    }
    else {
        sqlite3_bind_null(stmt, argnum);
    }

}

static void sqlBindTextOrDefault(sqlite3_stmt* stmt, int argnum, char* text, char* def) {

    if (!def) {
        sqlBindTextOrNull(stmt, argnum, text);
    }
    else {
        if (text) {
            sqlite3_bind_text(stmt, argnum, text, strlen(text), SQLITE_STATIC);
        }
        else {
            sqlite3_bind_text(stmt, argnum, def, strlen(def), SQLITE_STATIC);
        }
    }

}


// Boe!Man 5/27/13: Misc. SQLite functions.
static int processTableStructure(void* pData, int nColumns,
    char** values, char** columns)
{
    sqlite3* db;

    if (nColumns != 1)
        return 1; // Error

    db = (sqlite3*)pData;
    sqlite3_exec(db, values[0], NULL, NULL, NULL);

    return 0;
}

static int processTableData(void* pData, int nColumns,
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

static void migrateGameDatabase(sqlite3* db, int gameMigrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically 
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.



    if (gameMigrationLevel < 1) {
        // fresh database, create all tables.
        char* migration = "CREATE TABLE IF NOT EXISTS adminlist (adminname VARCHAR(64) COLLATE NOCASE, ip VARCHAR(24), adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP);"
            "CREATE TABLE IF NOT EXISTS banlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), reason VARCHAR(512), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0, adminlevel INTEGER);"
            "CREATE TABLE IF NOT EXISTS subnetbanlist (playername VARCHAR(64), ip VARCHAR(24), adminname VARCHAR(64), reason VARCHAR(512), bannedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, banneduntil TIMESTAMP, endofmap INTEGER DEFAULT 0, adminlevel INTEGER);"
            "CREATE TABLE IF NOT EXISTS adminpasslist (adminname VARCHAR(64) COLLATE NOCASE, adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, password VARCHAR(80));"
            "CREATE TABLE IF NOT EXISTS adminguidlist (adminname VARCHAR(64) COLLATE NOCASE, adminlevel INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, acguid VARCHAR(20));"
            "CREATE TABLE IF NOT EXISTS aliases (alias VARCHAR(64), ip VARCHAR(24));"
            "CREATE INDEX IF NOT EXISTS idx_aliases_ip ON aliases (ip);"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (1);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }


}

static void migrateLogsDatabase(sqlite3* db, int logsMigrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.

    if (logsMigrationLevel < 1) {
        // fresh database, create all tables.
        char* migration = "CREATE TABLE IF NOT EXISTS rconlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, ip VARCHAR(50), action VARCHAR(1024));" // ip varchar50 because of IPv6, action 1024 because of a theoretical maximum of the rcon cvar.
            "CREATE TABLE IF NOT EXISTS adminlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), toip VARCHAR(50), toname VARCHAR(64), action VARCHAR(100), reason VARCHAR(1024), adminlevel INTEGER, adminname VARCHAR(64), admintype INTEGER);"
            "CREATE TABLE IF NOT EXISTS loginlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), adminlevel INTEGER, admintype INTEGER);"
            "CREATE TABLE IF NOT EXISTS gameslog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, byip VARCHAR(50), byname VARCHAR(64), toip VARCHAR(50), toname VARCHAR(64), action VARCHAR(1024));"
            "CREATE TABLE IF NOT EXISTS systemlog (dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP, loglevel INTEGER, logmsg VARCHAR(512));"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (1);"
            ;

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the logs database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", logsMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

}
/*
static void migrateCountryDatabase(sqlite3* db, int countryMigrationLevel) {

    // whenever a database structure changes due to updates to the mod, we will note down a new if clause here which basically
    // lets the mod migrate the database to the correct structure.
    // if (migrationlevel < 1) then do this
    // if (migrationlevel < 2) then do this
    // .. and so on. Downside is that this will run a lot of queries to catch up, but it's a one-off call, therefore not expensive.

    if (countryMigrationLevel < 1) {
        // fresh database, create all tables.
        char* migration = "CREATE TABLE IF NOT EXISTS ip2country (ip VARCHAR(50), countrycode VARCHAR(10), country VARCHAR(50), blocklevel INTEGER, added TIMESTAMP DEFAULT CURRENT_TIMESTAMP);" // ip varchar50 because of IPv6
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (1);"
            ;

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {

            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the country database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", countryMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

}
*/

char* getIpOrArg(gentity_t* ent, char* arg) {
    return ent && ent->client ? ent->client->pers.ip : arg;
}

admLevel_t getAdminLevel(gentity_t* ent) {
    return ent && ent->client ? ent->client->sess.adminLevel : ADMLVL_NONE;
}

char* getAdminName(gentity_t* ent) {
    return ent && ent->client ? ent->client->sess.adminName : "";
}

admType_t getAdminType(gentity_t* ent) {
    return ent && ent->client ? ent->client->sess.adminType : ADMTYPE_NONE;
}

char* getNameOrArg(gentity_t* ent, char* arg, qboolean cleanName) {

	if (cleanName) {
		return ent && ent->client ? ent->client->pers.cleanName : arg;
	}
	else {
		return ent && ent->client ? ent->client->pers.netname : arg;
	}
}

void loadDatabases(void) {
    Com_PrintInfo("loadDatabases()\n");
	struct stat st = { 0 };
	sqlite3* db;

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

	int rc = sqlite3_open("./1fx/databases/game.db", &db);

	if (rc) {
		logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write game.db file in databases folder.\n");
		return;
	}

	// check for the migration level, use create table if not exists to create migration table.

	if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        sqlite3_close(db);
		logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write migrationlevel table into game.db.\n");
		return;
	}

	sqlite3_stmt* stmt;
	int gameMigrationLevel = -1;

	sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		gameMigrationLevel = sqlite3_column_int(stmt, 0);
	}

	sqlite3_finalize(stmt);
	if (SQL_GAME_MIGRATION_LEVEL != gameMigrationLevel) {
		Com_Printf("    Migrating gameDb to be migrated from level %d to level %d.\n", gameMigrationLevel, SQL_GAME_MIGRATION_LEVEL);
		migrateGameDatabase(db, gameMigrationLevel);
	}
	else {
		Com_Printf("    gameDb is up to date!\n");
	}

    Com_Printf("    Attaching gameDB to memory...");
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
    sqlite3_exec(gameDb, "DELETE FROM adminpasslist WHERE password IS NULL", 0, 0, 0);
    sqlite3_exec(gameDb, "VACUUM", NULL, NULL, NULL);

    rc = sqlite3_open("./1fx/databases/logs.db", &db);

    if (rc) {
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write game.db file in databases folder.\n");
        return;
    }

    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        sqlite3_close(db);
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write migrationlevel table into game.db.\n");
        return;
    }

    sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    int logMigrationLevel = -1;

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logMigrationLevel = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (SQL_LOG_MIGRATION_LEVEL != logMigrationLevel) {
        Com_Printf("    Migrating logsDb from level %d to level %d.\n", logMigrationLevel, SQL_LOG_MIGRATION_LEVEL);
        migrateLogsDatabase(db, logMigrationLevel);
    }
    else {
        Com_Printf("    logsDb is up to date!\n");
    }

    sqlite3_close(db); // logs DB can get too large to run just in memory, therefore we're opening / closing it every time when it's needed.
    // can have a performance implication, but haven't seen any lag due to it in 3D which already runs the same logic.
    // FIXME if server gets weird lagouts every second or so, then look here.
    /*
    rc = sqlite3_open("./1fx/databases/country.db", &db);

    if (rc) {
        // with the gameDb completely failing and we cannot write to it, there's no point continuing.
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write country.db file in databases folder.\n");
        return;
    }

    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        sqlite3_close(db);
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write migrationlevel table into country.db.\n");
        return;
    }

    sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    int countryMigrationLevel = -1;

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        countryMigrationLevel = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (SQL_COUNTRY_MIGRATION_LEVEL != countryMigrationLevel) {
        Com_Printf("    Migrating countryDb from level %d to level %d.\n", countryMigrationLevel, SQL_COUNTRY_MIGRATION_LEVEL);
        migrateCountryDatabase(db, countryMigrationLevel);
    }
    else {
        Com_Printf("    countryDb is up to date!\n");
    }

    sqlite3_close(db);
    */
    Com_PrintInfo("Database checks done!\n");
}


void unloadInMemoryDatabases(void) {
    backupInMemoryDatabases();
    sqlite3_exec(gameDb, "DETACH DATABASE game", NULL, NULL, NULL);
    sqlite3_close(gameDb);
}

void backupInMemoryDatabases(void) {
    sqlite3_backup* pBackup;    // Boe!Man 5/27/13: Backup handle used to copy data.
    sqlite3* pFile;
    int             rc;

    rc = sqlite3_open("./1fx/databases/game.db", &pFile);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Opening in-memory database game.db for backups failed. Code: %d, message: %s\n", rc, sqlite3_errmsg(pFile));
        return;
    }

    // Boe!Man 7/1/13: Fixed the backup being very slow when databases are relatively full, can especially be noted when restarting the map.
    sqlite3_exec(pFile, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(pFile, "BEGIN TRANSACTION", NULL, NULL, NULL);

    pBackup = sqlite3_backup_init(pFile, "main", gameDb, "main");

    if (pBackup) {
        sqlite3_backup_step(pBackup, -1);
        // Boe!Man 5/27/13: Release resources allocated by backup_init().
        sqlite3_backup_finish(pBackup);
    }

    rc = sqlite3_errcode(pFile);
    if (rc) {
        logSystem(LOGLEVEL_WARN, "Backing up in-memory databases failed with error code %d. Message: %s\n", rc, sqlite3_errmsg(pFile));
    }

    sqlite3_exec(pFile, "COMMIT", NULL, NULL, NULL);
    sqlite3_close(pFile);

    level.nextSQLBackupTime = level.time + 50000;
}

/*
======================
This function gets back the admin level for the respective client.
On NULL password, the password table can still be queried, expectation 
is that this is just for a check whether admin exists.
======================
*/
int dbGetAdminLevel(admType_t adminType, gentity_t* ent, char* passguid) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    int returnable = -1;

    char* query = va("SELECT ROWID, adminlevel FROM admin%slist WHERE adminname = ? %s", 
        adminType == ADMTYPE_PASS ? "pass" : 
            (adminType == ADMTYPE_GUID ? "guid" : ""), 
        adminType == ADMTYPE_IP ? "AND ip = ?" : 
            (adminType == ADMTYPE_PASS && passguid && strlen(passguid) > 0 ? "AND password = ?" : 
                (adminType == ADMTYPE_GUID && passguid && strlen(passguid) > 0 ? "AND guid = ?" : "")
            )
    
    );

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);

    if (adminType == ADMTYPE_GUID) {
        sqlBindTextOrNull(stmt, 2, ent->client->sess.roxGuid);
    }
    else if (adminType == ADMTYPE_IP) {
        sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);
    }
    else if ((adminType == ADMTYPE_PASS || adminType == ADMTYPE_GUID) && passguid && strlen(passguid)) {
        sqlBindTextOrNull(stmt, 2, passguid);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        returnable = sqlite3_column_int(stmt, 1);
    }

    sqlite3_finalize(stmt);

    return returnable;
}

static void dbAddIpAdmin(admLevel_t adminLevel, gentity_t* ent, gentity_t* adm) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("INSERT INTO adminlist (adminname, ip, adminlevel, addedby) VALUES (?, ?, ?, ?)");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);
    sqlite3_bind_int(stmt, 3, adminLevel);
    sqlBindTextOrNull(stmt, 4, getNameOrArg(adm, "RCON", qtrue));

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}


static void dbAddPassAdmin(admLevel_t adminLevel, gentity_t* ent, gentity_t* adm, char* pass) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("INSERT INTO adminpasslist (adminname, adminlevel, addedby, password) VALUES (?, ?, ?, ?)");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlite3_bind_int(stmt, 2, adminLevel);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));
    
    sqlBindTextOrNull(stmt, 4, pass);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

}


static void dbAddGuidAdmin(admLevel_t adminLevel, gentity_t* ent, gentity_t* adm) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("INSERT INTO adminguidlist (adminname, adminlevel, addedby, acguid) VALUES (?, ?, ?, ?)");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlite3_bind_int(stmt, 2, adminLevel);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));

    sqlBindTextOrNull(stmt, 4, ent->client->sess.roxGuid);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

}

void dbAddAdmin(admType_t adminType, admLevel_t adminLevel, gentity_t* ent, gentity_t* adm, char* password) {

    switch (adminType) {
    case ADMTYPE_GUID:
        dbAddGuidAdmin(adminLevel, ent, adm);
        break;
    case ADMTYPE_IP:
        dbAddIpAdmin(adminLevel, ent, adm);
        break;
    case ADMTYPE_PASS:
        dbAddPassAdmin(adminLevel, ent, adm, password);
        break;
    }

}

qboolean dbGetAdminDataByRowId(admType_t adminType, int rowId, int* adminLevel, char* adminName, int adminNameLength) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    qboolean success = qfalse;

    char* query = va("SELECT adminname, adminlevel FROM admin%slist WHERE ROWID = ?", adminType == ADMTYPE_GUID ? "guid" : (adminType == ADMTYPE_PASS ? "pass" : ""));

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return qfalse;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Q_strncpyz(adminName, sqlite3_column_text(stmt, 0), adminNameLength);
        *adminLevel = sqlite3_column_int(stmt, 1);
        success = qtrue;
    }

    sqlite3_finalize(stmt);

    return success;
}

int dbRemoveAdminByRowId(admType_t adminType, int rowId) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    char* query = va("DELETE FROM admin%slist WHERE ROWID = ?", adminType == ADMTYPE_GUID ? "guid" : (adminType == ADMTYPE_PASS ? "pass" : ""));

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    rowsAffected = sqlite3_changes(db);

    sqlite3_exec(db, "VACUUM", NULL, NULL, NULL);

    return rowsAffected;
}

int dbUpdateAdminPass(char* adminName, char* password) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    char* query = "UPDATE adminpasslist SET password = ? WHERE adminname = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, password);
    sqlBindTextOrNull(stmt, 2, adminName);

    sqlite3_step(stmt);

    rowsAffected = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    return rowsAffected;

}

void dbPrintAdminlist(gentity_t* ent, admType_t adminType) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char buf[MAX_PACKET_BUF];
    qboolean isRcon = ent && ent->client ? qfalse : qtrue;

    Com_Memset(buf, 0, sizeof(buf));
    char* query = va("SELECT ROWID, adminname, adminlevel, addedby, DATE(addedwhen)%s FROM admin%slist", adminType == ADMTYPE_IP ? ", ip" : (adminType == ADMTYPE_GUID ? ", acguid" : ""), adminType == ADMTYPE_PASS ? "pass" : (adminType == ADMTYPE_GUID ? "guid" : ""));
    
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    if (isRcon) {
        Com_Printf("\n^3 %-5s%-5s%-16s %-16s %-16s Date\n", "#", "Lvl", "IP (GUID)", "Name", "By");
        Com_Printf("^7-----------------------------------------------------------------------------\n");
    }
    else {
        Q_strcat(buf, sizeof(buf), va("^3 %-5s%-5s%-16s %-16s %-16s Date\n^7-----------------------------------------------------------------------------\n", "#", "Lvl", "IP (GUID)", "Name", "By"));
    }
    int i = 0;
    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            int rowId = sqlite3_column_int(stmt, 0);
            char* adminName = sqlite3_column_text(stmt, 1);
            int adminLevel = sqlite3_column_int(stmt, 2);
            char* addedBy = sqlite3_column_text(stmt, 3);
            char* addedWhen = sqlite3_column_text(stmt, 4);
            char* guidIp = "";
            if (adminType == ADMTYPE_GUID || adminType == ADMTYPE_IP) {
                guidIp = sqlite3_column_text(stmt, 5);
            }

            if (isRcon) {
                Com_Printf("[^3%-3.3d^7] %-5d%-16.16s %-16.16s %-16.16s %s\n", rowId, adminLevel, guidIp, adminName, addedBy, addedWhen);
            }
            else {

                if (strlen(buf) + strlen(va("[^3%-3.3d^7] %-5d%-16.16s %-16.16s %-16.16s %s\n", rowId, adminLevel, guidIp, adminName, addedBy, addedWhen)) >= sizeof(buf) - 5) {

                    trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
                    i++;
                    Com_Memset(buf, 0, sizeof(buf));

                    if (i == 20) break; // Above this, clients can start crashing, too much printing...
                }

                Q_strcat(buf, sizeof(buf), va("[^3%-3.3d^7] %-5d%-16.16s %-16.16s %-16.16s %s\n", rowId, adminLevel, guidIp, adminName, addedBy, addedWhen));
            }
        }
    }

    if (!isRcon) {

        trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
        if (i == 20) {
            trap_SendServerCommand(ent - g_entities, "print \"... rest truncated\"");
        }
        trap_SendServerCommand(ent - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n\"");
    }
    else {
        Com_Printf("\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n");
    }

    sqlite3_finalize(stmt);

}

void dbRunTruncate(char* table) {

    sqlite3* db = gameDb;

    if (!Q_stricmp("all", table)) {
        sqlite3_exec(db, "DELETE FROM adminguidlist", NULL, NULL, NULL);
        sqlite3_exec(db, "DELETE FROM adminlist", NULL, NULL, NULL);
        sqlite3_exec(db, "DELETE FROM adminpasslist", NULL, NULL, NULL);
        sqlite3_exec(db, "DELETE FROM aliases", NULL, NULL, NULL);
        sqlite3_exec(db, "DELETE FROM banlist", NULL, NULL, NULL);
        sqlite3_exec(db, "DELETE FROM subnetbanlist", NULL, NULL, NULL);

        logSystem(LOGLEVEL_INFO, "All tables truncated.\n");
        backupInMemoryDatabases();
        return;
    }
    else if (!Q_stricmp("adminguidlist", table)) {
        sqlite3_exec(db, "DELETE FROM adminguidlist", NULL, NULL, NULL);
    }
    else if (!Q_stricmp("adminlist", table)) {
        sqlite3_exec(db, "DELETE FROM adminlist", NULL, NULL, NULL);
    }
    else if (!Q_stricmp("adminpasslist", table)) {
        sqlite3_exec(db, "DELETE FROM adminpasslist", NULL, NULL, NULL);
    }
    else if (!Q_stricmp("aliases", table)) {
        sqlite3_exec(db, "DELETE FROM aliases", NULL, NULL, NULL);
    }
    else if (!Q_stricmp("banlist", table)) {
        sqlite3_exec(db, "DELETE FROM banlist", NULL, NULL, NULL);
    }
    else if (!Q_stricmp("subnetbanlist", table)) {
        sqlite3_exec(db, "DELETE FROM subnetbanlist", NULL, NULL, NULL);
    }
    else {
        logSystem(LOGLEVEL_INFO, "No table \"%s\" found.\n", table);
        return;
    }
    backupInMemoryDatabases();
    logSystem(LOGLEVEL_INFO, "Table \"%s\" truncated.\n", table);

}

void dbGetAliases(gentity_t* ent, char* output, int outputSize, char* separator) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    qboolean first = qtrue;

    if (!g_maxAliases.integer) {
        return;
    }

    Com_Memset(output, 0, outputSize);

    char* query = va("SELECT alias FROM aliases WHERE alias != ? AND ip = ?");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);

    int rc = 0;
    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            char* alias = sqlite3_column_text(stmt, 0);

            if (!alias) continue;

            if (strlen(alias) + 1 + strlen(output) > outputSize) {
                logSystem(LOGLEVEL_WARN, "Alias buffer overflow.\n");
                break;
            }

            if (!first) {
                Q_strcat(output, outputSize, separator);
            }
            first = qfalse;
            Q_strcat(output, outputSize, alias);
            
        }
    }

    sqlite3_finalize(stmt);

}

void dbAddAlias(gentity_t* ent) {
    
    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    if (!g_maxAliases.integer) {
        return;
    }

    char* query = va("INSERT INTO aliases (alias, ip) VALUES (?, ?)");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    dbClearOldAliases(ent);
}

void dbClearOldAliases(gentity_t* ent) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("DELETE FROM aliases WHERE ROWID IN (SELECT ROWID FROM aliases WHERE ip = ? ORDER BY ROWID ASC LIMIT (SELECT COUNT(*) - ? FROM aliases WHERE ip = ?))");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.ip);
    sqlite3_bind_int(stmt, 2, g_maxAliases.integer);
    sqlBindTextOrNull(stmt, 3, ent->client->pers.ip);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

void dbAddBan(gentity_t* ent, gentity_t* adm, char* reason, qboolean subnet, qboolean endofmap, int days, int hours, int minutes) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char banDurationString[MAX_STRING_CHARS];

    if (endofmap) {
        Q_strncpyz(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", 365, 0, 0), sizeof(banDurationString));
    }
    else {
        Q_strncpyz(banDurationString, va("datetime('now', '+%d day', '+%d hour', '+%d minute')", days, hours, minutes), sizeof(banDurationString));
    }

    char* query = va("INSERT INTO %sbanlist (playername, ip, adminname, reason, banneduntil, endofmap) VALUES (?, ?, ?, ?, %s, ?)", subnet ? "subnet" : "", banDurationString);
    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, subnet ? ent->client->pers.ip : ent->client->pers.subnet);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));
    sqlBindTextOrNull(stmt, 4, reason);
    sqlite3_bind_int(stmt, 5, endofmap);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

}

int dbRemoveBan(qboolean subnet, int rowId) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    int rowsAffected = 0;

    char* query = va("DELETE FROM %sbanlist WHERE ROWID = ?", subnet ? "subnet" : "");
    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_int(stmt, 1, rowId);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    rowsAffected = sqlite3_changes(db);

    sqlite3_exec(db, "VACUUM", NULL, NULL, NULL);

    return rowsAffected;
}

void dbLogAdmin(char* byIp, char* byName, char* toIp, char* toName, char* action, char* reason, admLevel_t adminLevel, char* adminName, admType_t adminType) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to log admin. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO adminlog (byip, byname, toip, toname, action, reason, adminlevel, adminname, admintype) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlBindTextOrNull(stmt, 3, toIp);
    sqlBindTextOrNull(stmt, 4, toName);
    sqlBindTextOrNull(stmt, 5, action);
    sqlBindTextOrNull(stmt, 6, reason);
    sqlite3_bind_int(stmt, 7, adminLevel);
    sqlBindTextOrNull(stmt, 8, adminName);
    sqlite3_bind_int(stmt, 9, adminType);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void dbLogGame(char* byIp, char* byName, char* toIp, char* toName, char* action) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    if (!g_dbLogRetention.integer) {
        return; // Not a good idea to have NO retention, but it's up to the server owners in the end.
    }

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to log game. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO gameslog (byip, byname, toip, toname, action) VALUES (?, ?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlBindTextOrNull(stmt, 3, toIp);
    sqlBindTextOrNull(stmt, 4, toName);
    sqlBindTextOrNull(stmt, 5, action);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void dbLogLogin(char* byIp, char* byName, admLevel_t adminLevel, admType_t adminType) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    if (!g_dbLogRetention.integer) {
        return; // Not a good idea to have NO retention, but it's up to the server owners in the end.
    }

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to log login. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO loginlog (byip, byname, adminlevel, admintype) VALUES (?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlite3_bind_int(stmt, 3, adminLevel);
    sqlite3_bind_int(stmt, 4, adminType);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void dbLogRcon(char* ip, char* action) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    if (!g_dbLogRetention.integer) {
        return; // Not a good idea to have NO retention, but it's up to the server owners in the end.
    }

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to log RCON. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO rconlog (ip, action) VALUES (?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, action);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void dbLogSystem(loggingLevel_t logLevel, char* msg) {

    if (logLevel == LOGLEVEL_FATAL_DB) return;

    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to log system. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO systemlog (loglevel, logmsg) VALUES (?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlite3_bind_int(stmt, 1, logLevel);
    sqlBindTextOrNull(stmt, 2, msg);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);

}

void dbLogRetention() {

    sqlite3* db;
    sqlite3_stmt* stmt;

    if (!g_dbLogRetention.integer) {
        return; // Not a good idea to have NO retention, but it's up to the server owners in the end.
    }

    int rc = sqlite3_open_v2("./1fx/databases/logs.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open logs.db file to run retention. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "DELETE FROM adminlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    query = "DELETE FROM gameslog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    query = "DELETE FROM loginlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    query = "DELETE FROM rconlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    query = "DELETE FROM systemlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    sqlite3_prepare(db, query, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);


    sqlite3_close(db);

}

static qboolean dbQueryBan(char* ip, qboolean subnet, char* reason, int reasonSize, int* endOfMap, int* banEnd) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    qboolean success = qfalse;

    char* query = va("SELECT reason, endofmap, ROUND((JULIANDAY(till) - JULIANDAY()) * 1440) AS timeleft FROM %sbanlist WHERE ip = ?", subnet ? "subnet" : "");

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        success = qtrue;

        Q_strncpyz(reason, sqlite3_column_text(stmt, 0), reasonSize);
        *endOfMap = sqlite3_column_int(stmt, 1);
        *banEnd = sqlite3_column_int(stmt, 2);
    }

    sqlite3_finalize(stmt);

    return success;
}

qboolean dbCheckBan(gentity_t* ent, char* reason, int reasonSize, int* endOfMap, int* banEnd) {

    // Try IP first.
    qboolean banned = dbQueryBan(ent->client->pers.ip, qfalse, reason, reasonSize, endOfMap, banEnd);

    if (!banned) {
        // subnetban
        banned = dbQueryBan(ent->client->pers.subnet, qtrue, reason, reasonSize, endOfMap, banEnd);
    }

    return banned;
}

/*
qboolean dbGetCountry(char* ip, char* countryCode, int countryCodeSize, char* country, int countrySize, int* blocklevel) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    qboolean success = qfalse;

    if (!g_useCountryDb.integer) {
        return; 
    }

    int rc = sqlite3_open_v2("./1fx/databases/country.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open country.db file to add a new country. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    // First, clean out aged country rows.
    char* query;

    if (g_countryAging.integer) {
        query = "DELETE FROM ip2country WHERE added < DATETIME('now', '-' || ? || ' days')";

        sqlite3_prepare(db, query, -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, g_countryAging.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    query = "SELECT countrycode, country, blocklevel FROM ip2country WHERE ip = ?";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Q_strncpyz(countryCode, sqlite3_column_text(stmt, 0), countryCodeSize);
        Q_strncpyz(country, sqlite3_column_text(stmt, 1), countrySize);
        *blocklevel = sqlite3_column_int(stmt, 2);
        success = qtrue;
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);


    return success;
}

void dbAddCountry(char* ip, char* countryCode, char* country, int blocklevel) {

    sqlite3* db;
    sqlite3_stmt* stmt;

    if (!g_useCountryDb.integer || !g_useIPHubAPI.integer) {
        return; 
    }

    int rc = sqlite3_open_v2("./1fx/databases/country.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open country.db file to add a new country. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO ip2country (ip, countrycode, country, blocklevel) VALUES (?, ?, ?, ?)";

    sqlite3_prepare(db, query, -1, &stmt, 0);

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, countryCode);
    sqlBindTextOrNull(stmt, 3, country);
    sqlite3_bind_int(stmt, 4, blocklevel);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}
*/
