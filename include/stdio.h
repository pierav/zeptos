#pragma once

#include <stdarg.h>
#include <stddef.h>

#define EOF (-1)

typedef struct FILE FILE;

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

int getchar(void);
int printf(const char *, ...);
int putchar(int);
int puts(const char *);
int snprintf(char *, size_t, const char *, ...);
int vprintf(const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);

int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);

FILE *fopen(const char *filenale, const char *mode);
int fclose(FILE *stream);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
