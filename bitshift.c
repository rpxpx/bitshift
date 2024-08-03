
/* Logical operation bitmaps.
 * Bitmap images formed by taking the Boolean (zero/non-zero) value for each pixel from a chain of
 * simple logical and arithmetic operations on the (x,y) coordinates of that pixel, and a constant, k.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define X 674
#define Y 32
#define X_STRIDE 0
#define Y_STRIDE 0
#define WIDTH 700
#define HEIGHT 256
/* Whether white should mark zero or non-zero operation results */
/* (In the bpm file standard, 0 codes white, 1 codes black.) */
#define WHITE 0

#define LOGIC_OP(x,y,k) MUL_AND((x),(y),(k))
#define K 1023
#define K_STRIDE 1
#define N 2


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define AND_DIV(a,b,div) (div)?(bool)(((a) & (b))%(div)):(0)
#define OR_DIV(a,b,div)  (div)?(bool)(((a) | (b))%(div)):(0)
#define XOR_DIV(a,b,div) (div)?(bool)(((a) ^ (b))%(div)):(0)
#define SUM_DIV(a,b,div) (div)?(bool)(((a) + (b))%(div)):(0)
#define MUL_DIV(a,b,div) (div)?(bool)(((a) * (b))%(div)):(0)
#define MUL_AND(a,b,c) (bool)(((a) * (b)) & (c))


typedef unsigned int uint;
typedef unsigned char uchar;


uint digit_count_int(int a);

uint digit_count_int(int a){
  uint c = 0;
  do
    c++;
  while (a/=10);
  return c;
}


int main(int argc, char *argv[]){

  if (argc < 2){
    printf("err: supply base file name at invocation.\n");
    return 1;
  }

  char *base_name = argv[1];
  
  FILE *fp;
  char mask = 0-WHITE;
  uint len_base_name = strlen(base_name) + 1;  
  uint len_N = digit_count_int( N-1 );
  uint len_X = digit_count_int( MAX(X, X + N*X_STRIDE) );
  uint len_Y = digit_count_int( MAX(Y, Y + N*Y_STRIDE) );
  uint len_K = digit_count_int( MAX(K, K + N*K_STRIDE) );

  /* file name + num + x,y,k + .pbm \0 */
  char *fn = (char *)malloc(sizeof(char)*(len_base_name + len_N+1 + len_X+1 + len_Y+1 + len_K+5)); 
  uint fillzs;

  uint n,i,j;
  int x, y, k;
  uchar b, b_i, b_l;
  
  /* Cycle through N frames */
  for (n=0; n<N; n++){
    
    x = X + n*X_STRIDE;
    y = Y + n*Y_STRIDE;
    k = K + n*K_STRIDE;
    
    /* Create file name */ 
    sprintf(fn, "%s_", base_name);
    fillzs = len_N - digit_count_int(n);
    /* printf("%d\n", fillzs);  */
    sprintf(fn + len_base_name + fillzs, "%d_%d_%d_%d.pbm", n, x, y, k);
    while (fillzs--)
      *(fn + len_base_name + fillzs) = '0';
    
    fp = fopen(fn, "w");
    fprintf(fp,"P4\n");
    fprintf(fp,"# https://clearlight.systems\n");
    fprintf(fp,"# https://github.com/rpxpx/primed\n");
    fprintf(fp,"# --------------------------------#\n");
    fprintf(fp,"# logically defined bitmap");
    fprintf(fp,"# x range %d->%d; y range %d->%d; k = %d\n", x, x+WIDTH, y, y+HEIGHT, k);
    fprintf(fp,"%d %d\n", WIDTH, HEIGHT);
    
    for (j=0; j<HEIGHT; j++){
      for (i=0; i<WIDTH; i+=(8-b_l)){
	b = 0;
	b_l = 8 - MIN(WIDTH-i, 8);
	for (b_i=8; b_i>b_l; b_i--)
	  b |= (LOGIC_OP( x+i+8-b_i, y+j, k )) << (b_i-1);
	fprintf(fp, "%c", b ^ mask); /* Flip whole byte if white should code 1. */
      }    
    }      
    
    fclose(fp);    
  }
  free(fn);
  return 0;
}
