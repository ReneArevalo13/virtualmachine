/* vm.c */

#include "vm.h"
#include "birchutils.h"
#include <string.h>
#include <stdarg.h>

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
    int16 dst;
    dst = $2 a1;
    if (higher(vm) && lower(vm)) {
        error(vm, ErrInstr);
    }

    switch(opcode) {
        // mov ax
        case 0x08:
            if (higher(vm)) {
                vm $ax = (((Reg)a1 <<8) | (vm $ax & 0xFF));
            } else if (lower(vm)) {
                vm $ax = ((Reg)a1 | (vm $ax & 0xFF00));
            } else {
                vm $ax = (Reg)a1;
            }
            break;
        // mov bx
        case 0x09:
        if (higher(vm)) {
                vm $bx = (((Reg)a1 <<8) | (vm $bx & 0xFF));
            } else if (lower(vm)) {
                vm $bx = ((Reg)a1 | (vm $bx & 0xFF00));
            } else {
                vm $bx = (Reg)a1;
            }
            break;

        // mov cx
        case 0x0a:
        if (higher(vm)) {
                vm $cx = (((Reg)a1 <<8) | (vm $cx & 0xFF));
            } else if (lower(vm)) {
                vm $cx = ((Reg)a1 | (vm $cx & 0xFF00));
            } else {
                vm $cx = (Reg)a1;
            }
            break;
        // mov dx
        case 0x0b:
        if (higher(vm)) {
                vm $dx = (((Reg)a1 <<8) | (vm $dx & 0xFF));
            } else if (lower(vm)) {
                vm $dx = ((Reg)a1 | (vm $dx & 0xFF00));
            } else {
                vm $dx = (Reg)a1;
            }
            break;

        // sp
        case 0x0c:
            vm $sp = (Reg)dst;
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
  //  printf("execute size is %d\n", size);
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
            printf("ax= %.04hx\n", $i vm $ax);
             
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

Program *exampleprogram(VM *vm, ...) {
   Program *p, *instr;
   va_list ap;
   Opcode opc;
   int8 size;

   va_start(ap, vm);
   p = vm->m;
   do {
       instr = va_arg(ap, Program*);
       opc = (Opcode)*instr;
      // printf("I 0x%.02hhx\n", $c *instr);
       size = map(opc);
       copy($1 p, $1 instr, size);
       p += size;
       vm->brk += size;
       //printf("P 0x%.02hhx\n", $c *p);
   } while(opc != hlt);

   va_end(ap);
   return p; 
}
Instruction *i0(Opcode op) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;
    return i;
}
Instruction *i1(Opcode op, Args a1) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;
    i->a[0] = a1;
    return i;
}

Instruction *i2(Opcode op, Args a1, Args a2) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;
    i->a[0] = a1;
    i->a[1] = a2;
    return i;
}

Program *i(Instruction *i) {
    Program *p;
    int8 size;

    size = map(i->o);
    p = (Program *)malloc($i size);
    assert(p);
    copy(p, $1 i, size);
    copy((p+1), $1 i->a, (size-1));

    return p;
}

Program *exampleprogram2(VM *vm) {
    Program *p;
    Instruction *i0,*i1, *i2, *i3, *i4;
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
    i0 = (Instruction *)malloc(s2);
    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc( s2);
    i3 = (Instruction *)malloc( s2);
    i4 = (Instruction *)malloc( s2);

    assert(i1 && i2 && i3);

    zero($1 i0, s1);
    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s2);
    zero($1 i4, s2);

    i0->o = nop;
    p = vm->m;
    copy($1 p, $1 i0, 1);
    p++;

    vm $ax = 1;
    i1->o = mov;     // mov to a register

    if (s1) {
        // this is how we are setting the mov instruction argument stirng
        a1 = 0x05;
        i1->a[0] = a1;
    }

    //p = vm->m;   // going into the vm's memory and copying there

    copy($1 p, $1 i1, 1);
    p++;

    if (a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    } 

    i2->o = nop; 
    copy($1 p, $1 i2, 1);

    p++;
    i3->o = nop;
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
  //  exampleprogram(vm, i(instr));
   // exit(0);
    prog = exampleprogram(vm, 
           i(i1(mov, 0x06)), i(i0(ste)), i(i0(hlt)));
   // prog = exampleprogram2(vm);
  //  printf("prog = %p\n", prog);
    
    
    execute(vm);
    printf("ax = %.04hx\n", $i vm $ax);
    printhex($1 prog, (map(mov) + map(nop) + map(hlt)), ' ');
    
    return 0;
}
    
