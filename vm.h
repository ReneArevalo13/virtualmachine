/* vm.h */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include "birchutils.h"

#define NoErr   0x00           // 00 00
#define SysHlt  0x01           // 00 01
#define ErrMem  0x02           // 00 10
#define ErrSegv 0x04           // 01 00


typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sp ->c.r.sp
#define $ip ->c.r.ip

#define segfault(x)       error((x), ErrSegv)
/*
   Will be making a 16 bit computer.
   16 bits gives us 65,536 memory addresses.
   65kb of memory.

   CPU will be simple.
   Ax, Bx, Cx, Dx registers.
   Stack pointer, sp.
   Instruction pointer, ip.
   other ports to come.
*/
typedef unsigned short int Reg;

struct s_registers {
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg sp;
    Reg ip;
};
typedef struct s_registers Registers;

enum e_opcode {
    mov = 0x01,
    nop = 0x02,
    hlt = 0x03
};
typedef enum e_opcode Opcode;

struct s_instrmap {
    Opcode o;
    int8 s;
};
typedef struct s_instrmap IM;

struct s_cpu {
    Registers r;
};
typedef struct s_cpu CPU;

typedef int16 Args;

struct s_instruction {
    Opcode o;
    Args a[];
};
typedef struct s_instruction Instruction;

typedef int8 Memory[((int16)(-1))];
typedef int8 Program;
typedef unsigned char Errorcode;

typedef Memory *Stack;

struct s_vm {
    CPU c;
    Memory m;
    int16 brk;   // break line between stack and code segment
};
typedef struct s_vm VM;

static IM instrmap[] = {
    {mov, 0x03},
    {nop, 0x01},
    {hlt, 0x01}
};
#define IMs sizeof(instrmap) / sizeof(struct s_instrmap)
 Program *exampleprogram(VM *);

void error(VM *, Errorcode);
void execute(VM *); 
void executeinstr(VM *, Instruction *);
int8 map(Opcode); 
VM *virtualmachine (void);

void __mov(VM *, Opcode, Args, Args);

int main (int, char**);


