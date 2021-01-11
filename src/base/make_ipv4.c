#include "../header.h"
#include "../base/make_ipv4.h"

struct iphdr prepare_empty_ipv4() {

	struct iphdr ip_head;
	ip_head.ihl = 5;
	ip_head.version = 4;
	ip_head.tos = 0;
	ip_head.tot_len = sizeof(struct iphdr);
	ip_head.id = htons(31337 + (rand() % 100));
	ip_head.frag_off = 0;
	ip_head.ttl = 255;
	ip_head.protocol = IPPROTO_TCP;
	ip_head.check = 0;
	ip_head.saddr = 0;
	ip_head.daddr = 0;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));

	return ip_head;
}

struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol) {
	ip_head.protocol = protocol;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}

struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr) {
	ip_head.saddr = saddr;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}

struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr) {
	ip_head.daddr = daddr;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}
struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size) {
	ip_head.tot_len += data_size;

	ip_head.check = in_cksum((unsigned short*) &ip_head,
			sizeof(struct iphdr) + data_size);
	return ip_head;
}

char* packet_assemble(struct iphdr ip_head, void *data, __u32 data_size) {
	char *packet = (char*) malloc(sizeof(ip_head) + data_size);

	memcpy(packet, (char*) &ip_head, sizeof(ip_head));
	memcpy((char*) packet + sizeof(ip_head), (char*) data, data_size);
	return packet;
}




int make_socket(int PROTOCOL) {
	int sock;
	sock = socket(PF_INET, SOCK_RAW, PROTOCOL);
	if (sock < 0) {
		perror("socket() error");
		exit(-1);
	}

	int one = 1;

	const int *val = &one;
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
		perror("setsockopt() error");
		exit(-1);
	}

	return sock;
}

void send_packet(int sock, struct iphdr ip_head, char *packet, int port) {
	struct sockaddr_in dest;

	dest.sin_family = AF_INET;

	dest.sin_port = htons(port);

	dest.sin_addr.s_addr = ip_head.daddr;

	//printf("Message length = %d\n", ip_head.tot_len);
	if (sendto(sock, (void*) packet, ip_head.tot_len, 0,
			(struct sockaddr*) &dest, sizeof(dest)) < 0) {
		perror("sendto() error");
		exit(-1);
	}
	//printf(" sendto() is OK\n");
}

__u16 in_cksum(unsigned short *ptr, int nbytes) {

	register long sum;
	u_short oddbyte;
	register u_short answer;
	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}

	if (nbytes == 1) {
		oddbyte = 0;
		*((u_char*) &oddbyte) = *(u_char*) ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

void next_ip_addr(char *current, __u32 offset) {
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



