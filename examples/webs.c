
#include "../web-server.h"

int main(int argc, char **argv)
{
    WebServer ws = WebServer_init(3890);
    WebServer_listen(ws);
    WebServer_free(&ws);
}
