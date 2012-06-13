#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stats.h"
#include "matrix.h"

float min(float* x, int n) {
  int i;
  float val = x[0];
  for(i=1;i<n;i++)
    if(x[i]<val) val = x[i];
  return val;
}

float max(float* x, int n) {
  int i;
  float val = x[0];
  for(i=1;i<n;i++)
    if(x[i]>val) val = x[i];
  return val;
}

float mean(float* x, int n) {
  int i;
  float sum = 0;
  for(i=0;i<n;i++) {
    sum += x[i];
  }
  return sum / (float)n;
}

void lm(float *y, float *x, float *z, int n) {
  int i, j;
  float X[n*2];
  float tX[2*n];
  float tXX[2*2];
  float tXXX[2*n];

  /* Add the intercept column into the design matrix */
  for(i=0;i<2;i++) {
    for(j=0;j<n;j++) {
      if(i==0) X[i*n+j] = 1;
      else X[i*n+j] = x[j];
    }
  }

  t(X, tX, 2, n);
  mult(tX, X, tXX, 2, n, 2);
  invert(tXX, 2);
  mult(tXX, tX, tXXX, 2, 2, n);
  mult(tXXX, y, z, 2, n, 1);
}

int nlm(float *y, float *x, float *z, int n, int p) {
  int i, j;

  /* Damping parameter */
  float a = 1;

  /* Matrices */
  float X[n*p];
  float tX[p*n];
  float tXX[p*p];
  float tXXX[p*n];

  for(i=0;i<p;i++) {
    for(j=0;j<n;j++) {
      X[i*n+j] = x[i*n+j];
    }
  }

  t(X, tX, p, n);
  mult(tX, X, tXX, p, n, p);
  if(!invert(tXX, p)) return 0;
  mult(tXX, tX, tXXX, p, p, n);
  mult(tXXX, y, z, p, n, 1);
  return 1;
}
