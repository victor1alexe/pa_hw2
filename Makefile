CC = g++
CFLAGS = -Wall -Wextra -g -std=c++11

P1 = supercomputer.cpp
P2 = ferate.cpp
P3 = .
P4 = magazin.cpp

build: $(P1) $(P2) $(P4)
	$(CC) $(CFLAGS) $(P1) -o supercomputer
	$(CC) $(CFLAGS) $(P2) -o ferate
	$(CC) $(CFLAGS) $(P4) -o magazin

run-p1:
	./supercomputer

run-p2:
	./ferate

run-p4:
	./magazin

clean:
	rm -f supercomputer magazin ferate *.out