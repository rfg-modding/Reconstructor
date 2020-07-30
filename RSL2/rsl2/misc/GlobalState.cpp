#include "GlobalState.h"

RSL2_GlobalState GlobalData;

RSL2_GlobalState* GetGlobalState()
{
    return &GlobalData;
}
