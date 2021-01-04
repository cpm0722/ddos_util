##########

CC = gcc
CCOPTS = -c
DEBUG = -g
RENAME = -o
LINKOPTS = -lpthread -lm

ERASE = erase
TARGET = attack

BASE_DIR = base/
DDOS_DIR = ddos/
OBJ_DIR = obj/

##########

ASM_TYPES_H = $(BASE_DIR)asm_types.h
MAKE_IPV4_H = $(BASE_DIR)make_ipv4.h
MAKE_TCP_H  = $(BASE_DIR)make_tcp.h
MAKE_HTTP_H  = $(BASE_DIR)make_http.h
RECEIVER_H = $(BASE_DIR)receiver.h

CONN_FLOOD_H = $(DDOS_DIR)conn_flood.h
GET_FLOOD_H = $(DDOS_DIR)get_flood.h
SYN_FLOOD_H = $(DDOS_DIR)syn_flood.h
UDP_FLOOD_H = $(DDOS_DIR)udp_flood.h
ICMP_FLOOD_H = $(DDOS_DIR)icmp_flood.h

HEADER = header.h

BASE_HEADER = $(HEADER) $(ASM_TYPES_H) $(MAKE_IPV4_H)

#########

MAKE_IPV4_C = $(BASE_DIR)make_ipv4.c
MAKE_TCP_C  = $(BASE_DIR)make_tcp.c
MAKE_HTTP_C  = $(BASE_DIR)make_http.c
RECEIVER_C = $(BASE_DIR)receiver.c

CONN_FLOOD_C = $(DDOS_DIR)conn_flood.c
GET_FLOOD_C = $(DDOS_DIR)get_flood.c
SYN_FLOOD_C = $(DDOS_DIR)syn_flood.c
UDP_FLOOD_C = $(DDOS_DIR)udp_flood.c
ICMP_FLOOD_C = $(DDOS_DIR)icmp_flood.c

MAIN_C = main.c

#########

MAKE_IPV4_O = $(OBJ_DIR)make_ipv4.o
MAKE_TCP_O  = $(OBJ_DIR)make_tcp.o
MAKE_HTTP_O  = $(OBJ_DIR)make_http.o
RECEIVER_O = $(OBJ_DIR)receiver.o

CONN_FLOOD_O = $(OBJ_DIR)conn_flood.o
GET_FLOOD_O = $(OBJ_DIR)get_flood.o
SYN_FLOOD_O = $(OBJ_DIR)syn_flood.o
UDP_FLOOD_O = $(OBJ_DIR)udp_flood.o
ICMP_FLOOD_O = $(OBJ_DIR)icmp_flood.o

MAIN_O = $(OBJ_DIR)main.o

#########

.SUFFIXES : .c.h.o.out

$(TARGET): $(MAIN_O) $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) 
	$(CC) $(MAIN_O) $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) $(RENAME) $(TARGET).out $(LINKOPTS)

$(MAIN_O): $(MAIN_C)
	$(CC) $(CCOPTS) $(MAIN_C) $(LINKOPTS) $(RENAME) $(MAIN_O)

$(MAKE_IPV4_O): $(MAKE_IPV4_C) $(BASE_HEADER)
	$(CC) $(CCOPTS) $(MAKE_IPV4_C) $(LINKOPTS) $(RENAME) $(MAKE_IPV4_O)

$(MAKE_TCP_O): $(MAKE_TCP_C) $(BASE_HEADER) $(MAKE_TCP_H) 
	$(CC) $(CCOPTS) $(MAKE_TCP_C) $(LINKOPTS) $(RENAME) $(MAKE_TCP_O)

$(RECEIVER_O): $(RECEIVER_C) $(BASE_HEADER) $(RECEIVER_H)
	$(CC) $(CCOPTS) $(RECEIVER_C) $(LINKOPTS) $(RENAME) $(RECEIVER_O)

$(CONN_FLOOD_O): $(CONN_FLOOD_C) $(BASE_HEADER) $(MAKE_TCP_H) $(RECEIVER_H) $(CONN_FLOOD_H)
	$(CC) $(CCOPTS) $(CONN_FLOOD_C) $(LINKOPTS) $(RENAME) $(CONN_FLOOD_O)

$(SYN_FLOOD_O): $(SYN_FLOOD_C) $(BASE_HEADER) $(MAKE_TCP_H) $(SYN_FLOOD_H)
	$(CC) $(CCOPTS) $(SYN_FLOOD_C) $(LINKOPTS) $(RENAME) $(SYN_FLOOD_O)

$(UDP_FLOOD_O): $(UDP_FLOOD_C) $(BASE_HEADER) $(UDP_FLOOD_H)
	$(CC) $(CCOPTS) $(UDP_FLOOD_C) $(LINKOPTS) $(RENAME) $(UDP_FLOOD_O)

$(ICMP_FLOOD_O): $(ICMP_FLOOD_C) $(BASE_HEADER) $(ICMP_FLOOD_H)
	$(CC) $(CCOPTS) $(ICMP_FLOOD_C) $(LINKOPTS) $(RENAME) $(ICMP_FLOOD_O)

$(ERASE):
	rm $(OBJ_DIR)*.o

