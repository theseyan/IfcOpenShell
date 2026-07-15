# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ifcopenshell.api
import pytest


def test_invalid_call_arguments_receive_enhanced_signature_diagnostic():
    def usecase(value):
        return value

    wrapped = ifcopenshell.api.wrap_usecase("test.usecase", usecase)
    with pytest.raises(TypeError) as error:
        wrapped(unexpected=True)

    assert "Incorrect function arguments provided for test.usecase" in str(error.value)
    assert "Correct signature is (value)" in str(error.value)


def test_internal_type_error_is_propagated_unchanged():
    internal_error = TypeError("usecase() deliberately resembles a call-signature error")

    def usecase(value):
        raise internal_error

    wrapped = ifcopenshell.api.wrap_usecase("test.usecase", usecase)
    with pytest.raises(TypeError) as error:
        wrapped(1)

    assert error.value is internal_error
