#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

S struct {//parse state
	const B* curr;const B* file;//lexer
	K* n; B* nt;//parser
} ps;

enum { N_N, N_M, N_D, N_A, CLOSE, SEMI, ERR };

//character classes
#define CV 1
#define CL 2
#define CD 4
#define CA 8
#define CO 16
#define CC 32
#define CS 64
#define CH 128
S const B cls[256] = {
	CC, 0, 0, 0, 0, 0, 0, 0, 0, 0, CC, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, CV, 0, CV, CV, CV, CV, CA, CO, CC|CS, CV, CV, CV, CV, CV, CA,// !"#$%&'()*+,-./
	CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CD|CH, CV, CC, CV, CV, CV, CV,//0-9:;<=>?
	CV, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL,//@A-O
	CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CO, CA, CC|CS, CV, CV,//P-Z[\]^_
	0, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL,//`a-o
	CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CL, CO, CV, CC|CS, CV, 0//p-z{|}~\x7f
};
S const B verbs[96] = {
	0,  0,  0, 1, 2, 3, 4, 0, 0, 0, 5, 6, 7, 8, 9, 1,// !"#$%&'()*+,-./
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13,//0-9:;<=>?
	14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//@A-O
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 15, 16,//P-Z[\]^_
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//`a-o
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 0, 18, 0//p-z{|}~\x7f
};

S bool expr(int i, B close);

S int obj(int i, B close, K t) {
#define GROWCHECK if (!(j & j-1)) { K* b = vec(2*j); I(k,j) b[k] = a[k]; a = b; }//grow each 2^n
	K* a = vec(2); a[0] = t; int j = 1;
	while (expr(i, close)) { GROWCHECK; a[j] = ps.n[i]; ++j; } GROWCHECK; a[j] = ps.n[i]; ++j;
	L(a) = j; ps.n[i] = (K)a;
	return j;
#undef GROWCHECK
}

S B item(int i, B close) {
	B c; int class;
#define MOVE c = *(ps.curr++); class = cls[c]
#define PEEK *ps.curr
#define LAST *(ps.curr-2)
	while (*ps.curr == ' ') ++ps.curr;
	MOVE;

	if (c == ';' || c == '\n') return SEMI;
	if (c == close) return CLOSE;
	if (class & CC) return ERR;

	if (i >= L(ps.n)) { puts("'limit:token"); return ERR; }

	if (c == '\"') {return ERR;}//TODO

	if (class & CD) {//n //TODO N
		K num = c - '0';
		while (cls[PEEK] & CD) { MOVE; num *= 10; num += c - '0'; }
		ps.n[i] = N(1, Ti8, num);
		return N_N;
	}

	if (class & CA) {//adverb //TODO LAST == ' '
		if (PEEK == ':') {
			++ps.curr;
			ps.n[i] = (verbs[c-32] + 3) | KT(Tadverb);
			return N_A;
		}
		ps.n[i] = verbs[c-32] | KT(Tadverb);
		return N_A;
	}
	if (class & CV) {//verb
		if (PEEK == ':') {
			++ps.curr;
			ps.n[i] = verbs[c-32] | KT(Tmonad);
			return N_M;
		}
		ps.n[i] = verbs[c-32] | KT(Tverb);
		return N_D;
	}

	if (class & CO) {
		if (c == '(') {
			int j = obj(i, ')', PSl);//(;)
			if (j == 2) { ps.n[i] = ((K*)ps.n[i])[1]; }//(x)
			return N_N;
		}
		if (c == '{') { obj(i, '}', PSf); return N_N; }
		obj(i, ']', PSp); return N_N;
	}

	if (class & CL) {
		K v = 0;
		do { v = (v << 8) | c; MOVE; } while (class & (CL|CD));
		ps.n[i] = v | KT(TPsym);
		return N_N;
	}

	if (c == '`') {return ERR;}//TODO

	return ERR;
#undef MOVE
#undef PEEK
#undef LAST
}

struct sr { B s; B r; };//strength, result
S const struct sr pairs[] = {//nmda
	{1,0}, {0,6}, {3,1}, {4,1},//n
	{2,0}, {1,1}, {0,2}, {4,2},//m
	{2,0}, {1,1}, {1,2}, {4,2},//d
	{0,6}, {0,6}, {0,6}, {0,6}
};

S void bind(struct sr cr, int ic, int ir) {
	//TODO error if cr.r==ERR
	ps.nt[ic] = cr.r;

	if (cr.r && cr.s < 2) err(nyi);

	K node = ps.n[ic];
	if (TV(node) && L(node) == 3) {
		if (!((K*)node)[2]) {
			((K*)node)[2] = ps.n[ir];
			return;
		}
	}

	K* v;
	if (cr.r == 1) {//todo unfuck
		v = vec(3); v[0] = ps.n[ir]; v[1] = node; v[2] = 0;
	} else if (cr.r == 2) {
		v = vec(2); v[0] = ps.n[ir]; v[1] = node;
	} else {
		v = vec(2); v[0] = node; v[1] = ps.n[ir];
	}
	ps.n[ic] = (K)v;
}

S bool expr(int i0, B close) {
#define MOVE --ic; if (ic <= i0) break; ir = ic+d[ic-i0]; l = ps.nt[ic-1]
#define CR c = ps.nt[ic]; r = ps.nt[ir]
	int i = i0; B t;

	do {
		t = item(i, close);
		ps.nt[i++] = t;
	} while (t < CLOSE);

	if (t == ERR) {//TODO
		puts("'parse"); return 0;
	}

	int n = (--i) - i0;
	u2 d[n]; I(j,n) d[j] = 1;

	if (n > 2) {
		int ic,ir; B l,c,r;
		ic = i - 2; ir = i - 1;
		l = ps.nt[ic-1]; CR;
		while (ic > i0) {
			struct sr lc = pairs[l*4+c]; struct sr cr = pairs[c*4+r];
			if (lc.s > cr.s) { MOVE; CR; continue; }

			bind(cr, ic, ir);

			ir = ic + (d[ic-i0] += d[ir-i0]); --n;
			if (ir >= i) { MOVE; } CR;
		}
	}
	while (n > 1) {
		int ir = i0 + d[0]; d[0] += d[d[0]];
		bind(pairs[ps.nt[i0]*4+ps.nt[ir]], i0, ir);
		--n;
	}

	return t == SEMI;
}

K parse(const char* file) {
	ps.file = (B*)file; ps.curr = (B*)file;
	ps.n = vec(128);
	ps.nt = malloc(128);
	obj(0, 0, PSp);
	return ps.n[0];
}
