// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_GUID_H
#define IFCAPI_GUID_H

#include <string>

namespace ifcapi {

/// Generates a random UUID v4 as a 32-hex-character string (no dashes).
std::string uuid4_hex();

/// Compresses a 32-hex-char UUID to a 22-char IFC-encoded GlobalId.
std::string guid_compress(const std::string& uuid_hex);

/// Expands a 22-char IFC-encoded GlobalId to a 32-hex-char UUID.
std::string guid_expand(const std::string& guid);

/// Generates a new random 22-character IFC GlobalId.
std::string guid_new();

} // namespace ifcapi

#endif // IFCAPI_GUID_H
