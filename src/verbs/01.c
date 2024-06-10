#define V m1
#include "verb.h"

//@ #x length    #"abc" -> 3   #4 -> 1
dispatch { vmtail(KT(Ti4) | (TV(x) ? L(x) : 1), y); }

#undef V
#define V d1

//@i#X nyi
dispatch { nyi; }
