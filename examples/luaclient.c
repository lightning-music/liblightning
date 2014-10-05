#include "includes.h"
#include "luahelpers.h"
#include "zhelpers.h"

#define CALL_SELF 0

// lua_Writer function passed to lua_dump
static int
lw(lua_State *lua_state, const void *p, size_t sz, void *luasock);

int
main(int argc, char **argv)
{
    int error;
    void *ctx = zmq_ctx_new();
    void *luasock = zmq_socket(ctx, ZMQ_PUSH);
    int rc = zmq_connect(luasock, "tcp://localhost:5557");
    assert(rc == 0);
    // TODO: compile and send some lua bytecode
    lua_State *lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    unsigned char buff[4096];
    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        /* push a compiled chunk onto the stack */
        error = luaL_loadstring(lua_state, buff);
        if (error) {
            fprintf(stderr, "lua_loadstring failed\n");
            print_lua_error(lua_state);
        }
        /* dump compiled chunk to writer */
        error =  lua_dump(lua_state, lw, luasock);
        if (error) {
            fprintf(stderr, "lua_dump failed\n");
            print_lua_error(lua_state);
        }
        if (CALL_SELF) {
            error = lua_pcall(lua_state, 0, 0, 0);
            if (error) {
                fprintf(stderr, "lua_pcall failed\n");
                print_lua_error(lua_state);
            }
        }
   }
    // cleanup
    zmq_close(luasock);
    zmq_ctx_destroy(ctx);
    return 0;
}

/*
  If this function feeds me pieces of chunks, how do
  I know when I've been given the whole chunk and it is
  ready to send over the socket?
 */
static int
lw(lua_State *lua_state, const void *p, size_t sz, void *luasock)
{
    /* flag to mark the end of the chunk */
    /* as of lua 5.2.3, end of chunk always seems to be
       { 0x5F, 0x45, 0x4E, 0x56, 0x00, } */
    const unsigned char *buf = (const unsigned char *) p;

    int end = 0;
    if (sz == 5) {
        if (buf[0] == 0x5f && buf[1] == 0x45 && buf[2] == 0x4e
            && buf[3] == 0x56 && buf[4] == 0x00) {
            end = 1;
        }
    }

    /* size_t i; */
    /* printf("unsigned char sent_bytes[] = { "); */
    /* for (i = 0; i < sz; i++) { */
    /*     printf("0x%02X, ", ((unsigned char *)p)[i]); */
    /* } */
    /* printf("}\n"); */

    size_t sent;
    if (end) {
        /* printf("done sending\n"); */
        sent = zmq_send(luasock, p, sz, 0);
    } else {
        // SNDMORE
        /* printf("sndmore\n"); */
        sent = zmq_send(luasock, p, sz, ZMQ_SNDMORE);
        return 0;
    }

    if (sent < sz) {
        fprintf(stderr, "Could not send entire chunk\n");
        return sz - sent;
        return 1;
    }
    return 0;
}
