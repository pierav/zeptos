/*
CONFORMING TO
       strtok()
              POSIX.1-2001, POSIX.1-2008, C89, C99, SVr4, 4.3BSD.

       strtok_r()
              POSIX.1-2001, POSIX.1-2008.

$ ./a.out 'a/bbb///cc;xxx:yyy:' ':;' '/'
1: a/bbb///cc
        --> a
        --> bbb
        --> cc
2: xxx
        --> xxx
3: yyy
        --> yyy
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *_argv[] = {__FILE__, "a/bbb///cc;xxx:yyy:", ":;", "/"};
int _argc = 4;

int
main(int argc, char *argv[])
{
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int j;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s string delim subdelim\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    for (j = 1, str1 = argv[1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, argv[2], &saveptr1);
        if (token == NULL)
            break;
        printf("%d: %s\n", j, token);

        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, argv[3], &saveptr2);
            if (subtoken == NULL)
                break;
            printf(" --> %s\n", subtoken);
        }
    }

    exit(EXIT_SUCCESS);
}
