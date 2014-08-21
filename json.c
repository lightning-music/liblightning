#include <assert.h>
#include <jansson.h>
#include <stdarg.h>
#include <stddef.h>

#include "json.h"
#include "mem.h"

typedef enum {
    JsonCompact = JSON_COMPACT
} JsonEncodeFlag;

typedef enum {
    JsonRejectDuplicates = JSON_REJECT_DUPLICATES
} JsonDecodeFlag;

Json
Json_null(void)
{
    return json_null();
}

Json
Json_true(void)
{
    return json_true();
}

Json
Json_false(void)
{
    return json_false();
}

Json
Json_bool(int b)
{
    return json_boolean(b);
}

Json
Json_int(JsonInt i)
{
    return json_integer(i);
}

JsonInt
Json_int_value(Json val)
{
    return json_integer_value(val);
}

Json
Json_real(double val)
{
    return json_real(val);
}

double
Json_real_value(Json val)
{
    return json_real_value(val);
}

Json
Json_string(const char *s)
{
    return json_string(s);
}

const char *
Json_string_value(const Json string)
{
    return json_string_value(string);
}

Json
Json_array(void)
{
    return json_array();
}

int
Json_array_append(Json array, Json el)
{
    return json_array_append_new(array, el);
}

Json
Json_object(void)
{
    return json_object();
}

int
Json_object_set(Json obj, const char *key, Json value)
{
    return json_object_set_new(obj, key, value);
}

char *
Json_encode(const Json obj)
{
    return json_dumps(obj, JSON_COMPACT);
}

int
Json_encode_fp(const Json obj, FILE *fp)
{
    return json_dumpf(obj, fp, JSON_COMPACT);
}

Json
Json_decode(const char *str, JsonError error)
{
    return json_loads(str, JSON_REJECT_DUPLICATES, error);
}

/* Json */
/* Json_pack(JsonError error, const char *fmt, ...) */
/* { */
/*     va_list ap; */
/*     va_start(ap, fmt); */
/*     Json result = json_vpack_ex(error, 0, fmt, ap); */
/*     va_end(ap); */
/*     return result; */
/* } */

int
Json_equal(Json x, Json y)
{
    return json_equal(x, y);
}

/**
 * Is this useful?
 */
void
Json_free(Json *json)
{
    size_t refcount = (*json)->refcount;
    size_t i;
    for (i = 0; i < refcount; i++) {
        json_decref(*json);
    }
}

