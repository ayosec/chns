all: chns

chns: chns.c
	gcc -Wall -O2 -o chns chns.c

clean:
	rm chns

.PHONY: clean
