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
    Instruction *ip;
    int16 size;

    assert(vm && *vm->m);
    pp = vm->m;


    /* 
    Our instruction format:
    instr1 arg1 instr2 instr3
    mov ax, 0x05; nop; hlt;
    Binary representation:
    0x01 0x00 0x05
    0x02
    0x03
    */
    while((*pp != (Opcode)hlt) && (pp <= (Program *) vm->brk)) {
        ip = (Instruction *)pp;
        size = map(ip->o);
        
        executeinstr(vm, ip);

        vm $ip += size;
        pp += size;
    }

    // make sure we are not in the stack
    if (pp > (Program *) vm->brk) {
        printf("DEBUG Segfault line 53");
        segfault(vm);
    }
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


    size = map(i->o);
    switch (size) {
        case 0:
            break;
        case 1:
            a1 = i->a[0];
            break;
        case 2:
            a1 = i->a[0];
            a2 = i->a[1];
            break;
        default:
            printf("Debug 1\n");
            segfault(vm);
            break;
    }

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
    if (vm) {
        free(vm);
    }
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

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s2);
    i3 = (Instruction *)malloc($i s2);

    assert(i1 && i2);

    zero($1 i1, s1);
    zero($1 i2, s2);

    i1->o = mov;
    sa1 = s1 - 1;    // everything is 1 byte in our "Program instructions"

    if (s1) {
        // this is how we are setting the mov instruction argument stirng
        a1    = 0x0005;
    }


    p = vm->m;   // going into the vm's memory and copying there
    copy($1 p, $1 i1, 1);
    p++;
    if (a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    } 

    i2->o = nop;
    copy($1 p, $1 i2, 1);
    p++;
    i3->o = hlt;
    copy($1 p, $1 i3, 1);

    vm->brk = (s1+sa1+s2+s2);//break line set to the offset of progam size
    vm $ip = (Reg) vm->m;
    vm $sp = (Reg) -1;
    free(i1);
    free(i2);
    free(i3);

    return (Program *)&vm->m;

}

int main (int argc, char *argv[]){
    Program *prog;
    VM *vm;
    int8 size;
    size = map(mov) + map(nop);

    vm = virtualmachine();
    printf("vm = %p (sz: %d)\n", vm, sizeof(VM));

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

    execute(vm);
    printf("ax = %.04hx\n", $i vm $ax);
    printhex($1 prog, (map(mov)+map(nop)), ' ');
    
    return 0;
}
