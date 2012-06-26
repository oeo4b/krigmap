#ifndef GRID_H
#define GRID_H
#include "features.h"

typedef struct {
  unsigned int n;
  unsigned int m;
  unsigned int xlim[2];
  unsigned int ylim[2];
  unsigned short level;
  unsigned int depth;
  unsigned char** land;
  float** value;
} grid;

void startgrid(grid*, features*, int, int, int);

void readgrid(grid*);

void writegrid(grid*);

void printgrid(grid*);

void freegrid(grid*);

#endif
