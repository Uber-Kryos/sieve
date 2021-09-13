#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define LIMIT 1000000000
//24*x < 2097152 need to be 24 (3*8) due to 3 vals in preVal
//24*x < 262144
#define CACHE_SIZE 1920000 
//1920000
//CACHE_SIZE / 8
#define LENGTH (CACHE_SIZE / 8)
#define THREAD_COUNT 32

void primeFinder (unsigned char list[], unsigned long long start);
void assignPos (unsigned char list[], unsigned long long pos);
void preProcess (unsigned char list[], unsigned long long start);
unsigned char primePos (unsigned char list[], unsigned long long pos);
unsigned long long listPrinter (unsigned char list[]);
unsigned long long stLoc (unsigned long long st, unsigned long long mul);
static unsigned char incList[8] = {2,4,2,4,6,2,6,4}; 

int main(int argc, char * argv[]) {

   unsigned long long limit = LIMIT;

   if (argc <= 1 || sscanf(argv[1], "%llu", &limit) != 1){
      printf("No arguments given, default %llu used.\n", limit);
   }

   limit /= CACHE_SIZE;
   limit++;
   printf("%llu\n",limit*CACHE_SIZE);
   unsigned char list[THREAD_COUNT][LENGTH];
   
   static unsigned char preList[LENGTH];
   preProcess(preList,1);

   unsigned long long total = 0;

   #pragma omp parallel num_threads(THREAD_COUNT) reduction(+:total)
   {
      unsigned long long i = omp_get_thread_num();
      unsigned char j = i;
      if (i==0){
         preProcess(list[j],i);
         primeFinder(list[j], i);
         total += listPrinter(list[j]);
         i+=THREAD_COUNT;
      }
      while (i<limit){
         memcpy(list[j],preList,LENGTH);
         primeFinder(list[j], i);

         total += listPrinter(list[j]);
         i+=THREAD_COUNT;
      }
   }

   printf("%llu\n",total);

   return EXIT_SUCCESS;
}

void primeFinder (unsigned char list[], unsigned long long start) {

   unsigned long long startPos = (start*CACHE_SIZE);
   unsigned long long startValue = 7;
   unsigned long long i = stLoc(startPos, startValue) - 1;
   unsigned long long endPos = startPos+CACHE_SIZE;
   unsigned long long j = 0;
   unsigned long long count = 7;

   double lenCalc = ceil(sqrt(endPos));

   //needed due to presieve (stLoc can return a value out of phase)
   i += (i&1)?startValue:0;
   unsigned char *listPointerLoc; 
   while (startValue < lenCalc) {
      //plus 1 as arrays start at 0
      j = i;
      listPointerLoc = list + (j>>3);
      while (j < CACHE_SIZE) {
         assignPos(listPointerLoc, j);
         j += startValue*2;
         listPointerLoc = list + (j>>3);
      }
      //this increments to the next preprocessed prime
      startValue += incList[count%8];
      count++;

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
   *list |= (power << internalPos);
}

unsigned long long listPrinter(unsigned char list[]) {

   unsigned long long count = 0;
   unsigned long long i = 0;
   unsigned int n = 0;

   //hamming weight code from wikipedia
   while (i < (LENGTH>>2)) {
      n = ~((unsigned int *)list)[i];
      count += __builtin_popcount(n);
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
   unsigned char preVal[15] = {125, 215, 93, 245, 247, 93, 119, 215, 221, 117, 223, 95, 117, 215, 125};
   
   if(!start){
      list[0]    = 149;
      i++;
   }

   while (i<LENGTH){
      list[i] = preVal[i%15];
      i++;
   }
}
