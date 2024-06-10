#include <stdio.h>
#include <stdlib.h>

#include "common.h"

S struct {//parse state
	const B* curr;const B* file;//lexer
	K* n; B* nt;//parser
} ps;

S void init(const void* file) {
	ps.file = file; ps.curr = file;
	ps.n = vec(128);
	ps.nt = malloc(128);
}

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
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CC, 0, 0, 0, 0, 0,
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

S void expr(int i, B close);
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
			expr(i, ')');
			if (L(ps.n[i]) == 2) {//(x)
				K k = PK(ps.n[i])[1];
				ps.n[i] = k;
			} else {//(;)
				PK(ps.n[i])[0] = PSl;
			}
			return N_N;
		}
		if (c == '{') {
			expr(i, '}');
			PK(ps.n[i])[0] = PSf;
			return N_D;
		}
		expr(i, ']');
		PK(ps.n[i])[0] = PSp;
		return N_N;
	}

	if (class & CL) {
		const B* start = ps.curr-1;
		do { MOVE; } while (class & (CL|CD));
		ps.n[i] = ((K)start) | KT(Tchar);//TODO copy
		return N_N;
	}

	if (c == '`') {return ERR;}//TODO

	return ERR;
#undef MOVE
#undef PEEK
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

	if (cr.r && cr.s < 2) {
		printf("compose nyi");
	}

	K node = ps.n[ic];
	if (TV(node) && L(node) >= 3) {
		K* v = PK(node);
		if (L(v) == 3) {
			if (!v[2]) {
				v[2] = ps.n[ir];
				return;
			}
		} else {//TODO find hole
			printf("hole nyi");
		}
	}

	K* v;
	if (cr.r == 1) {
		v = vec(3);
		v[0] = ps.n[ir]; v[1] = node;
		v[2] = 0;
	} else if (cr.r == 2) {
		v = vec(2);
		v[0] = ps.n[ir]; v[1] = node;
	} else {
		v = vec(2);
		v[0] = node; v[1] = ps.n[ir];
	}
	ps.n[ic] = (K)v;
}

S void expr(int i0, B close) {
	int i = i0; B t;

	do {
		t = item(i, close);
		ps.nt[i++] = t;
	} while (t < CLOSE);

	if (t == ERR) {//TODO
		puts("'parse");
		return;
	}

	--i;
	int n = i-i0;
	u2 d[n]; I(j,n) d[j] = 1;

	if (n > 2) {
		int ic,ir; B l,c,r;
		ic = i - 2; ir = i - 1;
		l = ps.nt[ic-1]; c = ps.nt[ic]; r = ps.nt[ir];
		while (ic > i0) {
			struct sr lc = pairs[l*4+c];
			struct sr cr = pairs[c*4+r];
			if (lc.s > cr.s) {//move
				--ic; if (ic <= i0) break; ir = ic+d[ic-i0];
				l = ps.nt[ic-1]; c = ps.nt[ic]; r = ps.nt[ir];
				continue;
			}

			bind(cr, ic, ir);

			int dc = d[ic-i0] += d[ir-i0];
			ir = ic+dc; --n;
			if (ir >= i) {//move if no more r
				--ic; if (ic <= i0) break;
				ir = ic+d[ic-i0]; l = ps.nt[ic-1];
			}
			c = ps.nt[ic]; r = ps.nt[ir];
		}
	}
	while (n > 1) {
		int ir = i0 + d[0]; d[0] += d[d[0]];
		bind(pairs[ps.nt[i0]*4+ps.nt[ir]], i0, ir);
		--n;
	}

	K* r = vec(2);//TODO ;
	r[1] = ps.n[i0];
	ps.n[i0] = (K)r;

	return;
}

K parse(const char* file) {
	init((const void*)file);
	expr(0, 0);
	PK(ps.n[0])[0] = PSp;
	return ps.n[0];
}
