/* vm.c */

#include "vm.h"
#include "birchutils.h"

VM *virtualmachine() {
    VM *p;
    int16 size;

    //Program *pp;

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

    int count = 0;
   do {
        vm $ip += size;
        pp += size;
        ip.o = *pp;
        printf("CURRENT INSTRUCTION: %d\n", ip.o);
        if ((int16)pp > (int16)brkaddr) {
            segfault(vm);
        }
        size = map(ip.o);
        printf("THE SIZE IS NOW: %d\n", size);
        printf("EXECUTING INSTRUCTION\n");
        executeinstr(vm, &ip);
        count++;

    } while(count < 3);
//   (*pp != (Opcode)hlt)
   return;
}


void __mov(VM *vm, Opcode o, Args a1, Args a2) {
    vm $ax = (Reg) a1;
    return;
}

void executeinstr(VM *vm, Instruction *i) {
    int16 size;
    Args a1, a2;
    a1 = 0;
    a2 = 0;

    printf("instruction is : %d\n", i->o);
    size = map(i->o);
    printf("size is %d\n", size);
    switch (size) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            a1 = i->a[0];
            break;
        case 3:
            a1 = i->a[0];
            a2 = i->a[1];
            break;
        default:
            printf("Debug 1\n");
            segfault(vm);
            break;
    }
    
    printf("switch case: %d\n", i->o);
    switch(i->o) {
        case mov:
            __mov(vm, i->o, a1, a2);
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
    int16 s1, s2, s3, sa1;
    Args a1;

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);
    s3 = map(hlt);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s2);
    i3 = (Instruction *)malloc($i s3);

    assert(i1 && i2 && i3);

    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s3);

    i1->o = mov;
    printf("I1 is : %d\n", i1->o);
    sa1 = s1 - 1;    // everything is 1 byte in our "Program instructions"

    if (s1) {
        // this is how we are setting the mov instruction argument stirng
        a1    = 0x0007;
    }


    p = vm->m;   // going into the vm's memory and copying there
   // copy($1 p, $1 i1, 1);
    p++;

    if (a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    } 

    i2->o = nop;
   // copy($1 p, $1 i2, 1);
    printf("I2 is : %d\n", i2->o);

   // p += s2; 

    i3->o = 3;
    copy($1 p, $1 i3, 1);

    printf("I3 is : %d\n", i3->o);

    vm->brk = (s1+sa1+s2+s2);//break line set to the offset of progam size
    vm $ip = (Reg) vm->m;
    vm $sp = (Reg) -1;

    free(i1);
    free(i2);
   // free(i3);
    return (Program *)&vm->m;

}

int main (int argc, char *argv[]){
    Program *prog;
    VM *vm;
    int8 size;

    vm = virtualmachine();
    printf("vm = %p (sz: %d)\n", vm, sizeof(VM));

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

   // execute(vm);
    printf("ax = %.04hx\n", $i vm $ax);
    printf("map hlt : %d\n", map(hlt));
    printhex($1 prog, (map(mov) + map(nop) + map(hlt)), ' ');
    
    return 0;
}
