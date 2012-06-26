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
  unsigned int i, j, k, l;
  unsigned int a, b, c, d;
  unsigned int iblk, ipxl;
  float pixel = ((float)90 / pow(2, (float)g->level)) / (float)g->depth;
  float x, y;

  for(i=0;i<p->n;i++) {
    a = floor((p->p[i].xlim[0]-(-180))/pixel) - g->xlim[0]*g->depth;
    b = ceil((p->p[i].xlim[1]-(-180))/pixel) - g->xlim[0]*g->depth;
    c = floor((p->p[i].ylim[0]-(-90))/pixel) - g->ylim[0]*g->depth;
    d = ceil((p->p[i].ylim[1]-(-90))/pixel) - g->ylim[0]*g->depth;
  
    /* Check if polygons are in grid */
    if((a<0&&b<0) || (a>(g->n*g->depth)&&b>(g->n*g->depth))) continue;
    if((c<0&&d<0) || (c>(g->m*g->depth)&&d>(g->m*g->depth))) continue;

    /* Bounded to grid box */
    if(a<0) a = 0;
    if(b>g->n*g->depth) b = g->n*g->depth;
    if(c<0) c = 0;
    if(d>g->n*g->depth) d = g->m*g->depth;

    for(j=a;j<b;j++) {
      for(k=c;k<d;k++) {
        x = (-180)+pixel*(g->xlim[0]*g->depth+j);
        y = (-90)+pixel*(g->ylim[0]*g->depth+k);
        if(pip(p->p[i].n, p->p[i].x, p->p[i].y, x, y)) {
          iblk = floor(j/g->depth)*g->m+floor(k/g->depth);
          ipxl = (j%g->depth)*g->depth+(k%g->depth);
          g->land[iblk][ipxl] = 1;
        }
      }
    }
    printf("Polygon: %d\n", i);
  }
}

int main(int argc, char** argv) {
  if(argc<3) {
    fprintf(stderr, "Error: not enough arguments.\n");
    exit(1);
  }
  int level = atoi(argv[1]);
  int depth = atoi(argv[2]);

  /* Init objects */
  polygons p;
  features f;
  grid g;
  
  /* Read polygons and set features */
  readpolygons(&p);
  setlimits(&f, &p);  

  /* ... */
  startgrid(&g, &f, level, depth, 1);
  if(argc==4) {
    readgrid(&g);
    printgrid(&g);
  }
  else {
    setland(&g, &p);
    writegrid(&g);
  }

  return 0;
}
