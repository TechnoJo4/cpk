#define V m8
#include "verb.h"
#include "arith.h"

dispatch { nyi; }

#undef V
#define V d8

//@i-i sub       1-2 -> -1
arith(-)

dispatch {
    dispatch_arith
    nyi;
}