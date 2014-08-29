
#include "../event.h"
#include "../lightning.h"
#include "../metro.h"
#include "../slave.h"

int main(int argc, char **argv)
{
    Lightning lightning = Lightning_init(120);
    Lightning_free(&lightning);
    return 0;
}
