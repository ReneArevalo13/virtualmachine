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
  //  p $flags = (Reg)-1;

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


void __ste(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x08;
}
void __stg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x04;
}
void __sth(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x02;
}
void __stl(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x01;
}
void __cle(VM *vm, Opcode opcode, Args a1, Args a2) {
    //    1011   (flag set)
    //    0111   (mask) 0x07
    //    0011   ( & result) flag is cleared
    vm $flags &= 0x07;
}
void __clg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0b;
}
void __clh(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0d;
}
void __cll(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0e;
}
void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    int16 dst, dstl, dsth;
    // will change XXXX
    dst = $2 a1;
    // splits our number into the high and low respectively
    dstl = ((($2 a1) & 0x03) >> 2);
    dsth = ((($2 a1) & 0x0c) >> 2);
    // a switch for all options 0x08-0x0f
    switch(opcode) {
        /* mov ax    1000 */
        case 0x08:
            if (higher(vm)) {
                vm $ax = (Reg)dsth; 
            } else if (lower(vm)) {
                vm $ax = (Reg)dstl;
            } else {
                vm $ax = (Reg)dst;
            }
            break;
        /* mov bx    1001 */
        case 0x09:
            if (higher(vm)) {
                vm $bx = (Reg)dsth; 
            } else if (lower(vm)) {
                vm $bx = (Reg)dstl;
            } else {
                vm $bx = (Reg)dst;
            }
            break;
        /* mov cx    1010 */
        case 0x0a:
            if (higher(vm)) {
                vm $cx = (Reg)dsth; 
            } else if (lower(vm)) {
                vm $cx = (Reg)dstl;
            } else {
                vm $cx = (Reg)dst;
            }
            break;
        /* mov dx    1011 */
        case 0x0b:
            if (higher(vm)) {
                vm $dx = (Reg)dsth; 
            } else if (lower(vm)) {
                vm $dx = (Reg)dstl;
            } else {
                vm $dx = (Reg)dst;
            } 
            break;
        /* mov sp 1100 */
        case 0x0c:
            if (higher(vm)) {
                vm $sp = (Reg)dsth; 
            } else if (lower(vm)) {
                vm $sp = (Reg)dstl;
            } else {
                vm $sp = (Reg)dst;
            } 
            break;
        /* 1101 
        case 0x0d:
            vm $ax = (Reg)dst;
            break;
        /* 1110 
        case 0x0e:
            vm $ax = (Reg)dst;
            break;
            */
        /* mov [addr]   1111 */
        case 0x0f:
            // TODO
            break;
        default:
            error(vm, ErrInstr);
    

    }
    return;
}

void executeinstr(VM *vm, Program *p) {
    int16 size;
    Args a1, a2;
    a1 = 0;
    a2 = 0;

    /* Important to note the endianess of our data 
       and how it's stored. 
    */
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
            a1 = (
                (((int16)*(p+2) & 0xff) << 8)
                 | ((int16)*(p+1) & 0xff)
                ); 
            break;
        case 5:
            a1 = (
                (((int16)*(p+2) & 0xff) << 8)
                 | ((int16)*(p+1) & 0xff)
                ); 
            a2 = (
                (((int16)*(p+4) & 0xff) << 8)
                 | ((int16)*(p+3) & 0xff)
                ); 
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
        /* keeping the arguments in the function signature
           for consistency amongst all our operation funcs.
        */
        case ste:    __ste(vm, (Opcode)*p, a1, a2); break;
        case stg:    __stg(vm, (Opcode)*p, a1, a2); break;
        case sth:    __sth(vm, (Opcode)*p, a1, a2); break;
        case stl:    __stl(vm, (Opcode)*p, a1, a2); break;
        case cle:    __cle(vm, (Opcode)*p, a1, a2); break;
        case clg:    __clg(vm, (Opcode)*p, a1, a2); break;
        case clh:    __clh(vm, (Opcode)*p, a1, a2); break;
        case cll:    __cll(vm, (Opcode)*p, a1, a2); break;
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
        case ErrInstr:
            fprintf(stderr, "%s\n", "VM Illegal instruction");
            break;
        case SysHlt:
            fprintf(stderr, "%s\n", "System halted");
            exitcode = 0;
            // CHANGING FOR TESTING 
            printf("flags = %.04hx\n", $i vm $flags);
            if (equal(vm)) {
                printf("Equal flag is set\n");
            } else {
                printf("E flag is not set\n");
            }
            if (gt(vm)) {
                printf("Greater than flag is set\n");
            } else {
                printf("G flag not set\n");
            }
            if (higher(vm)) {
                printf("Higher than flag is set\n");
            } else {
                printf("H flag not set\n");
            }
            if (lower(vm)) {
                printf("Lower than flag is set\n");
            } else {
                printf("L flag not set\n");
            }
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
    Instruction *i1, *i2, *i3, *i4;
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
    i4 = (Instruction *)malloc( s2);

    assert(i1 && i2 && i3);

    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s2);
    zero($1 i4, s2);

    i1->o = 0x0a;     // mov to c register

    if (s1) {
        // this is how we are setting the mov instruction argument stirng
        a1    = 0xfbcd;
        i1->a[0] = a1;
    }

    p = vm->m;   // going into the vm's memory and copying there

    copy($1 p, $1 i1, 1);
    p++;

    if (a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    } 

    i2->o = ste;
    copy($1 p, $1 i2, 1);

    p++;
    i3->o = stg;
    copy($1 p, $1 i3, 1);

    p++;
    i4->o = hlt;
    copy($1 p, $1 i4, 1);
    

    vm->brk = (s1+sa1+s2+s2+s2);//break line set to the offset of progam size
    vm $ip = (Reg) vm->m;

    vm $sp = (Reg) -1;

    free(i1);
    free(i2);
    free(i3);
    free(i4);
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
