// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <stdio.h>

int ifcapi_run_root_smoke_tests(void);
int ifcapi_run_selector_smoke_tests(void);
int ifcapi_run_pset_smoke_tests(void);
int ifcapi_run_style_smoke_tests(void);
int ifcapi_run_material_smoke_tests(void);
int ifcapi_run_unit_smoke_tests(void);
int ifcapi_run_resource_smoke_tests(void);
int ifcapi_run_nest_smoke_tests(void);
int ifcapi_run_geometry_smoke_tests(void);
int ifcapi_run_alignment_smoke_tests(void);
int ifcapi_run_pset_template_cogo_library_smoke_tests(void);
int ifcapi_run_edit_wrapper_smoke_tests(void);
int ifcapi_run_error_contract_smoke_tests(void);

int main(void) {
    int failures = 0;

    failures += ifcapi_run_root_smoke_tests();
    failures += ifcapi_run_selector_smoke_tests();
    failures += ifcapi_run_pset_smoke_tests();
    failures += ifcapi_run_style_smoke_tests();
    failures += ifcapi_run_material_smoke_tests();
    failures += ifcapi_run_unit_smoke_tests();
    failures += ifcapi_run_resource_smoke_tests();
    failures += ifcapi_run_nest_smoke_tests();
    failures += ifcapi_run_geometry_smoke_tests();
    failures += ifcapi_run_alignment_smoke_tests();
    failures += ifcapi_run_pset_template_cogo_library_smoke_tests();
    failures += ifcapi_run_edit_wrapper_smoke_tests();
    failures += ifcapi_run_error_contract_smoke_tests();

    if (failures) {
        printf("ifcapi C smoke test suites failed: %d\n", failures);
        return 1;
    }

    printf("ifcapi C smoke test suites passed\n");
    return 0;
}
