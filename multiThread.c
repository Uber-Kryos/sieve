/***
*
* Sieve of Eratosthenes
*
* Killian Kinsella <kkinsella15@gmail.com>
* Date started: 29/04/2016
*
* i7-6700k @ 4.00Ghz -> 30s for size 10 billion
*
* Uses 2,3,5 wheel, bitmaps, multithreaded presieving
*
***/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define NUM_B 8
#define DEFAULT_SIZE 1000000000

//for sending multiple arguments to a pthread
typedef struct _arg_struct {
   unsigned long long length;
   unsigned char * list;
   unsigned char start;
   unsigned char bitValue;
} *args;

void primeFinder (unsigned char list[], unsigned long long length);
void assignPos (unsigned char list[], unsigned long long pos);
void listPrinter (unsigned char list[], unsigned long long length);
void preProcess (unsigned char list[], unsigned long long length);

unsigned char primePos (unsigned char list[], unsigned long long pos);

void *preS (void* argu);

int main(int argc, char * argv[]) {

   unsigned long long size = DEFAULT_SIZE;

   if (argc <= 1 || sscanf(argv[1], "%llu", &size) != 1){
      printf("No arguments given, default %llu used.\n", size);
   }

   //make it divisible by number of bits
   int off = size%NUM_B;
   if (off) {
      size += (NUM_B-off);
   }

   unsigned char *list = (unsigned char*)malloc((size/NUM_B) * sizeof(unsigned char));
   //checks if alloc worked
   if (list == NULL) {
      printf("alloc didn't work\n");
      exit(EXIT_FAILURE);
   }

   //Hard coded for a 2,3,5 wheel
   preProcess(list, size/NUM_B);

   primeFinder(list, size);

   listPrinter(list, size/NUM_B);

   free(list);

   return EXIT_SUCCESS;
}

void primeFinder (unsigned char list[], unsigned long long length) {

   //start at 7 due to presieve
   unsigned long long i = 6;
   unsigned long long j = 0;
   unsigned long long adder = 0;

   double lenCalc = ceil(sqrt(length));

   int posI = 0;
   int posJ = 0;
   //Pattern in a 2,3,5 wheel
   int gap[8] = {4,2,4,2,4,6,2,6};


   while (i < lenCalc) {

      //plus 1 as arrays start at 0
      adder = i+1;
      j = (adder * adder) - 1;
      if (!primePos(list, i)){

         //for finding position in gaps
         posJ = posI;
         while (j < length) {

            assignPos(list, j);

            //multiplied by gap due to Euler
            j += (adder * gap[posJ]);
            posJ+=1;
            posJ&=7;
         }
      }

      //this increments semi-efficiently
      i+=gap[posI];
      posI+=1;
      posI&=7;

   }
}

void assignPos (unsigned char list[], unsigned long long pos){

   unsigned char power = 1;
   int internalPos = (~pos)&7;
   list[pos/NUM_B] = list[pos/NUM_B] | (power << internalPos);
}

void listPrinter(unsigned char list[], unsigned long long length) {

/*
   FILE *prime = fopen("test.txt","w");

   fwrite(list, NUM_B/8, length, prime);

   fclose(prime);
*/

   unsigned long long i = 0;
   unsigned char n = 0;
   unsigned long long count = 0;

   while (i < length) {
      n = ~list[i];
      while (n) {
         n &= (n-1);
         count++;
      }
      i++;
   }
   printf("\n%llu Primes Counted\n", count);

}

unsigned char primePos (unsigned char list[], unsigned long long pos){

   unsigned char toFind = list[pos/NUM_B];
   int internalPos = (~pos)&7;
   return ((toFind >> internalPos) & 1);
}

//The values for the bitmap where multiples of 2,3,5 have been flipped
void preProcess (unsigned char list[], unsigned long long length) {

   list[0] = 149;

   int x;

   args i[15];

   for(x=0;x<15;x++) i[x] = malloc(sizeof(struct _arg_struct));

   for (x=0;x<15;x++) i[x]->length = length;
   
   for (x=0;x<15;x++) i[x]->list = list;


   i[0]->start = 1;
   i[0]->bitValue = 215;
   i[1]->start = 2;
   i[1]->bitValue = 93;
   i[2]->start = 3;
   i[2]->bitValue = 245;
   i[3]->start = 4;
   i[3]->bitValue = 247;
   i[4]->start = 5;
   i[4]->bitValue = 93;
   i[5]->start = 6;
   i[5]->bitValue = 119;
   i[6]->start = 7;
   i[6]->bitValue = 215;
   i[7]->start = 8;
   i[7]->bitValue = 221;
   i[8]->start = 9;
   i[8]->bitValue = 117;
   i[9]->start = 10;
   i[9]->bitValue = 223;
   i[10]->start = 11;
   i[10]->bitValue = 95;
   i[11]->start = 12;
   i[11]->bitValue = 117;
   i[12]->start = 13;
   i[12]->bitValue = 215;
   i[13]->start = 14;
   i[13]->bitValue = 125;
   i[14]->start = 15;
   i[14]->bitValue = 125;

   pthread_t tid[15];
   
   for (x=0;x<15;x++) pthread_create(&tid[x], NULL, preS, (void*)i[x]);

   for (x=0;x<15;x++) pthread_join(tid[x],NULL);

   for (x=0; x<15; x++) free(i[x]);


}

void *preS (void* argu){

   args argument = (args) argu;

   unsigned long long i = argument->start;
   unsigned long long size = argument->length;
   unsigned char *list = argument->list;
   unsigned char bMap = argument->bitValue;

   while (i<size){

      list[i] = bMap;
      i+=15;
   }
   pthread_exit((void*)NULL);
}

