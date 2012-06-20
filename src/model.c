#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "model.h"
#include "grid.h"
#include "features.h"
#include "math/stats.h"

float (*models[N_MDL+1])(float, model*);
float* Xinv;
features* f_worker;
model* m_worker;

float linear(float h, model* m) {
  return (m->nugget + h * (m->sill - m->nugget) / m->range);
}

float spherical(float h, model* m) {
  if(h>m->range) return m->sill;
  if(h<=m->range && h>0) {
    return (m->nugget + (m->sill-m->nugget) * (((float)3*h) / ((float)2*m->range) - pow(h, 3) / ((float)2*pow(m->range, 3))));
  }
  else return (float)0;
}

float exponential(float h, model* m) {
  if(h==0) return (float)0;
  else {
    return (m->nugget+(m->sill-m->nugget) * ((float)1-exp(-h/(m->range*m->a))));
  }
}

float gaussian(float h, model* m) {
  if(h==0) return (float)0;
  else {
    return (m->nugget+(m->sill-m->nugget) * ((float)1-exp(-pow(h, 2)/(pow(m->range, 2)*m->a))));
  }
}

void fitmodel(model* m, float* distance, float* semivariance) {
  /* Jacobian matrix, residuals and parameter estimates */
  float B[3];
  float J[3*LAGS];
  float r[LAGS];
  float delta[3];

  /* Switch through models */
  int i, j, k;
  float sum = 0;

  /* Set the starting parameters */
  B[0] = min(semivariance, LAGS);
  B[1] = max(semivariance, LAGS) - min(semivariance, LAGS);
  B[2] = max(distance, LAGS);
  m->a = (float)1 / (float)3;

  switch(m->type) {
  case 1: /* Linear model */
    lm(semivariance, distance, B, LAGS);
    B[1] = B[1] * max(distance, LAGS);
    B[2] = max(distance, LAGS);
    for(i=0;i<LAGS;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*(distance[i]/B[2]), 2);
    break;

  case 2: /* Spherical */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<LAGS;j++) {
	J[0*LAGS+j] = -1;
	J[1*LAGS+j] = (pow(distance[j], 3) - (float)3*pow(B[2], 2)*distance[j]) / ((float)2*pow(B[2], 3));
	J[2*LAGS+j] = -((float)3*B[1]*pow(distance[j], 3) - (float)3*B[1]*pow(B[2], 2)*distance[j]) / ((float)2*pow(B[2], 4));
	r[j] = semivariance[j] - B[0] - B[1]*(((float)3/(float)2)*(distance[j]/B[2]) - ((float)1/(float)2)*pow(distance[j]/B[2], 3));
      }
      if(!nlm(r, J, delta, LAGS, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<LAGS;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*(((float)3/(float)2)*(distance[i]/B[2]) - ((float)1/(float)2)*pow(distance[i]/B[2], 3)), 2);
    break;

  case 3: /* Exponential */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<LAGS;j++) {
	J[0*LAGS+j] = -1;
	J[1*LAGS+j] = -exp(-distance[j]/(B[2]*m->a)) * (exp(distance[j]/(B[2]*m->a)) - (float)1);
	J[2*LAGS+j] = (B[1]*distance[j]*exp(-distance[j]/(B[2]*m->a))) / (pow(B[2], 2)*m->a);
	r[j] = semivariance[j] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*(distance[j]/B[2])));
      }
      if(!nlm(r, J, delta, LAGS, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<LAGS;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*(distance[i]/B[2]))), 2);
    break;
  case 4: /* Gaussian */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<LAGS;j++) {
	J[0*LAGS+j] = -1;
	J[1*LAGS+j] = -exp(-pow(distance[j], 2)/(pow(B[2], 2)*m->a)) * (exp(pow(distance[j], 2)/(pow(B[2], 2)*m->a)) - (float)1);
	J[2*LAGS+j] = ((float)2*B[1]*pow(distance[j], 2)*exp(-pow(distance[j], 2)/(pow(B[2], 2)*m->a))) / (pow(B[2], 3)*m->a);
	r[j] = semivariance[j] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*pow(distance[j]/B[2], 2)));
      }
      if(!nlm(r, J, delta, LAGS, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<LAGS;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*pow(distance[i]/B[2], 2))), 2);
    break;
  }

  m->MSE = sum/((float)LAGS - (float)3);
  m->nugget = B[0];
  m->sill = B[1] + B[0];
  m->range = B[2];
}

void variogram(model* m, features* f, int mdl) {
  /* Partition coordinate set into lags */
  float** D = (float**)malloc(sizeof(float*)*f->n);
  float** V = (float**)malloc(sizeof(float*)*f->n);

  int i, j, k;
  for(i=0;i<f->n;i++) {
    D[i] = (float*)malloc(sizeof(float)*f->n);
    V[i] = (float*)malloc(sizeof(float)*f->n);
    for(j=0;j<f->n;j++) {
      D[i][j] = pow(pow(f->x[i]-f->x[j], 2) + pow(f->y[i]-f->y[j], 2), 0.5);
      V[i][j] = pow(f->response[i]-f->response[j], 2);
    }
  }

  float cutoff = pow(pow(max(f->x, f->n)-min(f->x, f->n), 2) + pow(max(f->y, f->n)-min(f->y, f->n), 2), 0.5) / (float)3;

  /* Fit -> semivariance ~ distance */
  float* distance = (float*)malloc(sizeof(float)*LAGS);
  float* semivariance = (float*)malloc(sizeof(float)*LAGS);

  int n;
  float sum;
  for(i=0;i<LAGS;i++) {
    sum = 0;
    n = 0;
    for(j=0;j<(f->n-1);j++) {
      for(k=(j+1);k<f->n;k++) {
        if(D[j][k]<=((float)(i+1) * cutoff/(float)LAGS)) {
          n++;
          sum += V[j][k];
        }
      }
    }
    if(n>0) {
      distance[i] = (float)(i+1) * cutoff / (float)LAGS;
      semivariance[i] = sum / n;
    }
    else {
      fprintf(stderr, "Error: Unable to calculate semivariance\n");
      exit(1);
    }
  }

  if(mdl==DEFAULT_MDL) {
    /* Cycle through models and pick best fit */
    model mt[N_MDL];
    for(i=0;i<N_MDL;i++) {
      mt[i].type = i+1;
      fitmodel(&mt[i], distance, semivariance);
    }
    m->MSE = 1000;
    for(i=0;i<N_MDL;i++) {
      if(mt[i].MSE<m->MSE) {
        m->type = mt[i].type;
        m->nugget = mt[i].nugget;
        m->range = mt[i].range;
        m->sill = mt[i].sill;
        m->a = mt[i].a;
        m->MSE = mt[i].MSE;
      }
    }
  }
  else {
    m->type = mdl;
    fitmodel(m, distance, semivariance);
  }
  free(D);
  free(V);
  free(distance);
  free(semivariance);
}

void predict(grid* g, model* m, features* f) {
  /* Set up copies to n and model for the worker thread */
  f_worker = f;
  m_worker = m;

  /* Pointers to variogram models */
  models[0] = NULL; /* Pointer to null for first */
  models[1] = &linear;
  models[2] = &spherical;
  models[3] = &exponential;
  models[4] = &gaussian;

  /* Distance matrix */
  int i, j;
  float* D = (float*)malloc(sizeof(float)*f->n*f->n);
  for(i=0;i<f->n;i++) 
    for(j=0;j<f->n;j++) 
      D[i*f->n+j] = pow(pow(f->x[i]-f->x[j], 2) + pow(f->y[i]-f->y[j], 2), 0.5);

  /* X^-1 * Y ~ W */
  Xinv = (float*)malloc(sizeof(float)*(f->n+1)*(f->n+1));
  for(i=0;i<=f->n;i++) {
    for(j=0;j<=f->n;j++) {
      if(i==f->n && j!=f->n) Xinv[i*(f->n+1)+j] = 1;
      else {
        if(i!=f->n && j==f->n) Xinv[i*(f->n+1)+j] = 1;
        else {
          if(i==f->n && j==f->n) Xinv[i*(f->n+1)+j] = 0;
          else {
            Xinv[i*(f->n+1)+j] = models[m->type](D[i*f->n+j], m);
          }
        }
      }
    }
  }
  invert(Xinv, f->n+1);

  /* Estimate values based on model */
  int k = g->xlim[1]-g->xlim[0];
  int l = g->ylim[1]-g->xlim[0];
  packet* p = (packet*)malloc(sizeof(packet)*k*l*g->depth*g->depth);
  for(i=0;i<k*g->depth;i++) {
    for(j=0;j<l*g->depth;j++) {
      if(g->land[i*l*g->depth+j]) {
        p[i*l*g->depth+j].value = &g->value[i*l*g->depth+j];
        p[i*l*g->depth+j].x = f->n;
        p[i*l*g->depth+j].y = f->n;
        worker(&p[i*l*g->depth+j]);
      }
      else g->value[i*l*g->depth+j] = DEFAULT_VAL;
    }
  }

  for(i=0;i<k*g->depth;i++) {
    for(j=0;j<l*g->depth;j++) {
      printf("%f ", g->value[i*l*g->depth+j]);
    }
    printf("\n");
  }

  free(Xinv);
  free(D);
}

void* worker(void* v) {
  packet* p = (packet*)v;

  int i, j;
  float w;
  *p->value = 0;
  for(i=0;i<f_worker->n;i++) {
    w = 0;
    for(j=0;j<f_worker->n;j++)
      w += Xinv[i*(f_worker->n+1)+j] * models[m_worker->type](pow(pow(f_worker->x[j]-p->x, 2) + pow(f_worker->y[j]-p->y, 2), (float)0.5), m_worker);
     w += Xinv[i*(f_worker->n+1)+f_worker->n];
     *p->value += w * f_worker->response[i];
   }
}
