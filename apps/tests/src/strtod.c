#include <assert.h>
#include <stdio.h>

int main(void) {

    char *s = "1.7777 3.1415e-1 -456e7";
    double x, y, z;
    int hit = sscanf(s, "%lf %lf %lf", &x, &y, &z);
    printf("HIT = %d\n", hit);
    assert(hit == 3);
    printf("%lf %lf %lf", x, y, z);

    return 0;
}