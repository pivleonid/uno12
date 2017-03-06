/*
 * File: _coder_rem_x_api.h
 *
 * MATLAB Coder version            : 3.0
 * C/C++ source code generated on  : 06-Mar-2017 12:54:04
 */

#ifndef ___CODER_REM_X_API_H__
#define ___CODER_REM_X_API_H__

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_rem_x_api.h"

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern int64_T rem_x(real_T M, real_T N);
extern void rem_x_api(const mxArray * const prhs[2], const mxArray *plhs[1]);
extern void rem_x_atexit(void);
extern void rem_x_initialize(void);
extern void rem_x_terminate(void);
extern void rem_x_xil_terminate(void);

#endif

/*
 * File trailer for _coder_rem_x_api.h
 *
 * [EOF]
 */
