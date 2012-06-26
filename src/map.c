/****
 * krigmap v 0.3 (c) 2012
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include "grid.h"
#include "features.h"
#include "model.h"

int parseargs(int argc, char** argv) {

}

int 
main(int argc, char** argv)
{
  /**
   * fprintf(stdout) | ./map dest
   * option:
   *   -x [gpu, cpu]
   *   -m [lin, sph, exp, gau]
   *   -l [1..13]
   */
  if(argc<3) {
    fprintf(stderr, "Error: not enough arguments.\n");
    exit(1);
  }
  int level = atoi(argv[1]);
  int depth = atoi(argv[2]);

  grid g;
  features f;
  model m;

  /* Stdin -> features */
  readfeatures(&f);

  /* Fit to model */
  variogram(&m, &f, DEFAULT_MDL);

  /* Estimate grid based on model */
  startgrid(&g, &f, level, depth, 0);
  readgrid(&g); 

  /* Stdout */
  printmodel(&m);

  predict(&g, &m, &f);

  return 0;
}
