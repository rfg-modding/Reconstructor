#pragma once
#include "common/Typedefs.h"
#include <vector>

class GuiBase;

//Manages custom reconstructor guis that get drawn on top of the game at the end of each frame.
//Uses Dear ImGui. Creates a main menu bar an dockspace as a base for other UIs to be docked onto.
class OverlayGuis
{
public:
    std::vector<GuiBase*> Guis = {};

    static OverlayGuis& Instance();
    void RegisterGui(GuiBase* gui);
    void RemoveGui(GuiBase* gui);
    void Draw();

private:
    void DrawDockspace();
    void DrawMainMenuBar();
};

class GuiBase
{
public:
    string Name;
    bool Open = false;
    bool Widget = false; //If true this gui will only been drawn when the main overlay is disabled. Meant for overlays that the player can't interact. Such as spedometers of FPS counters.

    virtual void Draw()
    {

    }

    //Use to extend the reconstructor main menu bar
    virtual void MainMenu()
    {

    }
};