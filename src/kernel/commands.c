#include <stdio.h>
#include "commands.h"
#include <stdint.h>
#include <stddef.h>
#include <arch/i686/io.h>

typedef void (*command_func_t)(const char* args);

typedef struct {
    const char* name;
    command_func_t func;
    const char* desc;
} Command;



static void cmd_help(const char* args);
static void cmd_echo(const char* args);
static void cmd_clear(const char* args);
static void cmd_sysinfo(const char* args);
static void cmd_shutdown(const char* args);
static void cmd_reboot(const char* args);
static void cmd_about(const char* args);

static Command commands[] = {
    {"help", cmd_help,  "Show this help"},
    {"echo", cmd_echo,  "Print text"},
    {"clear", cmd_clear, "Clear the screen"},
    {"sysinfo", cmd_sysinfo, "Show system information"},
    {"shutdown", cmd_shutdown, "Shutdown the machine"},
    {"reboot", cmd_reboot, "Reboot the machine"},
    {"about", cmd_about, "About Crusader OS"},
};

#define COMMAND_COUNT (sizeof(commands)/sizeof(Command))



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
    
    
    clrscr();
}

static void cmd_about(const char* args) {
    puts("Crusader OS v06 - lightweight 32-bit OS \n"
        "Made by David Zapletal \n" 
        "Github: Daja201 \n");
}


static void cpuid(uint32_t eax_in, uint32_t ecx_in, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
{
    __asm__ __volatile__("cpuid"
                  : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
                  : "a" (eax_in), "c" (ecx_in));
}

static void cmd_sysinfo(const char* args)
{
    uint32_t a, b, c, d;

    
    cpuid(0, 0, &a, &b, &c, &d);
    char vendor[13];
    ((uint32_t*)vendor)[0] = b; 
    ((uint32_t*)vendor)[1] = d; 
    ((uint32_t*)vendor)[2] = c; 
    vendor[12] = '\0';

    printf("CPU Vendor: %s\n", vendor);

    
    cpuid(1, 0, &a, &b, &c, &d);
    uint32_t stepping = a & 0xF;
    uint32_t model = (a >> 4) & 0xF;
    uint32_t family = (a >> 8) & 0xF;
    uint32_t ext_model = (a >> 16) & 0xF;
    uint32_t ext_family = (a >> 20) & 0xFF;

    uint32_t display_family = family;
    uint32_t display_model = model;
    if (family == 0xF)
        display_family = family + ext_family;
    if (family == 0x6 || family == 0xF)
        display_model = (ext_model << 4) | model;

    printf("CPU Family: %u  Model: %u  Stepping: %u\n", display_family, display_model, stepping);

    
    puts("Features: ");
    if (d & (1 << 23)) puts("MMX ");
    if (d & (1 << 25)) puts("SSE ");
    if (d & (1 << 26)) puts("SSE2 ");
    if (c & (1 << 0))  puts("SSE3 ");
    if (c & (1 << 9))  puts("SSSE3 ");
    if (c & (1 << 19)) puts("SSE4.1 ");
    if (c & (1 << 20)) puts("SSE4.2 ");
    if (d & (1 << 5))  puts("PAE ");
    if (c & (1 << 9))  puts("CMPXCHG16B ");
    putc('\n');

    
    cpuid(0x80000000u, 0, &a, &b, &c, &d);
    if (a >= 0x80000004u)
    {
        char brand[49];
        uint32_t* p = (uint32_t*)brand;
        cpuid(0x80000002u, 0, &p[0], &p[1], &p[2], &p[3]);
        cpuid(0x80000003u, 0, &p[4], &p[5], &p[6], &p[7]);
        cpuid(0x80000004u, 0, &p[8], &p[9], &p[10], &p[11]);
        brand[48] = '\0';
        printf("CPU Brand: %s\n", brand);
    }

    
    puts("Memory: total size not available (bootloader doesn't expose it)\n");
    puts("GPU: VGA text mode @ 0xB8000 (80x25) or framebuffer if present\n");

    
    puts("Note: live CPU/RAM usage counters are not implemented in this build.\n");
}

static void cmd_shutdown(const char* args)
{
    puts("Shutting down...\n");

    /* Try QEMU/Bochs ACPI-style port to request poweroff. Many emulators
     * listen on 0x604 (16-bit write). We also try 0x4004 as some setups
     * accept that. If those fail, fall back to halting the CPU. */
    i686_outw(0x604, 0x2000);
    i686_outw(0x4004, 0x2000);

    
    i686_DisableInterrupts();
    for (;;) __asm__ __volatile__("hlt");
}

static void cmd_reboot(const char* args)
{
    puts("Rebooting...\n");

    /* Try keyboard controller reset (8042) which pulses the CPU reset line. */
    i686_outb(0x64, 0xFE);

    /* If that didn't work, halt the CPU forever as a fallback. */
    for (;;) __asm__ __volatile__("hlt");
}

void execute_command(char* cmd)
{
    
    
    int simple_strcmp(const char* a, const char* b);

    
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


int simple_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
