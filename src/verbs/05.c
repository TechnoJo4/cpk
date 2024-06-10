#define V m5
#include "verb.h"
#include "arith.h"

//@ *x first     *!3 -> 0
dispatch { vmcont(first(x), y); }

#undef V
#define V d5

//@I*I mul       2*3 -> 6   (!5)*!5 -> 0 1 4 9 16
arith(*)

dispatch {
    dispatch_arith
    nyi;
}
