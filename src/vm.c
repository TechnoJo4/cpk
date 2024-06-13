#include "common.h"
#include "bc.h"
#include "print.h"
#include "verbs.h"

#include <stdio.h>

#define retail [[clang::musttail]] return
#define vmtail(x,y) retail vm(x, y, c, s, b)
#define vmcont(x_,y_) ({ x = x_; y = y_; vmtail(x, y); })
#define verb(name,x_...) void name(K x, K y, K* c, B* b) { vmcont((x_), y); }
#define C(a) case a:
#define C2(a) C(a)C(a+1)
#define C4(a) C2(a)C2(a+2)
#define C8(a) C4(a)C4(a+4)
#define C16(a) C8(a)C8(a+8)
#define C32(a) C16(a)C16(a+16)

typedef void (*F)(K x, K y, K* c, K* s, B* b);

#define V(_v) v_##_v##_dispatch
static F monads[32] = {V(m0),V(m1),V(m2),V(m3),V(m4),V(m5),V(m6),V(m7),V(m8),V(m9),V(m10),V(m11),V(m12),V(m13),V(m14),V(m15),V(m16),V(m17),V(m18),0};
static F dyads[32] =  {V(d0),V(d1),V(d2),V(d3),V(d4),V(d5),V(d6),V(d7),V(d8),V(d9),V(d10),V(d11),V(d12),V(d13),V(d14),V(d15),V(d16),V(d17),V(d18),0};
#undef V


void vm(K x, K y, K* c, K* s, B* b) {
    B o = *(b++);
    switch (o) {
    C(OP_TRAP) default: puts("'trap"); return;
    C(OP_RETN) if (!c[-2]) { K str = stringify(x); printf("'retn x=%s\n", (const char*)PK(str)); return; }//temporary
               else { retail vm(x, y, (K*)c[-3], (K*)c[-2], (B*)c[-4]); }
    C(OP_xNIL) vmcont(0, y);
    C(OP_yNIL) vmcont(x, 0);
    C(OP_xKST) vmcont(((K**)c[-1])[1][*(b++)], y);
    C(OP_yKST) vmcont(x, ((K**)c[-1])[1][*(b++)]);
    C(OP_xTOy) vmtail(x, x);
    C(OP_yTOx) vmtail(y, y);
    C(OP_SWAP) vmtail(y, x);
    C(OP_TRCE) vmtail(x, y);//nyi
    C(OP_xGET) vmcont(c[*(b++)], y);
    C(OP_yGET) vmcont(x, c[*(b++)]);
    C(OP_xSET) c[*(b++)] = x; vmcont(x, y);
    C(OP_ySET) c[*(b++)] = y; vmcont(x, y);
    C(OP_xSTA) x = *(s--); vmcont(x, y);
    C(OP_ySTA) y = *(s--); vmcont(x, y);
    C(OP_STAx) *(++s) = x; vmcont(x, y);
    C(OP_STAy) *(++s) = y; vmcont(x, y);
    C(OP_CALL) {//todo unfuck
        K* d = PK(x);
        *(++s) = (K)b;
        *(++s) = (K)c;
        ++s; *s = (K)s;
        *(++s) = (K)d;
        retail vm(y, y, s + 1, s, ((B**)d)[2]);
    }
    C32(OP_CDBV) retail dyads[o-OP_CDBV](x, y, c, s, b);
    C32(OP_CMBV) retail monads[o-OP_CMBV](x, y, c, s, b);
    C32(OP_xKBV) vmtail(x, y);//nyi
    C32(OP_CDBX) y = x; x = ((K**)c[-1])[1][*(b++)]; retail dyads[o-OP_CDBX](x, y, c, s, b);
    }
}

S K stack[2048] = {0};
void vmentry(K* chunk) {
    stack[0] = 0;
    stack[1] = (K)chunk;
    vm(0, 0, stack+2, stack+2, (B*)chunk[2]);
}
