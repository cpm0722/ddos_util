#include "header.h"

void next_ip_addr(char *current, __u32 offset)
{
	char *loc = current + 0;
	__u8 ips[4];

	int i = 0;

	i = 0;
	char *buf = strtok(loc, ".");
	ips[i++] = atoi(buf);
	while ((buf = strtok(NULL, ".")) != NULL) {
		ips[i++] = atoi(buf);
	}

	int top = 0;
	__u16 val_check;
	for (i = 3; i >= 0; i--) {
		if (i != 3 && top == 0)
			break;
		else{
			ips[i]+=top;
			val_check = ips[i]+top;
		}


		top = 0;

		if(i==3){
		val_check = ips[i] + offset;
		ips[i] += offset;
		}

		while (val_check > 255) {
			top ++;
			val_check -= 255;

		}

		while (val_check < 0) {
			top --;
			val_check += 255;
		}
		val_check = 0;
	}
	char ips_c[16];

	sprintf(ips_c, "%d.%d.%d.%d", ips[0], ips[1], ips[2], ips[3]);
	strcpy(current, ips_c);

}
