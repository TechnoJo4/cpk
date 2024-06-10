#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "print.h"

typedef char *C;

S inline C grow(C* str) {
	K sz = *((K*)*str);
	C buf = realloc(*str, Ksz+2*sz);
	*((K*)buf) = 2*sz; *str = buf;
	return buf;
}

S C chksz(C* str, int i) {
	K sz = *((K*)*str);
	if (sz >= i) return *str;
	return grow(str);
}

S const C verbs = "!#$%&*+,-.<=>?@^_|~";
S const C adverbs = "'/\\";

#define AVAIL L(*str)-i
S int addtostr(C* str, int i, K val);
S int item(C* str, int i, void* v, B t) {
	C b;
	switch (t) {
	case 0:
		return addtostr(str, i, (K)v);
	case Ti8:
		int n = snprintf((*str)+i, AVAIL, "%lld", *((i8*)v));
		if (n >= AVAIL) {
			b = grow(str);
			n = snprintf(b+i, AVAIL, "%lld", *((i8*)v));
		}
		return i + n;
	}
	return i;
}

S int addtostr(C* str, int i, K val) {
	C b;
	if (TV(val)) {
		if (L(val) == 1) {
			(b = chksz(str, i+4))[i] = ',';
			return item(str, i+1, (K*)val, TI(val));
		}

		B t = TI(val);
		b = chksz(str, i);
		b[i] = '(';
		if (!t) I(j,L(val)) {
			i = addtostr(str, i+1, ((K*)val)[j]);
			(b = chksz(str, i))[i] = ';';
		} else if (t == Ti8) I(j,L(val)) {
			i = item(str, i+1, ((i8*)val)+j, Ti8);
			(b = chksz(str, i))[i] = ' ';
		} else {
			++i;
			b = chksz(str, i+7); b[i++] = '!'; b[i++] = 'n'; b[i++] = 'y'; b[i++] = 'i';
			i += snprintf(b+i, AVAIL, "%02x", t); ++i;
		}

		b[i] = ')'; ++i;
		return i;
	}

	B t = T(val);
	if (t == Ti4) {
		int n = snprintf((*str)+i, AVAIL, "%d", K4(val));
		if (n >= AVAIL) {
			b = grow(str);
			n = snprintf(b+i, AVAIL, "%d", K4(val));
		}
		i += n;
	} else if (t == Ti8) {
		i = item(str, i, PK(val), Ti8);
	} else if (t == Tverb) {
		b = chksz(str, i+3); b[i++] = verbs[KK(val)];
	} else if (t == Tmonad) {
		b = chksz(str, i+4); b[i++] = verbs[KK(val)]; b[i++] = ':';
	} else if (t == Tadverb) {
		b = chksz(str, i+4);
		K v = KK(val);
		if (v < 3) b[i++] = adverbs[v]; else {
			b[i++] = adverbs[v-3]; b[i++] = ':';
		}
	} else if (t == TPsym) {
		b = chksz(str, i+9); b[i++] = '`';
		val = KK(val);
		while (val) { b[i++] = val & 0xff; val = val >> 8; }
	} else {
		b = chksz(str, i+7); b[i++] = '!'; b[i++] = 'n'; b[i++] = 'y'; b[i++] = 'i';
		i += snprintf(b+i, AVAIL, "%02x", t);
	}
	return i;
}
#undef AVAIL

K stringify(K val) {
	C str = malloc(Ksz+128);

	int end = addtostr(&str, Ksz, val);
	str[end] = '\0';
	K* k = ((K*)str)+1;
	L(k) = end-Ksz; TI(k) = Tchar;
	return (K)k;
}
