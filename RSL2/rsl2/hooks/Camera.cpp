#include "Camera.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/math/Math.h"
#include "RFGR_Types/rfg/Camera.h"
#include "RFGR_Types/rfg/Player.h"

//Speed used by CameraMove enum function
f32 CameraSpeed = 0.3f;
//If true the player will be teleported to the free cam position as it moves.
//This fixes some bugs that can occur when the player is very far from the camera.
bool TeleportPlayerToFreeCam = true;
//If true the player will be teleported back to their original location once the free cam is disabled.
bool FreeCamRememberPlayerPosition = false;
//Movement smoothing amount for free cam
f32 FreeCamSmoothingFactor = 0.125f;


//Addresses of patches used to lock/unlock the camera. Set by InitCameraPatches
u32 Patch1Address = 0;
u32 Patch2Address = 0;
//Whether or not the camera is locked
bool CameraLocked = true;
//Target position of the free camera that we lerp towards each frame. Creates smooth camera movement
vector CameraTargetPos = vector(0.0f, 0.0f, 0.0f);

//Pointer to the RSL2 global state. Used for convenience so GetGlobalState only needs to be called once here.
RSL2_GlobalState* globalState = nullptr;

//Todo: Move these two free cam functions into a script
//Ran when the free cam is enabled and disabled respectively
void OnFreeCamEnable();
void OnFreeCamDisable();


void InitCameraPatches()
{
    globalState = GetGlobalState();
    Patch1Address = globalState->ModuleBase + 0x2E0454;
    Patch2Address = globalState->ModuleBase + 0x2E0494;
}

void UnlockCamera()
{
    if (!CameraLocked)
    {
        printf("Warning. Attempted to unlock the camera when it's already unlocked.\n");
        return;
    }

    //Patch camera_do_frame so the game doesn't edit the camera's position
    globalState->SnippetManager.BackupSnippet("CameraLockPatch1", Patch1Address, 8, true);
    globalState->SnippetManager.BackupSnippet("CameraLockPatch2", Patch2Address, 6, true);
    CameraLocked = false;
    OnFreeCamEnable(); //Todo: Separate free cam behavior from locking/unlocking behavior
}

void LockCamera()
{
    if (CameraLocked)
    {
        printf("Warning. Attempted to lock the camera when it's already locked.\n");
        return;
    }

    //Remove camera_do_frame patches to return normal camera position behavior
    globalState->SnippetManager.RestoreSnippet("CameraLockPatch1", true);
    globalState->SnippetManager.RestoreSnippet("CameraLockPatch2", true);
    CameraLocked = true;
    OnFreeCamDisable(); //Todo: Separate free cam behavior from locking/unlocking behavior
}

void ToggleCameraLock()
{
    if (CameraLocked)
        UnlockCamera();
    else
        LockCamera();
}

bool IsCameraLocked()
{
    return CameraLocked;
}

void CameraMove(vector& translation)
{
    CameraTargetPos += translation;
}

void CameraMove(MoveDirection direction)
{
    switch (direction)
    {
    case Forward:
        CameraMove(globalState->MainCamera->real_orient.fvec * CameraSpeed);
        break;
    case Back:
        CameraMove(globalState->MainCamera->real_orient.fvec * -CameraSpeed);
        break;
    case Left:
        CameraMove(globalState->MainCamera->real_orient.rvec * -CameraSpeed);
        break;
    case Right:
        CameraMove(globalState->MainCamera->real_orient.rvec * CameraSpeed);
        break;
    case Up:
        CameraMove(globalState->MainCamera->real_orient.uvec * CameraSpeed);
        break;
    case Down:
        CameraMove(globalState->MainCamera->real_orient.uvec * -CameraSpeed);
        break;
    }
}

void CameraSetPosition(vector& newPosition)
{
    globalState->MainCamera->real_pos = newPosition;
}


//Todo: Add InputManager that tracks this and provides callbacks easy input handling
static bool w_down = false;
static bool a_down = false;
static bool s_down = false;
static bool d_down = false;
static bool q_down = false;
static bool e_down = false;
static bool z_down = false;
static bool x_down = false;

void CameraProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //Track key up/down state so camera movement doesn't halt when changing directions
    //Note: This is temporary, will add class / function which tracks this for us later
    if (msg == WM_KEYDOWN)
    {
        switch (wParam)
        {
            //If camera is unlocked then pass input to it. Currently the free cam is enabled automatically if the camera is unlocked.
            //The plan is to separate them eventually so other custom cameras can be added more easily.
        case VK_NUMPAD3:
            ToggleCameraLock(); //Todo: This currently toggles the lock and the free cam. Separate these
            break;
        case 0x57: //w key
            w_down = true;
            break;
        case 0x53: //s key
            s_down = true;
            break;
        case 0x44: //d key
            d_down = true;
            break;
        case 0x41: //a key
            a_down = true;
            break;
        case 0x5A: //z
            z_down = true;
            break;
        case 0x58: //x
            x_down = true;
            break;
        case 0x51: //q
            q_down = true;
            break;
        case 0x45: //e
            e_down = true;
            break;
        }
    }
    if (msg == WM_KEYUP)
    {
        switch (wParam)
        {
        case 0x57: //w key
            w_down = false;
            break;
        case 0x53: //s key
            s_down = false;
            break;
        case 0x44: //d key
            d_down = false;
            break;
        case 0x41: //a key
            a_down = false;
            break;
        case 0x5A: //z
            z_down = false;
            break;
        case 0x58: //x
            x_down = false;
            break;
        case 0x51: //q
            q_down = false;
            break;
        case 0x45: //e
            e_down = false;
            break;
        }
    }
}

//Temporary function for handling input using key up/down state tracked in CameraProcessInput
void UpdateCameraInput()
{
    if (w_down)
        CameraMove(Forward);
    if (s_down)
        CameraMove(Back);
    if (d_down)
        CameraMove(Right);
    if (a_down)
        CameraMove(Left);
    if (z_down)
        CameraMove(Down);
    if (x_down)
        CameraMove(Up);
    if (q_down)
        CameraSpeed -= 0.02f;
    if (e_down)
        CameraSpeed += 0.02f;
}

void CameraUpdate()
{
    if (CameraLocked || !globalState || !globalState->Player || !rfg::human_teleport_unsafe)
        return;

    UpdateCameraInput();

    if (TeleportPlayerToFreeCam)
        rfg::human_teleport_unsafe(globalState->Player, globalState->MainCamera->real_pos + vector(0.0f, 1.5f, 0.0f), globalState->Player->orient);

    //Set flags so AI ignores player and doesn't kill them while they're in the free cam
    globalState->Player->hflags.ai_ignore = true;
    globalState->Player->hflags.invulnerable = true;
    globalState->Player->hit_points = 2147483647.0f;

    //Lerp between real position and target position of camera for smooth movement
    globalState->MainCamera->real_pos = math::Lerp(globalState->MainCamera->real_pos, CameraTargetPos, FreeCamSmoothingFactor);
}

vector preFreeCamActivationsPlayerPosition = { 0.0f, 0.0f, 0.0f };
void OnFreeCamEnable()
{
    if (FreeCamRememberPlayerPosition)
        preFreeCamActivationsPlayerPosition = globalState->Player->pos;

    CameraTargetPos = globalState->MainCamera->real_pos;
}

void OnFreeCamDisable()
{
    //Todo: Make this "smart" so it only sets these values based on their values before the free cam was activated
    globalState->Player->hflags.ai_ignore = false;
    globalState->Player->hflags.invulnerable = false;
    globalState->Player->hit_points = globalState->Player->max_hit_points;

    if (FreeCamRememberPlayerPosition)
        rfg::human_teleport_unsafe(globalState->Player, preFreeCamActivationsPlayerPosition, globalState->Player->orient);
}