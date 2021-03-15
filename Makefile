##########
CC = gcc
INCLUDE_DIR = includes
CCOPTS = -c -I $(INCLUDE_DIR)
RENAME = -o
LINKOPTS = -lpthread -lm

ALL = all
MKDIR_OBJ = mkdir_obj
LIB = lib
ERASE = erase
CLEAN = clean

LIB_NAME = ddos
TARGET = attack.out

SRC_DIR = srcs
BASE_DIR = base
DDOS_DIR = ddos
OBJ_DIR = objs

##########

ASM_TYPES_H = $(BASE_DIR)/asm_types.h
MAKE_IPV4_H = $(BASE_DIR)/make_ipv4.h
MAKE_TCP_H  = $(BASE_DIR)/make_tcp.h
MAKE_HTTP_H  = $(BASE_DIR)/make_http.h
RECEIVER_H = $(BASE_DIR)/receiver.h
SUBNET_MASK_H = $(BASE_DIR)/subnet_mask.h
TIME_CHECK_H = $(BASE_DIR)/time_check.h

CONN_FLOOD_H = $(DDOS_DIR)/conn_flood.h
GET_FLOOD_H = $(DDOS_DIR)/get_flood.h
SYN_FLOOD_H = $(DDOS_DIR)/syn_flood.h
UDP_FLOOD_H = $(DDOS_DIR)/udp_flood.h
ICMP_FLOOD_H = $(DDOS_DIR)/icmp_flood.h
HASH_DOS_H = $(DDOS_DIR)/hash_dos.h
HEADER_BUFFERING_H = $(DDOS_DIR)/header_buffering.h
BODY_BUFFERING_H = $(DDOS_DIR)/body_buffering.h
RESPONSE_BUFFERING_H = $(DDOS_DIR)/response_buffering.h

HEADER = header.h

BASE_HEADER = $(HEADER) $(ASM_TYPES_H) $(MAKE_IPV4_H)

#########

MAKE_IPV4_C = $(SRC_DIR)/$(BASE_DIR)/make_ipv4.c
MAKE_TCP_C  = $(SRC_DIR)/$(BASE_DIR)/make_tcp.c
MAKE_HTTP_C  = $(SRC_DIR)/$(BASE_DIR)/make_http.c
RECEIVER_C = $(SRC_DIR)/$(BASE_DIR)/receiver.c
SUBNET_MASK_C = $(SRC_DIR)/$(BASE_DIR)/subnet_mask.c
TIME_CHECK_C = $(SRC_DIR)/$(BASE_DIR)/time_check.c

CONN_FLOOD_C = $(SRC_DIR)/$(DDOS_DIR)/conn_flood.c
GET_FLOOD_C = $(SRC_DIR)/$(DDOS_DIR)/get_flood.c
SYN_FLOOD_C = $(SRC_DIR)/$(DDOS_DIR)/syn_flood.c
UDP_FLOOD_C = $(SRC_DIR)/$(DDOS_DIR)/udp_flood.c
ICMP_FLOOD_C = $(SRC_DIR)/$(DDOS_DIR)/icmp_flood.c
HASH_DOS_C = $(SRC_DIR)/$(DDOS_DIR)/hash_dos.c
HEADER_BUFFERING_C = $(SRC_DIR)/$(DDOS_DIR)/header_buffering.c
BODY_BUFFERING_C = $(SRC_DIR)/$(DDOS_DIR)/body_buffering.c
RESPONSE_BUFFERING_C = $(SRC_DIR)/$(DDOS_DIR)/response_buffering.c

MAIN_C = $(SRC_DIR)/main.c

#########

MAKE_IPV4_O = $(OBJ_DIR)/make_ipv4.o
MAKE_TCP_O  = $(OBJ_DIR)/make_tcp.o
MAKE_HTTP_O  = $(OBJ_DIR)/make_http.o
RECEIVER_O = $(OBJ_DIR)/receiver.o
SUBNET_MASK_O = $(OBJ_DIR)/subnet_mask.o
TIME_CHECK_O = $(OBJ_DIR)/time_check.o

CONN_FLOOD_O = $(OBJ_DIR)/conn_flood.o
GET_FLOOD_O = $(OBJ_DIR)/get_flood.o
SYN_FLOOD_O = $(OBJ_DIR)/syn_flood.o
UDP_FLOOD_O = $(OBJ_DIR)/udp_flood.o
ICMP_FLOOD_O = $(OBJ_DIR)/icmp_flood.o
HASH_DOS_O = $(OBJ_DIR)/hash_dos.o
HEADER_BUFFERING_O = $(OBJ_DIR)/header_buffering.o
BODY_BUFFERING_O = $(OBJ_DIR)/body_buffering.o
RESPONSE_BUFFERING_O = $(OBJ_DIR)/response_buffering.o

MAIN_O = $(OBJ_DIR)/main.o

#########

.SUFFIXES : .c.h.o.a.out

$(ALL): $(CLEAN) $(TARGET)

$(LIB): $(MKDIR_OBJ) $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(SUBNET_MASK_O) $(TIME_CHECK_O) $(GET_FLOOD_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) $(HASH_DOS_O) $(HEADER_BUFFERING_O) $(RESPONSE_BUFFERING_O) $(BODY_BUFFERING_O)
	ar -rc lib$(LIB_NAME).a $(MAKE_IPV4_O) $(MAKE_TCP_O) $(RECEIVER_O) $(SUBNET_MASK_O) $(TIME_CHECK_O) $(GET_FLOOD_O) $(CONN_FLOOD_O) $(SYN_FLOOD_O) $(UDP_FLOOD_O) $(ICMP_FLOOD_O) $(HASH_DOS_O) $(HEADER_BUFFERING_O) $(RESPONSE_BUFFERING_O) $(BODY_BUFFERING_O)
	ranlib lib$(LIB_NAME).a

$(TARGET): $(LIB) $(MAIN_O)
	gcc -I includes $(MAIN_O) -L./ -l$(LIB_NAME) $(LINKOPTS) $(RENAME) $(TARGET)

$(MKDIR_OBJ):
	mkdir -p obj

$(CLEAN):
	rm -f $(OBJ_DIR)/*.o lib$(LIB_NAME).a attack.out

$(MAIN_O): $(LIB) $(MAIN_C)
	$(CC) $(CCOPTS) $(MAIN_C) $(LINKOPTS) $(RENAME) $(MAIN_O)

$(MAKE_IPV4_O): $(MAKE_IPV4_C)
	$(CC) $(CCOPTS) $(MAKE_IPV4_C) $(LINKOPTS) $(RENAME) $(MAKE_IPV4_O)

$(MAKE_TCP_O): $(MAKE_TCP_C)
	$(CC) $(CCOPTS) $(MAKE_TCP_C) $(LINKOPTS) $(RENAME) $(MAKE_TCP_O)

$(RECEIVER_O): $(RECEIVER_C)
	$(CC) $(CCOPTS) $(RECEIVER_C) $(LINKOPTS) $(RENAME) $(RECEIVER_O)

$(SUBNET_MASK_O): $(SUBNET_MASK_C)
	$(CC) $(CCOPTS) $(SUBNET_MASK_C) $(LINKOPTS) $(RENAME) $(SUBNET_MASK_O)

$(TIME_CHECK_O): $(TIME_CHECK_C)
	$(CC) $(CCOPTS) $(TIME_CHECK_C) $(LINKOPTS) $(RENAME) $(TIME_CHECK_O)

$(GET_FLOOD_O): $(GET_FLOOD_C)
	$(CC) $(CCOPTS) $(GET_FLOOD_C) $(LINKOPTS) $(RENAME) $(GET_FLOOD_O)

$(CONN_FLOOD_O): $(CONN_FLOOD_C)
	$(CC) $(CCOPTS) $(CONN_FLOOD_C) $(LINKOPTS) $(RENAME) $(CONN_FLOOD_O)

$(SYN_FLOOD_O): $(SYN_FLOOD_C)
	$(CC) $(CCOPTS) $(SYN_FLOOD_C) $(LINKOPTS) $(RENAME) $(SYN_FLOOD_O)

$(UDP_FLOOD_O): $(UDP_FLOOD_C)
	$(CC) $(CCOPTS) $(UDP_FLOOD_C) $(LINKOPTS) $(RENAME) $(UDP_FLOOD_O)

$(ICMP_FLOOD_O): $(ICMP_FLOOD_C)
	$(CC) $(CCOPTS) $(ICMP_FLOOD_C) $(LINKOPTS) $(RENAME) $(ICMP_FLOOD_O)

$(HASH_DOS_O): $(HASH_DOS_C)
	$(CC) $(CCOPTS) $(HASH_DOS_C) $(LINKOPTS) $(RENAME) $(HASH_DOS_O)

$(HEADER_BUFFERING_O) : $(HEADER_BUFFERING_C)
	$(CC) $(CCOPTS) $(HEADER_BUFFERING_C) $(LINKOPTS) $(RENAME) $(HEADER_BUFFERING_O)

$(BODY_BUFFERING_O) : $(BODY_BUFFERING_C)
	$(CC) $(CCOPTS) $(BODY_BUFFERING_C) $(LINKOPTS) $(RENAME) $(BODY_BUFFERING_O)

$(RESPONSE_BUFFERING_O) : $(RESPONSE_BUFFERING_C)
	$(CC) $(CCOPTS) $(RESPONSE_BUFFERING_C) $(LINKOPTS) $(RENAME) $(RESPONSE_BUFFERING_O)
