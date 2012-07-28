/****
 * krigmap v 0.3 (c) 2012
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include "features.h"
#include "model.h"

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
  features f;
  model m;

  /* Stdin -> features */
  readfeatures(&f);
 
  /* Fit model and predict */
  variogram(&m, &f, DEFAULT_MDL);

  /* Print model */
  printmodel(&m);

  return 0;
}
