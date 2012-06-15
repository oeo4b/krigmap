#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "grid.h"
#include "features.h"
#include "math/stats.h"

void startgrid(grid* g, features* f) {
  /* Spatial limits */
  float xlim[] = { min(f->x, f->n), max(f->x, f->n) };
  float ylim[] = { min(f->y, f->n), max(f->y, f->n) };

  /* Round to blocks -> 90 / n^l */
  g->pixel = (float)90 / pow((float)2, LEVEL);
  int i = floor((xlim[0] - (-180)) / g->pixel);
  int j = floor((ylim[0] - (-90)) / g->pixel);
  int k = ceil((xlim[1] - (-180)) / g->pixel);
  int l = ceil((ylim[1] - (-90)) / g->pixel);

  /* Coordinate limits */
  g->n = k-i;
  g->m = l-j;
  g->xlim[0] = (-180) + i*g->pixel;
  g->xlim[1] = (-180) + k*g->pixel;
  g->ylim[0] = (-90) + j*g->pixel;
  g->ylim[1] = (-90) + l*g->pixel;

  /* Alloc to grid */
  g->land = (char*)malloc(sizeof(char)*(g->n*SIDE)*(g->m*SIDE));
  g->value = (float*)malloc(sizeof(float)*(g->n*SIDE)*(g->m*SIDE));

  /* Use the grids to figure out land area */
  FILE* fg;
  char fname[100];
  int a, b;
  for(a=i;a<k;a++) {
    for(b=j;b<l;b++) {
      sprintf(fname, "grid/g%d_%d.ppm", a, b);
    }
  }
  for(i=0;i<(g->n*SIDE)*(g->m*SIDE);i++) g->land[i] = 1; /* Set all to land by default */
}

void writegrid(grid* g) {

}

void printgrid(grid* g) {

}

