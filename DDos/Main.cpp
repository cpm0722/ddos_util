#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>

#include "icmp_flood.h"

#define __SIZE_OF_INPUT__ 100
#define __MAX_TOKEN_NUM__ 10

char input[__SIZE_OF_INPUT__];
char *tokens[__MAX_TOKEN_NUM__];

void get_input() {
	printf("$ ");
	fgets(input, __SIZE_OF_INPUT__, stdin);

}
void make_tokens() {
	int i=0;
	for(i=0;i<__MAX_TOKEN_NUM__;i++)
		tokens[i]=0;
	i=0;

	tokens[i] = strtok(input, " ");
	while (tokens[i] != NULL) {
		i++;
		tokens[i] = strtok(NULL, " ");
	}

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

	while (1) {
		switch (type_choose_menu()) {
		case 1:
			break; //header buffering
		case 2:
			break; //body buffering
		case 3:
			break; //response buffering
		case 4:
			break; //connection flooding
		case 5:
			break; //get flooding
		case 6:
			break; //SYN flooding
		case 7:
			break; //UDP flooding
		case 8:
			icmp_flood_print_usage();
			get_input();
			make_tokens();
			icmp_flood_run(tokens);
			break; //ICMP flooding
		case 9:
			break; //Hash Dos
		case 10:
			break; //Ref Ref

		}

	}
}
