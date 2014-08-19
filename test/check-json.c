#include <check.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "../json.h"

START_TEST (check_Json_null)
{
    Json a = Json_null();
    Json b = Json_null();
    ck_assert_ptr_eq(a, b);
}
END_TEST

Suite *
json_suite(void) {
    Suite *s = suite_create("Json");
    TCase *tc_null = tcase_create("null");

    tcase_add_test( tc_null, check_Json_null       );

    suite_add_tcase( s, tc_null );

    return s;
}

int main(void) {
    int number_failed;
    Suite *s = json_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
