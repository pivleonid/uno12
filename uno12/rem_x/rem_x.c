/*
 * File: rem_x.c
 *
 * MATLAB Coder version            : 3.0
 * C/C++ source code generated on  : 06-Mar-2017 12:54:04
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "rem_x.h"

/* Function Declarations */
static void Double2MultiWord(double u1, unsigned long y[], int n);
static double rt_remd_snf(double u0, double u1);
static double rt_roundd_snf(double u);

/* Function Definitions */

/*
 * Arguments    : double u1
 *                unsigned long y[]
 *                int n
 * Return Type  : void
 */
static void Double2MultiWord(double u1, unsigned long y[], int n)
{
  boolean_T isNegative;
  int currExp;
  double b_yn;
  int msl;
  int cb;
  int i;
  int prevExp;
  double yd;
  unsigned long u1i;
  unsigned long yi;
  isNegative = (u1 < 0.0);
  b_yn = frexp(u1, &currExp);
  if (currExp <= 0) {
    msl = -1;
  } else {
    msl = (currExp - 1) / 32;
  }

  cb = 1;
  for (i = msl + 1; i < n; i++) {
    y[i] = 0UL;
  }

  if (isNegative) {
    b_yn = -b_yn;
  }

  prevExp = 32 * msl;
  for (i = msl; i >= 0; i--) {
    b_yn = ldexp(b_yn, currExp - prevExp);
    yd = floor(b_yn);
    b_yn -= yd;
    if (i < n) {
      y[i] = (unsigned long)yd;
    }

    currExp = prevExp;
    prevExp -= 32;
  }

  if (isNegative) {
    for (i = 0; i < n; i++) {
      u1i = ~y[i];
      yi = u1i + cb;
      y[i] = yi;
      cb = (yi < u1i);
    }
  }
}

/*
 * Arguments    : double u0
 *                double u1
 * Return Type  : double
 */
static double rt_remd_snf(double u0, double u1)
{
  double y;
  double b_u1;
  double tr;
  if (!((!rtIsNaN(u0)) && (!rtIsInf(u0)) && ((!rtIsNaN(u1)) && (!rtIsInf(u1)))))
  {
    y = rtNaN;
  } else {
    if (u1 < 0.0) {
      b_u1 = ceil(u1);
    } else {
      b_u1 = floor(u1);
    }

    if ((u1 != 0.0) && (u1 != b_u1)) {
      tr = u0 / u1;
      if (fabs(tr - rt_roundd_snf(tr)) <= DBL_EPSILON * fabs(tr)) {
        y = 0.0;
      } else {
        y = fmod(u0, u1);
      }
    } else {
      y = fmod(u0, u1);
    }
  }

  return y;
}

/*
 * Arguments    : double u
 * Return Type  : double
 */
static double rt_roundd_snf(double u)
{
  double y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/*
 * REM_X Summary of this function goes here
 *    Detailed explanation goes here
 * Arguments    : double M
 *                double N
 * Return Type  : int64m_T
 */
int64m_T rem_x(double M, double N)
{
  double d0;
  int64m_T r0;
  static int64m_T r1 = { { 0UL, 2147483648UL } };

  static int64m_T r2 = { { MAX_uint32_T, 2147483647UL } };

  static int64m_T r3 = { { 0UL, 0UL } };

  d0 = rt_roundd_snf(rt_remd_snf(4.294967296E+9 * M, N));
  if (d0 < 9.2233720368547758E+18) {
    if (d0 >= -9.2233720368547758E+18) {
      Double2MultiWord(d0, &r0.chunks[0U], 2);
    } else {
      r0 = r1;
    }
  } else if (d0 >= 9.2233720368547758E+18) {
    r0 = r2;
  } else {
    r0 = r3;
  }

  return r0;
}

/*
 * File trailer for rem_x.c
 *
 * [EOF]
 */
