// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_Init		( void );
void	GT_RunFrame	( int time );
int		GT_Event	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
void    GT_Shutdown(void);

gametypeLocals_t	gametype;



vmCvar_t	gt_flagReturnTime;
vmCvar_t	gt_simpleScoring;

static cvarTable_t gametypeCvarTable[] = 
{
	// don't override the cheat state set by the system
	{ &gt_flagReturnTime,	"gt_flagReturnTime",	"30", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
	{ &gt_simpleScoring,	"gt_simpleScoring",		"0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
};

static int gametypeCvarTableSize = sizeof( gametypeCvarTable ) / sizeof( gametypeCvarTable[0] );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain(int command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6, intptr_t arg7, intptr_t arg8, intptr_t arg9, intptr_t arg10, intptr_t arg11)
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
			GT_Shutdown();
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
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = gametypeCvarTable ; i < gametypeCvarTableSize ; i++, cv++ ) 
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
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = gametypeCvarTable ; i < gametypeCvarTableSize ; i++, cv++ ) 
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
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register the global sounds
	gametype.flagReturnSound  = trap_Cmd_RegisterSound ( "sound/ctf_return.mp3" );
	gametype.flagTakenSound   = trap_Cmd_RegisterSound ( "sound/ctf_flag.mp3" );
	gametype.flagCaptureSound = trap_Cmd_RegisterSound ( "sound/ctf_win.mp3" );

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
	if ( gametype.redFlagDropTime && time - gametype.redFlagDropTime > gt_flagReturnTime.integer * 1000 )
	{
		trap_Cmd_ResetItem ( ITEM_REDFLAG );
		trap_Cmd_TextMessage ( -1, "The Red Flag has returned!" );
		trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
		gametype.redFlagDropTime = 0;
	}

	// See if we need to return the blue flag yet
	if ( gametype.blueFlagDropTime && time - gametype.blueFlagDropTime > gt_flagReturnTime.integer * 1000 )
	{
		trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
		trap_Cmd_TextMessage ( -1, "The Blue Flag has returned!" );
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
					trap_Cmd_TextMessage ( -1, "The Red Flag has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.redFlagDropTime = 0;
					return 1;

				case ITEM_BLUEFLAG:
					trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
					trap_Cmd_TextMessage ( -1, "The Blue Flag has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.blueFlagDropTime = 0;
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
					trap_Cmd_TextMessage ( -1, va("%s has dropped the Blue Flag!", clientname ) );
					gametype.blueFlagDropTime = time;
					break;
				
				case ITEM_REDFLAG:
					trap_Cmd_TextMessage ( -1, va("%s has dropped the Red Flag!", clientname ) );
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
						trap_Cmd_TextMessage ( -1, va("%s has taken the Blue Flag!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						gametype.blueFlagDropTime = 0;

						return 1;
					}
					break;

				case ITEM_REDFLAG:
					if ( arg2 == TEAM_BLUE )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s has taken the Red Flag!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						gametype.redFlagDropTime = 0;

						return 1;
					}
					break;
			}

			return 0;

		case GTEV_TRIGGER_TOUCHED:
			switch ( arg0 )
			{
				case TRIGGER_BLUECAPTURE:
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s has captured the Red Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_REDFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );

						if ( !gt_simpleScoring.integer )
						{
							trap_Cmd_AddClientScore ( arg1, 10 );
						}
						gametype.redFlagDropTime = 0;
						return 1;
					}
					break;

				case TRIGGER_REDCAPTURE:
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s has captured the Blue Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );

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
