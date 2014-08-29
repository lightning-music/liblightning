#include <check.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "../metro.h"
#include "../types.h"

int
metro_callback(position_t pos, void *data) {
    return 0;
}

START_TEST (check_Metro_init)
{
    Metro m = Metro_init(120);
    ck_assert_msg(m != NULL,
                  "Metro_init returns non-NULL");
    Metro_free(&m);
}
END_TEST

START_TEST (check_Metro_start)
{
    Metro m = Metro_init(120);
    ck_assert_msg(m != NULL,
                  "Metro_init returns non-NULL");
    Metro_start(m);
    Metro_free(&m);
}
END_TEST

START_TEST (check_Metro_stop)
{
    Metro m = Metro_init(120);
    ck_assert_msg(m != NULL,
                  "Metro_init returns non-NULL");
    Metro_start(m);
    sleep(2);
    Metro_stop(m);
    Metro_free(&m);
}
END_TEST

Suite *
metro_suite(void) {
    Suite *s = suite_create("Metro");
    TCase *tc_init = tcase_create("init");
    TCase *tc_start = tcase_create("start");
    TCase *tc_stop = tcase_create("stop");

    tcase_add_test( tc_init,  check_Metro_init       );
    tcase_add_test( tc_start, check_Metro_start      );
    tcase_add_test( tc_stop,  check_Metro_stop       );

    suite_add_tcase( s, tc_init  );
    suite_add_tcase( s, tc_start );
    suite_add_tcase( s, tc_stop  );

    return s;
}

int main(void) {
    int number_failed;
    Suite *s = metro_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
