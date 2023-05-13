#pragma once

#include <stdarg.h>
#include <stddef.h>

#define EOF (-1)

typedef struct FILE FILE;

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

int getchar(void);
int putchar(int);
int puts(const char *);

// - formatted output conversion
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);

int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vdprintf(int fd, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

FILE *fopen(const char *filenale, const char *mode);
int fclose(FILE *stream);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
