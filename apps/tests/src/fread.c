#include <assert.h>
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("/hello.txt", "r");
    if (!fp) {
        perror(NULL);
        return 1;
    }

    char buf[16];
    size_t size;
    while ((size = fread(buf, 1, 16, fp)) != 0) {
        printf("%.*s", size, buf);
    }

    fclose(fp);

    return 0;
}