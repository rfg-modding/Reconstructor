Note: Development of this project is on hold to focus on [Nanoforge](https://github.com/Moneyl/Nanoforge/).

# Reconstructor
A rewrite of the [RSL (Remarstered Script Loader)](https://github.com/rsl-dev/RSL) aiming to learn from the mistakes in design and direction of RSL. Due to the amount of issues with the old codebase and how clunky it had become to develop for it was easier just to start from scratch and take a new approach. 

The rewrite was originally called RSL2. Reconstructor was chosen as the new name because it better reflects the goals of the project. The primary goals are to make the game more moddable and fix bugs. Scripting is only one part of achieving those goals.

Reconstructor uses the same code injection and function hooking code as [DashFaction](https://github.com/rafalh/dashfaction), a Red Faction 1 Community Patch.

## Reasons for a rewrite
- Long iteration times: The RSL codebase got to a point where incremental compiles could take up to 2 minutes. On top of that the game needed to be restarted for each change. This made development a long and tedious process even for small changes
- Large but mostly useless scripting API: When the scripting API was added the intent was to give modders as many tools as possible. This meant binding every member variable the C++ code could access to Lua. The intent was good but in practice this backfired. Many of variables bound to Lua had no known use, meaning it was hard for users to sort out the useful variables from the chaff. It likely also contributed to the long compile times.
- Scope creep. Given it's name RSL was originally only meant to add scripting to the game to make it more moddable. Instead it ended up being more like a trainer. Many UIs were added to it for tweaking various aspects of gameplay. These should've been added to the lua scripting API for modders to make use of.
## How the rewrite fixes these issues
- Plugin based architecture: Most of the code lies in DLLs which can be hot reloaded without needing to restart the game. This improves developer iteration times significantly. It should also help with compile times long term because any code with a long compile time could be moved into it's own DLL.
- Selective scripting API additions: Reconstructor doesn't have scripting yet, but when it's added a different approach will be taken. Instead of binding everything available to it only variables and functions with a proven use will be added. This will result in a smaller API but everything in it will have a clear and well described purpose.
- More strict guidelines on what should be included in this project, and what should be implemented by mods.
