CC = gcc
CFLAGS = -c -Wall -Wextra
LFLAGS = -pthread -Wall -Wextra

all: multi-lookup

multi-lookup: multi-lookup.o queue.o util.o
	$(CC) $(LFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o multi-lookup
	rm output/reqlog.txt
	rm output/reslog.txt
	touch output/reqlog.txt
	touch output/reslog.txt

run:
	./multi-lookup 8 5 output/reqlog.txt output/reslog.txt input/names1.txt input/names2.txt input/names3.txt input/names4.txt input/names5.txt

valgrind:
	valgrind ./multi-lookup 8 5 output/reqlog.txt output/reslog.txt input/names1.txt input/names2.txt input/names3.txt input/names4.txt input/names5.txt
