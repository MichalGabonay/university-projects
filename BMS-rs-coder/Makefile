CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2

all: bms2A bms2B


bms2A: bms2A.cpp
	$(CC) $(CFLAGS) $^ -o $@

bms2B: bms2B.cpp
	$(CC) $(CFLAGS) $^ -o $@

# %.o: %.c
# 	$(CC) $(CFLAGS) $^ -c -o $@

# rscode/libecc.a:
# 	$(MAKE) -C rscode

clean:
	rm -f *.o bms2A bms2B
