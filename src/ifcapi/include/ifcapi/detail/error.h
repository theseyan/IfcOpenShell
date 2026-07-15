// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ERROR_H
#define IFCAPI_DETAIL_ERROR_H

#include <exception>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace ifcopenshell {
namespace capi {
void set_last_error(const std::string& message);
void set_last_error(int kind, const std::string& message);
void set_last_error(int kind, int code, const std::string& message);
int last_error_kind();
} // namespace capi
} // namespace ifcopenshell

namespace ifcapi {
namespace detail {

// Values are authored in binding_generator.abi_ir.ERROR_CATALOG. The generated
// internal C++ header statically asserts every native entry against that catalog.
enum ErrorKind {
    ERROR_NONE = 0,
    ERROR_RUNTIME = 1,
    ERROR_VALUE = 2,
    ERROR_TYPE = 3,
    ERROR_NOT_IMPLEMENTED = 4,
    ERROR_KEY = 5,
    ERROR_RECURSION = 6,
    ERROR_CANCELLED = 7,
};

enum ErrorCode {
    ERROR_CODE_NONE = 0,
    ERROR_CODE_UNSPECIFIED = 1,
    ERROR_CODE_INVALID_ARGUMENT = 2,
    ERROR_CODE_DOMAIN_ERROR = 3,
    ERROR_CODE_INVALID_QUADRANT_BEARING = 100,
    ERROR_CODE_UNSUPPORTED_RESOURCE_QUANTITY = 101,
    ERROR_CODE_INVALID_RESOURCE_QUANTITY_CLASS = 102,
    ERROR_CODE_RECURSIVE_SCHEDULE_CASCADE = 103,
    ERROR_CODE_CYCLIC_TASK_GRAPH = 104,
    ERROR_CODE_OPERATION_CANCELLED = 105,
};

class Error : public std::runtime_error {
public:
    Error(ErrorKind kind, ErrorCode code, const std::string& message)
        : std::runtime_error(message), kind_(kind), code_(code) {}

    ErrorKind kind() const noexcept { return kind_; }
    ErrorCode code() const noexcept { return code_; }

private:
    ErrorKind kind_;
    ErrorCode code_;
};

inline void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

inline void set_error(ErrorKind kind, const std::string& message) {
    ifcopenshell::capi::set_last_error(static_cast<int>(kind), message);
}

inline void set_error(ErrorKind kind, ErrorCode code, const std::string& message) {
    ifcopenshell::capi::set_last_error(static_cast<int>(kind), static_cast<int>(code), message);
}

inline void set_error(const std::exception& error, const std::string& message) {
    if (ifcopenshell::capi::last_error_kind() != ERROR_NONE) {
        return;
    }
    if (const auto* typed = dynamic_cast<const Error*>(&error)) {
        set_error(typed->kind(), typed->code(), message);
    } else if (dynamic_cast<const std::invalid_argument*>(&error)) {
        set_error(ERROR_VALUE, ERROR_CODE_INVALID_ARGUMENT, message);
    } else if (dynamic_cast<const std::domain_error*>(&error)) {
        set_error(ERROR_VALUE, ERROR_CODE_DOMAIN_ERROR, message);
    } else if (dynamic_cast<const std::bad_cast*>(&error) || dynamic_cast<const std::bad_typeid*>(&error)) {
        set_error(ERROR_TYPE, ERROR_CODE_UNSPECIFIED, message);
    } else {
        set_error(ERROR_RUNTIME, ERROR_CODE_UNSPECIFIED, message);
    }
}

inline void set_error(const std::exception& error) {
    set_error(error, error.what());
}

inline void set_error_with_context(const std::exception& error, const std::string& context) {
    set_error(error, context + error.what());
}

inline void set_error(const std::exception& error, const char* context) {
    set_error_with_context(error, context);
}

inline void set_error_from_current_exception() {
    if (ifcopenshell::capi::last_error_kind() != ERROR_NONE) {
        return;
    }
    try {
        throw;
    } catch (const std::exception& error) {
        set_error(error);
    } catch (...) {
        set_error(ERROR_RUNTIME, ERROR_CODE_UNSPECIFIED, "Unknown C++ exception");
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_ERROR_H */
