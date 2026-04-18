# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def _dms2dd(degrees, minutes, seconds, us=0):
    """Convert degrees, minutes, seconds, microseconds to decimal degrees."""
    all_positive = degrees >= 0 and minutes >= 0 and seconds >= 0 and us >= 0
    all_negative = degrees <= 0 and minutes <= 0 and seconds <= 0 and us <= 0
    if not (all_positive or all_negative):
        raise ValueError("Invalid bearing string")
    return degrees + minutes / 60.0 + seconds / 3600.0 + us / 3600000000.0


def bearing2dd(bearing):
    error_msg = "Invalid bearing string"

    bearing = bearing.strip()
    bearing = " ".join(bearing.split())
    parts = bearing.split()
    n_parts = len(parts)
    if n_parts < 3 or 5 < n_parts:
        raise ValueError(error_msg)

    c_y = parts[0].upper()
    if c_y not in ("N", "S"):
        raise ValueError(error_msg)

    c_x = parts[-1].upper()
    if c_x not in ("E", "W"):
        raise ValueError(error_msg)

    d = 0
    m = 0
    s = 0.0

    if n_parts == 3:
        d = int(parts[1])
    elif n_parts == 4:
        d = int(parts[1])
        m = int(parts[2])
    elif n_parts == 5:
        d = int(parts[1])
        m = int(parts[2])
        s = float(parts[3])

    ms = 100.0 * (s - int(s))
    s = int(s)

    if d < 0 or (m < 0 or 60 <= m) or (s < 0 or 60 <= s) or ms < 0:
        raise ValueError(error_msg)

    if c_y == "N" and c_x == "E":
        angle = 90.0
        sign = -1.0
    elif c_y == "N" and c_x == "W":
        angle = 90.0
        sign = 1.0
    elif c_y == "S" and c_x == "E":
        angle = 270.0
        sign = 1.0
    elif c_y == "S" and c_x == "W":
        angle = 270.0
        sign = -1.0

    try:
        dms = _dms2dd(d, m, s, ms)
    except ValueError:
        raise ValueError(error_msg)

    if dms < 0.0 or 90.0 < dms:
        raise ValueError(error_msg)

    angle += sign * dms

    if angle == 360.0:
        angle = 0.0

    return angle
