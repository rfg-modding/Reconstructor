#include "GlobalState.h"

Reconstructor_GlobalState GlobalData;

Reconstructor_GlobalState* GetGlobalState()
{
    return &GlobalData;
}
