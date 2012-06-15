#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "polygons.h"

int pip(int n, float *X, float *Y, float x, float y) {
  int i, j, c = 0;
  for(i=0,j=n-1;i<n;j=i++) {
    if( ((Y[i]>y)!=(Y[j]>y)) && (x<(X[j]-X[i])*(y-Y[i])/(Y[j]-Y[i])+X[i]) )
      c = !c;
  }
  return c;
}

void readpolygons(polygons* p) {
  if(access("polygons/all.ply", F_OK) == -1) {
    fprintf(stderr, "Error: file polygons/all.ply does not exist.\n");
    exit(1);
  }
  FILE* f;
  f = fopen("polygons/all.ply", "rb");
  fread(&p->n, 1, sizeof(unsigned int), f);

  int i;
  p->p = (polygon*)malloc(sizeof(polygon)*p->n);
  for(i=0;i<p->n;i++) {
    fread(&p->p[i].n, 1, sizeof(unsigned int), f);
    fread(p->p[i].xlim, 2, sizeof(float), f);
    fread(p->p[i].ylim, 2, sizeof(float), f);

    p->p[i].x = (float*)malloc(sizeof(float)*p->p[i].n);
    p->p[i].y = (float*)malloc(sizeof(float)*p->p[i].n);
    fread(p->p[i].x, p->p[i].n, sizeof(float), f);
    fread(p->p[i].y, p->p[i].n, sizeof(float), f);
  }

}

void writepolygons(polygons* p) {
  FILE* f;
  f = fopen("polygons/all.ply", "wb");
  fwrite(&p->n, 1, sizeof(unsigned int), f);
  
  int i;
  for(i=0;i<p->n;i++) {
    fwrite(&p->p[i].n, 1, sizeof(unsigned int), f);
    fwrite(p->p[i].xlim, 2, sizeof(float), f);
    fwrite(p->p[i].ylim, 2, sizeof(float), f);
    fwrite(p->p[i].x, p->p[i].n, sizeof(float), f);
    fwrite(p->p[i].y, p->p[i].n, sizeof(float), f);
  }
  fclose(f);
}

void printpolygons(polygons* p) {
  /* JSON formatted stdout */
  int i, j;
  printf("{\n\t\"n\": %d,\n", p->n);
  printf("\t\"polygons\": [\n");
  for(i=0;i<p->n;i++) {
    printf("\t\t{\n");
    printf("\t\t\t\"n\": %d,\n", p->p[i].n);
    printf("\t\t\t\"xlim\": [%f, %f],\n", p->p[i].xlim[0], p->p[i].xlim[1]);
    printf("\t\t\t\"ylim\": [%f, %f],\n", p->p[i].ylim[0], p->p[i].ylim[1]);

    printf("\t\t\t\"x\": [\n");
    for(j=0;j<p->p[i].n;j++) {
      printf("\t\t\t\t%f", p->p[i].x[j]);
      if(j==(p->p[i].n-1)) printf("\n");
      else printf(",\n");
    }
    printf("\t\t\t],\n");

    printf("\t\t\t\"y\": [\n");
    for(j=0;j<p->p[i].n;j++) {
      printf("\t\t\t\t%f", p->p[i].y[j]);
      if(j==(p->p[i].n-1)) printf("\n");
      else printf(",\n");
    }
    printf("\t\t\t]\n");

    printf("\t\t}");
    if(i==(p->n-1)) printf("\n");
    else printf(",\n");
  }
  printf("\t]\n");
  printf("}\n");
}
