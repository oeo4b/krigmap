#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "grid.h"
#include "polygons.h"
#include "math/stats.h"

void setlimits(features* f, polygons* p) {
  int i;
  f->n = 2;
  f->x = (float*)malloc(sizeof(float)*f->n);
  f->y = (float*)malloc(sizeof(float)*f->n);

  for(i=0;i<f->n;i++) {
    f->x[i] = p->p[0].xlim[i];
    f->y[i] = p->p[0].ylim[i];
  }

  for(i=1;i<p->n;i++) {
    if(p->p[i].xlim[0]<f->x[0]) f->x[0] = p->p[i].xlim[0];
    if(p->p[i].xlim[1]>f->x[1]) f->x[1] = p->p[i].xlim[1];
    if(p->p[i].ylim[0]<f->y[0]) f->y[0] = p->p[i].ylim[0];
    if(p->p[i].ylim[1]>f->y[1]) f->y[1] = p->p[i].ylim[1];
  }

}

void setland(grid* g, polygons* p) {
  unsigned int i, j, k;
  unsigned int a, b, c, d;
  unsigned int n = g->xlim[1]-g->xlim[0];
  unsigned int m = g->ylim[1]-g->ylim[0];
  float pixel = ((float)90 / pow(2, (float)g->level)) / (float)g->depth;
  float x, y;

  for(i=0;i<p->n;i++) {
    a = floor((p->p[i].xlim[0]-(-180))/pixel) - g->xlim[0]*g->depth;
    b = ceil((p->p[i].xlim[1]-(-180))/pixel) - g->xlim[0]*g->depth;
    c = floor((p->p[i].ylim[0]-(-90))/pixel) - g->ylim[0]*g->depth;
    d = ceil((p->p[i].ylim[1]-(-90))/pixel) - g->ylim[0]*g->depth;
  
    /* Check if polygons are in grid */
    if((a<0&&b<0) || (a>(n*g->depth)&&b>(n*g->depth))) continue;
    if((c<0&&d<0) || (c>(m*g->depth)&&d>(m*g->depth))) continue;

    /* Bounded to grid box */
    if(a<0) a = 0;
    if(b>n*g->depth) b = n*g->depth;
    if(c<0) c = 0;
    if(d>n*g->depth) d = m*g->depth;

    for(j=a;j<b;j++) {
      for(k=c;k<d;k++) {
        x = (-180)+pixel*(g->xlim[0]*g->depth+j);
        y = (-90)+pixel*(g->ylim[0]*g->depth+k);
        printf("%f, %f, %d\n", x, y, j*m*g->depth);
        if(pip(p->p[i].n, p->p[i].x, p->p[i].y, x, y)) {
          g->land[j*m*g->depth+k] = 1;
        }
      }
    }

  }

}

#define MAX_DEPTH 1000

int main(int argc, char** argv) {
  /* Init objects */
  polygons p;
  features fpoly, fgrid;
  grid g;
  int i, j;
  
  /* Read polygons and set features */
  readpolygons(&p);
  setlimits(&fpoly, &p);  

  /* ... */
  startgrid(&g, &fgrid, 12, 200); /* level: 12, depth: 200 */
  setland(&g, &p);

  /* Write to file */
  //writegrid(&g);

  /* Deallocate */
  freegrid(&g);

  return 0;
}
