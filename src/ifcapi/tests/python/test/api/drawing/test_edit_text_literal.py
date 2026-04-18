# This file was generated with the assistance of an AI coding tool.

import ifcopenshell.api.drawing
import test.bootstrap


class TestEditTextLiteral(test.bootstrap.IFC4):
    def test_run(self):
        text = self.file.createIfcTextLiteralWithExtent()
        ifcopenshell.api.drawing.edit_text_literal(
            self.file,
            text_literal=text,
            attributes={
                "Literal": "Literal",
                "Path": "RIGHT",
                "BoxAlignment": "middle",
            },
        )
        assert text.Literal == "Literal"
        assert text.Path == "RIGHT"
        assert text.BoxAlignment == "middle"


class TestEditTextLiteralIFC2X3(test.bootstrap.IFC2X3, TestEditTextLiteral):
    pass
