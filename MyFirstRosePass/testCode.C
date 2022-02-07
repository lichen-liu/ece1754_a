#include <stdio.h>

int main() {
     int i;
     int j;
     float a[100][100];
     float b[100][100];

     i = 0;
     while (i < 100) {
       j = 0;
       while (j < 100) {
         b[i][j] = 9.0;
         ++j;
       }
       ++i;
     }
     for (i=0; i < 100 ; ++i) {
         for (j=0; j <100 ; ++j) {
             a[i][j] = b[i][j]+5;
         }
     }
     printf("%f %f\n",a[0][0],b[99][99]);
     return 0;
   }
