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
// gt_shared.h - Definitions included by all gametype modules.

#ifndef __GT_SHARED_H
#define __GT_SHARED_H

#include "../qcommon/q_shared.h"
#include "gt_public.h"
#include "gt_syscalls.h"

//=============================================

typedef struct {
    vmCvar_t* vmCvar;
    char* cvarName;
    char* defaultString;
    int         cvarFlags;
    float       mMinValue, mMaxValue;
    int         modificationCount;  // for tracking changes
    qboolean    trackChange;        // track this variable, and announce if changed
    qboolean    teamShader;         // track and if changed, update shader state
} cvarTable_t;

//=============================================

//
// gt_shared.c
//

void QDECL GT_Printf(const char* fmt, ...)    __attribute__((format(printf, 1, 2)));
void QDECL GT_Error(const char* fmt, ...)    __attribute__((noreturn, format(printf, 1, 2)));

#endif // __GT_SHARED_H
