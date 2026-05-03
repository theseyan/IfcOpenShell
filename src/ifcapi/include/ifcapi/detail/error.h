// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ERROR_H
#define IFCAPI_DETAIL_ERROR_H

#include <string>

namespace ifcopenshell {
namespace capi {
void set_last_error(const std::string& message);
void set_last_error(int kind, const std::string& message);
} // namespace capi
} // namespace ifcopenshell

namespace ifcapi {
namespace detail {

enum ErrorKind {
    ERROR_NONE = 0,
    ERROR_RUNTIME = 1,
    ERROR_VALUE = 2,
    ERROR_TYPE = 3,
    ERROR_NOT_IMPLEMENTED = 4,
    ERROR_KEY = 5,
};

inline void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

inline void set_error(ErrorKind kind, const std::string& message) {
    ifcopenshell::capi::set_last_error(static_cast<int>(kind), message);
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_ERROR_H */
