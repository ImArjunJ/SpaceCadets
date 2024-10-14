#include "terminal.h"

char* video_memory = VIDEO_MEMORY;
unsigned int current_line = 0;
unsigned int current_col = 0;

// Clears the screen and resets cursor position
void clear_screen()
{
    unsigned int i = 0;
    while (i < (SCREEN_WIDTH * SCREEN_HEIGHT * 2))
    {
        video_memory[i] = ' ';
        video_memory[i + 1] = TXT_WHITE;
        i += 2;
    }
    current_line = 0; // Reset current line after clearing the screen
    current_col = 0;  // Reset current column
}

void scroll_screen()
{
    unsigned int bytes_per_line = SCREEN_WIDTH * 2;
    unsigned int total_bytes = bytes_per_line * (SCREEN_HEIGHT - 1);

    // Shift video memory up by one line
    mem_memcpy(video_memory, video_memory + bytes_per_line, total_bytes);

    // Clear the last line
    unsigned int i = total_bytes;
    for (; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2)
    {
        video_memory[i] = ' ';
        video_memory[i + 1] = TXT_WHITE;
    }

    // Move cursor to the last line
    current_line = SCREEN_HEIGHT - 1;
    current_col = 0;
}

// Prints a character with specified color
void print_char_color(char c, char color)
{
    if (c == '\n')
    {
        current_line++;
        current_col = 0; // Reset column on newline
        if (current_line >= SCREEN_HEIGHT)
        {
            scroll_screen();
        }
        return;
    }

    if (c == '\r')
    {
        current_col = 0; // Move cursor to the beginning of the line
        return;
    }

    if (c == '\b')
    {
        if (current_col > 0)
        {
            current_col--;
        }
        else if (current_line > 0)
        {
            current_line--;
            current_col = SCREEN_WIDTH - 1;
        }
        unsigned int offset = (current_line * SCREEN_WIDTH * 2) + (current_col * 2);
        video_memory[offset] = ' ';
        video_memory[offset + 1] = TXT_WHITE;
        return;
    }

    unsigned int offset = (current_line * SCREEN_WIDTH * 2) + (current_col * 2);
    video_memory[offset] = c;
    video_memory[offset + 1] = color;

    // Move to the next character position
    current_col++;
    if (current_col >= SCREEN_WIDTH)
    {
        current_col = 0; // Wrap around to the next line
        current_line++;
        if (current_line >= SCREEN_HEIGHT)
        {
            scroll_screen();
        }
    }
}

// Prints a single character with default color
void print_char(char c)
{
    print_char_color(c, TXT_WHITE);
}

void print_string_color(const char* str, char color)
{
    while (*str)
    {
        print_char_color(*str++, color);
    }
}

void print_string(const char* str)
{
    print_string_color(str, TXT_WHITE);
}

void print_int(int num)
{
    if (num < 0)
    {
        print_char_color('-', TXT_WHITE);
        num = -num;
    }

    if (num == 0)
    {
        print_char_color('0', TXT_WHITE);
        return;
    }

    char buffer[10]; // Enough for 32-bit integers
    int i = 0;

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Print the number in reverse order
    while (i > 0)
    {
        print_char_color(buffer[--i], TXT_WHITE);
    }
}

void print_hex(unsigned int num)
{
    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[9]; // 8 hex digits + null terminator
    buffer[8] = '\0';
    for (int i = 7; i >= 0; i--)
    {
        buffer[i] = hex_digits[num & 0xF];
        num >>= 4;
    }
    print_string("0x");
    print_string(buffer);
}
