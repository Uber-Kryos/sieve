/***
*
* Sieve of Eratosthenes, segmented
*
* Killian Kinsella <kkinsella15@gmail.com>
* Date started: 26/02/2017
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
   unsigned long long arg0;
   unsigned char * arg1;
   unsigned char arg2;
   unsigned char arg3;
} *args;

void primeFinder (unsigned char list[], unsigned long long length);
void assignPos (unsigned char list[], unsigned long long pos);
void listPrinter (unsigned char list[], unsigned long long length);
void preProcess (unsigned char list[], unsigned long long length);

unsigned char primePos (unsigned char list[], unsigned long long pos);

void *preS (void* argu);

int main(int argc, char * argv[]) {

   unsigned long long size = DEFAULT_SIZE;

   if (argc > 1){
      if (sscanf(argv[1], "%llu", &size) != 1){
         printf("No arguments given, default %llu used.\n", size);
         size = DEFAULT_SIZE;
      }
   } else {
      printf("No arguments given, default %llu used.\n", size);
      size = DEFAULT_SIZE;
   }

   //make it divisible by number of bits
   if (size%NUM_B) {
      size += (NUM_B-size%NUM_B);
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

//TODO add multithreading somehow
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

   //int internalPos = ((NUM_B-1)-pos%NUM_B);
   int internalPos = (~pos)&7;
   list[pos/NUM_B] = list[pos/NUM_B] | (power << internalPos);
}

void listPrinter(unsigned char list[], unsigned long long length) {


   FILE *prime = fopen("test.txt","w");

   fwrite(list, NUM_B/8, length, prime);

   fclose(prime);


}

unsigned char primePos (unsigned char list[], unsigned long long pos){

   unsigned char toFind = list[pos/NUM_B];
   //int internalPos = ((NUM_B-1)-pos%NUM_B);
   int internalPos = (~pos)&7;
   return ((toFind >> internalPos) & 1);
}

//The values for the bitmap where multiples of 2,3,5 have been flipped
void preProcess (unsigned char list[], unsigned long long length) {

   list[0] = 149;

   int x;

   args i[15];

   for(x=0;x<15;x++) i[x] = malloc(sizeof(struct _arg_struct));

   for (x=0;x<15;x++) i[x]->arg0 = length;
   
   for (x=0;x<15;x++) i[x]->arg1 = list;


   i[0]->arg2 = 1;
   i[0]->arg3 = 215;
   i[1]->arg2 = 2;
   i[1]->arg3 = 93;
   i[2]->arg2 = 3;
   i[2]->arg3 = 245;
   i[3]->arg2 = 4;
   i[3]->arg3 = 247;
   i[4]->arg2 = 5;
   i[4]->arg3 = 93;
   i[5]->arg2 = 6;
   i[5]->arg3 = 119;
   i[6]->arg2 = 7;
   i[6]->arg3 = 215;
   i[7]->arg2 = 8;
   i[7]->arg3 = 221;
   i[8]->arg2 = 9;
   i[8]->arg3 = 117;
   i[9]->arg2 = 10;
   i[9]->arg3 = 223;
   i[10]->arg2 = 11;
   i[10]->arg3 = 95;
   i[11]->arg2 = 12;
   i[11]->arg3 = 117;
   i[12]->arg2 = 13;
   i[12]->arg3 = 215;
   i[13]->arg2 = 14;
   i[13]->arg3 = 125;
   i[14]->arg2 = 15;
   i[14]->arg3 = 125;

   pthread_t tid[15];
   
   for (x=0;x<15;x++) pthread_create(&tid[x], NULL, preS, (void*)i[x]);

   for (x=0;x<15;x++) pthread_join(tid[x],NULL);

   for (x=0; x<15; x++) free(i[x]);


}

void *preS (void* argu){

   args argument = (args) argu;

   unsigned long long i = argument->arg2;
   unsigned long long size = argument->arg0;
   unsigned char *list = argument->arg1;
   unsigned char bMap = argument->arg3;

   while (i<size){

      list[i] = bMap;
      i+=15;
   }
   pthread_exit((void*)NULL);
}

