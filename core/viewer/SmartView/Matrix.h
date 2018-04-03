#ifndef _MATRIX_FUNCTIONS_HEAD_INCLUDED_
#define _MATRIX_FUNCTIONS_HEAD_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewDef.h"

bool EXPORT_SMARTVIEW matrix_reverse(const double *m, int n, double *r);
void EXPORT_SMARTVIEW matrix_multiply(const double *m1, const double *m2, int n, double *r);
void EXPORT_SMARTVIEW matrix_toIdentity(double *m, int n);
bool EXPORT_SMARTVIEW matrix_isIdentity(const double *m, int n);
void EXPORT_SMARTVIEW matrix_multiply_byvector(const double *mt, int m, int n, double *v, double *r);
double EXPORT_SMARTVIEW matrix_modulus(const double *m, int n);

#endif