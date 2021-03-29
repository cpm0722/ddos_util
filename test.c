
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* get_masked_ip_addr(char *current, unsigned short mask, char* buffer)
{

		unsigned int avail = pow(2,(32-mask))-2;
		buffer = malloc(sizeof(char) * 16 * avail);

		char c_buffer[16];
		unsigned short ips[4];
		int i = 0;

		i = 0;
		char *loc = current+0;

		char *buf = strtok(loc, ".");

		ips[i++] = atoi(buf);
		while ((buf = strtok(NULL, ".")) != NULL) {
			ips[i++] = atoi(buf);
		}


		unsigned char ip_in_bits[4];
		for(i=0;i<4;i++){
			ip_in_bits[i] = ips[i];

			if(mask<8 && mask>0){
				ip_in_bits[i] = ip_in_bits[i] & ((1<<mask)-1);
				mask=0;
			}
			else{
				ip_in_bits[i] = ip_in_bits[i] & ((1<<8)-1);
				mask -= 8;
			}

			ips[i] = ip_in_bits[i];
		}

		ips[3]=0;
		int j;
		for(j=0;j<avail;j++){
		int top = 0;
		unsigned int val_check;
		for (i = 3; i >= 0; i--) {
			if (i != 3 && top == 0)
				break;
			else{
				ips[i]+=top;
				val_check = ips[i]+top;
			}


			top = 0;

			if(i==3){
			val_check = ips[i] + 1;
			ips[i] +=1;
			}

			while (val_check > 255) {
				top ++;
				ips[i] -= 256;
				val_check -= 255;
			}

			val_check = 0;
		}

		sprintf(c_buffer,"%d.%d.%d.%d", ips[0], ips[1], ips[2], ips[3]);
		memcpy(buffer+16*j,c_buffer,16);
		}

		return buffer;

}
int main(void)
{

	char str[] = "192.168.0.1";
	char *t;
	t =get_masked_ip_addr(str,24,NULL);
	int i;
	for(i=0;i<pow(2,8)-2;i++)
	{
		printf("%s\n",t+i*16);
	}
	free(t);
	return 0;

	
}
