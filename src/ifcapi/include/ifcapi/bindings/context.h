/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTEXT_H
#define IFCAPI_BINDINGS_CONTEXT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a geometric representation context or subcontext.
 */
struct ContextAddContextOptions {
    /// Context type string (e.g. "Plan", "Model", or a custom label).
    std::string context_type;
    /// Context identifier (e.g. "Model View", "Design Transfer View"). Used only for subcontexts.
    std::string context_identifier;
    /// Target view for subcontexts (e.g. "MODEL_VIEW", "PLAN_VIEW", "SKETCH_VIEW").
    std::string target_view;
    /// Optional target scale for subcontexts. Omitted if not set.
    std::optional<double> target_scale;
    /// Optional parent context. If provided, creates an IfcGeometricRepresentationSubContext;
    /// if omitted, creates an IfcGeometricRepresentationContext attached to the IfcProject.
    std::optional<express::Base> parent;
};

/**
 * Create a geometric representation context or subcontext.
 *
 * When parent is omitted, creates an IfcGeometricRepresentationContext with
 * a world coordinate system and attaches it to the IfcProject's
 * RepresentationContexts. A context_type of "Plan" creates a 2D context
 * (coordinate space dimension 2); all other values create a 3D context.
 *
 * When parent is provided, creates an IfcGeometricRepresentationSubContext
 * as a child of the given parent context.
 */
IFCAPI_BINDING express::Base context_add_context(
    ifcopenshell::file* file,
    const ContextAddContextOptions& options);

/**
 * Remove a geometric representation context and its subcontexts recursively.
 *
 * For subcontexts, references from IfcCoordinateOperation entities are
 * deep-removed; other referencing entities are redirected to the parent
 * context. For top-level contexts, representations using the context are
 * unassigned from their elements and removed.
 */
IFCAPI_BINDING void context_remove_context(ifcopenshell::file* file, express::Base* context);

/**
 * Edit attributes of an existing geometric representation context.
 *
 * Applies attribute key-value pairs from the props builder to the context entity.
 */
IFCAPI_BINDING void context_edit_context(
    ifcopenshell::file* file,
    express::Base* context,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTEXT_H */
