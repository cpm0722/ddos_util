#################
#  DIRECTORIES  #
#################
INCLUDE_DIR = includes
SRC_DIR = srcs
OBJ_DIR = objs
LIB_DIR = libs
BASE_DIR = base
DDOS_DIR = ddos

#################
#  GCC-OPTIONS  #
#################
CC = gcc
INCLUDE_OPT = -I $(INCLUDE_DIR)
CCOPTS = -c $(INCLUDE_OPT)
LINKOPTS = -lpthread -lm

#################
#   LIBRARIES   #
#################
LIBOPTS = -L./$(LIB_DIR)
DDOS_LIB_NAME = ddos
BASE_LIB_NAME = base
DDOS_LIB_A = $(LIB_DIR)/lib$(DDOS_LIB_NAME).a
BASE_LIB_A = $(LIB_DIR)/lib$(BASE_LIB_NAME).a

#################
#     FILES     #
#################
TARGET = attack.out
BASE_FNAMES = make_ipv4 make_tcp subnet_mask time_check
DDOS_FNAMES = syn_flood udp_flood icmp_flood conn_flood get_flood header_buffering body_buffering response_buffering hash_dos
BASE_OBJS = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(BASE_FNAMES)))
DDOS_OBJS = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(DDOS_FNAMES)))
MAIN_C = $(SRC_DIR)/main.c
MAIN_O = $(OBJ_DIR)/main.o

#################
#     RULES     #
#################

ALL = all
RE = re
MKDIR_OBJS = mkdir_objs
MKDIR_LIBS = mkdir_libs
CLEAN = clean
ERASE = erase
FCLEAN = fclean

#################
#  TCP  REPLAY  #
#################

TCP_REPLAY = tcp_replay

.SUFFIXES : .c.o
.PHONY: .c.o $(ALL) $(MKDIR_OBJS) $(CLEAN)

$(ALL): $(TARGET) $(TCP_REPLAY)

$(TARGET): $(MKDIR_OBJS) $(BASE_LIB_A) $(DDOS_LIB_A) $(MAIN_O)
	$(CC) $(MAIN_O) $(LIBOPTS) -l$(DDOS_LIB_NAME) -l$(BASE_LIB_NAME) $(LINKOPTS) -o $(TARGET)

$(BASE_LIB_A): $(MKDIR_LIBS) $(BASE_OBJS)
	ar -rc $(BASE_LIB_A) $(BASE_OBJS)
	ranlib $(BASE_LIB_A)

$(DDOS_LIB_A): $(MKDIR_LIBS) $(DDOS_OBJS)
	ar -rc $(DDOS_LIB_A) $(DDOS_OBJS)
	ranlib $(DDOS_LIB_A)

$(OBJ_DIR)/%.o: $(SRC_DIR)/$(BASE_DIR)/%.c
	$(CC) $(CCOPTS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/$(DDOS_DIR)/%.c
	$(CC) $(CCOPTS) -c -o $@ $<

$(MAIN_O): $(DDOS_LIB_A) $(MAIN_C)
	$(CC) $(CCOPTS) $(MAIN_C) $(LINKOPTS) -o $(MAIN_O)

$(MKDIR_OBJS):
	mkdir -p $(OBJ_DIR)

$(MKDIR_LIBS):
	mkdir -p $(LIB_DIR)

$(RE): $(FCLEAN) $(ALL)

$(ERASE): $(CLEAN)

$(FCLEAN): $(CLEAN)
	rm -f $(TARGET)

$(CLEAN):
	rm -f $(OBJ_DIR)/*.o $(DDOS_LIB_A) $(BASE_LIB_A)

$(TCP_REPLAY):
	rm -f tmp/a.out
	$(CC) $(INCLUDE_OPT) $(LIBOPTS) tmp/pcap_test.c -l$(BASE_LIB_NAME) -lpcap -lm -o tmp/a.out
