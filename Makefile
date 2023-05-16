CC = g++
CFLAGS = -Wall -Wextra -g -std=c++11

P1 = supercomputer.cpp
P2 = .
P3 = .
P4 = .

build: $(P1)
	$(CC) $(CFLAGS) $(P1) -o supercomputer

run-p1:
	./supercomputer

clean:
	rm -f supercomputer supercomputer.out