#include <assert.h>
#include <jansson.h>
#include <stdarg.h>
#include <stddef.h>

#include "json.h"
#include "mem.h"

Json
Json_null(void)
{
    return json_null();
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

Json
Json_object(void)
{
    return json_object();
}

char *
Json_encode(const Json obj, JsonEncodeFlags flags)
{
    return json_dumps(obj, flags);
}

int
Json_encode_fp(const Json obj, JsonEncodeFlags flags, FILE *fp)
{
    return json_dumpf(obj, fp, flags);
}

Json
Json_decode(const char *str, JsonDecodeFlags flags, JsonError error)
{
    return json_loads(str, flags, error);
}

Json
Json_pack(JsonError error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Json result = json_vpack_ex(error, 0, fmt, ap);
    va_end(ap);
    return result;
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

