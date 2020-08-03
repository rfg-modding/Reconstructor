#pragma once
#include "common/Typedefs.h"
#include "RFGR_Types/rfg/Vector.h"
#include "common/windows/WindowsWrapper.h"

/*
    This was done with a class in RSL1. Using this functional approach this time since it seems to work
    better in software like this and since the interface is pretty simple at the moment.
*/

//Todo: Provide an easy way to jump between custom cameras without needing to know which, if any are currently active
//Todo: Consider moving this into a class whose functions are implementation of a ICamera abstract class. Then could pass the ICamera interface to other plugins
//Todo: While I prefer the functional approach in the case of a single DLL the ICamera virtual interface approach would work nicely for cross-plugin interaction

//Directions for easy camera movement
enum MoveDirection
{
    Forward,
    Back,
    Left,
    Right,
    Up,
    Down
};


/*Public camera variables. These are public for convenience and to avoid needing many getter and setter functions.*/

//Speed used by CameraMove enum function
extern f32 CameraSpeed;
//If true the player will be teleported to the free cam position as it moves.
//This fixes some bugs that can occur when the player is very far from the camera.
extern bool TeleportPlayerToFreeCam;
//If true the player will be teleported back to their original location once the free cam is disabled.
extern bool FreeCamRememberPlayerPosition;
//Movement smoothing amount for free cam
extern f32 FreeCamSmoothingFactor;


/*General camera functions*/

//Called by RSL2_PluginInit. Initializes some values used internally by the camera patches.
extern void InitCameraPatches();
//Disables the games camera controls so we can manipulate the camera however we see fit.
extern void UnlockCamera();
//Re-enable the games camera controls. Custom cameras like the free cam won't work if the cam is locked.
extern void LockCamera();
//Toggles the camera lock
extern void ToggleCameraLock();
//Returns if the camera is locked or not. See UnlockCamera comments for more info on what that means.
extern bool IsCameraLocked();
//Conveniently translate the camera around. You can also edit the camera's state directly from RSL2_GlobalState::MainCamera.
extern void CameraMove(vector& translation);
//Move camera via direction enums
extern void CameraMove(MoveDirection direction);
//Conveniently change the camera's position. You can also edit the camera's state directly from RSL2_GlobalState::MainCamera.
extern void CameraSetPosition(vector& newPosition);

//Process keyboard & mouse input
extern void CameraProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Per frame updates to the camera
extern void CameraUpdate();