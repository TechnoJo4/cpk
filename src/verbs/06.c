#define V m6
#include "verb.h"
#include "arith.h"

dispatch { nyi; }

#undef V
#define V d6

//@i+i add       1+2 -> 3   2+!5 -> (2 3 4 5 6)
arith(+)

dispatch {
    dispatch_arith
    nyi;
}
