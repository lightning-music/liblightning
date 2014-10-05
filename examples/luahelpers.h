
static void
print_lua_error(lua_State *lua_state)
{
    fprintf(stderr, "LUA ERROR => %s\n", lua_tostring(lua_state, -1));
    // pop error message from the stack
    lua_pop(lua_state, 1);
    exit(1);
}
