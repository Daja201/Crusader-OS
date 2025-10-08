#include <stdio.h>
#include "commands.h"

// minimal strcmp and strncmp implementations local to this file
static int simple_strcmp(const char* a, const char* b)
{
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

static int simple_strncmp(const char* a, const char* b, unsigned n)
{
    while (n && *a && *b && *a == *b) { a++; b++; n--; }
    if (n == 0) return 0;
    return (unsigned char)*a - (unsigned char)*b;
}

void execute_command(char* cmd)
{
    if (simple_strcmp(cmd, "help") == 0)
    {
        puts("Available commands: help, echo\n");
    }
    else if (simple_strncmp(cmd, "echo ", 5) == 0)
    {
        puts(cmd + 5);
        putc('\n');
    }
    else if (simple_strcmp(cmd, "") == 0)
    {
        // ignore empty
    }
    else
    {
        puts("Unknown command\n");
    }
}
