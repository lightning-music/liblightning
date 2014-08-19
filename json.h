#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <jansson.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

typedef json_t         *Json;
typedef json_error_t   *JsonError;
typedef json_int_t      JsonInt;
typedef size_t          JsonEncodeFlags;
typedef size_t          JsonDecodeFlags;

Json
Json_null(void);

Json
Json_bool(int b);

Json
Json_int(JsonInt i);

Json
Json_real(double val);

double
Json_real_value(Json val);

Json
Json_string(const char *s);

const char *
Json_string_value(const Json string);

Json
Json_array(void);

Json
Json_object(void);

char *
Json_encode(const Json obj, JsonEncodeFlags flags);

int
Json_encode_fp(const Json obj, JsonEncodeFlags flags, FILE *fp);

Json
Json_decode(const char *str, JsonDecodeFlags flags, JsonError error);

Json
Json_pack(JsonError error, const char *fmt, ...);

void
Json_free(Json *json);

#endif
