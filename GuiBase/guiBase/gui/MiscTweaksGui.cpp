#include "MiscTweaksGui.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include "rsl2/functions/Functions.h"
#include "KnownCues.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <vector>

int lastSoundPlayed = 0;
void PlaySound(const char* name, rfg::RfgFunctions* functions)
{
    int cue_id = 0;
    audiolib_result result = functions->audiolib_cue_get_id(name, &cue_id);
    if (result != AUDIOLIB_ERR_NONE)
    {
        printf("Error encountered in PlaySound() when trying to find cue \"%s\". Error code: %d\n", name, (int)result);
        return;
    }
    int playId = (audiolib_result)functions->audiolib_cue_play(cue_id, nullptr, &result);
    if (result != AUDIOLIB_ERR_NONE)
    {
        printf("Error encountered in PlaySound() when trying to play cue \"%s\". Error code: %d. Return value: %d\n", name, (int)result, (int)playId);
        return;
    }
    lastSoundPlayed = playId;
}

int GetWeaponInfoIndex(const string& name, RSL2_GlobalState* globalState)
{
    if (!globalState->WeaponInfos.Initialized())
        return 0;

    for (int i = 0; i < globalState->WeaponInfos.Size(); i++)
    {
        if (name == globalState->WeaponInfos[i].name)
            return i;
    }

    return 0;
}

//Todo: Move into UI utility namespace or Imgui namespace
const ImVec4 SecondaryTextColor(0.32f, 0.67f, 1.0f, 1.00f); //Light blue;
static void LabelAndValue(const std::string& Label, const std::string& Value, bool formatted = true)
{
    if (formatted)
    {
        ImGui::Text(Label.c_str());
        ImGui::SameLine();
        ImGui::TextColored(SecondaryTextColor, Value.c_str());
    }
    else
    {
        ImGui::TextUnformatted(Label.c_str());
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, SecondaryTextColor);
        ImGui::TextUnformatted(Value.c_str());
        ImGui::PopStyleColor();
    }
}

void MiscTweaksGui_DoFrame(IRSL2* rsl2, bool* open)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    if (!ImGui::Begin("Debug gui", open))
    {
        ImGui::End();
        return;
    }

    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_BUG " Debug");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::InputFloat3("Player position", (f32*)&globalState->Player->pos);

    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_AUDIO_DESCRIPTION " Audio");
    ImGui::PopFont();
    ImGui::Separator();

    rfg::RfgFunctions* Functions = rsl2->GetRfgFunctions();


    static string input;
    ImGui::InputText("Custom sound name", &input);
    ImGui::SameLine();
    if (ImGui::Button("Play custom sound"))
    {
        PlaySound(input.c_str(), Functions);
    }
    if (ImGui::Button("Stop last sound"))
    {
        Functions->audiolib_play_fade_out(lastSoundPlayed, 100);
    }

    if (ImGui::CollapsingHeader("Known sounds"))
    {
        ImGui::BeginChild("##Sound presets list", ImVec2(500, 300), true);
        for (const char* presetName : SoundPresets)
        {
            if (ImGui::Button(presetName))
            {
                PlaySound(presetName, Functions);
            }
        }
        ImGui::EndChild();
    }


    ImGui::Separator();
    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_PLANE_ARRIVAL " Air bombs test");
    ImGui::PopFont();
    ImGui::Separator();

    static air_bomb_info customAirBomb;
    static bool firstRun = true;
    static u32 targetObjectHandle = 0xFFFFFFFF;
    static int lastAirBomb = -1;
    static int projectileInfoIndex = 0;
    if (firstRun)
    {
        //Todo: Use OffsetPtr<T> here
        //Todo: Come up with better way of initing these things post RSL2.dll init so their values aren't null like they might be at game start
        globalState->NumAirBombInfos = reinterpret_cast<u32*>(globalState->ModuleBase + 0x01E28610);
        auto airBombInfosPtr = reinterpret_cast<air_bomb_info**>(globalState->ModuleBase + 0x01E2860C);
        globalState->AirBombInfos.Init(*airBombInfosPtr, *globalState->NumAirBombInfos, *globalState->NumAirBombInfos, "AirBombInfos");

        globalState->NumWeaponInfos = reinterpret_cast<u32*>(globalState->ModuleBase + 0x03481C94);
        auto weaponInfosPtr = reinterpret_cast<weapon_info**>(globalState->ModuleBase + 0x03481C9C);
        globalState->WeaponInfos.Init(*weaponInfosPtr, *globalState->NumWeaponInfos, *globalState->NumWeaponInfos, "WeaponInfos");

        customAirBomb = globalState->AirBombInfos[0];
        projectileInfoIndex = GetWeaponInfoIndex(globalState->AirBombInfos[0].name, globalState);
        firstRun = false;
    }

    ImGui::PushItemWidth(250.0f);
    ImGui::InputScalar("Target handle", ImGuiDataType_U32, &targetObjectHandle);
    ImGui::SameLine();
    if (ImGui::Button("Player aim"))
    {
        targetObjectHandle = globalState->Player->aim_target;
    }
    ImGui::SameLine();
    if (ImGui::Button("Player handle"))
    {
        targetObjectHandle = globalState->Player->handle;
    }
    if (ImGui::Button("Start air bomb"))
    {
        lastAirBomb = Functions->air_bomb_start(&customAirBomb, targetObjectHandle, globalState->Player->handle, false);
    }
    if (ImGui::Button("Stop last air bomb"))
    {
        if (lastAirBomb != -1)
        {
            Functions->air_bomb_stop((air_bomb_handle)lastAirBomb);
        }
    }
    if (ImGui::Button("Stop all air bombs"))
    {
        Functions->air_bomb_stop_all();
    }

    //Todo: Add projectile spawner
    if (ImGui::TreeNode("Spawn settings"))
    {
        ImGui::Separator();
        ImGui::PushFont(globalState->FontLarge);
        ImGui::Text("Air bomb info:");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushItemWidth(250.0f);
        ImGui::Text("name: %s", customAirBomb.name);
        //Todo: Add weapon_info combo here. Presets don't use any interesting projectiles
        static ImGuiComboFlags flags = 0;
        ImGui::CheckboxFlags("ImGuiComboFlags_PopupAlignLeft", (unsigned int*)&flags, ImGuiComboFlags_PopupAlignLeft);
        ImGui::SameLine();
        if (ImGui::CheckboxFlags("ImGuiComboFlags_NoArrowButton", (unsigned int*)&flags, ImGuiComboFlags_NoArrowButton))
            flags &= ~ImGuiComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (ImGui::CheckboxFlags("ImGuiComboFlags_NoPreview", (unsigned int*)&flags, ImGuiComboFlags_NoPreview))
            flags &= ~ImGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static const char* item_current = globalState->WeaponInfos[projectileInfoIndex].name;            // Here our selection is a single pointer stored outside the object.
        if (ImGui::BeginCombo("Projectile", item_current, flags)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < globalState->WeaponInfos.Size(); n++)
            {
                if (globalState->WeaponInfos[n].ammo_type != WEAPON_AMMO_TYPE_PROJECTILE)
                    continue;

                bool is_selected = (item_current == globalState->WeaponInfos[n].name);
                if (ImGui::Selectable(globalState->WeaponInfos[n].name, is_selected))
                {
                    customAirBomb.w_info = &globalState->WeaponInfos[n];
                    item_current = globalState->WeaponInfos[n].name;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("num_projectiles", &customAirBomb.num_projectiles);
        ImGui::InputFloat("min_spread", &customAirBomb.min_spread);
        ImGui::InputFloat("max_spread", &customAirBomb.max_spread);
        ImGui::InputFloat("min_projectile_delay", &customAirBomb.min_projectile_delay);
        ImGui::InputFloat("max_projectile_delay", &customAirBomb.max_projectile_delay);
        ImGui::InputFloat("height", &customAirBomb.height);
        ImGui::InputFloat("radius", &customAirBomb.radius);
        ImGui::InputFloat("veh_radius", &customAirBomb.veh_radius);
        ImGui::InputFloat("rate_min", &customAirBomb.rate_min);
        ImGui::InputFloat("rate_max", &customAirBomb.rate_max);
        ImGui::InputFloat("projectile_travel_time", &customAirBomb.projectile_travel_time);
        ImGui::InputFloat("angle_min", &customAirBomb.angle_min);
        ImGui::InputFloat("angle_max", &customAirBomb.angle_max);
        ImGui::InputFloat("min_player_fvec_dot_prod", &customAirBomb.min_player_fvec_dot_prod);

        ImGui::TreePop();
    }

    if (globalState->AirBombInfos.Initialized() && ImGui::TreeNode("Preset air bombs"))
    {
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 260.0f);
        for (int i = 0; i < globalState->AirBombInfos.Size(); i++)
        {
            ImGui::BulletText("%s", globalState->AirBombInfos[i].name);
            ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::SetColumnWidth(1, 180.0f);

            if (ImGui::Button((string("Copy values##") + std::to_string(i)).c_str()))
            {
                customAirBomb = globalState->AirBombInfos[i];
                projectileInfoIndex = GetWeaponInfoIndex(globalState->AirBombInfos[i].name, globalState);
            }

            ImGui::NextColumn();
        }
        ImGui::TreePop();
    }
    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_WRENCH " Misc tweaks");
    ImGui::PopFont();
    ImGui::Separator();

    //Game speed multiplier
    ImGui::SetNextItemWidth(230.0f);
    ImGui::InputFloat("Game speed scale", globalState->SpeedScale);
    ImGui::SameLine();
    if (ImGui::Button("Reset##ResetButtonSpeedScale"))
        *globalState->SpeedScale = 1.0f;

    if (ImGui::Button("0.01")) { *globalState->SpeedScale = 0.01f; } ImGui::SameLine();
    if (ImGui::Button("0.1")) { *globalState->SpeedScale = 0.1f; } ImGui::SameLine();
    if (ImGui::Button("0.5")) { *globalState->SpeedScale = 0.5f; } ImGui::SameLine();
    if (ImGui::Button("1.0")) { *globalState->SpeedScale = 1.0f; } ImGui::SameLine();
    if (ImGui::Button("1.5")) { *globalState->SpeedScale = 1.5f; } ImGui::SameLine();
    if (ImGui::Button("2.0")) { *globalState->SpeedScale = 2.0f; } ImGui::SameLine();
    if (ImGui::Button("10.0")) { *globalState->SpeedScale = 10.0f; }

    //ImGui::SameLine();
    //Gui::ShowHelpMarker("1.0 = vanilla, # < 1.0 = slo-mo, # > 1.0 = \"fast-mo\"");

    //Stream containers & primitives
    if (ImGui::CollapsingHeader("Stream containers"))
    {
        stream2_container* container = globalState->Stream_containers;
        u32 i = 0;
        while (container)
        {
            //Container linked list is circular
            if (container == globalState->Stream_containers && i != 0)
                break;

            string containerName;
            if (container->name)
                containerName += container->name;
            else
                containerName += "Container (" + std::to_string((u32)container->name) + ")";

            ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 1.7f); //Increase spacing to differentiate leaves from expanded contents.
            if (ImGui::TreeNode(containerName.c_str()))
            {
                //General state
                LabelAndValue("Ref count:", std::to_string(container->ref_count));
                LabelAndValue("Num entries:", std::to_string(container->num_entries));
                LabelAndValue("Flags:", std::to_string(container->flags));
                LabelAndValue("Priority:", std::to_string(container->cur_priority));
                LabelAndValue("Category:", std::to_string(container->cur_category)); //Todo: Convert enum to string

                //Container definition
                if (container->cdef && ImGui::TreeNode("Definition"))
                {
                    stream2_container_def* def = container->cdef;
                    LabelAndValue("Name:", def->name);
                    LabelAndValue("Mode:", std::to_string(def->mode)); //Todo: Convert enum to string
                    LabelAndValue("Container type:", std::to_string(def->container_type)); //Todo: Convert enum to string
                    LabelAndValue("Allocator index:", std::to_string(def->allocator_idx)); //Todo: Convert enum to string
                    ImGui::TreePop();
                }

                //Entries
                if (ImGui::TreeNode("Entries"))
                {
                    if (container->entries)
                    {
                        for (u32 j = 0; j < container->num_entries; j++)
                        {
                            stream2_container::entry* entry = container->entries + j;
                            const bool hasPrimitive = (entry && entry->prim);

                            if (hasPrimitive) //Draw node with primitive name if one is present
                            {
                                stream2_prim* prim = entry->prim;
                                if (ImGui::TreeNode(prim->name))
                                {
                                    LabelAndValue("Name:", prim->name);
                                    LabelAndValue("Cpu size:", std::to_string(prim->cpu_size));
                                    LabelAndValue("Gpu size:", std::to_string(prim->gpu_size));
                                    LabelAndValue("Split ext index:", std::to_string(prim->split_ext_index));
                                    LabelAndValue("Flags:", std::to_string(prim->flags)); //Todo: Convert enum to string
                                    LabelAndValue("Allocator index:", std::to_string(prim->allocator_idx));
                                    LabelAndValue("Ref count:", std::to_string(prim->ref_count));
                                    LabelAndValue("Container ref count:", std::to_string(prim->container_ref_count));

                                    if (ImGui::TreeNode("Entry data"))
                                    {
                                        LabelAndValue("Enable: ", std::to_string(entry->enable));
                                        LabelAndValue("User0: ", std::to_string(entry->user0));
                                        LabelAndValue("User1: ", std::to_string(entry->user1));
                                        LabelAndValue("Flags: ", std::to_string(entry->flags));
                                        ImGui::TreePop();
                                    }

                                    ImGui::TreePop();
                                }
                            }
                            else //Draw node with entry data if there's no primitive
                            {
                                string entryLabel = "Entry " + std::to_string(j) + " (No primitive)";
                                if (ImGui::TreeNode(entryLabel.c_str()))
                                {
                                    LabelAndValue("Enable: ", std::to_string(entry->enable));
                                    LabelAndValue("User0: ", std::to_string(entry->user0));
                                    LabelAndValue("User1: ", std::to_string(entry->user1));
                                    LabelAndValue("Flags: ", std::to_string(entry->flags));
                                    ImGui::TreePop();
                                }
                            }
                        }
                    }
                    else
                    {
                        ImGui::Text(ICON_FA_EXCLAMATION_CIRCLE " No entries");
                    }

                    ImGui::TreePop();
                }


                ImGui::TreePop();
            }
            ImGui::PopStyleVar(); //Pop tree node spacing style var
            container = container->next;
            i++;
        }
    }

    ImGui::End();
}

void DebugOverlay_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    static bool p_open = true;

    // FIXME-VIEWPORT: Select a default viewport
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_area_pos = viewport->WorkPos;   // Instead of using viewport->Pos we use GetWorkPos() to avoid menu bars, if any!
        ImVec2 work_area_size = viewport->WorkSize;
        ImVec2 window_pos = ImVec2((corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) :
            (work_area_pos.x + DISTANCE), (corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
    }
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;

    //if (ImGui::Begin("Example: Simple overlay", &p_open, window_flags))
    //{
    //    ImGui::Text("FPS: %.2f", io.Framerate);
    //    ImGui::Separator();
    //    if (globalState->Player)
    //        ImGui::Text("Player Position: (%.2f,%.2f, %.2f)", globalState->Player->pos.x, globalState->Player->pos.y, globalState->Player->pos.z);
    //    else
    //        ImGui::Text("Player Position: <invalid>");

    //    if (ImGui::BeginPopupContextWindow())
    //    {
    //        if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
    //        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
    //        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
    //        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
    //        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
    //        if (p_open && ImGui::MenuItem("Close")) p_open = false;
    //        ImGui::EndPopup();
    //    }
    //}
    //ImGui::End();
}