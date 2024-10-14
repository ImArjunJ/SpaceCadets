#ifndef TERMINAL_H
#define TERMINAL_H

#define TXT_BLACK 0x00
#define TXT_BLUE 0x01
#define TXT_GREEN 0x02
#define TXT_CYAN 0x03
#define TXT_RED 0x04
#define TXT_MAGENTA 0x05
#define TXT_BROWN 0x06
#define TXT_WHITE 0x07
#define TXT_LIGHT_GREY 0x08
#define TXT_DARK_GREY 0x08
#define TXT_LIGHT_BLUE 0x09
#define TXT_LIGHT_GREEN 0x0A
#define TXT_LIGHT_CYAN 0x0B
#define TXT_LIGHT_RED 0x0C
#define TXT_LIGHT_MAGENTA 0x0D
#define TXT_LIGHT_BROWN 0x0E
#define TXT_BRIGHT_WHITE 0x0F

#define VIDEO_MEMORY (char*) 0xb8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void clear_screen();
void scroll_screen();
void print_char_color(char c, char color);
void print_char(char c);
void print_string_color(const char* str, char color);
void print_string(const char* str);
void print_int(int num);
void print_hex(unsigned int num);

#endif // TERMINAL_H
