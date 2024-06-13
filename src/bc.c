#include "common.h"
#include "bc.h"

#define BK(k) ((B)KK(k))
#define E err(compile)

struct cs {//compile state
    K* cur; K* p;
    B* b; B* end;
    int arity; K* vars;
};

S B addconst(K t, struct cs* s) {
    K* k = (K*)s->cur[1];
    if (!k) {
        *(k = vec(4)) = first(t);
        s->cur[1] = (K)k;
        k[-1] = 1;
        return 0;
    }
    k[k[-1]] = first(t);
    return k[-1]++;
}

S void expr(K t, struct cs* s) {
#define O(op) *((s->b)++) = (op)
    if (!TV(t)) E;

    if (L(t) == 1) { O(OP_xKST); O(addconst(t, s)); return; }

    K* a = (K*)t; K n0 = a[0];
    switch (T(n0)) {
    case TPsym:
        switch (n0) {
        case PSl: E;//list
        case PSf://fun
            if ((K)s->b != s->cur[2]) { O(OP_xKST); O(addconst(KT(Tfun) | (K)compile(t, s->cur), s)); break; }
            else if (L(t) == 2) {//TODO recognize x y xfy xfx yfx yfy
            }
        case PSp://progn
            for (i4 i = 1; i < L(t); ++i) expr(a[i], s);
            break;
        default: E;
        }
        break;
    case 0://temporary
        if (L(a) != 2) err(nyi);
        expr(a[1], s);
        O(OP_STAx);
        expr(a[0], s);
        O(OP_ySTA);
        O(OP_CALL);
        break;
    case Tverb:
        if (L(a) == 3) {
            if (L(a[1]) == 1) {
                expr(a[2], s);
                O(OP_CDBX+BK(n0)); O(addconst(a[1], s));
            } else {
                expr(a[2], s);
                O(OP_STAx);
                expr(a[1], s);
                O(OP_ySTA);
                O(OP_CDBV+BK(n0));
            }
            break;
        } else if (L(a) != 2) {
            err(nyi);
        }//fallthrough
    case Tmonad:
        expr(a[1], s);
        O(OP_CMBV+BK(n0));
        break;
    default: err(nyi);
    }
#undef O
}

K* compile(K t, K* parent) {
    K* c = vec(4); K* b = vec(256);
    c[0] = 0; c[1] = 0; c[2] = (K)b; c[3] = 0;
    struct cs state = {c, parent, (B*)b, ((B*)b)+(Ksz*256), 0, 0};
    expr(t, &state);
    *(state.b++) = OP_RETN;
    while (((K)state.b & (Ksz-1)) != (Ksz-1)) *(state.b++) = OP_TRAP;
    L(b) = state.b - (B*)b;
    TI(b) = Tchar;
    return c;
}
