/* vm.c */

#include "vm.h"
#include "birchutils.h"

VM *virtualmachine(Program pr, int16 progsz) {
    VM *p;
    int16 size;

    assert((pr) && (progsz));

    Program pp;

    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }

    zero($1 p, size);
    pp = (Program)malloc($i progsz);
    if (!pp) {
        free(p);
        errno = ErrMem;
        return (VM *)0;
    }
    copy($1 pp, $1 pr, progsz);
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

Program exampleprogram() {

    int8 size;
    //Program prog[2];
    Instruction i1, i2;

    size = map(mov);
    i1 = (Instruction)malloc($i size);
    if (!i1) {
        errno = ErrMem;
        return (Program)0;
    }

    size = map(nop);
    i2 = (Instruction)malloc($i size);
    if (!i2) {
        errno = ErrMem;
        return (Program)0;
    }
    Program prog = {i1, i2}; 
    return prog;
}

int main (int argc, char *argv[]){
    Program prog;
    VM *vm;
    int8 size;
    size = map(mov) + map(nop);

    prog = exampleprogram();
    printf("prog = %p\n", prog);
    vm = virtualmachine(prog, size);
    printf("vm = %p\n", vm);
    return 0;
}
