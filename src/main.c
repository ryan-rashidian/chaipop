/*
 * File: main.c
 *
 * chaipop - Mini virtual machine and toolchain in C (for fun and learning)
 *
 * Licensed under the MIT License. Copyright (c) 2026 Ryan A. Rashidian
 */

#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "interpreter.h"

enum {
    CMD_NONE,
    CMD_BUILD,
    CMD_RUN,
};

static int get_command(const char *cmd)
{
    size_t len = strlen(cmd);

    if (strncmp(cmd, "build", len) == 0) return CMD_BUILD;
    if (strncmp(cmd, "run", len) == 0)   return CMD_RUN;

    return CMD_NONE;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [command] [file]\n", argv[0]);
        return 1;
    }

    int cmd = get_command(argv[1]);

    switch (cmd) {
        case CMD_BUILD: {
            compile(argv[2], "out.bin");
        } break;
        case CMD_RUN: {
            interpret(argv[2], true);
        } break;
        default: {
            fprintf(stderr, "Invalid command, pick from the following:\n"
                            "build - Compile file to bytecode\n"
                            "run - Execute bytecode file on interpreter\n");
            return 2;
        } break;
    }

    return 0;
}

