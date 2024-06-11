#ifndef cpk_common_h
#define cpk_common_h

#include <stdio.h> // IWYU pragma: keep
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef uint64_t K;
typedef uint32_t u4;
typedef uint16_t u2;
typedef uint8_t B;

typedef int64_t i8;
typedef int32_t i4;
typedef int8_t i1;

#define S static

#define Ksz sizeof(K)

//types are top 7 bits - linux addresses go up to 56 bits in some cases, windows goes up to 57[citation needed]
#define T(k) ((k) >> 57)
#define KT(t) (((K)(t)) << 57)
#define TT(k) ((k) & 0xfe00000000000000ULL)
#define KK(k) ((k) & 0x01ffffffffffffffULL)
#define PK(k) ((K*)KK(k))

#define TPsym ((K)1)
#define Tchar ((K)2)
#define Ti1 ((K)3)
#define Ti4 ((K)4)

#define Ti8 ((K)5)//boxed
#define Tf8 ((K)6)

#define Tfun ((K)8)

#define Tdict ((K)20)
#define Tverb ((K)16)
#define Tmonad ((K)17)
#define Tadverb ((K)18)

#define TV(k) (!T(k))//vecs have type 0
#define L(k) (((u4*)(k))[-1])//len
#define R(k) (((u2*)(k))[-3])//rc
#define TI(k) (((B*)(k))[-7])//item type
#define BV(k) (((B*)(k))[-8])

#define err(s) ({ puts("'" #s); return; 0; })

#define I(v,n) for(i4 v=0; v<n; ++v)//iter

#define K4(k) ((int32_t)((k) & 0xffffffff))//unpack i4

#define BS(v,s) (((K)v)<<(s*8))
#define Psym(g,f,e,d,c,b,a) (KT(TPsym)|(BS(a,6)|BS(b,5)|BS(c,4)|BS(d,3)|BS(e,2)|BS(f,1)|BS(g,0)))//pack symbol

#define PSl Psym('l','i','s','t',0,0,0)
#define PSf Psym('f','u','n',0,0,0,0)
#define PSp Psym('p','r','o','g','n',0,0)


S inline K* vec(K s) {
	K* buf = malloc(Ksz*(s+1));
	K k = (K)(buf+1); L(k) = s; TI(k) = 0;
	return buf+1;
}
#define N(_n,_t,_v) ({ u4 n = _n; K* v = vec(n); TI(v) = _t; I(i,n) v[i] = (_v); (K)v; })

K first(K k);

#endif
