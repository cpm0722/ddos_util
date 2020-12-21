##########
CC = g++
CCOPTS = -c
DEBUG = -g
RENAME = -o
LINKOPTS = -lpthread -lm
HEADER = header.h asm_types.h

ERASE = erase

TARGET = attack
##########

.SUFFIXES : .cpp.o.out

$(TARGET): main.o make_ipv4.o make_tcp.o receiver.o conn_flood.o syn_flood.o udp_flood.o icmp_flood.o 
	$(CC) main.o make_ipv4.o make_tcp.o receiver.o conn_flood.o syn_flood.o udp_flood.o icmp_flood.o $(RENAME) $(TARGET).out $(LINKOPTS)

main.o: main.cpp $(HEADER)
	$(CC) $(CCOPTS) main.cpp $(LINKOPTS)

make_ipv4.o: make_ipv4.cpp $(HEADER) make_ipv4.h
	$(CC) $(CCOPTS) make_ipv4.cpp $(LINKOPTS)

make_tcp.o: make_tcp.cpp $(HEADER) make_ipv4.h make_tcp.h
	$(CC) $(CCOPTS) make_tcp.cpp $(LINKOPTS)

receiver.o: receiver.cpp $(HEADER) make_ipv4.h receiver.h
	$(CC) $(CCOPTS) receiver.cpp $(LINKOPTS)

conn_flood.o: conn_flood.cpp $(HEADER) make_ipv4.h make_tcp.h receiver.h
	$(CC) $(CCOPTS) conn_flood.cpp $(LINKOPTS)

syn_flood.o: syn_flood.cpp $(HEADER) make_ipv4.h make_tcp.h
	$(CC) $(CCOPTS) syn_flood.cpp $(LINKOPTS)

udp_flood.o: udp_flood.cpp $(HEADER) udp_flood.h
	$(CC) $(CCOPTS) udp_flood.cpp $(LINKOPTS)

icmp_flood.o: icmp_flood.cpp $(HEADER) icmp_flood.h make_ipv4.h
	$(CC) $(CCOPTS) icmp_flood.cpp $(LINKOPTS)

$(ERASE):
	rm *.o

