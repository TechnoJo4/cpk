#ifndef cpk_verb_h
#define cpk_verb_h

// IWYU pragma: begin_exports
#include "src/common.h"
#include <stdio.h>
// IWYU pragma: end_exports

void vm(K x, K y, K* c, B* b);

#define retail [[clang::musttail]] return
#define vmtail(x,y) retail vm(x, y, c, b)
#define vmcont(x_,y_) ({ x = x_; y = y_; vmtail(x, y); })

#define n(_v,_t) v_##_v##_##_t
#define M(v_,t_) n(v_,t_)
#define ndispatch(_v) v_##_v##_dispatch
#define Mdispatch(v_) ndispatch(v_)
#define C(t_) retail M(V, t_)(x, y, c, b);
#define D(t_,x_...) static void M(V, t_)(K x, K y, K* c, B* b) { x = (x_); vmtail(x, y); }
#define dispatch void Mdispatch(V)(K x, K y, K* c, B* b)

#define nyi err(nyi)
#define Etyp err(type)
#define Elen err(length)

#define xyL L(x) != L(y) ? Elen :

#define NxY(_v) xyL N(L(y), TI(y), _v)

#endif
