#pragma once
#include "RFGR_Types/rfg/DrawData.h"
#include "rsl2/IRSL2.h"

extern gr_state renderState;

extern void GeneralDebugDraw_DoFrame(IRSL2* rsl2);
extern void RfgOldMemoryOverlay_DoFrame(IRSL2* rsl2);