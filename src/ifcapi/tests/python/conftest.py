# SPDX-License-Identifier: LGPL-3.0-or-later

import sys
import os

# Ensure our native ifcopenshell package is on the path (before any system-installed one)
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))
