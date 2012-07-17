#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#include "grid.h"
#include "math/stats.h"

void color(float x, float* range, spectrum* s, char* rgb) {
  int i, j;

  if(x<range[0]) {
    for(i=0;i<3;i++)
      rgb[i] = 0;
    return;
  }

  if(x>range[1]) {
    for(i=0;i<3;i++)
      rgb[i] = 255;
    return;
  }

  j = ceil(s->n * (x-range[0])/(range[1]-range[0]));
  for(i=0;i<3;i++)
    rgb[i] = s->rgb[j-1][i];
}

void readppm(grid* g) {

}

void printppm(grid* g, features* x, spectrum* s) {
 /* Print block by block */
  int i, j, k, l, m, iblk, ipxl;
  char rgb[3];

  /* Find response range */
  float range[] = { min(x->response, x->n), max(x->response, x->n) };

  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      iblk = i*g->m+j;  
      printf("P6\n%d %d\n255\n", g->depth, g->depth);
      for(l=(g->depth-1);l>=0;l--) {
        for(k=0;k<g->depth;k++) {
          ipxl = k*g->depth+l;
          if(g->land[iblk][ipxl]) {
            color(g->value[iblk][ipxl], range, s, rgb);
            for(m=0;m<3;m++)
              printf("%c", rgb[m]);
          }
          else {
            for(m=0;m<3;m++) {
              printf("%c", g->land[iblk][ipxl]);
            }
          }
        }
      }
    }
  }
}

void writeppm(grid* g, features* x, spectrum* s) {
  /* Write block by block */
  FILE* f;
  int i, j, k, l, m, iblk, ipxl;
  char fname[100];
  char format[100];
  char rgb[3];

  /* Find response range */
  float range[] = { min(x->response, x->n), max(x->response, x->n) };

  for(i=0;i<g->n;i++) {
    for(j=0;j<g->m;j++) {
      iblk = i*g->m+j;
      printf("Writing block (%d, %d)\n", g->xlim[0]+i, g->ylim[0]+j);
      sprintf(fname, "tmp/l%d_d%d_%d_%d.ppm", g->level, g->depth, g->xlim[0]+i, g->ylim[0]+j);
      f = fopen(fname, "wb");
  
      fprintf(f, "P6\n%d %d\n255\n", g->depth, g->depth);
      for(l=(g->depth-1);l>=0;l--) {
        for(k=0;k<g->depth;k++) {
          ipxl = k*g->depth+l;
          if(g->land[iblk][ipxl]) {
            color(g->value[iblk][ipxl], range, s, rgb);  
            fwrite(rgb, 1, 3, f);
          }
          else {
            for(m=0;m<3;m++) {
              fwrite(&g->land[iblk][ipxl], 1, 1, f);
            }
          }
        }
      }
      fclose(f);
    }
  }
}

/* Color spectrums */
void heatspectrum(spectrum* s) {
  int i;
  s->n = 250;
  s->rgb = (unsigned char**)malloc(sizeof(unsigned char*)*s->n);
  for(i=0;i<s->n;i++)
    s->rgb[i] = (unsigned char*)malloc(sizeof(unsigned char)*3); /* 24-bit */

  for(i=0;i<s->n;i++) {
    s->rgb[i][0] = i+5;
    s->rgb[i][1] = 0;
    s->rgb[i][2] = 0;
  }
}
