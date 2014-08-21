#include <check.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../json.h"
#include "../mem.h"

START_TEST (check_Json_null)
{
    Json a = Json_null();
    Json b = Json_null();
    ck_assert_ptr_eq(a, b);
}
END_TEST

START_TEST (check_Json_bool)
{
    Json t = Json_bool(123);
    Json f = Json_bool(0);
    ck_assert_ptr_eq( t, Json_true() );
    ck_assert_ptr_eq( f, Json_false() );
}
END_TEST

START_TEST (check_Json_int)
{
    Json x = Json_int(2);
    Json y = Json_int(4);
    Json z = Json_int(2);
    ck_assert( Json_equal(x, z) );
    ck_assert( !Json_equal(x, y) );

    JsonInt xint = Json_int_value(x);
    JsonInt yint = Json_int_value(y);
    JsonInt zint = Json_int_value(z);
    ck_assert( xint == zint );
    ck_assert( xint != yint );
}
END_TEST

START_TEST (check_Json_real)
{
    Json x = Json_real(2.1111);
    Json y = Json_real(4.7);
    Json z = Json_real(2.1111);
    ck_assert( Json_equal(x, z) );
    ck_assert( !Json_equal(x, y) );

    double xreal = Json_real_value(x);
    double yreal = Json_real_value(y);
    double zreal = Json_real_value(z);
    ck_assert( xreal == zreal );
    ck_assert( xreal != yreal );
}
END_TEST

START_TEST (check_Json_string)
{
    Json s = Json_string("foo");
    Json t = Json_string("foo");
    Json u = Json_string("bar");
    ck_assert( Json_equal(s, t) );
    ck_assert( !Json_equal(s, u) );

    const char *sstr = Json_string_value(s);
    const char *tstr = Json_string_value(t);
    const char *ustr = Json_string_value(u);
    ck_assert_str_eq( sstr, tstr );
    ck_assert_str_ne( sstr, ustr );
}
END_TEST

START_TEST (check_Json_array)
{
    Json a = Json_array();
    Json_array_append( a, Json_int(1) );
    Json_array_append( a, Json_int(2) );
    Json_array_append( a, Json_int(3) );
    Json b = Json_array();
    Json_array_append( b, Json_int(1) );
    Json_array_append( b, Json_int(2) );
    Json_array_append( b, Json_int(3) );
    Json c = Json_array();
    Json_array_append( c, Json_int(1) );
    Json_array_append( c, Json_int(2) );
    Json_array_append( c, Json_int(4) );

    ck_assert( Json_equal(a, b) );
    ck_assert( !Json_equal(a, c) );
}
END_TEST

START_TEST (check_Json_object)
{
    Json a = Json_object();
    Json_object_set( a, "foo", Json_int(1) );
    Json_object_set( a, "bar", Json_int(2) );

    Json b = Json_object();
    Json_object_set( b, "foo", Json_int(1) );
    Json_object_set( b, "bar", Json_int(2) );

    Json c = Json_object();
    Json_object_set( c, "foo", Json_int(2) );
    Json_object_set( c, "bar", Json_int(3) );

    ck_assert( Json_equal(a, b) );
    ck_assert( !Json_equal(a, c) );
}
END_TEST

START_TEST (check_Json_encode)
{
    Json obj = Json_object();
    Json arr = Json_array();

    Json_array_append( arr, Json_true() );
    Json_array_append( arr, Json_null() );

    Json_object_set( obj, "foo", Json_int(3) );
    Json_object_set( obj, "bar", arr             );

    char *str = Json_encode(obj);
    ck_assert_str_eq(str, "{\"foo\":3,\"bar\":[true,null]}");
}
END_TEST

START_TEST (check_Json_decode)
{
    Json obj1 = Json_object();
    Json_object_set( obj1, "foo", Json_int(3) );
    Json_object_set( obj1, "bar", Json_null() );
    Json obj2 = Json_decode("{\"foo\":3,\"bar\":null}", NULL);
    ck_assert( Json_equal(obj1, obj2) );
}
END_TEST

Suite *
json_suite(void) {
    Suite *s = suite_create("Json");

    /* Json_null */

    TCase *tc_null = tcase_create("null");
    tcase_add_test( tc_null, check_Json_null       );
    suite_add_tcase( s, tc_null );

    /* Json_bool */

    TCase *tc_bool = tcase_create("bool");
    tcase_add_test( tc_bool, check_Json_bool       );
    suite_add_tcase( s, tc_bool );

    /* Json_int */

    TCase *tc_int = tcase_create("int");
    tcase_add_test( tc_int, check_Json_int       );
    suite_add_tcase( s, tc_int );

    /* Json_real */

    TCase *tc_real = tcase_create("real");
    tcase_add_test( tc_real, check_Json_real       );
    suite_add_tcase( s, tc_real );

    /* Json_string */

    TCase *tc_string = tcase_create("string");
    tcase_add_test( tc_string, check_Json_string       );
    suite_add_tcase( s, tc_string );

    /* Json_array */

    TCase *tc_array = tcase_create("array");
    tcase_add_test( tc_array, check_Json_array       );
    suite_add_tcase( s, tc_array );

    /* Json_object */

    TCase *tc_object = tcase_create("object");
    tcase_add_test( tc_object, check_Json_object       );
    suite_add_tcase( s, tc_object );

    /* Json_encode */

    TCase *tc_encode = tcase_create("encode");
    tcase_add_test( tc_encode, check_Json_encode       );
    suite_add_tcase( s, tc_encode );

    /* Json_decode */

    TCase *tc_decode = tcase_create("decode");
    tcase_add_test( tc_decode, check_Json_decode       );
    suite_add_tcase( s, tc_decode );

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
