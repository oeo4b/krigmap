#include <stdlib.h>
#include <math.h>
#include "matrix.h"

/* Matrix algebra */
void t(float *x, float *y, int n, int m) {
  int i, j;
  for(i=0;i<m;i++) {
    for(j=0;j<n;j++) {
      y[i*n+j] = x[j*m+i];
    }
  }
}

void mult(float *y, float *x, float *z, int p, int m, int n) {
  int i, j, k;
  for(i=0;i<n;i++) {
    for(j=0;j<p;j++) {
      z[i*p+j] = 0;
      for(k=0;k<m;k++) {
        z[i*p+j] += x[i*m+k] * y[k*p+j];
      }
    }
  }
}

int invert(float *a, int n) {
  int m = n;
  float b[n][n];
  int indxc[n];
  int indxr[n];
  int ipiv [n];

  int i, icol, irow, j, k, l, ll;
  float big, dum, pivinv, temp;

  for(i=0;i<n;i++) {
    for(j=0;j<n;j++) {
      if(i==j) b[i][j] = 1;
      else b[i][j] = 0;
    }
  }
  for(j=0;j<n;j++) ipiv[j] = 0;
  for(i=0;i<n;i++) {
    big = 0;
    for(j=0;j<n;j++) {
      if(ipiv[j]!=1) {
        for(k=0;k<n;k++) {
          if(ipiv[k]==0) {
            if(fabs(a[j*n+k])>=big) {
              big = fabs(a[j*n+k]);
              irow = j;
              icol = k;
            }
          }
        }
      }
    }
    ++(ipiv[icol]);
    if(irow!=icol) {
      for(l=0;l<n;l++) {
        temp = a[irow*n+l];
        a[irow*n+l] = a[icol*n+l];
        a[icol*n+l] = temp;
      }
      for(l=0;l<m;l++) {
        temp = b[irow][l];
        b[irow][l] = b[icol][l];
        b[icol][l] = temp;
      }
    }
    indxr[i] = irow;
    indxc[i] = icol;

    if(a[icol*n+icol]==0) { /* Singular matrix */
      return 0;
    }

    pivinv = (float)1 / a[icol*n+icol];
    a[icol*n+icol] = 1;
    for(l=0;l<n;l++) a[icol*n+l] *= pivinv;
    for(l=0;l<m;l++) b[icol][l] *= pivinv;

    for(ll=0;ll<n;ll++) {
      if(ll!=icol) {
        dum = a[ll*n+icol];
        a[ll*n+icol] = 0;
        for(l=0;l<n;l++) a[ll*n+l] -= a[icol*n+l]*dum;
        for(l=0;l<m;l++) b[ll][l] -= b[icol][l]*dum;
      }
    }
  }

  for(l=(n-1);l>=0;l--) {
    if(indxr[l]!=indxc[l]) {
      for(k=0;k<n;k++) {
        temp = a[k*n+indxr[l]];
        a[k*n+indxr[l]] = a[k*n+indxc[l]];
        a[k*n+indxc[l]] = temp;
      }
    }
  }
  return 1;
}

