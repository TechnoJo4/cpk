#include "common.h"

K first(K k) {
	if (!TV(k)) return k;
	B t = TI(k);
	if (!t) return *(K*)k;
	if (t == Ti4) return KT(Ti4) | ((K)*(i4*)k);
	if (t == Ti8) return KT(Ti8) | N(1, Ti8, *(i8*)k);
	return 0;
}
