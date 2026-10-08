CC = gcc
CFLAGS = -O3 -std=c99

all: 
	$(CC) $(CFLAGS) $(OBJS) kernel_driver.c  -o mmm.x -march=native -mfma -mavx
run:
	./mmm.x

clean:
	rm -f *.x *~ *.o
