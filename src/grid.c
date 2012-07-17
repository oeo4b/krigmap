#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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

  /* Alloc and memset */
  int i, j;
  g->n = g->xlim[1]-g->xlim[0];
  g->m = g->ylim[1]-g->ylim[0];
  g->land = (unsigned char**)malloc(sizeof(unsigned char*)*g->n*g->m);
  if(!land) {
    g->value = (float**)malloc(sizeof(float*)*g->n*g->m);
  }
  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      g->land[i*g->m+j] = (unsigned char*)malloc(sizeof(unsigned char)*g->depth*g->depth);
      memset(g->land[i*g->m+j], 0, g->depth*g->depth);
      if(!land) {
        g->value[i*g->m+j] = (float*)malloc(sizeof(float)*g->depth*g->depth);
      }
    }
  }
}

void singleblock(grid* g, int level, int depth, int x, int y) {
  g->level = level;
  g->depth = depth;
  float block = (float)90 / pow(2, (float)g->level);

  g->xlim[0] = x;
  g->xlim[1] = x+1;
  g->ylim[0] = y;
  g->ylim[1] = y+1;

  /* Alloc for a single block */
  int i, j;
  g->n = 1;
  g->m = 1;
  g->land = (unsigned char**)malloc(sizeof(unsigned char*)*g->n*g->m);
  g->value = (float**)malloc(sizeof(float*)*g->n*g->m);
  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      g->land[i*g->m+j] = (unsigned char*)malloc(sizeof(unsigned char)*g->depth*g->depth);
      memset(g->land[i*g->m+j], 0, g->depth*g->depth);
      g->value[i*g->m+j] = (float*)malloc(sizeof(float)*g->depth*g->depth);
    }
  }
}

void readgrid(grid* g) {
  /* Read block by block for the entire grid */
  FILE* f;
  unsigned int i, j, k, l;
  unsigned int iblk, ipxl;
  unsigned char byte;
  char fname[100];

  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      iblk = i*g->m+j;
      sprintf(fname, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, g->xlim[0]+i, g->ylim[0]+j);

      if(access(fname, F_OK)== -1) continue;

      f = fopen(fname, "rb");
      for(k=0;k<g->depth;k++) {
        for(l=0;l<g->depth;l++) {
          ipxl = k*g->depth+l;
          if((ipxl%8)==0) fread(&byte, 1, 1, f);
          if(byte&(1<<(ipxl%8))) g->land[iblk][ipxl] = 1;
        }
      }
      fclose(f);
    }
  }

}

void writegrid(grid* g) {
  /* Write block by block for the entire grid */
  FILE* f;
  unsigned int i, j, k, l;
  unsigned int iblk, ipxl;
  unsigned char byte;
  char fname[100];

  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      iblk = i*g->m+j;
      sprintf(fname, "grid/l%d_d%d_%d_%d.grd", g->level, g->depth, g->xlim[0]+i, g->ylim[0]+j);
      f = fopen(fname, "wb");
      for(k=0;k<g->depth;k++) {
        for(l=0;l<g->depth;l++) {
          ipxl = k*g->depth+l;
          if((ipxl%8)==0) {
            if(ipxl>0) fwrite(&byte, 1, 1, f);
            byte = 0;
          }
          if(g->land[iblk][ipxl]) byte |= 1<<(ipxl%8);
        }
      }
      fwrite(&byte, 1, 1, f); /* Don't forget the last byte */
      fclose(f);
    }
  }

}

void printgrid(grid* g) {
  int i, j, k, l;
  for(j=(g->m-1);j>=0;j--) {
    for(l=(g->depth-1);l>=0;l--) {
      for(i=0;i<g->n;i++) {
        for(k=0;k<g->depth;k++) {
	  printf("%d", g->land[i*g->m+j][k*g->depth+l]);
        }
      }
      printf("\n");
    }
  }
}

void freegrid(grid* g) {

}
