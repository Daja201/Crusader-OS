#include <stdio.h>
#include "commands.h"
#include <stddef.h>

typedef void (*command_func_t)(const char* args);

typedef struct {
    const char* name;
    command_func_t func;
    const char* desc;
} Command;

// ==== Definice příkazů ====

static void cmd_help(const char* args);
static void cmd_echo(const char* args);
static void cmd_clear(const char* args);
static void cmd_about(const char* args);

static Command commands[] = {
    {"help", cmd_help,  "Show this help"},
    {"echo", cmd_echo,  "Print text"},
    {"clear", cmd_clear, "Clear the screen"},
    {"about", cmd_about, "About Crusader OS"},
};

#define COMMAND_COUNT (sizeof(commands)/sizeof(Command))

// ==== Implementace ====

static void cmd_help(const char* args) {
    puts("Available commands:\n");
    for (int i = 0; i < COMMAND_COUNT; i++) {
        printf("%s\n", commands[i].name, commands[i].desc);
    }
}

static void cmd_echo(const char* args) {
    if (args && *args)
        puts(args);
    putc('\n');
}

static void cmd_clear(const char* args) {
    // Use the kernel screen-clear function instead of ANSI escape
    // sequences which are printed literally on bare metal.
    clrscr();
}

static void cmd_about(const char* args) {
    puts("Crusader OS v10.6 - lightweight 32-bit OS\n");
}

void execute_command(char* cmd)
{
    // rozděl příkaz a argumenty
    char* space = cmd;
    while (*space && *space != ' ') space++;
    if (*space) { *space = '\0'; space++; } else { space = NULL; }

    for (int i = 0; i < COMMAND_COUNT; i++) {
        if (simple_strcmp(cmd, commands[i].name) == 0) {
            commands[i].func(space);
            return;
        }
    }

    if (*cmd)
        puts("Unknown command\n");
}

// jednoduché strcmp pro minimal kernel
int simple_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
