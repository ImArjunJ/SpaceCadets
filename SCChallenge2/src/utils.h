#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>

void* util_memset(void* ptr, int value, size_t num);
size_t util_strlen(const char* str);
int util_sscanf(const char* str, const char* format, ...);
void util_reverse(char* str, int length);
char* util_itoa(int num, char* buffer, int base);
const char* util_strchr(const char* str, char c);
char* util_strncpy(char* dest, const char* src, size_t n);
int util_isspace(char c);
char* util_strstr(const char* haystack, const char* needle);
int util_strcmp(const char* str1, const char* str2);
int util_strncmp(const char* s1, const char* s2, unsigned int n);
char* util_trim(char* str);
char* util_strtok(char* str, const char* delim);
char* util_strdup(const char* s);
#endif // UTILS_H
