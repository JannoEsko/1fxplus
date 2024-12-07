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

static cvarTable_t gametypeCvarTable[] =
{
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
    GT_Printf("----- Gametype Initialization -----\n");
    GT_Printf("gametype: %s (%s)\n", GAMETYPE_NAME, GAMETYPE_NAME_FULL);

    memset ( &gametype, 0, sizeof(gametype) );

    // Register all cvars for this gametype
    GT_RegisterCvars ( );
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
        case GTEV_TEAM_ELIMINATED:
            switch ( arg0 )
            {
                case TEAM_RED:
                    trap_Cmd_TextMessage ( -1, "Red team eliminated!" );
                    trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
                    trap_Cmd_Restart ( 5 );
                    break;

                case TEAM_BLUE:
                    trap_Cmd_TextMessage ( -1, "Blue team eliminated!" );
                    trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
                    trap_Cmd_Restart ( 5 );
                    break;
            }
            break;

        case GTEV_TIME_EXPIRED:
            trap_Cmd_TextMessage ( -1, "Round Draw!" );
            trap_Cmd_Restart ( 5 );
            break;
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
