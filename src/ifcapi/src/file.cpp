// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcSpfHeader.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcException.h"

#include <cstdlib>
#include <string>

// Defined in root.cpp
#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

extern "C" {

bool ifcopenshell_taxonomy_function_item_start(ifcopenshell_ifcgeom_taxonomy_item_t* item, double* out) {
    ifcopenshell_clear_error();
    if (!item || !item->ptr) { set_error("item is NULL"); return false; }
    if (!out) { set_error("out is NULL"); return false; }
    auto fn = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(item->ptr);
    if (!fn) { set_error("item is not a function_item"); return false; }
    *out = fn->start();
    return true;
}

bool ifcopenshell_taxonomy_function_item_end(ifcopenshell_ifcgeom_taxonomy_item_t* item, double* out) {
    ifcopenshell_clear_error();
    if (!item || !item->ptr) { set_error("item is NULL"); return false; }
    if (!out) { set_error("out is NULL"); return false; }
    auto fn = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(item->ptr);
    if (!fn) { set_error("item is not a function_item"); return false; }
    *out = fn->end();
    return true;
}

void ifcopenshell_free_instance_array(ifcopenshell_ifc_instance_t** arr, uint32_t count) {
    if (!arr) return;
    for (uint32_t i = 0; i < count; ++i) {
        if (arr[i]) ifcopenshell_ifc_instance_destroy(arr[i]);
    }
    std::free(arr);
}

void ifcopenshell_free_int32_array(int32_t* arr) {
    if (arr) std::free(arr);
}

} // extern "C"
