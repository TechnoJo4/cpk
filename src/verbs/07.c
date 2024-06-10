#define V m7
#include "verb.h"

dispatch { y = (K)vec(1); ((K*)y)[0] = x; vmtail((K)x, y); }

#undef V
#define V d7

dispatch { nyi; }
