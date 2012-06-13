#ifndef STATS_H
#define STATS_H
#include "../model.h"

float min(float*, int);

float max(float*, int);

float mean(float*, int);

void lm(float*, float*, float*, int);

int nlm(float*, float*, float*, int, int);

#endif
