#include "includes.h"
#include "luahelpers.h"
#include "zhelpers.h"

// lua_Reader used when calling lua_load
const char *
lr(lua_State *lua_state, void *luasock, size_t *sz);

int
main(int argc, char **argv)
{
    void *ctx = zmq_ctx_new();
    void *luasock = zmq_socket(ctx, ZMQ_PULL);
    int rc = zmq_bind(luasock, "tcp://*:5557");
    assert(rc == 0);
    int error;
    lua_State *lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    int loaded;
    while (1) {
        loaded = lua_load(lua_state, lr, luasock, "lua_over_zmq", NULL);
        if (LUA_OK != loaded) {
            fprintf(stderr, "lua_load failed\n");
            print_lua_error(lua_state);
        }
        error = lua_pcall(lua_state, 0, 0, 0);
        if (error) {
            fprintf(stderr, "lua_pcall failed\n");
            print_lua_error(lua_state);
        }
    }
    zmq_close(luasock);
    zmq_ctx_destroy(ctx);
    return 0;
}

// reads lua chunks up to 64k from a zeromq socket
const char *
lr(lua_State *lua_state, void *luasock, size_t *sz)
{
    int error;
    const size_t chunksize = 65536;
    const size_t bufsize = 4096;
    unsigned char *chunk = malloc(chunksize * sizeof(unsigned char));
    size_t received = 0;
    int more = 0;

    do {
        // fetch data from zmq socket
        received += zmq_recv(luasock, chunk + received, bufsize, 0);
        if (-1 == received) {
            fprintf(stderr, "zmq_recv failed\n");
            return NULL;
        }
        // see if there are more parts
        size_t moresize = sizeof(more);
        error = zmq_getsockopt(luasock, ZMQ_RCVMORE, &more, &moresize);
    } while (more);    

    *sz = received;
    return chunk;
}
