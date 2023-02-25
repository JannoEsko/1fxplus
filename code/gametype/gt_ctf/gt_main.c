/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors
Copyright (C) 2017, SoF2Plus contributors

This file is part of the SoF2Plus source code.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
// gt_main.c - Main gametype module routines.

#include "gt_local.h"

gametypeLocals_t    gametype;

vmCvar_t            gt_flagReturnTime;
vmCvar_t            gt_simpleScoring;
vmCvar_t            gt_flagCaptureType;
vmCvar_t            gt_flagAutoReturn;
vmCvar_t            gt_playerCanReturnFlag;

static cvarTable_t gametypeCvarTable[] =
{
    // don't override the cheat state set by the system
    { &gt_flagReturnTime,   "gt_flagReturnTime",    "30", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &gt_simpleScoring,    "gt_simpleScoring",     "0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &gt_flagCaptureType,    "gt_flagCaptureType",     "0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &gt_flagAutoReturn,    "gt_flagAutoReturn",     "1",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { &gt_playerCanReturnFlag,    "gt_playerCanReturnFlag",     "0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
    { NULL, NULL, NULL, 0, 0.0f, 0.0f, 0, qfalse },
};

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6, intptr_t arg7, intptr_t arg8, intptr_t arg9, intptr_t arg10, intptr_t arg11 )
{
    switch ( command )
    {
        case GAMETYPE_INIT:
            GT_Init ( );
            return 0;

        case GAMETYPE_START:
            trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlag );
            trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlag );
            return 0;

        case GAMETYPE_RUN_FRAME:
            GT_RunFrame ( arg0 );
            return 0;

        case GAMETYPE_EVENT:
            return GT_Event ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );

        case GAMETYPE_SHUTDOWN:
            GT_Shutdown ( );
            return 0;
    }

    return -1;
}

/*
=================
GT_RegisterCvars
=================
*/
void GT_RegisterCvars( void )
{
    cvarTable_t *cv;

    for ( cv = gametypeCvarTable ; cv->cvarName != NULL; cv++ )
    {
        trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags, cv->mMinValue, cv->mMaxValue );

        if ( cv->vmCvar )
        {
            cv->modificationCount = cv->vmCvar->modificationCount;
        }
    }
}

/*
=================
GT_UpdateCvars
=================
*/
void GT_UpdateCvars( void )
{
    cvarTable_t *cv;

    for ( cv = gametypeCvarTable ; cv->cvarName != NULL; cv++ )
    {
        if ( cv->vmCvar )
        {
            trap_Cvar_Update( cv->vmCvar );

            if ( cv->modificationCount != cv->vmCvar->modificationCount )
            {
                cv->modificationCount = cv->vmCvar->modificationCount;
            }
        }
    }
}

/*
================
GT_Init

initializes the gametype by spawning the gametype items and
preparing them
================
*/
void GT_Init ( void )
{
    gtItemDef_t     itemDef;
    gtTriggerDef_t  triggerDef;

    GT_Printf("----- Gametype Initialization -----\n");
    GT_Printf("gametype: %s (%s)\n", GAMETYPE_NAME, GAMETYPE_NAME_FULL);

    memset ( &gametype, 0, sizeof(gametype) );

    // Register the global sounds
    gametype.flagReturnSound  = trap_Cmd_RegisterSound ( "sound/ctf_return.mp3" );
    gametype.flagTakenSound   = trap_Cmd_RegisterSound ( "sound/ctf_flag.mp3" );
    gametype.flagCaptureSound = trap_Cmd_RegisterSound ( "sound/ctf_win.mp3" );

    gametype.iconRedFlag = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_red" );
    gametype.iconBlueFlag = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_blue" );

    gametype.iconRedFlagDropped = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_red_dropped" );
    gametype.iconBlueFlagDropped = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_blue_dropped" );

    gametype.iconRedFlagCarried = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_red_carried" );
    gametype.iconBlueFlagCarried = trap_Cmd_RegisterIcon ( "gfx/menus/hud/ctf_blue_carried" );

    // Register all cvars for this gametype
    GT_RegisterCvars ( );

    // Register the items
    memset ( &itemDef, 0, sizeof(itemDef) );
    trap_Cmd_RegisterItem ( ITEM_REDFLAG,  "red_flag", &itemDef );
    trap_Cmd_RegisterItem ( ITEM_BLUEFLAG, "blue_flag", &itemDef );

    // Register the triggers
    memset ( &triggerDef, 0, sizeof(triggerDef) );
    trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
    trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );

    gametype.blueFlagAtSpawn = qtrue;
    gametype.redFlagAtSpawn = qtrue;

    trap_Cvar_VariableStringBuffer("g_redTeamPrefix", gametype.redTeamPrefix, sizeof(gametype.redTeamPrefix));
    trap_Cvar_VariableStringBuffer("g_blueTeamPrefix", gametype.blueTeamPrefix, sizeof(gametype.blueTeamPrefix));
}

/*
================
GT_RunFrame

Runs all thinking code for gametype
================
*/
void GT_RunFrame ( int time )
{
    gametype.time = time;

    // See if we need to return the red flag yet
    if ( gametype.redFlagDropTime && time - gametype.redFlagDropTime > gt_flagReturnTime.integer * 1000 && gt_flagAutoReturn.integer )
    {
        trap_Cmd_ResetItem ( ITEM_REDFLAG );
        trap_Cmd_TextMessage ( -1, va("The %s Flag has returned!" , gametype.redTeamPrefix));
        trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlag );
        trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
        gametype.redFlagDropTime = 0;
    }

    // See if we need to return the blue flag yet
    if ( gametype.blueFlagDropTime && time - gametype.blueFlagDropTime > gt_flagReturnTime.integer * 1000 && gt_flagAutoReturn.integer )
    {
        trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
        trap_Cmd_TextMessage ( -1, va("The %s Flag has returned!" , gametype.blueTeamPrefix) );
        trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlag );
        trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
        gametype.blueFlagDropTime = 0;
    }

    GT_UpdateCvars ( );
}

/*
================
GT_Event

Handles all events sent to the gametype
================
*/
int GT_Event ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
    switch ( cmd )
    {
        case GTEV_ITEM_DEFEND:
            if ( !gt_simpleScoring.integer )
            {
                trap_Cmd_AddClientScore ( arg1, 5 );
            }
            return 0;

        case GTEV_ITEM_STUCK:
            switch ( arg0 )
            {
                case ITEM_REDFLAG:
                    trap_Cmd_ResetItem ( ITEM_REDFLAG );
                    trap_Cmd_TextMessage ( -1, va("The %s Flag has returned!", gametype.redTeamPrefix) );
                    trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
                    gametype.redFlagDropTime = 0;
                    trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlag );
                    gametype.redFlagAtSpawn = qtrue;
                    return 1;

                case ITEM_BLUEFLAG:
                    trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
                    trap_Cmd_TextMessage ( -1, va("The %s Flag has returned!", gametype.blueTeamPrefix) );
                    trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
                    gametype.blueFlagDropTime = 0;
                    trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlag );
                    gametype.blueFlagAtSpawn = qtrue;
                    return 1;
            }

            break;

        case GTEV_ITEM_DROPPED:
        {
            char clientname[MAX_QPATH];

            trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );

            switch ( arg0 )
            {
                case ITEM_BLUEFLAG:
                    trap_Cmd_TextMessage ( -1, va("%s has dropped the %s Flag!", clientname, gametype.blueTeamPrefix ) );
                    trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlagDropped );
                    gametype.blueFlagDropTime = time;
                    break;

                case ITEM_REDFLAG:
                    trap_Cmd_TextMessage ( -1, va("%s has dropped the %s Flag!", clientname, gametype.redTeamPrefix ) );
                    trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlagDropped );
                    gametype.redFlagDropTime = time;
                    break;
            }
            break;
        }

        case GTEV_ITEM_TOUCHED:

            switch ( arg0 )
            {
                case ITEM_BLUEFLAG:
                    if ( arg2 == TEAM_RED )
                    {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has taken the %s Flag!", clientname, gametype.blueTeamPrefix ) );
                        trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
                        trap_Cmd_RadioMessage ( arg1, "got_it" );
                        trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlagCarried );
                        gametype.blueFlagAtSpawn = qfalse;
                        gametype.blueFlagDropTime = 0;

                        return 1;
                    } 
                    else if (arg2 == TEAM_BLUE && gt_playerCanReturnFlag.integer) {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has returned the %s Flag!", clientname, gametype.blueTeamPrefix ) );
                        trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
                        trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
                        gametype.blueFlagDropTime = 0;
                        trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlag );
                        gametype.blueFlagAtSpawn = qtrue;
                        return 0;
                    }
                    break;

                case ITEM_REDFLAG:
                    if ( arg2 == TEAM_BLUE )
                    {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has taken the %s Flag!", clientname, gametype.redTeamPrefix ) );
                        trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
                        trap_Cmd_RadioMessage ( arg1, "got_it" );
                        trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlagCarried );
                        gametype.redFlagAtSpawn = qfalse;
                        gametype.redFlagDropTime = 0;

                        return 1;
                    }
                    else if (arg2 == TEAM_RED && gt_playerCanReturnFlag.integer) {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has returned the %s Flag!", clientname, gametype.redTeamPrefix ) );
                        trap_Cmd_ResetItem ( ITEM_REDFLAG );
                        trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
                        gametype.redFlagDropTime = 0;
                        trap_Cmd_SetHUDIcon ( 1, gametype.iconRedFlag );
                        gametype.redFlagAtSpawn = qtrue;
                        return 0;
                    }
                    break;
            }

            return 0;

        case GTEV_TRIGGER_TOUCHED:
            switch ( arg0 )
            {
                case TRIGGER_BLUECAPTURE:
                    if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ) && (!gt_flagCaptureType.integer || (gt_flagCaptureType.integer && gametype.redFlagAtSpawn)) )
                    {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has captured the %s Flag!", clientname, gametype.redTeamPrefix ) );
                        trap_Cmd_ResetItem ( ITEM_REDFLAG );
                        trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
                        trap_Cmd_AddTeamScore ( arg2, 1 );
                        trap_Cmd_SetHUDIcon ( 0, gametype.iconRedFlag );

                        if ( !gt_simpleScoring.integer )
                        {
                            trap_Cmd_AddClientScore ( arg1, 10 );
                        }
                        gametype.redFlagDropTime = 0;
                        return 1;
                    }
                    break;

                case TRIGGER_REDCAPTURE:
                    if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) && (!gt_flagCaptureType.integer || (gt_flagCaptureType.integer && gametype.blueFlagAtSpawn)) )
                    {
                        char clientname[MAX_QPATH];
                        trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
                        trap_Cmd_TextMessage ( -1, va("%s has captured the %s Flag!", clientname, gametype.blueTeamPrefix ) );
                        trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
                        trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
                        trap_Cmd_AddTeamScore ( arg2, 1 );
                        trap_Cmd_SetHUDIcon ( 1, gametype.iconBlueFlag );

                        if ( !gt_simpleScoring.integer )
                        {
                            trap_Cmd_AddClientScore ( arg1, 10 );
                        }

                        gametype.blueFlagDropTime = 0;
                        return 1;
                    }
                    break;
            }

            return 0;
    }

    return 0;
}

/*
================
GT_Shutdown

Shutdown gametype and cleanup
resources, if necessary.
================
*/

void GT_Shutdown(void)
{
    GT_Printf("%s gametype shutdown.\n", GAMETYPE_NAME_FULL);
}
