/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_REPRESENTATION_H
#define IFCAPI_BINDINGS_REPRESENTATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Return a geometric representation context matching the given criteria.
 *
 * Searches for IfcGeometricRepresentationSubContext when subcontext or
 * target_view is provided, otherwise searches for
 * IfcGeometricRepresentationContext (excluding subcontexts).
 *
 * @param file The IFC file to search.
 * @param context_type Context type filter (e.g. "Model", "Plan").
 * @param subcontext Context identifier filter (e.g. "Body", "Axis").
 * @param target_view Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW").
 * @return The first matching context, or no result if none is found.
 */
IFCAPI_BINDING express::Base representation_get_context(
    ifcopenshell::file* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view);

/**
 * Resolve a representation through single mapped items.
 *
 * If a representation contains a single IfcMappedItem whose
 * MappingSource points to another representation, this function
 * follows the chain and returns the innermost representation. A representation
 * that does not meet this condition is returned unchanged.
 *
 * @param representation The IfcShapeRepresentation to resolve.
 * @return The resolved representation, or the original when no mapping is followed.
 */
IFCAPI_BINDING express::Base representation_resolve(express::Base* representation);

/**
 * Options for getting a product's representation filtered by context.
 */
struct RepresentationGetProductRepresentationOptions {
    /// Specific context entity to match against. When set, overrides context_type, subcontext, and target_view.
    std::optional<express::Base> context;
    /// Context type filter (e.g. "Model", "Plan"). Ignored when context is set.
    std::optional<std::string> context_type;
    /// Subcontext identifier filter (e.g. "Body", "Axis"). Ignored when context is set.
    std::optional<std::string> subcontext;
    /// Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW"). Ignored when context is set.
    std::optional<std::string> target_view;
};

/**
 * Return a product's representation filtered by context.
 *
 * For an IfcProduct, reads Representation/Representations. For an
 * IfcTypeProduct, reads RepresentationMaps/MappedRepresentation.
 * Returns the first representation matching the context filters.
 *
 * @param element The IfcProduct or IfcTypeProduct.
 * @param options Context filtering options.
 * @return The matching IfcShapeRepresentation, or no result if none is found.
 */
IFCAPI_BINDING express::Base representation_get_product_representation(
    express::Base* element,
    const RepresentationGetProductRepresentationOptions& options);

/**
 * Return the base items of a representation, unwrapping mapped items and boolean operands.
 *
 * Returns leaf-level representation items in traversal order. Within each
 * representation, later items are returned before earlier items; for boolean
 * results, the second operand is returned before the first operand.
 *
 * @param representation The IfcShapeRepresentation to resolve.
 * @return List of leaf-level IfcRepresentationItem entities.
 */
IFCAPI_BINDING std::vector<express::Base> representation_resolve_base_items(express::Base* representation);

/**
 * Return all geometric representation contexts sorted by priority.
 *
 * Sorts by ContextType (Model > Plan > Annotation), then by
 * ContextIdentifier (Body > Body-FallBack > ...), then by
 * TargetView (MODEL_VIEW > PLAN_VIEW > ...), then by TargetScale. Ties
 * preserve the order of contexts in the IFC file.
 *
 * @param file The IFC file to search.
 * @return Ordered list of IfcGeometricRepresentationContext entities.
 */
IFCAPI_BINDING std::vector<express::Base> representation_get_prioritised_contexts(ifcopenshell::file* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_REPRESENTATION_H */
