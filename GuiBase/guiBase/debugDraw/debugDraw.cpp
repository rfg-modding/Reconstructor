#include "debugDraw.h"
#include "common/windows/WindowsWrapper.h"
#include "guiBase/ImportedFunctions.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include "RFGR_Types/rfg/World.h"
#include "RFGR_Types/rfg/Camera.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>

gr_state renderState;

void GeneralDebugDraw_DoFrame()
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    static rfg::RfgFunctions* Functions = ext_GetRfgFunctions();

    //RSL2 debug overlay
    if (globalState->DrawRSLDebugOverlay)
    {
        //String over masons head
        vector stringPos = globalState->Player->pos;// -(globalState->MainCamera->real_orient.rvec.Magnitude() * 5.0f);
        stringPos += vector(0.0f, 2.0f, 0.0f);
        vector stringOffset = (globalState->Player->last_known_bmax - globalState->Player->last_known_bmin).Scale(-0.5f);;
        stringPos.x += stringOffset.x;
        stringPos.z += stringOffset.z;
        matrix stringOrient = globalState->MainCamera->real_orient;

        string positionString = globalState->Player->pos.GetDataString(true, false);
        int fontNum = 0;

        //rfg::gr_bbox_aligned(&globalState->Player->last_known_bmin, &globalState->Player->last_known_bmax, &renderState);
        //Todo: Use camera orientation instead
        Functions->gr_3d_string(&stringPos, &stringOrient, 0.002f, positionString.c_str(), fontNum, &renderState);
        Functions->gr_bbox_aligned(&globalState->Player->last_known_bmin, &globalState->Player->last_known_bmax, &renderState);

        //Todo: Fix range based iterators for base_array<T>
        for (u32 i = 0; i < globalState->World->all_objects.Size(); i++)
        {
            Object* object = globalState->World->all_objects[i];

            if (!object || object->obj_type != OT_HUMAN || object == globalState->Player)
                continue;

            Human* human = reinterpret_cast<Human*>(object);

            float maxDrawDistance = 100.0f;
            vector distance = object->pos - globalState->MainCamera->real_pos;
            if (distance.Magnitude() <= maxDrawDistance)
            {
                //Draw string over humans head + draw their bounding box
                vector stringPos = object->pos;// -(globalState->MainCamera->real_orient.rvec.Magnitude() * 5.0f);
                stringPos += vector(0.0f, 2.0f, 0.0f);
                vector stringOffset = (object->last_known_bmax - object->last_known_bmin).Scale(-0.5f);;
                stringPos.x += stringOffset.x;
                stringPos.z += stringOffset.z;
                matrix stringOrient = globalState->MainCamera->real_orient;

                string positionString = "Health: " + std::to_string(human->hit_points) + "/" + std::to_string(human->max_hit_points);
                int fontNum = 0;

                Functions->gr_bbox_aligned(&object->last_known_bmin, &object->last_known_bmax, &renderState);
                Functions->gr_3d_string(&stringPos, &stringOrient, 0.003f, positionString.c_str(), fontNum, &renderState);

                u32 maxPathNodeCount = 100;
                //Draw pathfinding line of human
                if (human->pf.path)
                {
                    path_node* curNode = human->pf.path;
                    path_node* nextNode = curNode->next;
                    u32 nodeCount = 1;
                    while (nextNode && nodeCount < maxPathNodeCount)
                    {
                        Functions->gr_3d_line(&curNode->pos, &nextNode->pos, &renderState);
                        curNode = nextNode;
                        nextNode = curNode->next;
                        nodeCount++;
                    }
                }
            }
        }
    }
}

//Todo: Make a general version of this for all plugins
template<class T>
T OffsetPtr(unsigned long Offset)
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (globalState->ModuleBase == 0)
    {
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    }
    return reinterpret_cast<T>(globalState->ModuleBase + Offset);
}

void RfgOldMemoryOverlay_DoFrame()
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (!globalState)
        return;

    static rfg::RfgFunctions* Functions = ext_GetRfgFunctions();

    //RFGR memory overlay, fixed and re-enabled by RSL2
    static split_memmgr* LevelMemManager = OffsetPtr<split_memmgr*>(0x0195FB28);
    if (globalState->DrawRfgMemoryTracker)
    {
        Functions->memmgr_debug_render(LevelMemManager, 150);
        Functions->memmgr_debug_render_tiny(LevelMemManager, 300);
    }
}
