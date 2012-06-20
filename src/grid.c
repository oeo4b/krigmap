#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "grid.h"
#include "features.h"
#include "math/stats.h"

void startgrid(grid* g, features* f, int level, int depth, int land) {
  g->level = level;
  g->depth = depth;
  float block = (float)90 / pow(2, (float)g->level);

  /* Calculate grid boundaries */
  float xlim[] = { min(f->x, f->n), max(f->x, f->n) };
  float ylim[] = { min(f->y, f->n), max(f->y, f->n) };

  g->xlim[0] = floor((xlim[0]-(float)(-180))/block);
  g->xlim[1] = ceil((xlim[1]-(float)(-180))/block);
  g->ylim[0] = floor((ylim[0]-(float)(-90))/block);
  g->ylim[1] = ceil((ylim[1]-(float)(-90))/block);

  /* Alloc mem */
  int n = g->xlim[1]-g->xlim[0];
  int m = g->ylim[1]-g->ylim[0];
  g->land = (char*)malloc(sizeof(char)*ceil(n*m*g->depth*g->depth/8));
  if(!land) g->value = (float*)malloc(sizeof(float)*n*m*g->depth*g->depth);

  printf("%d, %d, %d\n", n, m, n*m*g->depth*g->depth/8);
  /* Memset */
  memset(g->land, 0, ceil(n*m*g->depth*g->depth/8));
  if(!land) memset(g->value, 0, n*m*g->depth*g->depth);
}

void readgrid(grid* g) {
  /* Read block by block for the entire grid */
  FILE* f;
  char gridfile[100];
  int i, j, k;
  int n = g->xlim[1]-g->xlim[0];
  int m = g->ylim[1]-g->ylim[0];

  for(i=0;i<n;i++) {
    for(j=0;j<m;j++) {
      sprintf(gridfile, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, i+g->xlim[0], j+g->ylim[0]);
      f = fopen(gridfile, "rb");
      k = floor((i*m*g->depth+j*g->depth)/8);
      fread(g->land+k, 1, ceil(g->depth*g->depth/8), f);
      fclose(f);
    }
  }

}

void writegrid(grid* g) {
  /* Write block by block for the entire grid */
  FILE* f;
  char gridfile[100];
  int i, j, k;
  int n = g->xlim[1]-g->xlim[0];
  int m = g->ylim[1]-g->ylim[0];

  for(i=0;i<n;i++) {
    for(j=0;j<m;j++) {
      sprintf(gridfile, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, i+g->xlim[0], j+g->ylim[0]);
      printf("Writing file: %s\n", gridfile);
      f = fopen(gridfile, "wb");
      k = floor((i*m*g->depth+j*g->depth)/8);
      fwrite(g->land+k, 1, ceil(g->depth*g->depth/8), f);
      fclose(f);
    }
  }

}

void printgrid(grid* g) {

}

void freegrid(grid* g) {
  free(g->land);
  free(g->value);
}
