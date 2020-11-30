##########
CC = g++
CCOPTS = -c
LINKOPTS = -lpthread -lm
HEADER = header.h asm_types.h

ERASE = erase

TARGET = attack
##########

.SUFFIXES : .cpp.o

$(TARGET): main.o make_ipv4.o make_tcp.o icmp_flood.o syn_flood.o udp_flood.o
	$(CC) main.o make_ipv4.o make_tcp.o icmp_flood.o syn_flood.o udp_flood.o -o attack.out -lpthread

main.o: main.cpp $(HEADER)
	$(CC) $(CCOPTS) main.cpp $(LINKOPTS)

make_ipv4.o: make_ipv4.cpp $(HEADER) make_ipv4.h
	$(CC) $(CCOPTS) make_ipv4.cpp $(LINKOPTS)

make_tcp.o: make_tcp.cpp $(HEADER) make_ipv4.h make_tcp.h
	$(CC) $(CCOPTS) make_tcp.cpp $(LINKOPTS)

icmp_flood.o: icmp_flood.cpp $(HEADER) icmp_flood.h make_ipv4.h
	$(CC) $(CCOPTS) icmp_flood.cpp $(LINKOPTS)

syn_flood.o: syn_flood.cpp $(HEADER) make_ipv4.h make_tcp.h
	$(CC) $(CCOPTS) syn_flood.cpp $(LINKOPTS)

udp_flood.o: udp_flood.cpp $(HEADER) udp_flood.h
	$(CC) $(CCOPTS) udp_flood.cpp make_ipv4.cpp $(LINKOPTS)

$(ERASE):
	rm *.o
