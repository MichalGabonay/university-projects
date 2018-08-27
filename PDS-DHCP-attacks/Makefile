CC = g++
CFLAGS = -g -std=c++11 -Wall -Wextra
OBJS = pds-dhcpstarve.o ethernet.o socket.o dhcp.o
OBJR = pds-dhcprogue.o ethernet.o socket.o dhcp.o
SRC = $(wildcard *.cpp)

all: pds-dhcpstarve pds-dhcprogue

pds-dhcpstarve: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

pds-dhcprogue: $(OBJR)
	$(CC) $(CFLAGS) $(OBJR) -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o pds-dhcpstarve pds-dhcprogue