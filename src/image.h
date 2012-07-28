#ifndef IMAGE_H
#define IMAGE_H
#include "grid.h"

#define BLANK_VALUE 1

typedef struct {
  unsigned int n;
  unsigned char** rgb;
} spectrum;

void color(float, float*, spectrum*, char*);

void readppm(grid*);

void printppm(grid*, features*, spectrum*, int);

void writeppm(grid*, features*, spectrum*);

void heatspectrum(spectrum*);

#endif
