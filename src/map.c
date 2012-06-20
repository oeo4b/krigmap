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
  grid g;
  features f;
  model m;

  readfeatures(&f);
  variogram(&m, &f, DEFAULT_MDL);

  startgrid(&g, &f, 12, 200);
  predict(&g, &m, &f);


  return 0;
}
