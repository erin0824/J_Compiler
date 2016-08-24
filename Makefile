all: jc

jc: jc.c token.o toasm.o
	clang -g -o jc jc.c token.o toasm.o

toasm.o: toasm.c toasm.h token.h
	clang -g -c toasm.c

token.o: token.c token.h 
	clang -g -c token.c

clean:
	rm -f *.o

clobber: clean
	rm -f jc
