import gc

import pytest
from ifcopenshell import _ifcopenshell_capi as _capi


def test_numeric_result_exports_a_read_only_owner_retaining_buffer():
    values = _capi.placement_rotation(0.0, "Z")

    assert len(values) == 16
    assert values == tuple(values)
    assert values.tolist() == list(values)
    assert values[1:4] == tuple(values)[1:4]

    view = memoryview(values)
    assert view.format == "d"
    assert view.readonly
    expected = view.tolist()

    del values
    gc.collect()

    assert view.tolist() == expected
    with pytest.raises(TypeError):
        view[0] = 2.0
