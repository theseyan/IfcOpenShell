# IfcOpenShell - IFC toolkit and geometry engine
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi


def append_asset(
    file: ifcopenshell.file,
    library: ifcopenshell.file,
    element: ifcopenshell.entity_instance,
    reuse_identities: dict[object, ifcopenshell.entity_instance] | None = None,
    assume_asset_uniqueness_by_name: bool = True,
) -> ifcopenshell.entity_instance | None:
    """Append a supported asset using the native project API.

    The public dictionary remains compatible with the upstream Python API.
    Native code owns the reusable mapping while this wrapper only seeds and
    synchronizes that mapping around the generated C call.
    """
    cache = _capi.project_append_asset_cache_new()
    preserved_identities = {}
    try:
        if reuse_identities is not None:
            for source_identity, target in reuse_identities.items():
                if not (
                    isinstance(source_identity, tuple)
                    and len(source_identity) == 3
                    and source_identity[0] == id(library)
                ):
                    preserved_identities[source_identity] = target
                    continue
                try:
                    source = library.by_id(int(source_identity[1]))
                    if source.is_a() != source_identity[2]:
                        continue
                    _capi.project_append_asset_cache_set(
                        cache,
                        source._handle,
                        getattr(target, "_handle", target),
                    )
                except (RuntimeError, TypeError, ValueError):
                    continue
        result = _capi.project_append_asset(
            file._handle,
            {
                "library": library._handle,
                "element": element._handle,
                "cache": cache,
                "assume_asset_uniqueness_by_name": assume_asset_uniqueness_by_name,
            },
        )
        entries = _capi.project_append_asset_cache_entries(cache)
        if reuse_identities is not None:
            reuse_identities.clear()
            reuse_identities.update(preserved_identities)
            reuse_identities.update(
                (
                    (id(library), source_id, source_type),
                    ifcopenshell.entity_instance(file, target),
                )
                for source_id, source_type, target in zip(
                    entries.source_ids,
                    entries.source_types,
                    entries.targets,
                )
            )
        return ifcopenshell.entity_instance(file, result) if result else None
    finally:
        _capi.project_append_asset_cache_destroy(cache)
