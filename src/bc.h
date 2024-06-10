#ifndef cpk_bc_h
#define cpk_bc_h

#include "common.h"

enum {           //xx       OPER: description of operation
    OP_TRAP,     //00       TRAP: loudly terminate program
    OP_RETN,     //01       RETN: return
    OP_xNIL,     //02       xNIL: x:nil
    OP_yNIL,     //03       yNIL: y:nil
    OP_xKST,     //04    xx xKST: x:consts[xx]
    OP_yKST,     //05    xx yKST: y:consts[xx]
    OP_xTOy,     //06       xTOy: y:x
    OP_yTOx,     //07       yTOx: x:y
    OP_SWAP,     //08       SWAP: (x;y):(y;x)
    OP_TRCE,     //09       TRCE: print x
    OP_xGET,     //0A    xx xGET: x:vars[xx]
    OP_yGET,     //0B    xx yGET: y:vars[xx]
    OP_xSET,     //0C    xx xSET: vars[xx]:x
    OP_ySET,     //0D    xx ySET: vars[xx]:y
    OP_CDBV=0x10,//1x|2x    CDBV: call verb
    OP_CMBV=0x30,//3x|4x    CMBV: call monad
    OP_xKBV=0x50,//5x|6x    xKBV: x:verb
    OP_CDBX=0x70,//7x|8x xx CDBX: y:x; x:consts[xx]; call verb
};

// A chunk is b,k,c ([0] = bytecode, [1]=constants, [2]=children)
K* compile(K t, K* parent);

void vm(K x, K y, K* c, B* b);
void vmentry(K* chunk);

#endif
