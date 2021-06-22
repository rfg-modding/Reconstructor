#include "debugDraw.h"
#include "common/windows/WindowsWrapper.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include "RFGR_Types/rfg/GeneralMover.h"
#include "RFGR_Types/rfg/RfgMover.h"
#include "RFGR_Types/rfg/World.h"
#include "RFGR_Types/rfg/Camera.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>

gr_state renderState;

void DrawRfgRbbNode(IRSL2* rsl2, object* destroyable, rfg_rbb_node* node, i32 depth = 0);

void GeneralDebugDraw_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    rfg::RfgFunctions* Functions = rsl2->GetRfgFunctions();

    //RSL2 debug overlay
    if (globalState->DrawRSLDebugOverlay)
    {
        //Salvage debug render
        if(*globalState->Salvage_debug)
        {
            Functions->salvage_render_debug();
        }

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

        //Limit how many of the rbb trees are drawn since they are performance killers
        u32 rfgMoverRbbDrawCount = 0;
        u32 maxRbbDrawCount = 1;

        //Todo: Fix range based iterators for base_array<T>
        Functions->gr_set_color(255, 255, 255, 255);
        for (u32 i = 0; i < globalState->World->all_objects.Size(); i++)
        {
            object* object = globalState->World->all_objects[i];
            if (!object)
                continue;

            if (object->obj_type == OT_HUMAN && object != globalState->Player)
            {
                human* curHuman = reinterpret_cast<human*>(object);

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

                    string positionString = "Health: " + std::to_string(curHuman->hit_points) + "/" + std::to_string(curHuman->max_hit_points);
                    int fontNum = 0;

                    Functions->gr_set_color(255, 255, 255, 255);
                    Functions->gr_bbox_aligned(&object->last_known_bmin, &object->last_known_bmax, &renderState);
                    Functions->gr_3d_string(&stringPos, &stringOrient, 0.003f, positionString.c_str(), fontNum, &renderState);

                    u32 maxPathNodeCount = 30;
                    //Draw pathfinding line of human
                    if (curHuman->pf.path)
                    {
                        path_node* curNode = curHuman->pf.path;
                        path_node* nextNode = curNode->next;
                        u32 nodeCount = 1;
                        while (nextNode && nodeCount < maxPathNodeCount)
                        {
                            //Functions->gr_sphere_solid(&curNode->pos, 1.0f, true, &renderState);
                            vector temp1 = curNode->pos + vector(0.1f, 0.1f, 0.1f);
                            Functions->gr_set_color(255, 0, 0, 255);
                            Functions->gr_bbox_solid(&curNode->pos, &temp1, &renderState);
                            Functions->gr_set_color(255, 255, 255, 255);
                            Functions->gr_3d_line(&curNode->pos, &nextNode->pos, &renderState);
                            curNode = nextNode;
                            nextNode = curNode->next;
                            nodeCount++;
                        }
                    }
                }
            }
            else if (object->obj_type == OT_TRIGGER_REGION)
            {
                float maxDrawDistance = 400.0f;
                vector distance = object->pos - globalState->MainCamera->real_pos;
                if (distance.Magnitude() <= maxDrawDistance)
                {
                    Functions->gr_set_color(0, 163, 116, 255);
                    Functions->gr_bbox_aligned(&object->last_known_bmin, &object->last_known_bmax, &renderState);
                }
            }
            else if (object->obj_type == OT_RESTRICTED_AREA)
            {
                float maxDrawDistance = 400.0f;
                vector distance = object->pos - globalState->MainCamera->real_pos;
                if (distance.Magnitude() <= maxDrawDistance)
                {
                    Functions->gr_set_color(163, 0, 0, 255);
                    Functions->gr_bbox_aligned(&object->last_known_bmin, &object->last_known_bmax, &renderState);

                }
            }
            else if (object->obj_type == OT_MOVER && object->sub_type == OT_SUB_MOVER_GENERAL)
            {
                //Get object and it's stream data
                general_mover* mover = reinterpret_cast<general_mover*>(object);
                auto* rdd = mover->rdd.GetData();
                
                //Only draw object if it's fully loaded in and within the max draw distance
                float maxDrawDistance = 100.0f;
                vector distance = object->pos - globalState->MainCamera->real_pos;
                if (rdd && rdd->shape && distance.Magnitude() <= maxDrawDistance)
                {
                    rfg_destroyable_shape* destroyableShape = rdd->shape;
                    //DrawRfgRbbNode(rsl2, object, rdd->shape->aabb_tree);
                }
            }
            else if (object->obj_type == OT_MOVER && object->sub_type == OT_SUB_MOVER_RFG)
            {
                //Get object and it's stream data
                rfg_mover* mover = reinterpret_cast<rfg_mover*>(object);
                auto* rdd = mover->rdd.GetData();

                //Only draw object if it's fully loaded in and within the max draw distance
                float maxDrawDistance = 100.0f;
                vector distance = object->pos - globalState->MainCamera->real_pos;
                if (rdd && rdd->shape && distance.Magnitude() <= maxDrawDistance && rfgMoverRbbDrawCount < maxRbbDrawCount)
                {
                    rfg_destroyable_shape* destroyableShape = rdd->shape;
                    //if (destroyableShape->calc_rbb_child_count > 1000) //Dont draw very small/simple movers
                    //{
                    //    DrawRfgRbbNode(rsl2, object, rdd->shape->aabb_tree);
                    //    rfgMoverRbbDrawCount++;
                    //}
                }
            }
        }
    }
}

//Todo: Make a general version of this for all plugins
template<class T>
T OffsetPtr(RSL2_GlobalState* globalState, unsigned long Offset)
{
    if (globalState->ModuleBase == 0)
    {
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    }
    return reinterpret_cast<T>(globalState->ModuleBase + Offset);
}

void RfgOldMemoryOverlay_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (!globalState)
        return;

    rfg::RfgFunctions* Functions = rsl2->GetRfgFunctions();

    //RFGR memory overlay, fixed and re-enabled by RSL2
    static split_memmgr* LevelMemManager = OffsetPtr<split_memmgr*>(rsl2->GetGlobalState(), 0x0195FB28);
    if (globalState->DrawRfgMemoryTracker)
    {
        Functions->memmgr_debug_render(LevelMemManager, 150);
        Functions->memmgr_debug_render_tiny(LevelMemManager, 300);
    }
}

//Todo: Grab all gr_state constants from the game like this one an put them in a header somewhere
gr_state GM_FILTER_Z = { .value = 0x0FF00405, .tnl_value = 0x20 };
void DrawRfgRbbNode(IRSL2* rsl2, object* destroyable, rfg_rbb_node* node, i32 depth)
{
    const u32 maxDepth = 6;
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    rfg::RfgFunctions* Functions = rsl2->GetRfgFunctions();

    //Stop if node is empty or max depth has been reached
    if (node->num_objects == -1 || depth > maxDepth)
        return;

    //Draw subnodes if node has zero objects
    if (node->num_objects == 0)
    {
        auto* nodeData = &node->node_data;
        DrawRfgRbbNode(rsl2, destroyable, (rfg_rbb_node*)((int)&nodeData->m_offset + nodeData->m_offset), depth + 1);
        DrawRfgRbbNode(rsl2, destroyable, (rfg_rbb_node*)((int)&nodeData[5].m_offset + nodeData->m_offset), depth + 1);
    }

    //Calculate node color and bounding box
    i32 r = depth * 20;
    i32 g = 255 - (depth * 20);
    if (r > 255)
        r = 255;
    if (g < 0)
        g = 0;

    vector bmin = vector{ (f32)node->aabb.max_x, (f32)node->aabb.max_y, (f32)node->aabb.max_z } * 0.0625f;
    vector bmax = vector{ (f32)node->aabb.min_x, (f32)node->aabb.min_y, (f32)node->aabb.min_z } * 0.0625f;
    bmin += destroyable->pos;
    bmax += destroyable->pos;

    //Draw nodes bounding box
    Functions->gr_set_color(r, g, 255, 255);
    Functions->gr_bbox_aligned(&bmin, &bmax, &GM_FILTER_Z);
}