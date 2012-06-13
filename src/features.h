#ifndef FEATURES_H
#define FEATURES_H

#define MAX_LINE 100
#define MAX_FEATURES 1000

typedef struct {
  unsigned int n;
  float* x;
  float* y;
  float* response;
} features;

void readfeatures(features*);

#endif
