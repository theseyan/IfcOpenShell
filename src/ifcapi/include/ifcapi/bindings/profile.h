// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROFILE_H
#define IFCAPI_BINDINGS_PROFILE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Options for creating an arbitrary closed profile from a polyline.
 */
struct ProfileAddArbitraryProfileOptions {
    /// Ordered XYZ or XY points defining the closed outer curve, in SI metres.
    std::vector<std::vector<double>> profile;
    /// Optional profile name. When omitted, the profile name is empty.
    std::optional<std::string> name;
};

/**
 * Options for creating an arbitrary closed profile with inner voids.
 */
struct ProfileAddArbitraryProfileWithVoidsOptions {
    /// Ordered XYZ or XY points defining the outer curve, in SI metres.
    std::vector<std::vector<double>> outer_profile;
    /// Inner void curves, each as ordered XY or XYZ points in SI metres.
    std::vector<std::vector<std::vector<double>>> inner_profiles;
    /// Optional profile name. When omitted, the profile name is empty.
    std::optional<std::string> name;
};

/**
 * Create a parameterized IFC profile definition.
 *
 * @param file IFC file that receives the profile.
 * @param ifc_class IFC entity class name (e.g. "IfcRectangleProfileDef").
 * @param profile_type Profile type string, typically "AREA".
 * @return Newly created IfcProfileDef entity.
 */
IFCAPI_BINDING express::Base profile_add_parameterized_profile(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const std::string& profile_type);

/**
 * Create an IfcArbitraryClosedProfileDef from an ordered polyline.
 *
 * Points are converted from SI metres to project units before creating the
 * outer curve. The curve representation is chosen automatically (IfcPolyline
 * for IFC2X3, IfcIndexedPolyCurve otherwise).
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve points and optional name.
 * @return Newly created IfcArbitraryClosedProfileDef.
 */
IFCAPI_BINDING express::Base profile_add_arbitrary_profile(
    ifcopenshell::file* file,
    const ProfileAddArbitraryProfileOptions& options);

/**
 * Create an IfcArbitraryProfileDefWithVoids from an outer curve and inner voids.
 *
 * Points are converted from SI metres to project units. The outer curve uses
 * IfcCartesianPointList3D for IFC4+ schemas and IfcPolyline for IFC2X3.
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve, inner void curves, and optional name.
 * @return Newly created IfcArbitraryProfileDefWithVoids.
 */
IFCAPI_BINDING express::Base profile_add_arbitrary_profile_with_voids(
    ifcopenshell::file* file,
    const ProfileAddArbitraryProfileWithVoidsOptions& options);

/**
 * Copy a profile and its associated IfcProfileProperties.
 *
 * @param file IFC file that receives the copied profile.
 * @param profile IfcProfileDef entity to copy.
 * @return Newly created independent copy of the profile.
 */
IFCAPI_BINDING express::Base profile_copy_profile(
    ifcopenshell::file* file,
    express::Base* profile);

/**
 * Edit attributes of an existing profile definition.
 *
 * @param profile IfcProfileDef entity to modify.
 * @param attributes Attribute name-to-value mapping.
 */
IFCAPI_BINDING void profile_edit_profile(
    express::Base* profile,
    ifcopenshell_pset_props_t* attributes);

/**
 * Remove a profile definition and its directly referenced sub-entities.
 *
 * Removes associated IfcProfileProperties first, then removes the profile
 * entity and removes unreferenced entities belonging to its direct geometry,
 * such as curves and placements.
 *
 * @param file IFC file to modify.
 * @param profile IfcProfileDef entity to remove.
 */
IFCAPI_BINDING void profile_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROFILE_H */
