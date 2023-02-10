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
// gt_local.h - Local definitions for gametype module.

#ifndef __GT_LOCAL_H
#define __GT_LOCAL_H

#include "../gt_shared.h"

//=============================================
//
// Global gametype definitions.
//

#define GAMETYPE_NAME               "H&S"
#define GAMETYPE_NAME_FULL          "Hide & Seek"

#define ITEM_BRIEFCASE              100
#define TRIGGER_EXTRACTION          200

//=============================================
//
// Main gametype structures.
//

typedef struct {
    int     time;

    int     caseTakenSound;
    int     caseCaptureSound;
    int     caseReturnSound;
} gametypeLocals_t;

//=============================================

//
// gt_main.c
//

extern gametypeLocals_t gametype;

extern vmCvar_t         gt_simpleScoring;

void    GT_Init         ( void );
void    GT_RunFrame     ( int time );
int     GT_Event        ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
void    GT_Shutdown     ( void );

#endif // __GT_LOCAL_H
