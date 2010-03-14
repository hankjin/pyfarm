#use ld script
shello2: shello.o 
	ld -static -T shello.lds -o $@ $<

shello: shello.o
	ld -static -e nomain -o $@ $<

shello.o: shello.c
	cc -c -fno-builtin $<

relf: relf.c
	cc relf.c -o relf
	./relf ss.o

test: test.o hjz.o
test.o: test.c

hjz.o: hjz.jpg
	objcopy -I binary -O elf32-i386 -B i386 hjz.jpg hjz.o

clean: 
	rm -f *.o 
