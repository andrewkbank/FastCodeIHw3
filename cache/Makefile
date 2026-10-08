CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c11 -mavx2

OBJS = morton.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) driver.c -o transpose.x -march=native

correct: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) correctness_driver.c -o correctness.x -march=native

run: compile
	./transpose.x
	./transpose.x 1000 8 256

check: correct
	./correctness.x

all: compile correct

clean:
	rm -f *.x *~ *.o