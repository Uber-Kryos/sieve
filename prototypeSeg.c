#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define START 9537
#define CACHE_SIZE 1048576

void primeFinder (unsigned char list[], unsigned long long start);
void assignPos (unsigned char list[], unsigned long long pos);
void preProcess (unsigned char list[], unsigned long long start);
unsigned char primePos (unsigned char list[], unsigned long long pos);
unsigned long long listPrinter (unsigned char list[]);
unsigned long long stLoc (unsigned long long st, unsigned long long mul);

int main(int argc, char * argv[]) {

   unsigned long long size = CACHE_SIZE;

   unsigned char *list = (unsigned char*)malloc((size/8) * sizeof(unsigned char));
   //checks if malloc worked
   if (list == NULL) {
       printf("malloc didn't work\n");
       exit(EXIT_FAILURE);
   }

   unsigned long long i = 0;
   unsigned long long total = 0;

   while (i<START){
      preProcess(list, i);

      primeFinder(list, i);

      total += listPrinter(list);
      i++;
   }

   printf("%llu\n",total);

   free(list);

   return EXIT_SUCCESS;
}

void primeFinder (unsigned char list[], unsigned long long start) {

   unsigned long long startPos = (start*CACHE_SIZE);
   unsigned long long startValue = 5;
   unsigned long long i = stLoc(startPos, startValue) - 1;
   unsigned long long endPos = startPos+CACHE_SIZE;
   unsigned long long j = 0;

   double lenCalc = ceil(sqrt(endPos));

   //needed due to presieve (stLoc can return a value out of phase)
   i += (i&1)?startValue:0;

   while (startValue < lenCalc) {
      //plus 1 as arrays start at 0
      j = i;
        

      while (j < CACHE_SIZE) {
         assignPos(list, j);
         j += startValue*2;
      }

      //this skips the even numbers
      startValue++;
      startValue++;

      i = stLoc(startPos, startValue) - 1;
      i += (i&1)?startValue:0;

   }
}

unsigned long long stLoc (unsigned long long st, unsigned long long mul){

   //returns equivalent array position
   unsigned long long offset = st%mul;
   if (!st) return mul*mul;
   return ((offset)?(mul - offset):0);
}

void assignPos (unsigned char list[], unsigned long long pos){

   unsigned char power = 1;
   //I bitshift below to increase preformance
   int internalPos = (~pos)&7;
   //incase it wasn't noticed, /8 as each char holds 8 bits
   list[pos/8] = list[pos/8] | (power << internalPos);
}

unsigned long long listPrinter(unsigned char list[]) {

   unsigned char n = 0;
   unsigned long long i = 0;
   unsigned long long count = 0;
   unsigned long long length = CACHE_SIZE >> 3;

   while (i < length) {
      n = ~list[i];
      while (n) {
         n &= (n-1);
         count++;
      }
      i++;
   }
   return count;
}

unsigned char primePos (unsigned char list[], unsigned long long pos){
   unsigned char toFind = list[pos/8];
   int internalPos = (~pos)&7;
   return ((toFind >> internalPos) & 1);
}

void preProcess (unsigned char list[], unsigned long long start) {

   unsigned long long i = 0;
   unsigned long long length = CACHE_SIZE >> 3;
   
   unsigned char j = 0;

   unsigned char preVal[3] = {215,93,117};

   if(start){

      switch(start%3){

         case 1:
            preVal[0] = 93;
            preVal[1] = 117;
            preVal[2] = 215;
            break;

         case 2:
            break;
         case 0:
            preVal[0] = 117;
            preVal[1] = 215;
            preVal[2] = 93;
            break;
      }

   } else {

      list[i] = 149;
      i++;
   }

   while (i < length) {
      list[i] = preVal[j];
      i++;
      j++;
      if (i < length) {
         list[i] = preVal[j];
         i++;
         j++;
         if (i < length) {
            list[i] = preVal[j];
            i++;
            j=0;
         }
      }
   }
   

}
