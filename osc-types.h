#ifndef OSC_TYPES_H_INCLUDED
#define OSC_TYPES_H_INCLUDED

#include <lo/lo_osc_types.h>
#include <lo/lo_types.h>

typedef lo_arg OscArgument;

typedef lo_message OscMessage;

typedef lo_method OscMethod;

/**
 * OSC message attributes
 */
typedef struct OscAttributes *OscAttributes;

/**
 * Alias of lo_err_handler
 */
typedef void (* OscErrorHandler)(int num,
                                 const char *msg,
                                 const char *where);

/**
 * Alias of lo_method_handler
 */
typedef int (* OscMethodHandler)(const char *path,
                                 const char *types,
                                 OscArgument **argv,
                                 int argc,
                                 OscMessage msg,
                                 void *data);

#endif
