#ifndef cpk_arith_h
#define cpk_arith_h

#include "verb.h"

#define arith(o) \
    D(i4_i4, KT(Ti4) | (K4(x) o K4(y))) \
    D(i8_i8, KT(Ti8) | N(1, Ti8, *(i8*)PK(x) o *(i8*)PK(y))) \
    D(I_I, ({ i8* X = (i8*)x; i8*Y = (i8*)y; NxY(X[i] o Y[i]); })) \
    D(i4_I, ({ i8 X = (i8)K4(x); i8*Y = (i8*)y; N(L(y), Ti8, (K)(X o Y[i])); })) \
    D(i8_I, ({ i8 X = *(i8*)PK(x); i8* Y = (i8*)y; N(L(y), Ti8, (K)(X o Y[i])); }))

#define dispatch_arith \
    if (TV(x) && TI(x) == Ti8) { \
        if (!TV(y)) retail Mdispatch(V)(y, x, c, b); \
        else if (TI(y) == Ti8) C(I_I) } \
    if (T(x) == Ti4) { \
        if (T(y) == Ti4) C(i4_i4) \
        else if (TV(y) && TI(y) == Ti8) C(i4_I) } \
    if (T(x) == Ti8) { \
        if (T(y) == Ti8) C(i8_i8) \
        if (TV(y) && TI(y) == Ti8) C(i8_I) }

#endif
