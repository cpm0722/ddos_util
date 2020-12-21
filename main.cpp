#include "header.h"

#include "icmp_flood.h"
#include "syn_flood.h"
#include "udp_flood.h"
#include "conn_flood.h"

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
		case 1:
			break; //header buffering
		case 2:
			break; //body buffering
		case 3:
			break; //response buffering
		case 4:
			mode = choose_running_type();
			conn_flood_print_usage(mode);
			get_input();
			make_tokens();
			conn_flood_run(tokens,mode);
			break;
		case 5:
			break; //get flooding
		case 6:
			mode = choose_running_type();
			syn_flood_print_usage(mode);
			get_input();
			make_tokens();
			syn_flood_run(tokens, mode);
			break;
		case 7:
			udp_flood_print_usage();
			get_input();
			make_tokens();
			udp_flood_run(tokens);
			break; //UDP flooding
		case 8:
			mode = choose_running_type();
			icmp_flood_print_usage(mode);
			get_input();
			make_tokens();
			icmp_flood_run(tokens, mode);
			break; //ICMP flooding
		case 9:
			break; //Hash Dos
		case 10:
			break; //Ref Ref

		}

	}
}
