#include "utils.h"

#include <stdarg.h>
#include <stdbool.h>

#include "mem.h"

int util_sscanf(const char* str, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // Skip any leading whitespace in the input string
    while (*str && util_isspace(*str))
    {
        str++;
    }

    const char* p = format;
    int count = 0;

    while (*p)
    {
        if (util_isspace(*p))
        {
            // If format string has whitespace, skip any whitespace in input
            while (*str && util_isspace(*str))
            {
                str++;
            }
            p++;
        }
        else if (*p == '%')
        {
            p++; // Move to the format specifier character

            if (*p == 'c')
            {
                char* c = va_arg(args, char*);
                if (*str == '\0')
                {
                    break; // Prevent reading beyond the string
                }
                *c = *str++;
                count++;
            }
            else if (*p == 'd')
            {
                int* d = va_arg(args, int*);
                *d = 0;
                bool found = false;

                // Handle optional sign
                bool negative = false;
                if (*str == '-')
                {
                    negative = true;
                    str++;
                }

                while (*str >= '0' && *str <= '9')
                {
                    *d = (*d * 10) + (*str - '0');
                    str++;
                    found = true;
                }
                if (negative)
                {
                    *d = -(*d);
                }
                if (found)
                    count++;
            }
            else if (*p == 's')
            {
                char* s = va_arg(args, char*);
                // Skip any leading whitespace in input for %s
                while (*str && util_isspace(*str))
                {
                    str++;
                }
                while (*str && !util_isspace(*str) && *str != ';' && *str != '\n')
                {
                    *s++ = *str++;
                }
                *s = '\0'; // Null-terminate the string
                count++;
            }
            else
            {
                // Unsupported format specifier
                break;
            }
            p++;
        }
        else
        {
            // Match the literal character in the format string
            if (*p != *str)
            {
                break; // If there's a mismatch, exit
            }
            str++; // Move past the matched character
            p++;   // Move to the next format character
        }
    }

    va_end(args);
    return count; // Return the number of parsed items
}

// fills the first num bytes of the memory area pointed to by ptr with the constant byte value
void* util_memset(void* ptr, int value, size_t num)
{
    unsigned char* p = ptr;
    while (num--)
    {
        *p++ = (unsigned char) value;
    }
    return ptr;
}

// calculates the length of a string
size_t util_strlen(const char* str)
{
    const char* s = str;
    while (*s)
    {
        s++;
    }
    return s - str;
}

// reverse a string in place
void util_reverse(char* str, int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// convert an integer to a string (base can be 10 for decimal or 16 for hexadecimal)
char* util_itoa(int num, char* buffer, int base)
{
    if (base < 2 || base > 16)
    {
        return buffer; // Return original buffer if base is invalid
    }

    int is_negative = 0;
    if (num < 0 && base == 10)
    {
        is_negative = 1;
        num = -num;
    }

    int i = 0;
    do {
        int remainder = num % base;
        buffer[i++] = (remainder > 9) ? (remainder - 10) + 'a' : remainder + '0';
        num /= base;
    } while (num > 0);

    if (is_negative)
    {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';        // Null-terminate the string
    util_reverse(buffer, i); // Reverse the string to get the correct order
    return buffer;
}

// find the first occurrence of a character in a string
const char* util_strchr(const char* str, char c)
{
    while (*str)
    {
        if (*str == c)
        {
            return str;
        }
        str++;
    }
    return NULL;
}

// copy a string (up to n characters) to another buffer
char* util_strncpy(char* dest, const char* src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    for (; i < n; i++)
    {
        dest[i] = '\0'; // Null-fill the rest of the destination
    }
    return dest;
}

int util_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

// find the first occurrence of a substring in a string
char* util_strstr(const char* haystack, const char* needle)
{
    if (!*needle)
        return (char*) haystack;
    while (*haystack)
    {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n))
        {
            h++;
            n++;
        }
        if (!*n)
        {
            return (char*) haystack; // Return pointer to the start of the found substring
        }
        haystack++;
    }
    return NULL; // Return NULL if not found
}

int util_strcmp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char*) str1 - *(unsigned char*) str2;
}

char* util_trim(char* str)
{
    if (str == NULL)
    {
        return NULL;
    }

    char* start = str;
    while (*start && util_isspace(*start))
    {
        start++;
    }

    // if the string is all whitespace, return an empty string
    if (*start == '\0')
    {
        *str = '\0';
        return str;
    }

    // find the end of the string (skip trailing whitespaces)
    char* end = start + util_strlen(start) - 1;
    while (end > start && util_isspace(*end))
    {
        end--;
    }

    *(end + 1) = '\0';
    if (start != str)
    {
        util_strncpy(str, start, end - start + 2); // +1 for null terminator
    }

    return str;
}

// Compare the first n characters of two strings
int util_strncmp(const char* s1, const char* s2, unsigned int n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    return n ? *(unsigned char*) s1 - *(unsigned char*) s2 : 0; // Return the difference or 0 if n reached
}

char* util_strtok(char* str, const char* delim)
{
    static char* current = NULL; // Internal state

    if (str != NULL)
    {
        current = str;
    }

    if (current == NULL)
    {
        return NULL;
    }

    // Skip leading delimiters
    while (*current && util_strchr(delim, *current))
    {
        current++;
    }

    if (*current == '\0')
    {
        current = NULL;
        return NULL;
    }

    // Start of the token
    char* token_start = current;

    // Find the end of the token
    while (*current && !util_strchr(delim, *current))
    {
        current++;
    }

    if (*current)
    {
        *current = '\0'; // Null-terminate the token
        current++;
    }
    else
    {
        current = NULL;
    }

    return token_start;
}

char* util_strdup(const char* s)
{
    size_t len = 0;
    while (s[len] != '\0') len++; // Calculate string length
    len += 1;                     // Include null terminator

    char* dup = (char*) mem_malloc(len * sizeof(char));
    if (dup)
    {
        mem_memcpy(dup, s, len * sizeof(char));
    }
    return dup;
}