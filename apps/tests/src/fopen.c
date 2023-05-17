#include <stdio.h>
#include <assert.h>

int main(void){

    FILE *fp = fopen("/hello.txt", "r");
    if(!fp){
        perror(NULL);
        exit(1);
    }

    char buff[256];
    while(fgets(buff, 256, fp)) {
        printf("%s", buff);
    }

    fclose(fp);
}