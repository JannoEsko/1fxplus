// Copyright (C) 2001-2002 Raven Software.
//
// gt_local.h -- local definitions for gametype module

#include "../gt_shared.h"

#define GAMETYPE_NAME_FULL "Infiltration"

typedef struct gametypeLocals_s
{
	int		time;

	int		caseTakenSound;
	int		caseCaptureSound;
	int		caseReturnSound;

} gametypeLocals_t;

extern	gametypeLocals_t	gametype;

