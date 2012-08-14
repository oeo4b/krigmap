#ifndef MODEL_H
#define MODEL_H
#include "grid.h"
#include "features.h"

#define DEFAULT_MDL 0
#define LIN_MDL 1
#define SPHR_MDL 2
#define EXP_MDL 3
#define GAU_MDL 4
#define N_MDL 4

#define MAX_LAGS 10
#define MAX_ITER 25

#define MAX_SUBBLOCKS 16

#define DEFAULT_VAL -1

typedef struct {
  unsigned int type;
  float nugget;
  float range;
  float sill;
  float a;
  float MSE;
} model;

typedef struct {
  float x;
  float y;
  float* value;
} packet;

inline float linear(float, model*);

inline float spherical(float, model*);

inline float exponential(float, model*);

inline float gaussian(float, model*);

void fitmodel(model*, float*, float*, int);

void variogram(model*, features*, int);

void predict(grid*, model*, features*);

void printmodel(model*);

void* worker(void*);

#endif
