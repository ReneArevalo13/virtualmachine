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

#define ErrMem 0x01
#define NoArgs {0x00, 0x00}

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
    nop = 0x02
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

typedef int8 Args;

struct s_instruction {
    Opcode o;
    Args a[];
};
typedef struct s_instruction Instruction;

typedef int8 Memory[((unsigned int)(-1))];
typedef int8 Program;

typedef Memory *Stack;

struct s_vm {
    CPU c;
    Memory m;
    Program *p;
};
typedef struct s_vm VM;

static IM instrmap[] = {
    {mov, 0x03},
    {nop, 0x01}
};
#define IMs sizeof(instrmap) / sizeof(struct s_instrmap)
 Program *exampleprogram(VM *);

int8 map(Opcode); 
VM *virtualmachine (void);

int main (int, char**);


