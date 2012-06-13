#ifndef POLYGONS_H
#define POLYGONS_H

typedef struct {
  unsigned int n;
  float xlim[2];
  float ylim[2];
  float* x;
  float* y;
} polygon;

typedef struct {
  unsigned int n;
  polygon* p;
} polygons;

#endif
