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

    // refactor to make sure vm is on the stack and not heap
    /*
    zero($1 p, size);
    pp = (Program *)malloc($i progsz);
    if (!pp) {
        free(p);
        errno = ErrMem;
        return (VM *)0;
    }
    copy(pp, pr, progsz);
       */

//    printf("Virtual machine done\n");
    return p;

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
    Instruction *i1, *i2;
    int16 s1, s2, sa1;
    Args *a1;

    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s1);

    assert(i1 && i2);

    zero($1 i1, s1);
    zero($1 i2, s2);

    i1->o = mov;
    sa1 = s1 - 1;    // everything is 1 byte in our "Program instructions"

    if (s1) {
        a1 = (Args *)malloc($i sa1);
        assert(a1);
        zero(a1, sa1);
        // this is how we are setting the mov instruction argument stirng
        *a1    = 0x00;
        *(a1+1) = 0x05;
    }


    p = vm->m;   // going into the vm's memory and copying there
    copy($1 p, $1 i1, 1);
    p++;
    if (a1) {
        copy($1 p, $1 a1, sa1);
        p += sa1;
        free(a1);
    }

    i2->o = nop;
    copy($1 p, $1 i2, 1);
    free(i1);
    free(i2);
//    printf("end of example program\n");
    return vm->m;
}

int main (int argc, char *argv[]){
    Program *prog;
    VM *vm;
    int8 size;
    size = map(mov) + map(nop);

    vm = virtualmachine();
    printf("vm = %p\n", vm);

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);
    
    return 0;
}
