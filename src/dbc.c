#include "bc.h"
#include "dbc.h"
#include <stdio.h>

#define C(a,x) [a]=x,
#define C2(a,x) C(a,x)C(a+1,x)
#define C4(a,x) C2(a,x)C2(a+2,x)
#define C8(a,x) C4(a,x)C4(a+4,x)
#define C16(a,x) C8(a,x)C8(a+8,x)
#define C32(a,x) C16(a,x)C16(a+16,x)

static const char* names = "TRAPRETNxNILyNILxKSTyKSTxTOyyTOxSWAPTRCExGETyGETxSETySETCDBVCMBVxKBVCDBX";
static const B nameidx[256] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,C32(OP_CDBV,14)C32(OP_CMBV,15)C32(OP_xKBV,16)C32(OP_CDBX,17)};
static const B operands[256] = {0,C2(OP_xKST,1)C(OP_xTOy,0)C4(OP_xGET,1)C32(OP_CDBV,0)C32(OP_CDBX,1)0};
static const B showbyte[256] = {0,C32(OP_CDBV,1)C32(OP_CMBV,1)C32(OP_xKBV,1)C32(OP_CDBX,1)};

void dbc(B* b) {
    B* end = b + L((K)b);
    while (b < end) {
        B o = *(b++);
        printf("%.4s", names + 4 * nameidx[o]);
        if (showbyte[o]) printf(" (%02x)", o);
        for(int i = 0; i < operands[o]; ++i) printf(" %02x", *(b++));
        printf("\n");
    }
}
