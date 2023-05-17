#pragma once

extern char *optarg;
extern int optind, opterr, optopt;

int getopt(int argc, char *const argv[], const char *optstring);

// - remove a directory entry relative to directory file descriptor
int unlink(const char *path);
int unlinkat(int fd, const char *path, int flag);