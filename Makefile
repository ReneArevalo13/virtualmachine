

.PHONY: vm clean

vm: vm.c birchutils.c
	cc $(flags) -o $@ $^

clean:
	rm -f vm
