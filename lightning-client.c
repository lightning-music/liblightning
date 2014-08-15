#include <lo/lo.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *port = "41068";

static void
usage(char *prog);

static int
valid_command(char *cmd);

static int
add_directory(int argc, char **argv);

static int
load_sample(int argc, char **argv);

static int
play_sample(int argc, char **argv);

static int
export_cmd(int argc, char **argv);

static int
export_start(int argc, char **argv);

static int
export_stop(int argc, char **argv);

int main(int argc, char **argv) {
    if (argc < 2 || (!valid_command(argv[1]))) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (0 == strcmp("add", argv[1])) {
        return add_directory(argc, argv);
    } else if (0 == strcmp("load", argv[1])) {
        return load_sample(argc, argv);
    } else if (0 == strcmp("play", argv[1])) {
        return play_sample(argc, argv);
    } else if (0 == strcmp("export", argv[1])) {
        return export_cmd(argc, argv);
    } else {
        printf("unrecognized command %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    return 0;
}

static void
usage(char *prog) {
    fprintf(stderr, "Usage");
    fprintf(stderr, "$ %s command [options]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "commands\n");
    fprintf(stderr, "    add <directory>\n");
    fprintf(stderr, "    load <sample path>\n");
    fprintf(stderr, "    play <sample path> [pitch] [gain]\n");
    fprintf(stderr, "    export start|stop [filename]\n");
    fprintf(stderr, "\n");
}

static int
valid_command(char *cmd)
{
    return (0 == strcmp(cmd, "add"))
        || (0 == strcmp(cmd, "load"))
        || (0 == strcmp(cmd, "play"));
}

static int
add_directory(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "add command requires a directory\n");
    } else {
        lo_address addr = lo_address_new(NULL, port);
        const char *path = "/sample/search";
        int sent = lo_send(addr, path, "s", argv[2]);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

static int
load_sample(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "load command requires a directory\n");
    } else {
        lo_address addr = lo_address_new(NULL, port);
        const char *path = "/sample/load";
        int sent = lo_send(addr, path, "s", argv[2]);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

static int
play_sample(int argc, char **argv)
{
    if (argc < 5) {
        fprintf(stderr, "play command requires a sample name, pitch, and gain\n");
    } else {
        lo_address addr = lo_address_new(NULL, port);
        const char *path = "/sample";
        double pitch = atof(argv[3]);
        double gain = atof(argv[4]);
        int sent = lo_send(addr, path, "sff", argv[2], pitch, gain);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

static int
export_cmd(int argc, char **argv)
{
    if (argc < 3) {
        usage(argv[0]);
    } else if (0 == strcmp("start", argv[2])) {
        return export_start(argc, argv);
    } else if (0 == strcmp("stop", argv[2])) {
        return export_stop(argc, argv);
    } else {
        usage(argv[0]);
    }
    return 1;
}

static int
export_start(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "export start command requires a file name\n");
    } else {
        lo_address addr = lo_address_new(NULL, port);
        const char *path = "/export/start";
        int sent = lo_send(addr, path, "s", argv[2]);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

static int
export_stop(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "export stop command requires a sample name, pitch, and gain\n");
    } else {
        lo_address addr = lo_address_new(NULL, port);
        const char *path = "/sample";
        double pitch = atof(argv[3]);
        double gain = atof(argv[4]);
        int sent = lo_send(addr, path, "sff", argv[2], pitch, gain);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
