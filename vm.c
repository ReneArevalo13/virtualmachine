/* vm.c */

#include "vm.h"
#include "birchutils.h"

VM *virtualmachine() {
    VM *p;
    int16 size;

    assert((pr) && (progsz));

    //Program *pp;

    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }

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
    return p;
    */

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

Program *exampleprogram() {
    Program *prog;
    Instruction *i1, *i2;
    int16 s1, s2, sa1, progsz;
    Args *a1;

    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc($1 s1);
    i2 = (Instruction *)malloc($1 s1);

    assert(i1 && i2);

    zero($1 i1, s1);
    zero($1 i2, s2);

    i1->o = mov;
    sa1 = s1 - 1;    // everything is 1 byte in our "Program instructions"

    if (s1) {
        a1 = (Args *)malloc($1 sa1);
        assert(a1);
        zero(a1, sa1);
        // this is how we are setting the mov instruction argument stirng
        *a1    = 0x00;
        *(a+1) = 0x05;
    }
    progsz = s1 + s2;
    prog = (Prog *)
    
}

int main (int argc, char *argv[]){
    Program *prog;
    VM *vm;
    int8 size;
    size = map(mov) + map(nop);

    prog = exampleprogram();
    printf("prog = %p\n", prog);
    vm = virtualmachine(prog, size);
    printf("vm = %p\n", vm);
    return 0;
}
