#include <stdlib.h>
#include <stdio.h>
#include "grid.h"
#include "polygons.h"

int main(int argc, char** argv) {
  if(argc<5) {
    fprintf(stderr, "Error: Too few arguments\n");
    exit(1);
  }

  /* Set features to limits */
  int i;
  features f;
  f.n = 2;
  f.x = (float*)malloc(sizeof(float)*f.n);
  f.y = (float*)malloc(sizeof(float)*f.n);
  for(i=0;i<2;i++) {
    f.x[i] = atof(argv[i+1]);
    f.y[i] = atof(argv[i+3]);
  }

  /* Initialize grid */
  grid g;
  startgrid(&g, &f);

  return 0;
}
