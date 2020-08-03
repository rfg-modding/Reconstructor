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

        
        {
            static matrix baseOrient = globalState->Player->orient; //We just need a valid, unchanging orient for this
            vector basePos = { -1175.22f, 22.0f, -1206.9f };
            vector tempVec2 = basePos + vector(0.0f, 1.0f, -10.0f);
            vector tempVec3 = basePos + vector(0.0f, 1.0f, 0.0f);
            vector tempVec4 = basePos + vector(0.0f, 1.0f, 10.5f);
            vector tempVec5 = basePos + vector(0.0f, 1.0f, 20.5f);
            vector tempVec6 = basePos + vector(0.0f, 1.0f, 30.0f);
            vector tempVec7 = basePos + vector(0.0f, 11.0f, 30.0f);
            vector tempVec2b = basePos + vector(0.0f, 11.0f, -10.0f);
            vector tempVec2b2 = tempVec2b + vector(3.0f, 3.0f, 3.0f);
            vector tempVec3b = basePos + vector(0.0f, 11.0f, 0.0f);
            vector tempVec4b = basePos + vector(0.0f, 11.0f, 10.5f);
            vector tempVec5b = basePos + vector(0.0f, 11.0f, 20.5f);

            //Functions->gr_3d_line(&globalState->Player->pos, &globalState->Player->look_at_pos, &renderState);
            //Functions->gr_sphere(&tempVec1, 1.0f, &renderState);
            //Functions->gr_sphere_solid(&tempVec1, 1.0f, true, &renderState);
            Functions->gr_set_color(255, 255, 255, 255);
            Functions->gr_set_alpha(255);

            Functions->gr_plane(&tempVec2, &baseOrient, 3.0f, 3.0f, &renderState);
            Functions->gr_axis(&tempVec3, &baseOrient, 3.0f, &renderState);
            Functions->gr_cone(&tempVec4, &baseOrient, 3.0f, 15.0f, &renderState);
            Functions->gr_cylinder(&tempVec5, &baseOrient, 3.0f, 2.5f, &renderState);
            //Functions->gr_bitmap(11, 20, 20, &renderState);
            Functions->gr_3d_bitmap(11, &tempVec6, &baseOrient, 3.0f, &renderState);
            Functions->gr_3d_bitmap(8, &tempVec7, &baseOrient, 3.0f, &renderState);
            Functions->gr_set_color(0, 255, 122, 255);
            Functions->gr_bbox_solid(&tempVec2b, &tempVec2b2, &renderState);
            Functions->gr_set_color(255, 0, 0, 255);
            Functions->gr_sphere(&tempVec3b, 3.0f, &renderState);
            Functions->gr_set_color(0, 0, 255, 255);
            Functions->gr_sphere_solid(&tempVec4b, 4.5f, true, &renderState);
            Functions->gr_set_color(0, 255, 0, 255);
            Functions->gr_3d_circle(&tempVec5b, 3.0f, { 1.0f, 0.0f, 0.0f }, true, &renderState);
            Functions->gr_arc_xz(&tempVec5b, &tempVec4b, 50.0f, &renderState);
            //Functions->gr_sphere_solid()
        }

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
