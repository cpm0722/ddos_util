#include "header.h"
#include "main.h"

#include "base/make_ipv4.h"
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
int g_num_threads = 1;
int g_num_cores = 1;
int	g_fork_cnt = 0;

int g_pid_list[100];

char g_input[__SIZE_OF_INPUT__];
char *g_tokens[__MAX_TOKEN_NUM__ ];
void (*g_usage_functions[__ATTACK_TYPES__ + 1])(void) = { NULL,
				SynFloodPrintUsage,
				UdpFloodPrintUsage,
				IcmpFloodPrintUsage,
				ConnectionFloodPrintUsage,
				GetFloodPrintUsage,
				HeaderBufferingPrintUsage,
				BodyBufferingPrintUsage,
				ResponseBufferingPrintUsage,
				HashDosPrintUsage
			};
void (*g_main_functions[__ATTACK_TYPES__ + 1])(char *[]) = { NULL,
				SynFloodMain,
				UdpFloodMain,
				IcmpFloodMain,
				ConnFloodMain,
				GetFloodMain,
				HeaderBufferingMain,
				BodyBufferingMain,
				ResponseBufferingMain,
				HashDosMain
			};

void SigintHandler(int signo)
{
	int i;
	for(i=0;i<g_num_cores;i++)
	{
		printf("Cleaning...");
		kill(g_pid_list[i], SIGKILL);
	}

}

void GetInput(void)
{
	printf("$ ");
	void *ptr = fgets(g_input, __SIZE_OF_INPUT__, stdin);
	return;
}

void MakeTokens(void)
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

bool CheckOptions(char *argv[], int argc)
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
				PrintUsage(argv);
				exit(0);
			}
			i++;
		}
	}
	return is_command;
}

AttackType ArgvToTokens(char *argv[], int argc)
{
	int argv_idx;
	int token_idx;
	AttackType type;

	for (argv_idx = 1; argv_idx < argc; argv_idx++) {
		if (*argv[argv_idx] == '-') {
			argv_idx++;
		} else {
			break;
		}
	}
	if (argv_idx == argc) {
		return kNoneType;
	}
	if (!strcmp(argv[argv_idx], "syn") ||
			!strcmp(argv[argv_idx], "kSynFlooding") ||
			!strcmp(argv[argv_idx], "1")) {
		type = kSynFlooding;
	} else if (!strcmp(argv[argv_idx], "udp") ||
					   !strcmp(argv[argv_idx], "kUdpFlooding") ||
					   !strcmp(argv[argv_idx], "2")) {
		type = kUdpFlooding;
	} else if (!strcmp(argv[argv_idx], "icmp") ||
					   !strcmp(argv[argv_idx], "kIcmpFlooding") ||
					   !strcmp(argv[argv_idx], "3")) {
		type = kIcmpFlooding;
	} else if (!strcmp(argv[argv_idx], "conn") ||
					 !strcmp(argv[argv_idx], "kConnectionFlooding") ||
					 !strcmp(argv[argv_idx], "4")) {
		type = kConnectionFlooding;
	} else if (!strcmp(argv[argv_idx], "get") ||
					 !strcmp(argv[argv_idx], "kGetFlooding") ||
					 !strcmp(argv[argv_idx], "5")) {
		type = kGetFlooding;
	} else if (!strcmp(argv[argv_idx], "head") ||
					 !strcmp(argv[argv_idx], "kHeadBuffering") ||
					 !strcmp(argv[argv_idx], "6")) {
		type = kHeadBuffering;
	} else if (!strcmp(argv[argv_idx], "body") ||
					 !strcmp(argv[argv_idx], "kBodyBuffering") ||
					 !strcmp(argv[argv_idx], "7")) {
		type = kBodyBuffering;
	} else if (!strcmp(argv[argv_idx], "resp") ||
					 !strcmp(argv[argv_idx], "kResponseBuffering") ||
					 !strcmp(argv[argv_idx], "8")) {
		type = kResponseBuffering;
	} else if (!strcmp(argv[argv_idx], "hash") ||
					 !strcmp(argv[argv_idx], "kHashDos") ||
					 !strcmp(argv[argv_idx], "9")) {
		type = kHashDos;
	} else {
		type = kNoneType;
	}
	argv_idx++;
	if (argv_idx >= argc) {
		return kNoneType;
	}
	for (token_idx = 0; argv_idx < argc; argv_idx++) {
		if (*argv[argv_idx] == '-') {
			break;
		}
		g_tokens[token_idx++] = argv[argv_idx];
	}
	g_tokens[token_idx] = NULL;
	if (token_idx != 4) {
		return kNoneType;
	}
	return type;
}

AttackType ChooseAttackType(void)
{
	printf("\n"
			"   [Serverless Attacks]\n"
			"1. kSynFlooding flooding\n"
			"2. kUdpFlooding flooding\n"
			"3. kIcmpFlooding flooding\n"
			"\n"
			"   [Server Required Attacks]\n"
			"4. Connection flooding\n"
			"5. Get flooding\n"
			"6. Header buffering\n"
			"7. Body buffering\n"
			"8. Response buffering\n"
			"9. Hash Dos\n"
			"\n");
	GetInput();
	AttackType t = atoi(g_input);
	while (t <= 0 || t > 10) {
		printf("Input should be over 0 AND <=10\n");
		GetInput();
		t = atoi(g_input);
	}
	return t;
}

#define DRAW_HEIGHT 7
#define DRAW_WIDTHB 8
#define DRAW_WIDTHS 6

void PrintLogo(void)
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

void PrintUsage(char *argv[])
{
	printf("Usage: sudo %s [options] [AttackTypes] "
				 "[Src-IP] [Dest-IP] [Dest-Port] [#Request/s] ...\n",
			argv[0]);
	printf("\n");
	printf("\e[1mOPTIONS\e[0m \n"
				 "-r \n"
				 "    RECV Flags.\n"
				 "    It is a flag to choose between correct TCP connection and pseudo TCP connection. Enabling it causes a performance drop.\n"
				 "    Default: 0(Disable)\n"
				 "-t \n"
				 "    Number of threads.\n"
				 "    Default: 1\n"
				 "-c \n"
				 "    Number of CPU-cores(for multi-processing).\n"
				 "    Default: 1\n");
	printf("\n");
	printf("\e[1mATTACK TYPES\e[0m \n"
				 "  [Serverless Attacks]\n"
				 "    syn,  kSynFlooding,   1        kSynFlooding Flooding Attack\n"
				 "    udp,  kUdpFlooding,   2        kUdpFlooding Flooding Attack\n"
				 "    icmp, kIcmpFlooding,  3        kIcmpFlooding Flooding Attack\n"
				 "  [Server Required Attacks]\n"
				 "    conn, kConnectionFlooding,  4        Connection Flooding Attack\n"
				 "    get,  kGetFlooding,   5        kGetFlooding Flooding Attack\n"
				 "    head, kHeadBuffering,  6        Header Buffering Attack\n"
				 "    body, kBodyBuffering,  7        Body Buffering Attack\n"
				 "    resp, kResponseBuffering,  8        Response Buffering Attack\n"
				 "    hash, kHashDos,  9        Hash Dos Attack\n");
	printf("\n");
	printf("\e[1mSOURCE IP ADDRESS\e[0m \n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n"
				 "    Default: 32\n");
	printf("\n");
	printf("\e[1mDESTINATION IP ADDRESS\e[0m \n"
				 "    The format is [IPv4 Address]/[Subnet Masking(Optional)].\n"
				 "    IPv4 Address format is XXX.XXX.XXX.XXX.\n"
				 "    Subnet Masking format is 0 ~ 32 integer. It is optional.\n"
				 "    Default: 32\n");
	printf("\n");
	printf("\e[1mDESTINATION PORT NUMBER\e[0m \n"
				 "    The format is port_start[-port_end(Optional)].\n"
				 "    Each port number takes range from 0 to 65,535 integer.\n");
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
	AttackType type;

	is_command = CheckOptions(argv, argc);
	if (!is_command) {
		PrintLogo();
		type = ChooseAttackType();
		g_usage_functions[type]();
		GetInput();
		MakeTokens();
	} else {
		type = ArgvToTokens(argv, argc);
	}
	if (type == kNoneType) {
		PrintUsage(argv);
		return 0;
	}
	printf("num core : %d\n", g_num_cores);

	signal(SIGINT, SigintHandler);
	int i;
	for(i = 0; i < g_num_cores; i++)
	{
		g_pid_list[i] = fork();
		if(g_pid_list[i] < 0)
		{
			perror("fork");
			abort();
		} else if(g_pid_list[i] == 0)
		{
			PreparePid(g_pid_list, g_num_cores);
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
