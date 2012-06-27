#ifndef IMAGE_H
#define IMAGE_H
#include "grid.h"

typedef struct {
  unsigned int n;
  unsigned char** rgb;
} spectrum;

void color(float, float*, spectrum*, char*);

void readppm(grid*);

void writeppm(grid*, features*, spectrum*);

void heatspectrum(spectrum*);

#endif
