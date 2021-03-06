#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "model.h"
#include "grid.h"
#include "features.h"
#include "math/stats.h"

float (*models[N_MDL+1])(float, model*);
float* Xinv;

grid* g_worker;
features* f_worker;
model* m_worker;

inline float linear(float h, model* m) {
  return (m->nugget + h * (m->sill - m->nugget) / m->range);
}

inline float spherical(float h, model* m) {
  return (h==0) ? 0 : ((h>m->range) ? m->sill : (m->nugget + (m->sill-m->nugget) * (((float)3*h) / ((float)2*m->range) - pow(h, 3) / ((float)2*pow(m->range, 3)))));
}

inline float exponential(float h, model* m) {
  return (h==0) ? 0 : (m->nugget+(m->sill-m->nugget) * ((float)1-exp(-h/(m->range*m->a))));
}

inline float gaussian(float h, model* m) {
  return (h==0) ? 0 : (m->nugget+(m->sill-m->nugget) * ((float)1-exp(-pow(h, 2)/(pow(m->range, 2)*m->a))));
}

void fitmodel(model* m, float* distance, float* semivariance, int n) {
  /* Jacobian matrix, residuals and parameter estimates */
  float B[3];
  float J[3*n];
  float r[n];
  float delta[3];

  /* Switch through models */
  int i, j, k;
  float sum = 0;

  /* Set the starting parameters */
  B[0] = min(semivariance, n);
  B[1] = max(semivariance, n) - min(semivariance, n);
  B[2] = max(distance, n);
  m->a = (float)1 / (float)3;

  switch(m->type) {
  case 1: /* Linear model */
    lm(semivariance, distance, B, n);
    B[1] = B[1] * max(distance, n);
    B[2] = max(distance, n);
    for(i=0;i<n;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*(distance[i]/B[2]), 2);
    break;

  case 2: /* Spherical */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<n;j++) {
	J[0*n+j] = -1;
	J[1*n+j] = (pow(distance[j], 3) - (float)3*pow(B[2], 2)*distance[j]) / ((float)2*pow(B[2], 3));
	J[2*n+j] = -((float)3*B[1]*pow(distance[j], 3) - (float)3*B[1]*pow(B[2], 2)*distance[j]) / ((float)2*pow(B[2], 4));
	r[j] = semivariance[j] - B[0] - B[1]*(((float)3/(float)2)*(distance[j]/B[2]) - ((float)1/(float)2)*pow(distance[j]/B[2], 3));
      }
      if(!nlm(r, J, delta, n, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<n;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*(((float)3/(float)2)*(distance[i]/B[2]) - ((float)1/(float)2)*pow(distance[i]/B[2], 3)), 2);
    break;

  case 3: /* Exponential */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<n;j++) {
	J[0*n+j] = -1;
	J[1*n+j] = -exp(-distance[j]/(B[2]*m->a)) * (exp(distance[j]/(B[2]*m->a)) - (float)1);
	J[2*n+j] = (B[1]*distance[j]*exp(-distance[j]/(B[2]*m->a))) / (pow(B[2], 2)*m->a);
	r[j] = semivariance[j] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*(distance[j]/B[2])));
      }
      if(!nlm(r, J, delta, n, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<n;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*(distance[i]/B[2]))), 2);
    break;
  case 4: /* Gaussian */
    for(i=0;i<MAX_ITER;i++) {
      for(j=0;j<n;j++) {
	J[0*n+j] = -1;
	J[1*n+j] = -exp(-pow(distance[j], 2)/(pow(B[2], 2)*m->a)) * (exp(pow(distance[j], 2)/(pow(B[2], 2)*m->a)) - (float)1);
	J[2*n+j] = ((float)2*B[1]*pow(distance[j], 2)*exp(-pow(distance[j], 2)/(pow(B[2], 2)*m->a))) / (pow(B[2], 3)*m->a);
	r[j] = semivariance[j] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*pow(distance[j]/B[2], 2)));
      }
      if(!nlm(r, J, delta, n, 3)) break;
      if(delta[0]==0 && delta[1]==0 && delta[2]==0) break;
      for(j=0;j<3;j++) B[j] = B[j] - delta[j];
    }
    for(i=0;i<n;i++)
      sum += pow(semivariance[i] - B[0] - B[1]*((float)1 - exp(-((float)1/m->a)*pow(distance[i]/B[2], 2))), 2);
    break;
  }

  m->MSE = sum/(float)n;
  m->nugget = B[0];
  m->sill = B[1] + B[0];
  m->range = B[2];
}

void printmodel(model* m) {
  /* JSON output */
  printf("{\n");
  printf("\t\"type\": %d,\n", m->type);
  printf("\t\"nugget\": %f,\n", m->nugget);
  printf("\t\"range\": %f,\n", m->range);
  printf("\t\"sill\": %f,\n", m->sill);
  printf("\t\"a\": %f,\n", m->a);
  printf("\t\"MSE\": %f\n", m->MSE);
  printf("}");
}

void variogram(model* m, features* f, int mdl) {
  int i, j, k;
  int n = f->n;
  int p = (pow(n, 2)-n) / 2;

  /* Euclidean distance */
  float* D = (float*)malloc(sizeof(float)*p);
  float* V = (float*)malloc(sizeof(float)*p);
  for(i=0,k=0;i<(n-1);i++)
    for(j=(i+1);j<n;j++,k++) {
      D[k] = pow(pow(f->x[i]-f->x[j], 2) + pow(f->y[i]-f->y[j], 2), 0.5);
      V[k] = (float)pow(f->response[i]-f->response[j], 2);
    }

  /* Bubble sort */
  float a, b;
  for(i=(p-1);i>0;i--)
    for(j=1;j<=i;j++)
      if(D[j-1]>D[j]) {
        a = D[j-1];
        b = V[j-1];
        D[j-1] = D[j];
        D[j] = a;
        V[j-1] = V[j];
        V[j] = b;
      }

  /* Lags */
  float sum;
  int lags = p>MAX_LAGS ? MAX_LAGS : p;
  float* distance = (float*)malloc(sizeof(float)*lags);
  float* semivariance = (float*)malloc(sizeof(float)*lags);

  for(i=1,j=0;i<=lags;i++) {
    sum = 0;
    for(k=0;j<((float)i/(float)lags)*(float)p;j++,k++) {
      sum += V[j];
    }
    distance[i-1] = D[j-1];
    semivariance[i-1] = sum / (float)k;
  }

  if(mdl==DEFAULT_MDL) {
    /* Cycle through models and pick best fit */
    model mt[N_MDL];
    for(i=0;i<N_MDL;i++) {
      mt[i].type = i+1;
      fitmodel(&mt[i], distance, semivariance, lags);
    }
    m->MSE = 1000;
    for(i=0;i<N_MDL;i++)
      if(mt[i].MSE<m->MSE) {
        m->type = mt[i].type;
        m->nugget = mt[i].nugget;
        m->range = mt[i].range;
        m->sill = mt[i].sill;
        m->a = mt[i].a;
        m->MSE = mt[i].MSE;
      }
  }
  else {
    m->type = mdl;
    fitmodel(m, distance, semivariance, lags);
  }
  free(D);
  free(V);
  free(distance);
  free(semivariance);
}

void predict(grid* g, model* m, features* f) {
  /* Set up copies to n and model for the worker thread */
  g_worker = g;
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
  for(i=0;i<=f->n;i++)
    for(j=0;j<=f->n;j++)
      if(i==f->n && j!=f->n) Xinv[i*(f->n+1)+j] = 1;
      else
        if(i!=f->n && j==f->n) Xinv[i*(f->n+1)+j] = 1;
        else
          if(i==f->n && j==f->n) Xinv[i*(f->n+1)+j] = 0;
          else
            Xinv[i*(f->n+1)+j] = models[m->type](D[i*f->n+j], m);
  invert(Xinv, f->n+1);

  /* Set up workers */
  int k, l;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Sub-block or per-block threads */
  k = (g->n==1 && g->m==1) ? (int)floor(pow(MAX_SUBBLOCKS, 0.5)) : g->n;
  l = (g->n==1 && g->m==1) ? k : g->m;
  pthread_t* workers = (pthread_t*)malloc(sizeof(pthread_t)*k*l);
  int** x = (int**)malloc(sizeof(int*)*k*l);

  for(i=0;i<k;i++)
    for(j=0;j<l;j++) {
      x[i*l+j] = (int*)malloc(sizeof(int)*2);
      x[i*l+j][0] = i;
      x[i*l+j][1] = j;
      pthread_create(&workers[i*l+j], &attr, worker, (void*)x[i*l+j]);
    }
    
  /* Wait for threads to finish */
  void* status;
  pthread_attr_destroy(&attr);
  for(i=0;i<k;i++)
    for(j=0;j<l;j++)
      pthread_join(workers[i*l+j], &status);

  free(Xinv);
  free(D);
  free(x);
}

void* worker(void* v) {
  int i, j, k, l, iblk, ipxl, z;
  int* x = (int*)v;
  float a, b, w;
  float pixel = (float)(90) / pow(2, (float)g_worker->level);
  z = (g_worker->n==1 && g_worker->m==1)? g_worker->depth/floor(pow(MAX_SUBBLOCKS, 0.5)) : 0;
  iblk = z ? 0 : x[0]*g_worker->m+x[1];

  /**
   * Main loop for block estimation 
   */
  for(i=z*x[0];i<(z?z*(x[0]+1):g_worker->depth);i++) {
    for(j=z*x[1];j<(z?z*(x[1]+1):g_worker->depth);j++) {
      ipxl = i*g_worker->depth+j;
      if(g_worker->land[iblk][ipxl]) {
        a = (float)(-180) + pixel*(float)(g_worker->xlim[0]+(z?0:x[0]))+(float)i*pixel/g_worker->depth;
        b = (float)(-90) + pixel*(float)(g_worker->ylim[0]+(z?0:x[1]))+(float)j*pixel/g_worker->depth;

        g_worker->value[iblk][ipxl] = 0;
        for(k=0;k<f_worker->n;k++) {
          w = 0;
          for(l=0;l<f_worker->n;l++)
	    w += Xinv[k*(f_worker->n+1)+l] * models[m_worker->type](pow(pow(f_worker->x[l]-a, 2) + pow(f_worker->y[l]-b, 2), (float)0.5), m_worker);
          w += Xinv[k*(f_worker->n+1)+f_worker->n];
          g_worker->value[iblk][ipxl] += w * f_worker->response[k];
        }
      }
      else g_worker->value[iblk][ipxl] = DEFAULT_VAL;
    }
  }
  pthread_exit(NULL);
}
