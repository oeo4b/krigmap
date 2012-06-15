#ifndef GRID_H
#define GRID_H
#include "features.h"

#define LEVEL 12
#define SIDE 200

typedef struct {
  float xlim[2];
  float ylim[2];
  unsigned int n;
  unsigned int m;
  float pixel;
  char* land;
  float* value;
} grid;

void startgrid(grid*, features*);

void writegrid(grid*);

void printgrid(grid*);

#endif
