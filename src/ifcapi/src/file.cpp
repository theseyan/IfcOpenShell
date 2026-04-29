// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include <cstdlib>

#include "ifcopenshell_api_internal.hpp"

extern "C" {

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
