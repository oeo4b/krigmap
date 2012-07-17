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

  /* Fit to model */
  variogram(&m, &f, DEFAULT_MDL);

  /* Estimate grid based on model */
  startgrid(&g, &f, level, depth, 0);
  singleblock(&g, level, depth, xblock, yblock);
  readgrid(&g); 

  /* Estimate values */
  predict(&g, &m, &f);

  /* Write image */
  heatspectrum(&s);
  printppm(&g, &f, &s);

  return 0;
}
