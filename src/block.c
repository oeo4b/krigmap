/****
 * krigmap v 0.3 (c) 2012
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include "grid.h"
#include "features.h"
#include "model.h"
#include "image.h"

int 
main(int argc, char** argv)
{
  /**
   * fprintf(stdout) | ./block l d x y
   * option:
   *   -x [gpu, cpu]
   *   -m [lin, sph, exp, gau]
   *   -l [1..13]
   */
  if(argc<5) {
    fprintf(stderr, "Error: not enough arguments.\n");
    exit(1);
  }
  int level = atoi(argv[1]);
  int depth = atoi(argv[2]);
  int xblock = atoi(argv[3]);
  int yblock = atoi(argv[4]);

  grid g;
  features f;
  model m;
  spectrum s;

  /* Stdin -> features */
  readfeatures(&f);

  /* Estimate grid based on model */
  singleblock(&g, level, depth, xblock, yblock);
  readgrid(&g); 

  /* Predict using model */
  int i;
  if(f.n>0) {
    if(argc<11) {
      fprintf(stderr, "argc: %d\n", argc);
      for(i=0;i<argc;i++) {
        fprintf(stderr, "%s\n", argv[i]);
      }
      fprintf(stderr, "Error: not enough arguments.\n");
      exit(1);
    }
    m.type = atoi(argv[5]);
    m.nugget = atof(argv[6]);
    m.range = atof(argv[7]);
    m.sill = atof(argv[8]);
    m.a = atof(argv[9]);
    m.MSE = atof(argv[10]);

    predict(&g, &m, &f);
    heatspectrum(&s);
  }

  /* Write image */
  printppm(&g, &f, &s, f.n==0 ? 1 : 0);

  return 0;
}
