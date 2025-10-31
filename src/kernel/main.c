#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void crash_me();

void timer(Registers* regs)
{
    printf(".");
}

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    HAL_Initialize();

    clrscr();

printf( 
"   CCCCC  RRRRRR   U       U    SSSS     AAA      DDDD     EEEEEEE RRRRRR      \n"
"  C       R     R  U       U   S        A    A    D   D    E       R     R     \n"
" C        R      R U       U  S        A      A   D    D   E       R      R    \n"
" C        R    R   U       U  SSSSS   A        A  D     D  EEEE    R    R      \n"
" C        RRRRR    U       U       S  AAAAAAAAAA  D    D   E       RRRRRRR     \n"
"  C       R    R    U     U       S   A        A  D   D    E       R     R     \n"
"   CCCCC  R      R    UUU     SSSS    A        A  DDDD     EEEEEEE R      R    \n"
"                                                                               \n"
"  OOO  SSSS   -Made by David Zapletal                                          \n"
" O   O S      -Github: Daja201                                                 \n"
" O   O SSSS   -Basic operating system for x86 architecture using FAT12         \n"
" O   O    S   -Written in assembly, C                                          \n"
"  OO   SSSS   -Based on repo by github:nanobyte repo:nanobyte_OS branch:video10\n"
"                                                                               \n"
"    -TYPE HELP FOR HELP                                                        \n"
);                   



    //i686_IRQ_RegisterHandler(0, timer);

    //crash_me();

end:
    for (;;);
}
