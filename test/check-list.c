#include <check.h>
#include <stddef.h>
#include <stdlib.h>

#include "../list.h"

START_TEST (check_List_init)
{
  List l = List_init();
  ck_assert_msg(l != NULL,
		"List_init returns non-NULL");
}
END_TEST

Suite *
list_suite(void) {
  Suite *s = suite_create("List");
  TCase *tc_init = tcase_create("init");
  tcase_add_test( tc_init, check_List_init );
  suite_add_tcase( s, tc_init );
  return s;
}

int main(void) {
  int number_failed;
  Suite *s = list_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all( sr, CK_NORMAL );
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
