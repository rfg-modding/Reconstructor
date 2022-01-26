#pragma once
#include "common/Typedefs.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Vector.h"
#include "RFGR_Types/rfg/World.h"
#include "RFGR_Types/rfg/Object.h"
#include "rsl2/IRSL2.h"
#include "scripting/misc/GlobalState.h"
#define SOL_ALL_SAFETIES 1
#include <sol/sol.hpp>

void InitRfgLuaModule(sol::state& lua)
{
    sol::usertype<vector> vectorType = lua.new_usertype<vector>("vector",
        sol::constructors<vector(f32, f32, f32), vector()>(),
        "x", &vector::x,
        "y", &vector::y,
        "z", &vector::z);

    sol::usertype<object> objectType = lua.new_usertype<object>("object",
        "obj_type", &object::obj_type,
        "sub_type", &object::sub_type,
        "last_known_bmin", &object::last_known_bmin,
        "last_known_bmax", &object::last_known_bmax);

    sol::usertype<base_array<object*>> baseArrayObjectPtrType = lua.new_usertype<base_array<object*>>("base_array<object*>",
        "new", sol::no_constructor,
        "Length", &base_array<object*>::Length,
        "Size", &base_array<object*>::Size,
        "Capacity", &base_array<object*>::Capacity,
        sol::meta_function::index, &base_array<object*>::operator[]);

    sol::usertype<world> worldType = lua.new_usertype<world>("world",
        "new", sol::no_constructor,
        "tech_level", &world::tech_level,
        "all_objects", sol::readonly(&world::all_objects));

    lua.set_function("GetActiveWorld", []() { return rsl2_->GetGlobalState()->World; });
}