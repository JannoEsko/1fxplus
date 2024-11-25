// Copyright (C) 2001-2002 Raven Software.
//
// gt_local.h -- local definitions for gametype module

#include "../gt_shared.h"

#define GAMETYPE_NAME_FULL "Capture the Flag"

typedef struct gametypeLocals_s
{
	int		time;

	int		redFlagDropTime;
	int		blueFlagDropTime;

	int		flagReturnSound;
	int		flagTakenSound;
	int		flagCaptureSound;

	int		redCaptureEffect;
	int		blueCaptureEffect;

} gametypeLocals_t;

extern	gametypeLocals_t	gametype;





