#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "features.h"

void readfeatures(features* f) {
  float values[MAX_FEATURES][3];
  char line[MAX_LINE];
  char* value;

  int i = 0;
  int j;
  while(gets(line)==line) {
    value = strtok(line, ",");
    j = 0;
    while(value != NULL && j<3) {
      values[i][j] = atof(value);
      value = strtok(NULL, ",");
      j++;
    }
    if(j!=3) {
      fprintf(stderr, "Error: Column variables should be of format - x,y,response\n");
      exit(1);
    }
    i++;
  }
  if(i==0) {
    fprintf(stderr, "Error: No data provided\n");
    exit(1);
  }

  f->n = i;
  f->x = (float*)malloc(sizeof(float)*f->n);
  f->y = (float*)malloc(sizeof(float)*f->n);
  f->response = (float*)malloc(sizeof(float)*f->n);
  for(i=0;i<f->n;i++) {
    f->x[i] = values[i][0];
    f->y[i] = values[i][1];
    f->response[i] = values[i][2];
  }
}


