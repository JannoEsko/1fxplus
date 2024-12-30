
#include "../../ext/sqlite/sqlite3.h"
#include "../g_local.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#ifdef _WIN32
#include <direct.h>
#endif
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
            "CREATE TABLE IF NOT EXISTS aliases (alias VARCHAR(64), ip VARCHAR(24), UNIQUE(alias, ip));"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (1);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 1 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

    if (gameMigrationLevel < 2) {
        // Add clan membership information
        char* migration = "CREATE TABLE IF NOT EXISTS clanlist (membername VARCHAR(64) COLLATE NOCASE, memberkey VARCHAR(80), membertype INTEGER, addedby VARCHAR(64), addedwhen TIMESTAMP DEFAULT CURRENT_TIMESTAMP);" // memberKey = IP, GUID or pass. Membertype = clanType_t
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (2);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 2 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

    if (gameMigrationLevel < 3) {
        // Add session details to DB.
        // Intention is to make it persist even across gametype changes.
        // If the gametype changed, then refrain from reading information not relevant, but do read back e.g. clan, admin, country etc.
        char* migration = "CREATE TABLE IF NOT EXISTS sessions (clientnum INTEGER DEFAULT 0, team INTEGER DEFAULT 0, adminlevel INTEGER DEFAULT 0, admintype INTEGER DEFAULT 0, clanmember INTEGER DEFAULT 0, hasroxac INTEGER DEFAULT 0, roxguid VARCHAR(15), adminname VARCHAR(40), countrycode VARCHAR(10), country VARCHAR(50), blockseek INTEGER DEFAULT 0, clantype INTEGER DEFAULT 0, clanname VARCHAR(36));"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (3);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 3 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

    if (gameMigrationLevel < 4) {
        // Mute sessions to SQL
        char* migration = "CREATE TABLE IF NOT EXISTS sessionmutes (ip VARCHAR(25), remainingtime INTEGER, totalduration INTEGER);"
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (4);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 4 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

    if (gameMigrationLevel < 5) {
        // H&S stats
        char* migration = "CREATE TABLE IF NOT EXISTS hnsbestplayers (map VARCHAR(64), player VARCHAR(64), type INTEGER DEFAULT 0, statpoints INTEGER, dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP);" // Type = 0 => hider, statpoints = rounds won, Type = 1 => seeker, statpoints = kills.
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (5);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 5 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
            return;
        }
    }

    if (gameMigrationLevel < 6) {
        // Profanity filters.
        char* migration = "CREATE TABLE IF NOT EXISTS profanitylist (profanity VARCHAR(64) COLLATE NOCASE UNIQUE, dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP);" 
            "DELETE FROM migrationlevel;"
            "INSERT INTO migrationlevel (migrationlevel) VALUES (6);";

        if (sqlite3_exec(db, migration, 0, 0, 0) != SQLITE_OK) {
            sqlite3_close(db);
            logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to migrate the game database to level 6 (starting level: %d).\nSQLite error: %s\nCode: %d", gameMigrationLevel, sqlite3_errmsg(db), sqlite3_errcode(db));
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


char* getIpOrArg(gentity_t* ent, char* arg) {
    return ent && ent->client ? ent->client->pers.ip : arg;
}

admLevel_t getAdminLevel(gentity_t* ent) {
    return ent && ent->client ? ent->client->sess.adminLevel : ADMLVL_RCON;
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

	rc = sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_FATAL_DB, "miragtionLevel game.db prepare failed: %s", sqlite3_errmsg(db));
        return;
    }

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
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write logs.db file in databases folder.\n");
        return;
    }

    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS migrationlevel (migrationlevel INTEGER)", 0, 0, 0) != SQLITE_OK) {
        sqlite3_close(db);
        logSystem(LOGLEVEL_FATAL_DB, "Game dropped due to failing to write migrationlevel table into logs.db.\n");
        return;
    }

    rc = sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_FATAL_DB, "miragtionLevel logsDb prepare failed: %s", sqlite3_errmsg(db));
        return;
    }

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

    rc = sqlite3_prepare(db, "SELECT * FROM migrationlevel", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_FATAL_DB, "miragtionLevel countryDb prepare failed: %s", sqlite3_errmsg(db));
        return;
    }


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

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "getAdmnLevel prepare error: %s", sqlite3_errmsg(db));
        return returnable;
    }

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

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addAdmin prepare failed: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);
    sqlite3_bind_int(stmt, 3, adminLevel);
    sqlBindTextOrNull(stmt, 4, getNameOrArg(adm, "RCON", qtrue));

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addAdmin step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}


static void dbAddPassAdmin(admLevel_t adminLevel, gentity_t* ent, gentity_t* adm, char* pass) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("INSERT INTO adminpasslist (adminname, adminlevel, addedby, password) VALUES (?, ?, ?, ?)");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addPassAdmin prepare failed. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlite3_bind_int(stmt, 2, adminLevel);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));
    
    sqlBindTextOrNull(stmt, 4, pass);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addPassAdmin step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}


static void dbAddGuidAdmin(admLevel_t adminLevel, gentity_t* ent, gentity_t* adm) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("INSERT INTO adminguidlist (adminname, adminlevel, addedby, acguid) VALUES (?, ?, ?, ?)");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addGuidAdmin prepare failed. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlite3_bind_int(stmt, 2, adminLevel);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));

    sqlBindTextOrNull(stmt, 4, ent->client->sess.roxGuid);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addGuidAdmin step error: %s", sqlite3_errmsg(db));
    }

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
        return rowsAffected;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "removeAdminByRow step error: %s", sqlite3_errmsg(db));
    }

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
        return rowsAffected;
    }

    sqlBindTextOrNull(stmt, 1, password);
    sqlBindTextOrNull(stmt, 2, adminName);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "updateAdmPass step error: %s", sqlite3_errmsg(db));
    }

    rowsAffected = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    return rowsAffected;

}

void dbPrintAdminlist(gentity_t* ent, admType_t adminType, int page) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char buf[MAX_PACKET_BUF];
    qboolean isRcon = ent && ent->client ? qfalse : qtrue;

    Com_Memset(buf, 0, sizeof(buf));
    char* query = va("SELECT ROWID, adminname, adminlevel, addedby, DATE(addedwhen)%s FROM admin%slist %s", adminType == ADMTYPE_IP ? ", ip" : (adminType == ADMTYPE_GUID ? ", acguid" : ""), adminType == ADMTYPE_PASS ? "pass" : (adminType == ADMTYPE_GUID ? "guid" : ""), !isRcon ? "WHERE ROWID BETWEEN ? AND ?" : "");
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
        if (page > 0) page--;
        Q_strcat(buf, sizeof(buf), va("\n[^3Page %d^7]\n\n^3 %-5s%-5s%-16s %-16s %-16s Date\n^7-----------------------------------------------------------------------------\n", page + 1, "#", "Lvl", "IP (GUID)", "Name", "By"));
        sqlite3_bind_int(stmt, 1, page * 100);
        sqlite3_bind_int(stmt, 2, (page + 1) * 100);
    }

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
                    Com_Memset(buf, 0, sizeof(buf));
                }

                Q_strcat(buf, sizeof(buf), va("[^3%-3.3d^7] %-5d%-16.16s %-16.16s %-16.16s %s\n", rowId, adminLevel, guidIp, adminName, addedBy, addedWhen));
            }
        }
    }

    if (!isRcon) {

        trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
        trap_SendServerCommand(ent - g_entities, va("print \"\nYou can specify page by adding the page number at the end of the command.\nTo proceed to next page - /adm adminlist %s%d / !al %s%d \"", adminType == ADMTYPE_GUID ? "guid " : (adminType == ADMTYPE_PASS ? "pass " : ""), page + 2, adminType == ADMTYPE_GUID ? "guid " : (adminType == ADMTYPE_PASS ? "pass " : ""), page + 2));
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
        sqlite3_exec(db, "DELETE FROM profanitylist", NULL, NULL, NULL);

        logSystem(LOGLEVEL_INFO, "All tables truncated.");
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
    else if (!Q_stricmp("profanitylist", table)) {
        sqlite3_exec(db, "DELETE FROM profanitylist", NULL, NULL, NULL);
    }
    else {
        logSystem(LOGLEVEL_INFO, "No table \"%s\" found.", table);
        return;
    }
    backupInMemoryDatabases();
    logSystem(LOGLEVEL_INFO, "Table \"%s\" truncated.", table);

}

void dbGetAliases(gentity_t* ent, char* output, int outputSize, char* separator) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rc = 0;

    if (!g_maxAliases.integer) {
        return;
    }

    Com_Memset(output, 0, outputSize);

    char* query = va("SELECT alias FROM aliases WHERE alias != ? AND ip = ?");

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "getAliases prepare error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);

    
    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            char* alias = sqlite3_column_text(stmt, 0);

            if (!alias) continue;

            if (strlen(alias) + 1 + strlen(output) > outputSize) {
                logSystem(LOGLEVEL_WARN, "Alias buffer overflow.");
                break;
            }

            Q_strcat(output, outputSize, alias);
            Q_strcat(output, outputSize, separator);
            
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

    char* query = va("INSERT OR IGNORE INTO aliases (alias, ip) VALUES (?, ?)");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addAlias prepare error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addalias step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    dbClearOldAliases(ent);
}

void dbClearOldAliases(gentity_t* ent) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = va("DELETE FROM aliases WHERE ROWID IN (SELECT ROWID FROM aliases WHERE ip = ? ORDER BY ROWID ASC LIMIT (SELECT MAX(COUNT(*) - ? - 1, 0) FROM aliases WHERE ip = ?))");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "clearOldAliases prepare error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.ip);
    sqlite3_bind_int(stmt, 2, g_maxAliases.integer);
    sqlBindTextOrNull(stmt, 3, ent->client->pers.ip);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "clearaliases step error: %s", sqlite3_errmsg(db));
    }

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
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "Addban prepare error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlBindTextOrNull(stmt, 2, subnet ? ent->client->pers.subnet : ent->client->pers.ip);
    sqlBindTextOrNull(stmt, 3, getNameOrArg(adm, "RCON", qtrue));
    sqlBindTextOrNull(stmt, 4, reason);
    sqlite3_bind_int(stmt, 5, endofmap);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "Addban step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}

int dbRemoveBan(qboolean subnet, int rowId) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    int rowsAffected = 0;

    char* query = va("DELETE FROM %sbanlist WHERE ROWID = ?", subnet ? "subnet" : "");
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "removeBan prepare failed: %s", sqlite3_errmsg(db));
        return rowsAffected;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "removeBan step error: %s", sqlite3_errmsg(db));
    }

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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "logAdmin prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlBindTextOrNull(stmt, 3, toIp);
    sqlBindTextOrNull(stmt, 4, toName);
    sqlBindTextOrNull(stmt, 5, action);
    sqlBindTextOrNull(stmt, 6, reason);
    sqlite3_bind_int(stmt, 7, adminLevel);
    sqlBindTextOrNull(stmt, 8, adminName);
    sqlite3_bind_int(stmt, 9, adminType);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "logAdmin step error: %s", sqlite3_errmsg(db));
    }
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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "logGame prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlBindTextOrNull(stmt, 3, toIp);
    sqlBindTextOrNull(stmt, 4, toName);
    sqlBindTextOrNull(stmt, 5, action);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "logGame step error: %s", sqlite3_errmsg(db));
    }
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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "logLogin prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlBindTextOrNull(stmt, 1, byIp);
    sqlBindTextOrNull(stmt, 2, byName);
    sqlite3_bind_int(stmt, 3, adminLevel);
    sqlite3_bind_int(stmt, 4, adminType);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "loginLog step error: %s", sqlite3_errmsg(db));
    }
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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "logRcon prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, action);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "rconLog step error: %s", sqlite3_errmsg(db));
    }

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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        Com_PrintWarn("Failed to logSystem, db error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, logLevel);
    sqlBindTextOrNull(stmt, 2, msg);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        Com_PrintWarn("Failed to run step on logSystem, db error: %s", sqlite3_errmsg(db));
    }

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

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    query = "DELETE FROM gameslog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    query = "DELETE FROM loginlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    query = "DELETE FROM rconlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    query = "DELETE FROM systemlog WHERE dt < DATETIME('now', '-' || ? || ' days')";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, g_dbLogRetention.integer);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    


    sqlite3_close(db);

}

static qboolean dbQueryBan(char* ip, qboolean subnet, char* reason, int reasonSize, int* endOfMap, int* banEnd) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    qboolean success = qfalse;

    char* query = va("SELECT reason, endofmap, ROUND((JULIANDAY(banneduntil) - JULIANDAY()) * 1440) AS timeleft FROM %sbanlist WHERE ip = ?", subnet ? "subnet" : "");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "queryBan prepare error: %s", sqlite3_errmsg(db));
        return success;
    }

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

qboolean dbCheckBan(char* ip, char* reason, int reasonSize, int* endOfMap, int* banEnd) {

    // Try IP first.
    qboolean banned = dbQueryBan(ip, qfalse, reason, reasonSize, endOfMap, banEnd);

    if (!banned) {
        // subnetban

        char subnetIp[MAX_IP];

        getSubnet(ip, subnetIp, sizeof(subnetIp));

        banned = dbQueryBan(subnetIp, qtrue, reason, reasonSize, endOfMap, banEnd);
    }

    return banned;
}


qboolean dbGetCountry(char* ip, char* countryCode, int countryCodeSize, char* country, int countrySize, int* blocklevel) {

    sqlite3* db;
    sqlite3_stmt* stmt;
    qboolean success = qfalse;

    if (!g_useCountryDb.integer) {
        return success; 
    }

    int rc = sqlite3_open_v2("./1fx/databases/country.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open country.db file to add a new country. Error: %s\n", sqlite3_errmsg(db));
        return success;
    }

    // First, clean out aged country rows.
    char* query;

    if (g_countryAging.integer) {
        query = "DELETE FROM ip2country WHERE added < DATETIME('now', '-' || ? || ' days')";

        rc = sqlite3_prepare(db, query, -1, &stmt, 0);

        if (rc != SQLITE_OK) {
            logSystem(LOGLEVEL_WARN, "countryDb clearaging prepare error: %s", sqlite3_errmsg(db));
            sqlite3_close(db);
            return success;
        }

        sqlite3_bind_int(stmt, 1, g_countryAging.integer);
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            logSystem(LOGLEVEL_WARN, "countryDb aging step error: %s", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    }

    query = "SELECT countrycode, country, blocklevel FROM ip2country WHERE ip = ?";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "getCountry select prepare error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return success;
    }

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

    if (!g_useCountryDb.integer || !g_useCountryAPI.integer) {
        return; 
    }

    int rc = sqlite3_open_v2("./1fx/databases/country.db", &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc) {
        logSystem(LOGLEVEL_WARN, "Failed to open country.db file to add a new country. Error: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* query = "INSERT INTO ip2country (ip, countrycode, country, blocklevel) VALUES (?, ?, ?, ?)";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addCountry prepare error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ip);
    sqlBindTextOrNull(stmt, 2, countryCode);
    sqlBindTextOrNull(stmt, 3, country);
    sqlite3_bind_int(stmt, 4, blocklevel);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addCountry step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

int dbRemoveAdminByGentity(gentity_t* ent) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    // On GUID, you remove by name+GUID.
    // On IP, you remove by name+IP
    // On pass, you remove by name
    char* query = va("DELETE FROM admin%slist WHERE adminname = ? %s", ent->client->sess.adminType == ADMTYPE_PASS ? "pass" : (ent->client->sess.adminType == ADMTYPE_GUID ? "guid" : ""), ent->client->sess.adminType == ADMTYPE_IP ? "AND ip = ?" : (ent->client->sess.adminType == ADMTYPE_GUID ? "AND acguid = ?" : ""));

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return rowsAffected;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->sess.adminName);
    
    if (ent->client->sess.adminType == ADMTYPE_GUID) {
        sqlBindTextOrNull(stmt, 2, ent->client->sess.roxGuid);
    }
    else if (ent->client->sess.adminType == ADMTYPE_IP) {
        sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "removeAdmin step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    rowsAffected = sqlite3_changes(db);

    sqlite3_exec(db, "VACUUM", NULL, NULL, NULL);

    return rowsAffected;
}


void dbAddClan(clanType_t clanType, gentity_t* ent, gentity_t* adm, char* password) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "INSERT INTO clanlist (membername, memberkey, membertype, addedby) VALUES (?, ?, ?, ?)";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "addClan prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    if (clanType == CLANTYPE_GUID) {
        sqlBindTextOrNull(stmt, 2, ent->client->sess.roxGuid);
    }
    else if (clanType == CLANTYPE_IP) {
        sqlBindTextOrNull(stmt, 2, ent->client->pers.ip);
    }
    else {
        sqlBindTextOrNull(stmt, 2, password);
    }
    sqlite3_bind_int(stmt, 3, clanType);
    sqlBindTextOrNull(stmt, 4, getNameOrArg(adm, "RCON", qtrue));

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addIpClan step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}

int dbRemoveClanByGentity(gentity_t* ent) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    char* query = va("DELETE FROM clanlist WHERE membername = ? AND membertype = ? %s", ent->client->sess.clanType == CLANTYPE_IP || ent->client->sess.clanType == CLANTYPE_GUID ? "AND memberkey = ?" : "");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "removeClanByGentity prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return 0;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->sess.clanName);
    sqlite3_bind_int(stmt, 2, ent->client->sess.clanType);

    if (ent->client->sess.clanType == CLANTYPE_IP) {
        sqlBindTextOrNull(stmt, 3, ent->client->pers.ip);
    }
    else if (ent->client->sess.clanType == CLANTYPE_GUID) {
        sqlBindTextOrNull(stmt, 3, ent->client->sess.roxGuid);
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "addIpClan step error: %s", sqlite3_errmsg(db));
    }
    else {
        rowsAffected = sqlite3_changes(db);
    }

    sqlite3_finalize(stmt);

    return rowsAffected;

}

int dbRemoveClanByRowId(int rowId) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    char* query = "DELETE FROM clanlist WHERE ROWID = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return rowsAffected;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "removeClanByRow step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    rowsAffected = sqlite3_changes(db);

    sqlite3_exec(db, "VACUUM", NULL, NULL, NULL);

    return rowsAffected;
}

int dbUpdateClanPass(char* memberName, char* password) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    int rowsAffected = 0;

    char* query = "UPDATE clanlist SET memberkey = ? WHERE membername = ? AND membertype = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return rowsAffected;
    }

    sqlBindTextOrNull(stmt, 1, password);
    sqlBindTextOrNull(stmt, 2, memberName);
    sqlite3_bind_int(stmt, 3, CLANTYPE_PASS);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "updateClanlistPass step error: %s", sqlite3_errmsg(db));
    }

    rowsAffected = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    return rowsAffected;

}

/*
As with admin command - NULL password is allowed. NULL passwords will be used to query existing information if you're tryign to add a new clan member.
When doing an actual password check, then a NULL should never be passed.
*/
qboolean dbGetClan(clanType_t clanType, gentity_t* ent, char* password) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    qboolean returnable = qfalse;

    char* query = va("SELECT ROWID FROM clanlist WHERE membername = ? AND membertype = ? %s", clanType != CLANTYPE_PASS || (clanType == CLANTYPE_PASS && password && *password) ? "AND memberkey = ?" : "");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "getClan prepare error: %s", sqlite3_errmsg(db));
        return returnable;
    }

    sqlBindTextOrNull(stmt, 1, ent->client->pers.cleanName);
    sqlite3_bind_int(stmt, 2, clanType);

    if (clanType == CLANTYPE_GUID) {
        sqlBindTextOrNull(stmt, 3, ent->client->sess.roxGuid);
    }
    else if (clanType == CLANTYPE_PASS) {
        sqlBindTextOrNull(stmt, 3, password);
    }
    else {
        sqlBindTextOrNull(stmt, 3, ent->client->pers.ip);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
        returnable = qtrue;
    }

    sqlite3_finalize(stmt);

    return returnable;
}

qboolean dbGetClanDataByRowId(int rowId, char* memberName, int memberNameSize, int* memberType) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    qboolean success = qfalse;

    char* query = "SELECT membername, membertype FROM clanlist WHERE ROWID = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "getClanData prepare error: %s", sqlite3_errmsg(db));
        return success;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        success = qtrue;
        Q_strncpyz(memberName, sqlite3_column_text(stmt, 0), memberNameSize);
        *memberType = sqlite3_column_int(stmt, 1);
    }

    sqlite3_finalize(stmt);

    return success;

}

void dbPrintClanlist(gentity_t* ent, clanType_t clanType, int page) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char buf[MAX_PACKET_BUF];
    qboolean isRcon = ent && ent->client ? qfalse : qtrue;

    Com_Memset(buf, 0, sizeof(buf));
    char* query = va("SELECT ROWID, membername, memberkey, membertype, addedby, DATE(addedwhen) FROM clanlist WHERE %s %s", isRcon ? "1=1" : "ROWID BETWEEN ? AND ?", clanType != CLANTYPE_NONE ? "AND membertype = ?" : "");
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    if (isRcon) {
        Com_Printf("\n^3 %-5s%-6s%-15s %-16s %-16s Date\n", "#", "Type", "IP (GUID)", "Name", "By");
        Com_Printf("^7-----------------------------------------------------------------------------\n");

        if (clanType != CLANTYPE_NONE) {
            sqlite3_bind_int(stmt, 1, clanType);
        }
    }
    else {
        if (page > 0) page--;
        Q_strcat(buf, sizeof(buf), va("\n[^3Page %d^7]\n\n^3 %-5s%-6s%-15s %-16s %-16s Date\n^7-----------------------------------------------------------------------------\n", page + 1, "#", "Type", "IP (GUID)", "Name", "By"));

        sqlite3_bind_int(stmt, 1, page * 100);
        sqlite3_bind_int(stmt, 2, (page + 1) * 100);

        if (clanType != CLANTYPE_NONE) {
            sqlite3_bind_int(stmt, 3, clanType);
        }
    }

    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            int rowId = sqlite3_column_int(stmt, 0);
            char* memberName = sqlite3_column_text(stmt, 1);
            char* memberKey = sqlite3_column_text(stmt, 2);
            clanType_t memberType = sqlite3_column_int(stmt, 3);
            char* addedBy = sqlite3_column_text(stmt, 4);
            char* addedWhen = sqlite3_column_text(stmt, 5);
            
            if (memberType == CLANTYPE_PASS) {
                memberKey = "";
            }

            if (isRcon) {
                Com_Printf("[^3%-3.3d^7] %-6d%-15.15s %-16.16s %-16.16s %s\n", rowId, getClanTypeAsText(memberType), memberKey, memberName, addedBy, addedWhen);
            }
            else {

                if (strlen(buf) + strlen(va("[^3%-3.3d^7] %-6s%-15.15s %-16.16s %-16.16s %s\n", rowId, getClanTypeAsText(memberType), memberKey, memberName, addedBy, addedWhen)) >= sizeof(buf) - 5) {
                    trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
                    Com_Memset(buf, 0, sizeof(buf));
                }

                Q_strcat(buf, sizeof(buf), va("[^3%-3.3d^7] %-6s%-15.15s %-16.16s %-16.16s %s\n", rowId, getClanTypeAsText(memberType), memberKey, memberName, addedBy, addedWhen));
            }
        }
    }

    if (!isRcon) {

        trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
        trap_SendServerCommand(ent - g_entities, va("print \"\nYou can specify page by adding the page number at the end of the command.\nTo proceed to next page - /adm clanlist %s%d / !cl %s%d \"", clanType == CLANTYPE_GUID ? "guid " : (clanType == CLANTYPE_PASS ? "pass " : (clanType == CLANTYPE_IP ? "ip " : "")), page + 2, clanType == CLANTYPE_GUID ? "guid " : (clanType == CLANTYPE_PASS ? "pass " : (clanType == CLANTYPE_IP ? "ip " : "")), page + 2));
        trap_SendServerCommand(ent - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n\"");
    }
    else {
        Com_Printf("\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n");
    }

    sqlite3_finalize(stmt);

}

qboolean dbGetBanDetailsByRowID(qboolean subnet, int rowId, char* outputPlayer, int outputPlayerSize, char* outputIp, int outputIpSize) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    qboolean success = qfalse;

    char* query = va("SELECT playername, ip FROM %sbanlist WHERE ROWID = ?", subnet ? "subnet" : "");

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return success;
    }

    sqlite3_bind_int(stmt, 1, rowId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        success = qtrue;
        Q_strncpyz(outputPlayer, sqlite3_column_text(stmt, 0), outputPlayerSize);
        Q_strncpyz(outputIp, sqlite3_column_text(stmt, 0), outputIpSize);
    }

    sqlite3_finalize(stmt);

    return success;
}


void dbPrintBanlist(gentity_t* ent, qboolean subnet, int page) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char buf[MAX_PACKET_BUF];
    qboolean isRcon = ent && ent->client ? qfalse : qtrue;

    Com_Memset(buf, 0, sizeof(buf));
    char* query = va("SELECT ROWID, playername, ip, adminname, reason, ROUND((JULIANDAY(banneduntil) - JULIANDAY()) * 1440) AS timeleft, endofmap FROM %sbanlist %s", subnet ? "subnet" : "", ent && ent->client ? "WHERE ROWID BETWEEN ? AND ?" : "");
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    if (isRcon) {
        Com_Printf("\n^3 %-5s%-12.12s%-16.16s%-12.12s%-11.11s%-20.20s\n", "#", "Player", "IP", "Banned by", "Time left", "Reason");
        Com_Printf("^7-----------------------------------------------------------------------------\n");


    }
    else {
        if (page > 0) page--;
        Q_strcat(buf, sizeof(buf), va("\n[^3Page %d^7]\n\n^3 %-5s%-12.12s%-16.16s%-12.12s%-11.11s%-20.20s\n^7-----------------------------------------------------------------------------\n", page + 1, "#", "Player", "IP", "Banned by", "Time left", "Reason"));

        sqlite3_bind_int(stmt, 1, page * 100);
        sqlite3_bind_int(stmt, 2, (page + 1) * 100);

    }

    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            int rowId = sqlite3_column_int(stmt, 0);
            char* playerName = sqlite3_column_text(stmt, 1);
            char* playerIp = sqlite3_column_text(stmt, 2);
            char* adminName = sqlite3_column_text(stmt, 3);
            char* reason = sqlite3_column_text(stmt, 4);
            int timeleft = sqlite3_column_int(stmt, 5);
            qboolean endofmap = (qboolean) sqlite3_column_int(stmt, 6);

            char displayTimeLeft[64];

            if (endofmap) {
                Q_strncpyz(displayTimeLeft, "End of map", sizeof(displayTimeLeft));
            }
            else {
                Q_strncpyz(displayTimeLeft, va("%dd%02dh%02dm", timeleft / (60 * 24), (timeleft / 60) % 24, timeleft % 60), sizeof(displayTimeLeft));
            }

            if (isRcon) {
                Com_Printf("[^3%-3.3d^7] %-12.12s%-16.16s%-12.12s%-11.11s%-20.20s\n", rowId, playerName, playerIp, adminName, displayTimeLeft, reason);
            }
            else {

                if (strlen(buf) + strlen(va("[^3%-3.3d^7] %-12.12s%-16.16s%-12.12s%-10.10s%-20.20s\n", rowId, playerName, playerIp, adminName, displayTimeLeft, reason)) >= sizeof(buf) - 5) {
                    trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
                    Com_Memset(buf, 0, sizeof(buf));
                }

                Q_strcat(buf, sizeof(buf), va("[^3%-3.3d^7] %-12.12s%-16.16s%-12.12s%-11.11s%-20.20s\n", rowId, playerName, playerIp, adminName, displayTimeLeft, reason));
            }
        }
    }

    if (!isRcon) {

        trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
        trap_SendServerCommand(ent - g_entities, va("print \"\nYou can specify page by adding the page number at the end of the command.\nTo proceed to next page - /adm %sbanlist %d / !%sbl %d \"", subnet ? "subnet" : "", page + 2, subnet ? "s" : "", page + 2));
        trap_SendServerCommand(ent - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n\"");
    }
    else {
        Com_Printf("\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n");
    }

    sqlite3_finalize(stmt);

}

void dbRemoveSessionDataById(int clientNum) {
    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "DELETE FROM sessions WHERE clientnum = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session writing. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, clientNum);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void dbWriteSessionDataForClient(gclient_t* client) {

    clientSession_t* sess = &client->sess;
    int clientNum = client - level.clients;

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "INSERT INTO sessions (clientnum, team, adminlevel, admintype, clanmember, hasroxac, roxguid, adminname, countrycode, country, blockseek, clantype, clanname) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session writing. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, clientNum);
    sqlite3_bind_int(stmt, 2, sess->team);
    sqlite3_bind_int(stmt, 3, sess->adminLevel);
    sqlite3_bind_int(stmt, 4, sess->adminType);
    sqlite3_bind_int(stmt, 5, sess->clanMember);
    sqlite3_bind_int(stmt, 6, sess->hasRoxAC);
    sqlBindTextOrNull(stmt, 7, sess->roxGuid);
    sqlBindTextOrNull(stmt, 8, sess->adminName);
    sqlBindTextOrNull(stmt, 9, sess->countryCode);
    sqlBindTextOrNull(stmt, 10, sess->country);
    sqlite3_bind_int(stmt, 11, sess->blockseek);
    sqlite3_bind_int(stmt, 12, sess->clanType);
    sqlBindTextOrNull(stmt, 13, sess->clanName);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "writeSession step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}

void dbReadSessionDataForClient(gclient_t* client, qboolean gametypeChanged) {

    clientSession_t* sess = &client->sess;
    int clientNum = client - level.clients;

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "SELECT team, adminlevel, admintype, clanmember, hasroxac, roxguid, adminname, countrycode, country, blockseek, clantype, clanname FROM sessions WHERE clientnum = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session writing. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, clientNum);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {

        if (!gametypeChanged) {
            sess->team = sqlite3_column_int(stmt, 0);
        }

        sess->adminLevel = sqlite3_column_int(stmt, 1);
        sess->adminType = sqlite3_column_int(stmt, 2);
        sess->clanMember = sqlite3_column_int(stmt, 3);
        sess->hasRoxAC = sqlite3_column_int(stmt, 4);

        char roxAc[MAX_AC_GUID];
        Com_Memset(roxAc, 0, sizeof(roxAc));

        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            Q_strncpyz(roxAc, sqlite3_column_text(stmt, 5), sizeof(roxAc));
        }
        Q_strncpyz(sess->roxGuid, roxAc, sizeof(sess->roxGuid));

        char adminName[MAX_NETNAME];
        Com_Memset(adminName, 0, sizeof(adminName));

        if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
            Q_strncpyz(adminName, sqlite3_column_text(stmt, 6), sizeof(adminName));
        }
        Q_strncpyz(sess->adminName, adminName, sizeof(sess->adminName));

        char countryCode[MAX_COUNTRYCODE];
        char country[MAX_COUNTRYNAME];

        Com_Memset(countryCode, 0, sizeof(countryCode));
        Com_Memset(country, 0, sizeof(country));

        // if one of them is null, both will be kept on null.

        if (sqlite3_column_type(stmt, 7) != SQLITE_NULL && sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
            Q_strncpyz(countryCode, sqlite3_column_text(stmt, 7), sizeof(countryCode));
            Q_strncpyz(country, sqlite3_column_text(stmt, 8), sizeof(country));
        } 

        Q_strncpyz(sess->countryCode, countryCode, sizeof(sess->countryCode));
        Q_strncpyz(sess->country, country, sizeof(sess->country));

        sess->blockseek = sqlite3_column_int(stmt, 9);
        sess->clanType = sqlite3_column_int(stmt, 10);

        char clanName[MAX_NETNAME];
        Com_Memset(clanName, 0, sizeof(clanName));

        if (sqlite3_column_type(stmt, 11) != SQLITE_NULL) {
            Q_strncpyz(clanName, sqlite3_column_text(stmt, 11), sizeof(clanName));
        }

        Q_strncpyz(sess->clanName, clanName, sizeof(sess->clanName));
        
    }
    else if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "readSession step error: %s", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);

}

void dbReadSessionMutesBackIntoMuteInfo(void) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "SELECT ip, remainingtime, totalduration FROM sessionmutes";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session mutes reading. Error: %s", sqlite3_errmsg(db));
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        mute_t* muteInfo = &level.mutedClients[level.numMutedClients];

        muteInfo->used = qtrue;
        Q_strncpyz(muteInfo->ip, sqlite3_column_text(stmt, 0), sizeof(muteInfo->ip));
        muteInfo->startTime = level.time;
        muteInfo->time = sqlite3_column_int(stmt, 1);
        muteInfo->totalDuration = sqlite3_column_int(stmt, 2);

        level.numMutedClients++;

    }

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "reading sessionmutes error: %s", sqlite3_errmsg(db));
    }
    

    sqlite3_finalize(stmt);

}

void dbWriteMuteIntoSession(mute_t* muteInfo) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "INSERT INTO sessionmutes (ip, remainingtime, totalduration) VALUES (?, ?, ?)";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session mutes writing. Error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlBindTextOrNull(stmt, 1, muteInfo->ip);
    sqlite3_bind_int(stmt, 2, muteInfo->startTime + muteInfo->time - level.time);
    sqlite3_bind_int(stmt, 3, muteInfo->totalDuration);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "writing sessionmutes error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}

void dbClearSessionMutes(void) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "DELETE FROM sessionmutes";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb session mutes clearing. Error: %s", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "deleting sessionmutes error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void dbWriteHnsStats() {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query;
    int rc;

    // At the very first step, we check whether we can clear aged rows.
    if (g_hnsStatAging.integer) {
        query = "DELETE FROM hnsbestplayers WHERE dt < DATETIME('now', '-' || ? || ' days')";

        rc = sqlite3_prepare(db, query, -1, &stmt, 0);

        if (rc != SQLITE_OK) {
            logSystem(LOGLEVEL_WARN, "gameDb clearaging prepare error: %s", sqlite3_errmsg(db));
        }

        sqlite3_bind_int(stmt, 1, g_hnsStatAging.integer);
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            logSystem(LOGLEVEL_WARN, "gameDb aging step error: %s", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    }

    query = "INSERT INTO hnsbestplayers (map, player, type, statpoints) VALUES (?, ?, ?, ?)";

    for (int i = 0; i < level.numConnectedClients; i++) {

        gentity_t* ent = &g_entities[level.sortedClients[i]];

        if (ent->client->sess.team == TEAM_SPECTATOR) {
            continue;
        }

        rc = sqlite3_prepare(db, query, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb write hnsstats. Error: %s", sqlite3_errmsg(db));
            return;
        }

        char currentMap[MAX_QPATH];
        trap_Cvar_VariableStringBuffer("mapname", currentMap, sizeof(currentMap));

        sqlBindTextOrNull(stmt, 1, currentMap);
        sqlBindTextOrNull(stmt, 2, ent->client->pers.cleanName);
        sqlite3_bind_int(stmt, 3, ent->client->sess.team == TEAM_BLUE ? 1 : 0);
        sqlite3_bind_int(stmt, 4, ent->client->sess.team == TEAM_BLUE ? ent->client->sess.score : ent->client->sess.kills);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            logSystem(LOGLEVEL_WARN, "Writing hnsbestplayers failed: %s", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);

    }

    // After inserts, we're gonna trim it down to top 3 on both categories.
    // As we insert it all into the table and clear out by ageing variable,
    // We now run a query where we retain only top 3 in the mix.
    
    // Previous query removed as that only kept top3 hiders of ALL maps and top3 seekers of ALL maps.
    query = "DELETE FROM hnsbestplayers "
        "WHERE ROWID NOT IN ( "
            "SELECT ROWID FROM ( "
                "SELECT ROWID, map, type, "
                "RANK() OVER(PARTITION BY map, type ORDER BY statpoints DESC, dt DESC, ROWID DESC) as rank "
                "FROM hnsbestplayers "
            ") WHERE rank <= 3"
        ")";

    rc = sqlite3_prepare(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "gameDb clear non-top3 hns prepare error: %s", sqlite3_errmsg(db));
    }
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "gameDb clear non-top3 hns step error: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void dbWriteHnsBestPlayersIntoHnsStruct() {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "SELECT player, type, statpoints FROM hnsbestplayers WHERE map = ? ORDER BY type ASC, statpoints DESC, dt DESC";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb hnsbestplayers reading. Error: %s", sqlite3_errmsg(db));
        return;
    }

    char mapname[MAX_QPATH];
    trap_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));

    sqlBindTextOrNull(stmt, 1, mapname);

    int curHiderBestRow = 0;
    int curSeekerBestRow = 0;

    for (int i = 0; i < 3; i++) {
        level.hns.bestHiders[i].playerScore = -1;
        level.hns.bestSeekers[i].playerScore = -1;

        Q_strncpyz(level.hns.bestHiders[i].playerName, "none", sizeof(level.hns.bestHiders[0].playerName));
        Q_strncpyz(level.hns.bestSeekers[i].playerName, "none", sizeof(level.hns.bestSeekers[0].playerName));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        char* bestPlayer = sqlite3_column_text(stmt, 0);
        int type = sqlite3_column_int(stmt, 1);
        int stats = sqlite3_column_int(stmt, 2);

        if (type == 0) { // hider stat.
            level.hns.bestHiders[curHiderBestRow].playerScore = stats;
            Q_strncpyz(level.hns.bestHiders[curHiderBestRow].playerName, bestPlayer, sizeof(level.hns.bestHiders[0].playerName));
            curHiderBestRow++;
        }
        else { // seeker stat.
            level.hns.bestSeekers[curSeekerBestRow].playerScore = stats;
            Q_strncpyz(level.hns.bestSeekers[curSeekerBestRow].playerName, bestPlayer, sizeof(level.hns.bestSeekers[0].playerName));
            curSeekerBestRow++;
        }

    }

    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "reading hnsbestplayers error : %s", sqlite3_errmsg(db));
    }


    sqlite3_finalize(stmt);
}

int dbAddProfanity(char* profanity) {
    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    // CREATE TABLE IF NOT EXISTS profanitylist (profanity VARCHAR(64) COLLATE NOCASE UNIQUE, addedby VARCHAR(64), dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP)

    char* query = "INSERT OR IGNORE INTO profanitylist (profanity) VALUES (?)";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb profanity insert. Error: %s", sqlite3_errmsg(db));
        return 0;
    }

    sqlBindTextOrNull(stmt, 1, profanity);

    rc = sqlite3_step(stmt);
    int rowsAffected = 0;
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "writing profanity error: %s", sqlite3_errmsg(db));
    }
    else {
        rowsAffected = sqlite3_changes(db);
    }

    sqlite3_finalize(stmt);

    return rowsAffected;
}

int dbRemoveProfanity(char* profanity) {
    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    // CREATE TABLE IF NOT EXISTS profanitylist (profanity VARCHAR(64) COLLATE NOCASE UNIQUE, addedby VARCHAR(64), dt TIMESTAMP DEFAULT CURRENT_TIMESTAMP)

    char* query = "DELETE FROM profanitylist WHERE profanity = ?";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb profanity delete. Error: %s", sqlite3_errmsg(db));
        return 0;
    }

    sqlBindTextOrNull(stmt, 1, profanity);

    rc = sqlite3_step(stmt);
    int rowsAffected = 0;
    if (rc != SQLITE_DONE) {
        logSystem(LOGLEVEL_WARN, "delete profanity error: %s", sqlite3_errmsg(db));
    }
    else {
        rowsAffected = sqlite3_changes(db);
    }

    sqlite3_finalize(stmt);

    return rowsAffected;
}

void dbPrintProfanitylist(gentity_t* ent) {
    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;
    char buf[MAX_PACKET_BUF];
    qboolean isRcon = ent && ent->client ? qfalse : qtrue;

    Com_Memset(buf, 0, sizeof(buf));
    char* query = "SELECT profanity, dt FROM profanitylist";
    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    if (isRcon) {
        Com_Printf("\n^3%-65.65s%-11.11s\n", "Profanity", "Date");
        Com_Printf("^7-----------------------------------------------------------------------------\n");


    }
    else {
        Q_strcat(buf, sizeof(buf), va("\n\n^3%-65.65s%-11.11s\n^7-----------------------------------------------------------------------------\n", "Profanity", "Date"));

    }

    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            char* profanity = sqlite3_column_text(stmt, 0);
            char* addedWhen = sqlite3_column_text(stmt, 1);

            if (isRcon) {
                Com_Printf("%-65.65s%-11.11s\n", profanity, addedWhen);
            }
            else {

                if (strlen(buf) + strlen(va("%-65.65s%-11.11s\n", profanity, addedWhen)) >= sizeof(buf) - 5) {
                    trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
                    Com_Memset(buf, 0, sizeof(buf));
                }

                Q_strcat(buf, sizeof(buf), va("%-65.65s%-11.11s\n", profanity, addedWhen));
            }
        }
    }

    if (!isRcon) {

        trap_SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
        trap_SendServerCommand(ent - g_entities, "print \"\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n\"");
    }
    else {
        Com_Printf("\nUse [^3Page Up^7] and [^3Page Down^7] to scroll\n");
    }

    sqlite3_finalize(stmt);

}

void dbCheckStringForProfanities(char* input, int sizeofInput) {

    sqlite3* db = gameDb;
    sqlite3_stmt* stmt;

    char* query = "SELECT profanity FROM profanitylist";

    int rc = sqlite3_prepare(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        logSystem(LOGLEVEL_WARN, "sqlite3_prepare failed on gameDb. Error: %s", sqlite3_errmsg(db));
        return;
    }

    qboolean replaced = qfalse;
    char localBuffer[MAX_SAY_TEXT];
    Q_strncpyz(localBuffer, input, sizeof(localBuffer));
    Q_CleanStr(localBuffer);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char* profanity = sqlite3_column_text(stmt, 0);
        if (profanity != NULL) {
            qboolean output = replaceProfanityWithAsterisks(localBuffer, profanity);

            if (output) {
                replaced = qtrue;
            }
        }
    }

    sqlite3_finalize(stmt);

    if (replaced) {
        Q_strncpyz(input, localBuffer, sizeofInput);
    }

}
