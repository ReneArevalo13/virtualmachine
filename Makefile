

.PHONY: vm clean

vm: vm.c birchutils.c
	cc vm.c birchutils.c -Wall -g -o vm	

clean:
	rm -f vm
