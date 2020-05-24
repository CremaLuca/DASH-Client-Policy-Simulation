#include <stdio.h>

int main(){
   short n = 1;
   char * p = &n;
   if(*p == 1) printf("Architettura little endian\n");
   else printf("Architettura big endian\n");
   return 0;
}
