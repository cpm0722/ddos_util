#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "tcp_replay.h"
#include <pcap.h>
#include <netinet/in.h>

#define BUFFER_SIZE 8192
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN  6

#define PACKET_NUM_MAXIMUM 1024
#define MODIFY_MAXIMUM 512

pthread_mutex_t tcp_replay_mutex;

u_char *packet_data[PACKET_NUM_MAXIMUM];
__u16 packet_data_size[PACKET_NUM_MAXIMUM];

char src_ip_addr[16];
char dest_ip_addr[16];
	__u8 dest_port;
int packetCount = 0;

__u16 send_count=1;
__u8 thread_count=1;
__u16 sent_count=0;
int loop_index=-1;

int main(int argc, char *argv[]) {

	int i;

	char *filename;
	char *modify_filename;
	int manual_mode = 1;

	__u16 ip_table_c = 0;
	__u16 port_table_c = 0;

	struct ip_pair ip_table[MODIFY_MAXIMUM];
	struct port_pair port_table[MODIFY_MAXIMUM];



	for (i = 1; i < argc; i++) {

		if(!strcmp(argv[i], "--help"))
		{
			printf("Usage : ./a.out [PCAP_FILE] -sip [SOURCE_ADDR] -dip [DEST_ADDR] -p [DEST_PORT]");
			printf("Options:\n-f [Config-File Name]\n"
					"-dip [Destination IP address]\n"
					"-sip [Source IP address]\n"
					"-p [Dest Port]\n"
					"-c [Send Count]\n"
					"-t [Thread Count]\n");
			exit(1);
		}

		if (strstr(argv[i], ".pcap") != NULL || strstr(argv[i], ".pcapng"))
			filename = argv[i];

		if (!strcmp(argv[i], "-f")) {
			manual_mode = 0;
			i++;
			modify_filename = argv[i];
		}

		if (!strcmp(argv[i], "-dip")) {
			i++;
			strcpy(dest_ip_addr, argv[i]);
		}

		if (!strcmp(argv[i], "-sip")) {
			i++;
			strcpy(src_ip_addr, argv[i]);
		}

		if (!strcmp(argv[i], "-p")) {
			i++;
			dest_port = atoi(argv[i]);
		}

		if(!strcmp(argv[i],"-c"))
		{
			i++;
			send_count = atoi(argv[i]);
		}

		if(!strcmp(argv[i],"-t"))
		{
			i++;
			thread_count = atoi(argv[i]);
		}

	}
	char ipA[20];
	char ipB[20];
	__u16 portA;
	__u16 portB;

	if (manual_mode == 1) {

		while (1) {
			char *ip_listA;
			char *ip_listB;
			short freeA_flag = 0;
			short freeB_flag = 0;
			__u8 availA = 0;
			__u8 availB = 0;

			char *slash_pointer;

			printf(
					"MODIFY IP\nIP-IP(0 for exit)\nEnter Orig IP(#.#.#.#/#) New IP(#.#.#.#/#)  :");
			if(scanf("%s %s", ipA, ipB)!=2)
			{
				printf("Wrong input.\n");
				continue;
			}
			if (!strcmp(ipA, "0"))
				break;

			if ((slash_pointer = strstr(ipA, "/")) != NULL) {
				freeA_flag = 1;
				__u8 subnet = atoi(slash_pointer + 1);
				memset(slash_pointer, 0x0, 4);
				ip_listA = get_masked_ip_addr(ipA, subnet);
				availA = pow(2, (32 - subnet)) - 2;
			} else {
				freeA_flag = 0;
				ip_listA = ipA;
				availA = 1;
			}

			if ((slash_pointer = strstr(ipB, "/")) != NULL) {
				freeB_flag = 1;
				__u8 subnet = atoi(slash_pointer + 1);
				memset(slash_pointer, 0x0, 4);
				ip_listB = get_masked_ip_addr(ipB, subnet);
				availB = pow(2, (32 - subnet)) - 2;
			} else {
				freeB_flag = 0;
				ip_listB = ipB;
				availB = 1;
			}

			int a, b;
			for (a = 0; a < availA; a++)
				for (b = 0; b < availB; b++) {
					struct ip_pair tmp;
					strcpy(tmp.ip1, ip_listA + 16 * a);
					strcpy(tmp.ip2, ip_listB + 16 * b);
					if (ip_pair_exist_check(ip_table, tmp, ip_table_c) == -1) {
						strcpy(ip_table[ip_table_c].ip1, tmp.ip1);
						strcpy(ip_table[ip_table_c].ip2, tmp.ip2);
						printf("IP registered! %s->%s\n",
								ip_table[ip_table_c].ip1,
								ip_table[ip_table_c].ip2);
						ip_table_c++;
					}

				}
			if (freeA_flag)
				free(ip_listA);
			if (freeB_flag)
				free(ip_listB);

		}
		while (1) {
			printf(
					"MODIFY PORT\nPORT-PORT(0 for exit)\nEnter Orig PORT(#) New Port(#)  :");
			if(scanf("%hu %hu", &portA, &portB)!=2)
			{
				printf("Wrong input.\n");
				continue;
			}

			if (portA == 0)
				break;

			if (port_table_check(port_table, portA, port_table_c) == -1) {
				port_table[port_table_c].port1 = portA;
				port_table[port_table_c].port2 = portB;
				port_table_c++;
			}

		}
	} else {
		/*
		 * FILE in format
		 * %c %s/%hu %s/%hu
		 */
		FILE *fp;
		fp = fopen(modify_filename, "r+");
		if (fp == NULL)
			perror("File open err\n");

		while (1) {
			char *token;
			char *line = NULL;
			size_t size = 0;

			if (getline(&line, &size, fp) != -1) {
				if (line[0] == 'I') {

					token = strtok(line, "#");

					char *ip_listA = NULL;
					char *ip_listB = NULL;
					short freeA_flag = 0;
					short freeB_flag = 0;
					__u8 availA = 0;
					__u8 availB = 0;

					char *slash_pointer;

					token = strtok(NULL, "#");
					strcpy(ipA, token);

					token = strtok(NULL, "#");
					strcpy(ipB, token);

					if ((slash_pointer = strstr(ipA, "/")) != NULL) {
						freeA_flag = 1;
						__u8 subnet = atoi(slash_pointer + 1);
						memset(slash_pointer, 0x0, 4);
						ip_listA = get_masked_ip_addr(ipA, subnet);
						availA = pow(2, (32 - subnet)) - 2;
					} else {
						freeA_flag = 0;
						ip_listA = ipA;
						availA = 1;
					}

					if ((slash_pointer = strstr(ipB, "/")) != NULL) {
						freeB_flag = 1;
						__u8 subnet = atoi(slash_pointer + 1);
						memset(slash_pointer, 0x0, 4);
						ip_listB = get_masked_ip_addr(ipB, subnet);
						availB = pow(2, (32 - subnet)) - 2;
					} else {
						freeB_flag = 0;
						ip_listB = ipB;
						availB = 1;
					}

					int a, b;
					for (a = 0; a < availA; a++)
						for (b = 0; b < availB; b++) {
							struct ip_pair tmp;
							strcpy(tmp.ip1, ip_listA + 16 * a);
							strcpy(tmp.ip2, ip_listB + 16 * b);

							if (ip_pair_exist_check(ip_table, tmp, ip_table_c)
									== -1) {
								strcpy(ip_table[ip_table_c].ip1, tmp.ip1);
								strcpy(ip_table[ip_table_c].ip2, tmp.ip2);
								printf("IP registered! %s->%s\n",
										ip_table[ip_table_c].ip1,
										ip_table[ip_table_c].ip2);
								ip_table_c++;
							}

						}
					if (freeA_flag)
						free(ip_listA);
					if (freeB_flag)
						free(ip_listB);

				} else if (line[0] == 'P') {

					token = strtok(line, "#");

					token = strtok(NULL, "#");
					portA = atoi(token);

					token = strtok(NULL, "#");
					portB = atoi(token);

					if (port_table_check(port_table, portA, port_table_c)
							== -1) {
						port_table[port_table_c].port1 = portA;
						port_table[port_table_c].port2 = portB;
						printf("Port Registered! %hu -> %hu\n", portA, portB);
						port_table_c++;
					}

				}
			} else {
				fclose(fp);
				break;
			}
			free(line);
		}

	}
	//pointer to save packet data;
	struct sudo_packet_data packets[PACKET_NUM_MAXIMUM];

	//packets to corresponding session
	__u8 packet_to_session[PACKET_NUM_MAXIMUM];
	unsigned char packet_to_session_reverse_bit[PACKET_NUM_MAXIMUM];

	__u16 sessions_c = 0;
	struct session sessions[MODIFY_MAXIMUM];

	//errbuff used like stderr in pcap.h
	char errbuff[PCAP_ERRBUF_SIZE];

	//open .pcap/.pcapng file
	pcap_t *handler = pcap_open_offline(filename, errbuff);
	if (handler == NULL)
		perror("handler failed");
	//to parse header;
	struct pcap_pkthdr *header;

	//actual packet;
	const u_char *packet;

	const char *tcp_payload;

	//to save result...
	//FILE *fp = fopen("result.txt", "wb+");

	//actually sniff ethernet, ip, tcp part.
	struct sniff_ethernet *ethernet;
	struct sniff_ip *ip;
	struct sniff_tcp *tcp;

	//size ip, tcp.
	u_int size_ip;
	u_int size_tcp;

	//input test

	i = 0;
	printf("--- Packet Parse Start ---\n");
	//get next pcap(packet).
	while (pcap_next_ex(handler, &header, &packet) >= 0) {

		printf("Packet # %i\n", ++packetCount);
		printf("Packet size : %d bytes\n", header->len);

		//warning when capture size != packet size
		if (header->len != header->caplen)
			printf("Warning! Capture size != packet size\n");



		//point to packet(ethernet = beginning).
		ethernet = (struct sniff_ethernet*) (packet);

		//poitn to packet ip part.
		ip = (struct sniff_ip*) (packet + sizeof(struct sniff_ethernet));
		size_ip = IP_HL(ip) * 4;

		if (size_ip < 20) {
			printf("   * Invalid IP header length : %u bytes\n", size_ip);
			packetCount--;
			continue;
		}

		//prepare data
		packet_data[packetCount - 1] = malloc(
				header->len - sizeof(struct sniff_ethernet));
		memcpy(packet_data[packetCount - 1],
				packet + sizeof(struct sniff_ethernet),
				header->len - sizeof(struct sniff_ethernet));
		packet_data_size[packetCount - 1] = header->len
				- sizeof(struct sniff_ethernet);

		//point to tcp part.
		tcp = (struct sniff_tcp*) (packet + sizeof(struct sniff_ethernet)
				+ size_ip);

		packets[packetCount - 1].sport = ntohs(tcp->th_sport);
		packets[packetCount - 1].dport = ntohs(tcp->th_dport);


		printf("src port: %hu dest port: %hu \n", packets[packetCount - 1].sport,
				packets[packetCount - 1].dport);

		strcpy(packets[packetCount - 1].src_ip, inet_ntoa(ip->ip_src));
		strcpy(packets[packetCount - 1].dest_ip, inet_ntoa(ip->ip_dst));

		//pair check;

		printf("src address: %s", packets[packetCount - 1].src_ip);
		printf(" dest address: %s \n", packets[packetCount - 1].dest_ip);

		tcp_payload = (char*) (packet
				+ (sizeof(struct sniff_ethernet) + size_ip
						+ sizeof(struct sniff_tcp)));

		packets[packetCount - 1].payload_size = header->len
				- (sizeof(struct sniff_ethernet) + size_ip
						+ sizeof(struct sniff_tcp));

		packets[packetCount - 1].payload = (char*) malloc(
				1 * packets[packetCount - 1].payload_size);

		memcpy(packets[packetCount - 1].payload, tcp_payload,
				packets[packetCount - 1].payload_size);

		int i;
		printf("data :\n");
		for (i = 0; i < packets[packetCount - 1].payload_size; i++)
			printf("%x ", packets[packetCount - 1].payload[i] & 0xff);

		printf("\n");
		printf("\n");
	}

	//session counting
	for (i = 0; i < packetCount; i++) {
		//session creation(packet -> temporal session)
		struct session next_session;
		strcpy(next_session.ip_src, packets[i].src_ip);
		strcpy(next_session.ip_dst, packets[i].dest_ip);
		next_session.sport = packets[i].sport;
		next_session.dport = packets[i].dport;


		short reverse_check;
		if ((reverse_check = session_exist_check(sessions, sessions_c,
				next_session)) < 1) {
			packet_to_session[i] = sessions_c;
			packet_to_session_reverse_bit[i] = 0;
			sessions[sessions_c] = next_session;
			sessions_c++;
		} else {

			packet_to_session[i] = session_table_check(sessions, sessions_c,
					next_session);

			//right order =1, reverse= 2;
			packet_to_session_reverse_bit[i] = reverse_check - 1;
		}
		printf("src_ip:%s\ndst_ip:%s\nsrc_port:%d\ndst_port:%d",next_session.ip_src,next_session.ip_dst,next_session.sport,next_session.dport);
		printf("\nrev_check:%d\npacket_to_session:%d\n\n",reverse_check,packet_to_session[i]);
	}

	printf("# Total Sessions = %d\n", sessions_c);

	//session manipulation
	for (i = 0; i < sessions_c; i++) {
		short index = 0;
		if ((index = ip_table_check(ip_table, sessions[i].ip_src, ip_table_c))
				!= -1) {
			strcpy(sessions[i].ip_src, ip_table[index].ip2);
			ip_table_element_to_end(ip_table, index, ip_table_c);
		}

		if ((index = ip_table_check(ip_table, sessions[i].ip_dst, ip_table_c))
				!= -1) {
			strcpy(sessions[i].ip_dst, ip_table[index].ip2);
			ip_table_element_to_end(ip_table, index, ip_table_c);
		}

		if ((index = port_table_check(port_table, sessions[i].sport,
				port_table_c)) != -1) {
			sessions[i].sport = port_table[index].port2;
			port_table_element_to_end(port_table, index, port_table_c);
		}
		if ((index = port_table_check(port_table, sessions[i].dport,
				port_table_c)) != -1) {
			sessions[i].dport = port_table[index].port2;
			port_table_element_to_end(port_table, index, port_table_c);
		}
	}



	for (i = 0; i < packetCount; i++) {
		ip = (struct sniff_ip*) (packet_data[i]);
		size_ip = IP_HL(ip) * 4;

		//modify ips

		if (!packet_to_session_reverse_bit[i]) {
			ip->ip_src.s_addr = inet_addr(
					sessions[packet_to_session[i]].ip_src);
			ip->ip_dst.s_addr = inet_addr(
					sessions[packet_to_session[i]].ip_dst);
		} else {
			ip->ip_src.s_addr = inet_addr(
					sessions[packet_to_session[i]].ip_dst);
			ip->ip_dst.s_addr = inet_addr(
					sessions[packet_to_session[i]].ip_src);
		}

		tcp = (struct sniff_tcp*) (packet_data[i] + size_ip);

		if (!packet_to_session_reverse_bit[i]) {
			tcp->th_sport = htons(sessions[packet_to_session[i]].sport);
			tcp->th_dport = htons(sessions[packet_to_session[i]].dport);
		} else {
			tcp->th_sport = htons(sessions[packet_to_session[i]].dport);
			tcp->th_dport = htons(sessions[packet_to_session[i]].sport);
		}

			/*struct iphdr ipv4_h;
			ipv4_h = prepare_empty_ipv4();
			ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
			ipv4_h = ipv4_set_saddr(ipv4_h,src_ip_addr);
			ipv4_h = ipv4_set_daddr(ipv4_h,dest_ip_addr);
			ipv4_h = ipv4_add_size(ipv4_h,sizeof(struct tcphdr) + packet_data_size[i]+packets[i].payload_size));
*/

			tcp->th_sum = tcp_get_checksum_for_tcp_replay(tcp,packets[i].payload,packets[i].payload_size);

		//print sent data.
		printf("New Packet # %d Session # %d\n", i + 1,packet_to_session[i]);
		printf("src port: %u dest port: %u \n", ntohs(tcp->th_sport),
				ntohs(tcp->th_dport));

		printf("src address: %s", inet_ntoa(ip->ip_src));
		printf(" dest address: %s \n", inet_ntoa(ip->ip_dst));

		printf("payload size : %d\n",packets[i].payload_size);
		int kl=0;
		for(kl=0;kl<packets[i].payload_size;kl++)
			printf("%x ",packets[i].payload[kl] & 0xff);
		printf("\n");
		printf("\n");

	}


	pthread_mutex_init(&tcp_replay_mutex,NULL);
	pthread_t *threads;

	threads = malloc(sizeof(pthread_t) * thread_count);

	for(i=0;i<thread_count;i++)
	{

		pthread_create(&threads[i],NULL,tcpreplay_thread,NULL);
	}

	for(i=0;i<thread_count;i++)
	{
		pthread_join(threads[i],NULL);
		printf("Thread %d finished.\n",i);
	}
	  pthread_mutex_destroy(&tcp_replay_mutex);
	pthread_exit(NULL);
	free(threads);
	for(i=0;i<packetCount;i++)
	{
		free(packet_data[i]);
		free(packets[i].payload);
	}


}

int session_match_check(struct session a_session, struct session b_session)
{

	if (b_session.sport == a_session.sport) {
		if (!strcmp(b_session.ip_src, a_session.ip_src)) {
			if (b_session.dport == a_session.dport) {
				if (!strcmp(b_session.ip_dst, a_session.ip_dst)) {
					return 1;
				}
			}
		}
	} else { //in reverse order
		if (b_session.sport == a_session.dport) {
			if (!strcmp(b_session.ip_src, a_session.ip_dst)) {
				if (b_session.dport == a_session.sport) {
					if (!strcmp(b_session.ip_dst, a_session.ip_src)) {
						return 2;
					}
				}
			}
		}
	}
	return 0;
}

int session_exist_check(struct session *session_list, __u16 session_num,
		struct session a_session) {
	int i = 0;

	int r;

	while (i < session_num) {
		//in right order

			if( (r=session_match_check(session_list[i],a_session))>0)
				return r;
		i++;
	}

	//no match
	return 0;
}
int session_table_check(struct session *session_list, __u16 session_num,
		struct session a_session) {
	int i = 0;
	while (i < session_num) {
		if (session_match_check(session_list[i], a_session) > 0)
			return i;
		i++;
	}
	return -1;

}

//0 no match, 1 in right order, 2 in reverse order.
int ip_pair_match_check(struct ip_pair pair1, struct ip_pair pair2) {
	//right order check;
	if (!strcmp(pair1.ip1, pair2.ip1)) {
		if (!strcmp(pair1.ip2, pair2.ip2))
			return 1;
		else
			return 0;
	} else //reverse order check;
	{
		if (!strcmp(pair1.ip1, pair2.ip2))
			if (!strcmp(pair1.ip2, pair2.ip1))
				return 2;
			else
				return 0;
	}
	return 0;
}

// not exist -1 , else # = already exists;
int ip_pair_exist_check(struct ip_pair *pairs, struct ip_pair new_pair,
		int count) {
	int i = 0;
	while (i < count) {
		if (ip_pair_match_check(pairs[i], new_pair) > 0)
			return i;

		i++;
	}
	return -1;
}
// returns location, -1 if no
int ip_table_check(struct ip_pair *table, char ip[16], int size) {
	int i = 0;
	while (i < size) {
		if (!strcmp(table[i].ip1, ip))
			return i;

		i++;
	}

	return -1;
}

struct ip_pair* ip_table_element_to_end(struct ip_pair *table_p, int index,
		int size) {
	char tmp_ip1[16];
	char tmp_ip2[16];
	strcpy(tmp_ip1, table_p[index].ip1);
	strcpy(tmp_ip2, table_p[index].ip2);

	strcpy(table_p[index].ip1, table_p[size - 1].ip1);
	strcpy(table_p[index].ip2, table_p[size - 1].ip2);

	strcpy(table_p[size - 1].ip1, tmp_ip1);
	strcpy(table_p[size - 1].ip2, tmp_ip2);

	return table_p;

}

//0 no match, 1 in right order, 2 in reverse order.
int port_pair_match_check(struct port_pair pair1, struct port_pair pair2) {
	//right order check;
	if (pair1.port1 == pair2.port1) {
		if (pair1.port2 == pair2.port2)
			return 1;
		else
			return 0;
	} else { //reverse order check;
		if (pair1.port2 == pair2.port1)
			if (pair1.port1 == pair2.port2)
				return 2;
			else
				return 0;
	}
	return 0;
}

//0 not exist -1 , else # = already exists;
int port_pair_exist_check(struct port_pair *pairs, struct port_pair new_pair,
		int count) {
	int i = 0;
	while (i < count) {

		if (port_pair_match_check(pairs[i], new_pair) > 0)
			return i;

		i++;
	}
	return -1;
}
// returns location, -1 if no
int port_table_check(struct port_pair *table, __u8 port, int size) {
	int i = 0;
	while (i < size) {
		if (table[i].port1 == port)
			return i;

		i++;
	}

	return -1;
}

struct port_pair* port_table_element_to_end(struct port_pair *table_p,
		int index, int size) {
	__u8 tmp_port1;
	__u8 tmp_port2;

	tmp_port1 = table_p[index].port1;
	tmp_port1 = table_p[index].port2;

	table_p[index].port1 = table_p[size - 1].port1;
	table_p[index].port2 = table_p[size - 1].port2;

	table_p[size - 1].port1 = tmp_port1;
	table_p[size - 1].port2 = tmp_port2;

	return table_p;

}
/*
 * returns available IPs in subnetmask.
 * avail = pow(2,(32-mask)) -2 ;
 */
char* get_masked_ip_addr(char *current, __u8 mask) {

	unsigned int avail = pow(2, (32 - mask)) - 2;
	char *buffer = malloc(sizeof(char) * 16 * avail);

	char c_buffer[16];
	__u8 ips[4];
	int i = 0;

	i = 0;
	char *loc = current + 0;

	char *buf = strtok(loc, ".");

	ips[i++] = atoi(buf);
	while ((buf = strtok(NULL, ".")) != NULL) {
		ips[i++] = atoi(buf);
	}

	unsigned char ip_in_bits[4];
	for (i = 0; i < 4; i++) {
		ip_in_bits[i] = ips[i];

		if (mask < 8 && mask > 0) {
			ip_in_bits[i] = ip_in_bits[i] & ((1 << mask) - 1);
			mask = 0;
		} else {
			ip_in_bits[i] = ip_in_bits[i] & ((1 << 8) - 1);
			mask -= 8;
		}

		ips[i] = ip_in_bits[i];
	}

	ips[3] = 0;
	int j;
	for (j = 0; j < avail; j++) {
		int top = 0;
		unsigned int val_check;
		for (i = 3; i >= 0; i--) {
			if (i != 3 && top == 0)
				break;
			else {
				ips[i] += top;
				val_check = ips[i] + top;
			}

			top = 0;

			if (i == 3) {
				val_check = ips[i] + 1;
				ips[i] += 1;
			}

			while (val_check > 255) {
				top++;
				ips[i] -= 256;
				val_check -= 255;
			}

			val_check = 0;
		}

		sprintf(c_buffer, "%d.%d.%d.%d", ips[0], ips[1], ips[2], ips[3]);
		memcpy(buffer + 16 * j, c_buffer, 16);
	}

	return buffer;

}

u_short tcp_get_checksum_for_tcp_replay(
															 struct  sniff_tcp *snifftcp,
															 char *data,
															 int datasize)
{
	struct tcp_pseudo_header psh;
	memset(&psh, 0, sizeof(struct tcp_pseudo_header));
	psh.source_address = inet_addr(src_ip_addr);
	psh.dest_address = inet_addr(dest_ip_addr);
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + datasize);
	int psize = sizeof(struct tcp_pseudo_header) + sizeof(struct tcphdr) + datasize;
	char *assembled = (char *) malloc(psize);
	memcpy(assembled, (char *) &psh, sizeof(struct tcp_pseudo_header));

	struct tcphdr tcph;

	memcpy(&tcph, snifftcp, sizeof(struct sniff_tcp));

	memcpy(assembled + sizeof(struct tcp_pseudo_header), &tcph, sizeof(struct tcphdr));
	if (data != NULL && datasize != 0)
	{
		printf("Copying data... cksum.\n");
		memcpy(assembled + sizeof(struct tcp_pseudo_header) + sizeof(struct tcphdr), data, datasize);
	}


	u_short ck_sum = in_cksum((__u16*) assembled, psize);
	free(assembled);
	return ck_sum;


}


void *tcpreplay_thread()
{
	printf("Thread init\n");
	//make socket for new data send;
		int sock = make_socket(IPPROTO_TCP);
		struct sockaddr_in dest;
		dest.sin_family = AF_INET;

		dest.sin_port = htons(dest_port);

		dest.sin_addr.s_addr = inet_addr(dest_ip_addr);

	while (1) {

		// *** begin of critical section ***
		pthread_mutex_lock(&tcp_replay_mutex);
		if(loop_index >= packetCount-1){
					loop_index=-1;
					sent_count++;
				}
		loop_index++;

		if(send_count<=sent_count)
		{
			printf("exit..\n");
			pthread_mutex_unlock(&tcp_replay_mutex);
			return NULL;
		}

		printf("Goal : %d, Now : %d, SessionMax : %d, loop_index : %d\n",send_count,sent_count,packetCount,loop_index);


		if (sendto(sock, (void*) packet_data[loop_index], packet_data_size[loop_index], 0,
					(struct sockaddr*) &dest, sizeof(dest)) < 0) {
				printf("ERROR!\n");
				exit(-1);
			}
		pthread_mutex_unlock(&tcp_replay_mutex);
	}
	close(sock);
	return NULL;
}
