# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import importlib

from .create_file import create_file

__all__ = ["create_file"]

def __getattr__(name):
    module = importlib.import_module(f".{name}", __name__)
    func = getattr(module, name)
    wrapper = lambda file, **kwargs: func(file, **kwargs)
    globals()[name] = wrapper
    return wrapper
