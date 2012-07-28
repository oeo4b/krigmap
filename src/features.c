#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "features.h"

void readfeatures(features* f) {
  float values[MAX_FEATURES][3];
  char line[MAX_LINE];
  char* value;

  int i = 0;
  int j;
  while(fgets(line, MAX_LINE, stdin)==line) {
    value = strtok(line, ",");
    j = 0;
    while(value != NULL && j<3) {
      values[i][j] = atof(value);
      value = strtok(NULL, ",");
      j++;
    }
    if(j==3) i++;
  }

  f->n = i;
  if(f->n==0) return; /* Return if no data provided */

  f->x = (float*)malloc(sizeof(float)*f->n);
  f->y = (float*)malloc(sizeof(float)*f->n);
  f->response = (float*)malloc(sizeof(float)*f->n);
  for(i=0;i<f->n;i++) {
    f->x[i] = values[i][0];
    f->y[i] = values[i][1];
    f->response[i] = values[i][2];
  }
}


