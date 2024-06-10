#define V m7
#include "verb.h"

//@ ,x enlist    ,!4 -> ,(0 1 2 3)
dispatch { vmcont(N(1, 0, x), y); }

#undef V
#define V d7

//@X,X nyi
dispatch { nyi; }
