# This file was generated with the assistance of an AI coding tool.

import ifcopenshell._value_api as subject


def test_python_dict_round_trips_through_native_value_api():
    data = {
        "name": "Wall",
        "count": 3,
        "active": True,
        "empty": None,
        "nested": {"height": 2.7, "tags": ["A", "B"]},
    }
    value = subject.python_to_value(None, data)
    try:
        assert subject.value_to_python(None, value, None) == data
    finally:
        subject._capi.value_destroy(value)
