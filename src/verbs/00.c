#include "verb.h"
#define V m0

//@ !i enum      !3 -> 0 1 2   !-3 -> -3 -2 -1
D(i4, K4(x) > 0
    ? N(K4(x), Ti8, (K)i)
    : ({ i8 X = (i8)K4(x); N(-X, Ti8, (K)(X+(i8)i)); }))
D(i8, *(i8*)PK(x) > 0
    ? N(*(i8*)PK(x), Ti8, (K)i)
    : ({ i8 X = *(i8*)PK(x); N(-X, Ti8, (K)(X+(i8)i)); }))

dispatch {
    if (T(x) == Ti4) C(i4);
    if (T(x) == Ti8) C(i8);
    nyi;
}

#undef V
#define V d0

//@i!i div       -10!1234 -> 123
//@i!i mod       10!1234 -> 4
D(i4_i4, ({
    x = KK(x); y = KK(y);
    KT(Ti4) | (x > 0 ? (y % x) : (y / -x));
}))

dispatch {
    if (T(x) == Ti4 && T(y) == Ti4) C(i4_i4);
    nyi;
}
