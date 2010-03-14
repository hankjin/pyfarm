p1: p1.c lib.so
	cc -o $@ p1.c ./lib.so

p2: p2.c lib.so
	cc -o $@ p2.c ./lib.so

lib.so: lib.c
	cc -shared -fPIC -o $@ $<

clean: p1 p2 lib.so 
	rm p1 p2 lib.so
