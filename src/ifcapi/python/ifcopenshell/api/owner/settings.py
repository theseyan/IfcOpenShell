# SPDX-License-Identifier: LGPL-3.0-or-later

"""Owner settings used by test bootstrap fixtures and API calls."""

import ifcopenshell


def get_application(ifc):
    app = next(iter(ifc.by_type("IfcApplication")), None)
    if not app and ifc.schema == "IFC2X3":
        raise Exception(
            "Please create an application to continue. See the owner.create_owner_history docs for more info."
            "https://docs.ifcopenshell.org/autoapi/ifcopenshell/api/owner/create_owner_history/index.html"
        )
    return app


def get_user(ifc):
    pao = next(iter(ifc.by_type("IfcPersonAndOrganization")), None)
    if not pao and ifc.schema == "IFC2X3":
        raise Exception(
            "Please create a user to continue. See the owner.create_owner_history docs for more info."
            "https://docs.ifcopenshell.org/autoapi/ifcopenshell/api/owner/create_owner_history/index.html"
        )
    return pao


get_application_factory = get_application
get_application_backup = get_application
get_user_factory = get_user
get_user_backup = get_user


def factory_reset():
    global get_application_factory
    global get_application_backup
    global get_application
    global get_user_factory
    global get_user_backup
    global get_user
    get_application_backup = get_application
    get_application = get_application_factory
    get_user_backup = get_user
    get_user = get_user_factory


def restore():
    global get_application_factory
    global get_application_backup
    global get_application
    global get_user_factory
    global get_user_backup
    global get_user
    get_application = get_application_backup
    get_user = get_user_backup
