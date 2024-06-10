#ifndef cpk_verbs_h
#define cpk_verbs_h

#include "src/common.h"

#define n(_v) v_##_v##_dispatch
#define V(v_) void n(v_)(K x, K y, K* c, K* s, B* b)

V(m0);  V(d0);  // !
V(m1);  V(d1);  // #
V(m2);  V(d2);  // $
V(m3);  V(d3);  // %
V(m4);  V(d4);  // &
V(m5);  V(d5);  // *
V(m6);  V(d6);  // +
V(m7);  V(d7);  // ,
V(m8);  V(d8);  // -
V(m9);  V(d9);  // .
V(m10); V(d10); // <
V(m11); V(d11); // =
V(m12); V(d12); // >
V(m13); V(d13); // ?
V(m14); V(d14); // @
V(m15); V(d15); // ^
V(m16); V(d16); // _
V(m17); V(d17); // |
V(m18); V(d18); // ~

#undef n
#undef V

#endif
