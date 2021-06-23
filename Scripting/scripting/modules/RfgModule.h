#pragma once
#include "common/Typedefs.h"
#include <pybind11/embed.h>
#include "RFGR_Types/rfg/Vector.h"
#include "RFGR_Types/rfg/World.h"
#include "RFGR_Types/rfg/Object.h"


RSL2_GlobalState* Scripting_GlobalState;

f32 TestFunc()
{
    return 2.0f;
}

PYBIND11_EMBEDDED_MODULE(rfg, m) {
    m.doc() = "rfg module"; // optional module docstring

    m.def("TestFunc", &TestFunc);

    pybind11::class_<vector>(m, "vector")
        .def_readwrite("x", &vector::x)
        .def_readwrite("y", &vector::y)
        .def_readwrite("z", &vector::z);

    pybind11::enum_<object_type>(m, "object_type")
        .value("Undefined", object_type::OT_UNDEFINED)
        .value("Human", object_type::OT_HUMAN)
        .value("VehicleSpawnNode", object_type::OT_VEHICLE_SPAWN_NODE)
        .export_values();

    pybind11::enum_<object_sub_type>(m, "object_sub_type")
        .value("Undefined", object_sub_type::OT_SUB_UNDEFINED)
        .value("Npc", object_sub_type::OT_SUB_HUMAN_NPC)
        .value("Player", object_sub_type::OT_SUB_HUMAN_PLAYER)
        .export_values();

    pybind11::class_<object>(m, "object")
        .def_readwrite("obj_type", &object::obj_type)
        .def_readwrite("sub_type", &object::sub_type)
        .def_readwrite("last_known_bmin", &object::last_known_bmin)
        .def_readwrite("last_known_bmax", &object::last_known_bmax);

    pybind11::class_<base_array<object*>>(m, "base_array<object*>")
        .def("Capacity", &base_array<object*>::Capacity)
        .def("__len__", &base_array<object*>::Size)
        .def("__getitem__",
            [](base_array<object*>& s, size_t i) {
                if (i >= s.Size())
                    throw pybind11::index_error();

                return s[i];
            }, pybind11::return_value_policy::reference)
        .def("__setitem__",
            [](base_array<object*>& s, size_t i, object* v) {
                if (i >= s.Size())
                    throw pybind11::index_error();
                
                s[i] = v;
            });

    pybind11::class_<world>(m, "world")
        .def_readwrite("all_objects", &world::all_objects);

    m.def("GetWorld", []() { return Scripting_GlobalState->World;  }, pybind11::return_value_policy::reference);
    //m.attr("ActiveWorld") = Scripting_GlobalState->World;
}
