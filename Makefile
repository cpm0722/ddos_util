##########
CC = gcc
CCOPTS = -c -g
DEBUG = -g
RENAME = -o
LINKOPTS = -lpthread -lm $(DEBUG)

ERASE = erase
CLEAN = clean
TARGET = attack

SRC_DIR = src/
BASE_DIR = $(SRC_DIR)base/
DDOS_DIR = $(SRC_DIR)ddos/
OBJ_DIR = obj/

##########

ASM_TYPES_H = $(BASE_DIR)asm_types.h
MAKE_IPV4_H = $(BASE_DIR)make_ipv4.h
MAKE_TCP_H  = $(BASE_DIR)make_tcp.h
MAKE_HTTP_H  = $(BASE_DIR)make_http.h
RECEIVER_H = $(BASE_DIR)receiver.h
SUBNET_MASK_H = $(BASE_DIR)subnet_mask.h
TIME_CHECK_H = $(BASE_DIR)time_check.h

CONN_FLOOD_H = $(DDOS_DIR)conn_flood.h
GET_FLOOD_H = $(DDOS_DIR)get_flood.h
SYN_FLOOD_H = $(DDOS_DIR)syn_flood.h
UDP_FLOOD_H = $(DDOS_DIR)udp_flood.h
ICMP_FLOOD_H = $(DDOS_DIR)icmp_flood.h
HASH_DOS_H = $(DDOS_DIR)hash_dos.h
HEADER_BUFFERING_H = $(DDOS_DIR)header_buffering.h
BODY_BUFFERING_H = $(DDOS_DIR)body_buffering.h
RESPONSE_BUFFERING_H = $(DDOS_DIR)response_buffering.h

HEADER = $(SRC_DIR)header.h

BASE_HEADER = $(HEADER) $(ASM_TYPES_H) $(MAKE_IPV4_H)

#########

MAKE_IPV4_C = $(BASE_DIR)make_ipv4.c
MAKE_TCP_C  = $(BASE_DIR)make_tcp.c
MAKE_HTTP_C  = $(BASE_DIR)make_http.c
RECEIVER_C = $(BASE_DIR)receiver.c
SUBNET_MASK_C = $(BASE_DIR)subnet_mask.c
TIME_CHECK_C = $(BASE_DIR)time_check.c

CONN_FLOOD_C = $(DDOS_DIR)conn_flood.c
GET_FLOOD_C = $(DDOS_DIR)get_flood.c
SYN_FLOOD_C = $(DDOS_DIR)syn_flood.c
UDP_FLOOD_C = $(DDOS_DIR)udp_flood.c
ICMP_FLOOD_C = $(DDOS_DIR)icmp_flood.c
HASH_DOS_C = $(DDOS_DIR)hash_dos.c
HEADER_BUFFERING_C = $(DDOS_DIR)header_buffering.c
BODY_BUFFERING_C = $(DDOS_DIR)body_buffering.c
RESPONSE_BUFFERING_C = $(DDOS_DIR)response_buffering.c

MAIN_C = $(SRC_DIR)main.c

#########

MAKE_IPV4_O = $(OBJ_DIR)make_ipv4.o
MAKE_TCP_O  = $(OBJ_DIR)make_tcp.o
MAKE_HTTP_O  = $(OBJ_DIR)make_http.o
RECEIVER_O = $(OBJ_DIR)receiver.o
SUBNET_MASK_O = $(OBJ_DIR)subnet_mask.o
TIME_CHECK_O = $(OBJ_DIR)time_check.o

CONN_FLOOD_O = $(OBJ_DIR)conn_flood.o
GET_FLOOD_O = $(OBJ_DIR)get_flood.o
SYN_FLOOD_O = $(OBJ_DIR)syn_flood.o
UDP_FLOOD_O = $(OBJ_DIR)udp_flood.o
ICMP_FLOOD_O = $(OBJ_DIR)icmp_flood.o
HASH_DOS_O = $(OBJ_DIR)hash_dos.o
HEADER_BUFFERING_O = $(OBJ_DIR)header_buffering.o
BODY_BUFFERING_O = $(OBJ_DIR)body_buffering.o
RESPONSE_BUFFERING_O = $(OBJ_DIR)response_buffering.o

MAIN_O = $(OBJ_DIR)main.o

#########

.SUFFIXES : .c.h.o.out

$(TARGET): MKDIR_OBJ $(MAIN_O) $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(SUBNET_MASK_O) $(TIME_CHECK_O) $(GET_FLOOD_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) $(HASH_DOS_O) $(HEADER_BUFFERING_O) $(RESPONSE_BUFFERING_O) $(BODY_BUFFERING_O)
	$(CC) $(MAIN_O) $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(SUBNET_MASK_O) $(TIME_CHECK_O) $(GET_FLOOD_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) $(HASH_DOS_O) $(HEADER_BUFFERING_O) $(RESPONSE_BUFFERING_O) $(BODY_BUFFERING_O) $(RENAME) $(TARGET).out $(LINKOPTS)

MKDIR_OBJ:
	mkdir -p obj

$(MAIN_O): $(MAIN_C) $(HEADER) $(GET_FLOOD_H) $(ICMP_FLOOD_H) $(SYN_FLOOD_H) $(UDP_FLOOD_H) $(CONN_FLOOD_H) $(HASH_DOS_H) $(HEADER_BUFFERING_H) $(BODY_BUFFERING_H) $(RESPONSE_BUFFERING_H)
	$(CC) $(CCOPTS) $(MAIN_C) $(LINKOPTS) $(RENAME) $(MAIN_O)

$(MAKE_IPV4_O): $(MAKE_IPV4_C) $(BASE_HEADER)
	$(CC) $(CCOPTS) $(MAKE_IPV4_C) $(LINKOPTS) $(RENAME) $(MAKE_IPV4_O)

$(MAKE_TCP_O): $(MAKE_TCP_C) $(BASE_HEADER) $(MAKE_TCP_H) 
	$(CC) $(CCOPTS) $(MAKE_TCP_C) $(LINKOPTS) $(RENAME) $(MAKE_TCP_O)

$(RECEIVER_O): $(RECEIVER_C) $(BASE_HEADER) $(RECEIVER_H)
	$(CC) $(CCOPTS) $(RECEIVER_C) $(LINKOPTS) $(RENAME) $(RECEIVER_O)

$(SUBNET_MASK_O): $(SUBNET_MASK_C) $(BASE_HEADER) $(SUBNET_MASK_H)
	$(CC) $(CCOPTS) $(SUBNET_MASK_C) $(LINKOPTS) $(RENAME) $(SUBNET_MASK_O)

$(TIME_CHECK_O): $(TIME_CHECK_C) $(BASE_HEADER) $(TIME_CHECK_H)
	$(CC) $(CCOPTS) $(TIME_CHECK_C) $(LINKOPTS) $(RENAME) $(TIME_CHECK_O)

$(GET_FLOOD_O): $(GET_FLOOD_C) $(BASE_HEADER) $(GET_FLOOD_H)
	$(CC) $(CCOPTS) $(GET_FLOOD_C) $(LINKOPTS) $(RENAME) $(GET_FLOOD_O)

$(CONN_FLOOD_O): $(CONN_FLOOD_C) $(BASE_HEADER) $(MAKE_TCP_H) $(RECEIVER_H) $(CONN_FLOOD_H)
	$(CC) $(CCOPTS) $(CONN_FLOOD_C) $(LINKOPTS) $(RENAME) $(CONN_FLOOD_O)

$(SYN_FLOOD_O): $(SYN_FLOOD_C) $(BASE_HEADER) $(MAKE_TCP_H) $(SYN_FLOOD_H)
	$(CC) $(CCOPTS) $(SYN_FLOOD_C) $(LINKOPTS) $(RENAME) $(SYN_FLOOD_O)

$(UDP_FLOOD_O): $(UDP_FLOOD_C) $(BASE_HEADER) $(UDP_FLOOD_H)
	$(CC) $(CCOPTS) $(UDP_FLOOD_C) $(LINKOPTS) $(RENAME) $(UDP_FLOOD_O)

$(ICMP_FLOOD_O): $(ICMP_FLOOD_C) $(BASE_HEADER) $(ICMP_FLOOD_H)
	$(CC) $(CCOPTS) $(ICMP_FLOOD_C) $(LINKOPTS) $(RENAME) $(ICMP_FLOOD_O)

$(HASH_DOS_O): $(HASH_DOS_C) $(BASE_HEADER) $(HASH_DOS_H)
	$(CC) $(CCOPTS) $(HASH_DOS_C) $(LINKOPTS) $(RENAME) $(HASH_DOS_O)

$(HEADER_BUFFERING_O) : $(HEADER_BUFFERING_C) $(BASE_HEADER) $(HEADER_BUFFERING_H)
	$(CC) $(CCOPTS) $(HEADER_BUFFERING_C) $(LINKOPTS) $(RENAME) $(HEADER_BUFFERING_O)

$(BODY_BUFFERING_O) : $(BODY_BUFFERING_C) $(BASE_HEADER) $(BODY_BUFFERING_H)
	$(CC) $(CCOPTS) $(BODY_BUFFERING_C) $(LINKOPTS) $(RENAME) $(BODY_BUFFERING_O)

$(RESPONSE_BUFFERING_O) : $(RESPONSE_BUFFERING_C) $(BASE_HEADER) $(RESPONSE_BUFFERING_H)
	$(CC) $(CCOPTS) $(RESPONSE_BUFFERING_C) $(LINKOPTS) $(RENAME) $(RESPONSE_BUFFERING_O)

$(ERASE):
	rm $(OBJ_DIR)*.o

$(CLEAN):
	rm $(OBJ_DIR)*.o
