/* vm.c */

#include "vm.h"
#include "birchutils.h"
#include <string.h>

VM *virtualmachine() {
    VM *p;
    int16 size;

    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }
    zero($1 p, size);

        return p;
}

void execute(VM *vm) {
    Program *pp;
    int16 brkaddr;
    Instruction ip;
    int16 size;

    assert(vm && *vm->m);
    pp = vm->m;
    // consider the break address as an offset from the vm memory
    // to 
    brkaddr = (int16)vm->m + vm->brk;

    size = 0;

    /* 
    Our instruction format:
    instr1 arg1 instr2 instr3
    mov ax, 0x05; nop; hlt;
    Binary representation:
    0x01 0x00 0x05
    0x02
    0x03
    */
    // goin to use a break address: brkaddr

   do {
        vm $ip += size;
        pp += size;
        ip.o = *pp;
        if ((int16)pp > brkaddr) {
            segfault(vm);
        }
        size = map(ip.o);
        copy($1 &ip, $1 pp, size);
        executeinstr(vm, pp);

    } while(*pp != (Opcode)hlt);
   
   return;
}


void __mov(VM *vm, Opcode o, Args a1, Args a2) {
    vm $ax = (Reg) a1;
    return;
}

void executeinstr(VM *vm, Program *p) {
    int16 size;
    Args a1, a2;
    a1 = 0;
    a2 = 0;

    size = map(*p);
    switch (size) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            a1 = *(p+1);
            break;
        case 3:
            a1 = *(p+1);
            a2 = *(p+3);
            break;
        default:
            segfault(vm);
            break;
    }
    
    switch(*p) {
        
        case 0x08 ... 0x0f:
            __mov(vm, *p, a1, a2);
            break;
        case nop:
            break;
        case hlt:
            error(vm, SysHlt);

            break;
    }
    return;
}

void error(VM *vm, Errorcode e) {
    int8 exitcode;
    exitcode = -1;
   
    switch(e) {
        case ErrSegv:
            fprintf(stderr, "%s\n", "VM Segmentation Fault");
            break;
        case SysHlt:
            fprintf(stderr, "%s\n", "System halted");
            exitcode = 0;
            printf("ax = %.04hx\n", $i vm $ax);
            break;
        default:
            break;
    }
     if (vm) {
        free(vm);
    }
    exit($i exitcode);
}


int8 map(Opcode o){
    int8 n, ret;
    IM *p;

    ret = 0;
    for (n=IMs, p=instrmap; n; n--, p++) {
        if (p->o == o) {
            ret = p->s;
            break;
        }
    }
    return ret; 
}

Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i1, *i2, *i3;
    int16 s1, s2, sa1;
    Args a1;

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);
    sa1 = s1 - 1;

    /* Here I allocated memory for the program
       according to how many instructions are 
       going to be passed. So far that is the
       mov (and mov arg), nop, and hlt.
    */

    p =  (Program *)malloc(s1+s2+sa1+s2);
    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc( s2);
    i3 = (Instruction *)malloc( s2);

    assert(i1 && i2 && i3);

    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s2);

    i1->o = mov;
   // printf("I1 is : %d\n", i1->o);
    //sa1 = s1 - 1;    // everything is 1 byte in our "Program instructions"

    if (s1) {
        // this is how we are setting the mov instruction argument stirng
        a1    = 0x0007;
        i1->a[0] = a1;
    }

    p = vm->m;   // going into the vm's memory and copying there

    copy($1 p, $1 i1, 1);
    //memcpy(p, i1, 1);
    p++;

    if (a1) {
        copy($1 p, $1 &a1, sa1);
      //  memcpy(p, &a1, sa1);
        p += sa1;
    } 

    i2->o = nop;
    copy($1 p, $1 i2, 1);
//    memcpy(p, i2, 1);

    p++;

    i3->o = hlt;
    copy($1 p, $1 i3, 1);
  //  memcpy(p, i3, 1);
    

    vm->brk = (s1+sa1+s2+s2);//break line set to the offset of progam size
    vm $ip = (Reg) vm->m;

    vm $sp = (Reg) -1;

    free(i1);
    free(i2);
    free(i3);
   // free(p);

    return (Program *)&vm->m;
}

int main (int argc, char *argv[]){
    Program *prog;
    VM *vm;

    vm = virtualmachine();
  //  printf("vm = %p (sz: %d)\n", vm, sizeof(VM));

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

    execute(vm);
    printf("ax = %.04hx\n", $i vm $ax);
    printhex($1 prog, (map(mov) + map(nop) + map(hlt)), ' ');
    
    return 0;
}
