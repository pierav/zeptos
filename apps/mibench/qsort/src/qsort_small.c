#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UNLIMIT
#define MAXARRAY 60000 /* this number, if too large, will cause a seg. fault!! */

struct myStringStruct {
  char qstring[128];
};

// Avoid stack overflow
struct myStringStruct array[MAXARRAY];

int compare(const void *elem1, const void *elem2)
{
  int result;
  
  result = strcmp((*((struct myStringStruct *)elem1)).qstring, (*((struct myStringStruct *)elem2)).qstring);

  return (result < 0) ? 1 : ((result == 0) ? 0 : -1);
}


int main(int argc, char **argv){
   puts("*** qsort ***\n");

  FILE *fp;
  int i, count=0;
  
  if (argc<2) {
    fprintf(stderr,"Usage: qsort_small <file>\n");
    return 1;
  }

  fp = fopen(argv[1],"r");
  if(!fp){
    printf("Invalid file %s\n", argv[1]);
    return 1;
  }
  char *fgets(char *s, int size, FILE *stream);

  while(fgets(array[count].qstring, 128, fp) != NULL && (count < MAXARRAY)) {
    count++;
  }

  printf("\nSorting %d elements.\n\n",count);
  qsort(array,count,sizeof(struct myStringStruct),compare);

  for(i=0;i<count;i+=count/10)
    printf("%s", array[i].qstring);
  return 0;
}
