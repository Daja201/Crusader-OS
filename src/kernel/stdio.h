#pragma once
#include <stdint.h>

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

/* Low-level VGA/text-mode helpers used by other modules */
void putchr(int x, int y, char c);
void putcolor(int x, int y, uint8_t color);
char getchr(int x, int y);
uint8_t getcolor(int x, int y);
void setcursor(int x, int y);
void scrollback(int lines);

/* Screen state (defined in stdio.c) */
extern const unsigned SCREEN_WIDTH;
extern const unsigned SCREEN_HEIGHT;
extern const uint8_t DEFAULT_COLOR;
extern uint8_t* g_ScreenBuffer;
extern int g_ScreenX;
extern int g_ScreenY;
