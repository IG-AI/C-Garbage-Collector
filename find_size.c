#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>



int main()
{
  printf("\nint:       %lu\n", sizeof(int));

  printf("char:      %lu\n", sizeof(char));

  printf("bool:      %lu\n", sizeof(bool));

  printf("double:    %lu\n", sizeof(double));

  printf("long:      %lu\n", sizeof(long));

  printf("float:     %lu\n", sizeof(float));

  printf("void *:    %lu\n\n", sizeof(void *));

  return 0;
}
