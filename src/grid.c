#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "grid.h"
#include "features.h"
#include "math/stats.h"

void startgrid(grid* g, features* f, int level, int depth) {
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
  int n = xlim[1]-xlim[0];
  int m = ylim[1]-ylim[0];
  g->land = (char*)malloc(sizeof(char)*n*m*g->depth*g->depth);
  g->value = (float*)malloc(sizeof(float)*n*m*g->depth*g->depth);

  /* Memset */
  memset(g->land, 0, n*m*g->depth*g->depth);
  memset(g->value, 0, n*m*g->depth*g->depth);
}

void readgrid(grid* g) {
  /* Read block by block for the entire grid */
  FILE* f;
  char gridfile[100];
  int i, j, k, l;
  int n = g->xlim[1]-g->xlim[0];
  int m = g->ylim[1]-g->ylim[0];
  unsigned char byte = 0;
  int pos;

  for(i=g->xlim[0];i<g->xlim[1];i++) {
    for(j=g->ylim[0];j<g->ylim[1];j++) {
      sprintf(gridfile, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, i, j);
      f = fopen(gridfile, "rb");
      for(k=0;k<n*g->depth;k++) {
        for(l=0;l<m*g->depth;l++) {
          pos = k*m*g->depth+l;
	  if((pos%8)==0) fread(&byte, 1, 1, f);
          if(byte&(1<<(pos%8))) g->land[pos] = 1;
        }
      }
      fclose(f);
    }
  }

}

void writegrid(grid* g) {
  /* Write block by block for the entire grid */
  FILE* f;
  char gridfile[100];
  int i, j, k, l;
  int n = g->xlim[1]-g->xlim[0];
  int m = g->ylim[1]-g->ylim[0];
  unsigned char byte = 0;
  int pos;

  for(i=g->xlim[0];i<g->xlim[1];i++) {
    for(j=g->ylim[0];j<g->ylim[1];j++) {
      sprintf(gridfile, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, i, j);
      f = fopen(gridfile, "wb");
      for(k=0;k<n*g->depth;k++) {
        for(l=0;l<m*g->depth;l++) {
          pos = k*m*g->depth+l;
	  if((pos%8)==0 && pos>0) {
            fwrite(&byte, 1, 1, f);
            byte = 0;
          }
          if(g->land[pos]) byte |= (1<<(pos%8));
        }
      }
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
