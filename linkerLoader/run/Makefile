CFLAGS:=-fno-builtin -nostdlib -fno-stack-protector
CPPFLAGS:=-nostdinc++ -fno-rtti -fno-exceptions
LDFLAGS:=-static -e mini_crt_entry entry.o

CLIB:=entry.c malloc.c stdio.c string.c print.c atexit.c
CPLIB:=hjzbegin.cc hjzend.cc ctors.cc new_delete.cc sysdep.cc iostream.cc

hi: xminicrt.a hi.cc
	g++ -c $(CPPFLAGS) $(CFLAGS) hi.cc
	ld $(LDFLAGS) hjzbegin.o hi.o xminicrt.a hjzend.o -o hi
xminicrt.a: $(CLIB) $(CPLIB) $(CPLIB)
	gcc -c $(CFLAGS) $(CLIB)
	g++ -c $(CPPFLAGS) $(CFLAGS) $(CPLIB)
	ar -rs xminicrt.a malloc.o print.o stdio.o string.o ctors.o atexit.o new_delete.o sysdep.o iostream.o
hello: hello.cc
	c++ -o $@ $<
	objdump -dr $@
	c++filt _Znwj

test: minicrt.a
	cc -c $(CFLAGS) test.c
	ld -static -e mini_crt_entry entry.o test.o minicrt.a -o test
minicrt.a: $(CLIB) 
	gcc -c $(CFLAGS) entry.c malloc.c stdio.c string.c print.c
	ar -rs minicrt.a malloc.o print.o stdio.o string.o

ate: ate.c

tls: tls.c

ab: a.o b.o
	cc a.o b.o -o $@

clean:
	rm -f *.o xminicrt.a minicrt.a
