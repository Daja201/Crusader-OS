#include "keyboard.h"
#include "irq.h"
#include "io.h"
#include "pic.h"
#include <stdio.h>
#include <commands.h>

#define KBD_DATA_PORT 0x60


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

    
    if (sc & 0x80) return;

    if (sc >= 0x02 && sc <= 0x39)
    {
        
        if (sc == 0x39)
        {
            extern void keyboard_handle_char(char c);
            keyboard_handle_char(' ');
            return;
        }

        char c = scancode_set1[sc - 0x00];
        if (!c) return;

        
        extern void keyboard_handle_char(char c);
        keyboard_handle_char(c);
    }
}

void keyboard_init()
{
    
    i686_IRQ_RegisterHandler(1, keyboard_irq_handler);
    
    i686_PIC_Unmask(1);
}

#define BUFFER_SIZE 256
static char input_buffer[BUFFER_SIZE];
static int buffer_index = 0;


void keyboard_handle_char(char c)
{
    
    if (c == '\b')
    {
        if (buffer_index > 0)
        {
            buffer_index--;
            input_buffer[buffer_index] = 0;

            
            extern int g_ScreenX, g_ScreenY;
            if (g_ScreenX > 0)
            {
                g_ScreenX--;
                
                putchr(g_ScreenX, g_ScreenY, ' ');
                putcolor(g_ScreenX, g_ScreenY, DEFAULT_COLOR);
                setcursor(g_ScreenX, g_ScreenY);
            }
        }
    }
    else if (c == '\n' || c == '\r')
    {
        
        if (buffer_index >= 0 && buffer_index < BUFFER_SIZE)
            input_buffer[buffer_index] = 0;
        
        /* Move to next line immediately so the command output starts on the
         * following line (avoid printing ANSI or other stray characters). */
        putc('\n');

        
        execute_command(input_buffer);
        buffer_index = 0;
    }
    else
    {
        if (buffer_index < BUFFER_SIZE - 1)
        {
            input_buffer[buffer_index++] = c;
            
            extern int g_ScreenX, g_ScreenY;
            putchr(g_ScreenX, g_ScreenY, c);
            putcolor(g_ScreenX, g_ScreenY, 0x2); 
            g_ScreenX++;
            if (g_ScreenX >= SCREEN_WIDTH)
            {
                g_ScreenY++;
                g_ScreenX = 0;
            }
            if (g_ScreenY >= SCREEN_HEIGHT)
                scrollback(1);
            setcursor(g_ScreenX, g_ScreenY);
        }
    }
}
