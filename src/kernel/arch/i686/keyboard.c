#include "keyboard.h"
#include "irq.h"
#include "io.h"
#include "pic.h"
#include <stdio.h>
#include <commands.h>

#define KBD_DATA_PORT 0x60

// simple US scancode set 1 map for keys 0x02..0x39 (printable subset)
static const char scancode_set1[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/'
};

static void keyboard_irq_handler(Registers* regs)
{
    (void)regs;
    uint8_t sc = i686_inb(KBD_DATA_PORT);

    // ignore key releases (high bit set)
    if (sc & 0x80) return;

    if (sc >= 0x02 && sc <= 0x39)
    {
        // handle space explicitly (scancode 0x39)
        if (sc == 0x39)
        {
            extern void keyboard_handle_char(char c);
            keyboard_handle_char(' ');
            return;
        }

        char c = scancode_set1[sc - 0x00];
        if (!c) return;

        // command input buffer handling (defined below)
        extern void keyboard_handle_char(char c);
        keyboard_handle_char(c);
    }
}

void keyboard_init()
{
    // register handler for IRQ1 (keyboard). IRQ numbers are 0-15; PIC remapped base is 0x20
    i686_IRQ_RegisterHandler(1, keyboard_irq_handler);
    // unmask IRQ1 on PIC
    i686_PIC_Unmask(1);
}

#define BUFFER_SIZE 256
static char input_buffer[BUFFER_SIZE];
static int buffer_index = 0;

// handle a printable character from keyboard IRQ
void keyboard_handle_char(char c)
{
    // backspace
    if (c == '\b')
    {
        if (buffer_index > 0)
        {
            buffer_index--;
            input_buffer[buffer_index] = 0;

            // erase character on screen: move cursor back and clear
            extern int g_ScreenX, g_ScreenY;
            extern uint8_t* g_ScreenBuffer;
            if (g_ScreenX > 0)
            {
                g_ScreenX--;
                putchr(g_ScreenX, g_ScreenY, '\0');
                setcursor(g_ScreenX, g_ScreenY);
            }
        }
    }
    else if (c == '\n' || c == '\r')
    {
        // terminate and execute
        if (buffer_index >= 0 && buffer_index < BUFFER_SIZE)
            input_buffer[buffer_index] = 0;
        execute_command(input_buffer);
        buffer_index = 0;
    }
    else
    {
        if (buffer_index < BUFFER_SIZE - 1)
        {
            input_buffer[buffer_index++] = c;
            // echo character
            putc(c);
        }
    }
}
