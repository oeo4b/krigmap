#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "polygons.h"
#include "math/stats.h"

#define LINE_BUF 200
#define VAL_BUF 100

void readcsv(FILE* csv, polygons* p) {
  char line[LINE_BUF];
  char* value;
  int i, j, n = 0;

  /* Calculate total number of polygons */
  while(fgets(line, LINE_BUF, csv)!=NULL) {
    value = strtok(line, ",");
    if(value==NULL) {
      fprintf(stderr, "Error: improper format.\n");
      exit(1);
    }
    i = atoi(value);
    while(i>1) {
      if(fgets(line, LINE_BUF, csv)==NULL) {
        fprintf(stderr, "Error: irregular polygon length in csv file.\n");
        exit(1);
      }
      i--;
    }
    n++;
  }

  /* Assign polygon lengths to array for each polygon */
  fseek(csv, 0, SEEK_SET); /* Reset to start of stream */
  int* m =(int*)malloc(sizeof(int)*n);
  i = 0;
  while(fgets(line, LINE_BUF, csv)!=NULL) {
    value = strtok(line, ",");
    j = atoi(value);
    m[i] = j;
    while(j>1) {
      fgets(line, LINE_BUF, csv);
      j--;
    }
    i++;
  }

  /* Set values to polygons object */
  float xlim[2];
  float ylim[2];
  int k;
  fseek(csv, 0, SEEK_SET); /* Reset to start of stream */
  p->n = n;
  p->p = (polygon*)malloc(sizeof(polygon)*p->n);
  i = 0;
  while(fgets(line, LINE_BUF, csv)!=NULL) {
    value = strtok(line, ",");
    j = atoi(value);

    /* Allocate mem for polygon object */
    p->p[i].n = j;
    p->p[i].x = (float*)malloc(sizeof(float)*j);
    p->p[i].y = (float*)malloc(sizeof(float)*j);
    
    /* Set coordinates */
    k = 0;
    while(j>1) {
      if(k>0) value = strtok(line, ",");
      value = strtok(NULL, ","); /* Longitude */
      if(value==NULL) {
        fprintf(stderr, "Error: improper format.\n");
        exit(1);
      }
      p->p[i].x[k] = atof(value);

      value = strtok(NULL, ","); /* Latitude */
      if(value==NULL) {
        fprintf(stderr, "Error: improper format.\n");
        exit(1);
      }
      p->p[i].y[k] = atof(value);

      fgets(line, LINE_BUF, csv);
      k++;
      j--;
    }
    value = strtok(line, ",");
    value = strtok(NULL, ",");
    p->p[i].x[k] = atof(value);
    value = strtok(NULL, ",");
    p->p[i].y[k] = atof(value);

    /* Limits for polygon */
    p->p[i].xlim[0] = min(p->p[i].x, p->p[i].n);
    p->p[i].xlim[1] = max(p->p[i].x, p->p[i].n);
    p->p[i].ylim[0] = min(p->p[i].y, p->p[i].n);
    p->p[i].ylim[1] = max(p->p[i].y, p->p[i].n);

    i++;
  }

  free(m);
}

int main(int argc, char** argv) {
  /**
   * format: ./convpoly [*.csv]
   */
  polygons p;
  if(argc<2) {
    readpolygons(&p);
  }
  else {
    /* Read and convert the csv file to a polygons type */
    FILE* f;
    f = fopen(argv[1], "r");
    readcsv(f, &p);
    fclose(f);
    writepolygons(&p);
  }

  /* Output polygons object */
  printpolygons(&p);

  return 0;
}
