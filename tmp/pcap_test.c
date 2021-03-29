#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include <pcap.h>
#include <netinet/in.h>

#define BUFFER_SIZE 8192
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN  6

#define PACKET_NUM_MAXIMUM 1024
#define MODIFY_MAXIMUM 512

struct ip_pair {
	char ip1[16];
	char ip2[16];
};

struct port_pair {
	__u8 port1;
	__u8 port2;
};

struct session {
	char ip_src[16];
	char ip_dst[16];
	__u8 sport;
	__u8 dport;

	//to be used with modification count;
	__u16 modify_id;

};

struct sudo_packet_data {
	__u8 sport;
	__u8 dport;
	char src_ip[16];
	char dest_ip[16];
	char *payload;
	__u16 payload_size;
};

struct pcap_hdr {
	__u32 magic_number;
	__u16 version_major;
	__u16 version_minor;
	__s32 thiszone;
	__u32 sigfigs;
	__u32 snaplen;
	__u32 network;
};

struct pcaprec_hdr {
	__u32 ts_sec;
	__u32 ts_usec;
	__u32 incl_len;
	__u32 orig_len;
};

struct sniff_ethernet {
	u_char ether_dhost[6]; /* Destination host address */
	u_char ether_shost[6]; /* Source host address */
	u_short ether_type; /* IP? ARP? RARP? etc */
};

struct sniff_ip {
	u_char ip_vhl; /* version << 4 | header length >> 2 */
	u_char ip_tos; /* type of service */
	u_short ip_len; /* total length */
	u_short ip_id; /* identification */
	u_short ip_off; /* fragment offset field */
#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
	u_char ip_ttl; /* time to live */
	u_char ip_p; /* protocol */
	u_short ip_sum; /* checksum */
	struct in_addr ip_src, ip_dst; /* source and dest address */
};
#define IP_HL(ip)       (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)        (((ip)->ip_vhl) >> 4)

struct sniff_tcp {
	u_short th_sport; /* source port */
	u_short th_dport; /* destination port */
	tcp_seq th_seq; /* sequence number */
	tcp_seq th_ack; /* acknowledgement number */
	u_char th_offx2; /* data offset, rsvd */
#define TH_OFF(th)  (((th)->th_offx2 & 0xf0) >> 4)
	u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short th_win; /* window */
	u_short th_sum; /* checksum */
	u_short th_urp; /* urgent pointer */
};

//Function prototypes
int ip_pair_match_check(struct ip_pair pair1, struct ip_pair pair2);
int ip_pair_exist_check(struct ip_pair *pairs, struct ip_pair new_pair,
		int count);
int ip_table_check(struct ip_pair *table, char ip[16], int size);

int port_pair_match_check(struct port_pair pair1, struct port_pair pair2);
int port_pair_exist_check(struct port_pair *pairs, struct port_pair new_pair,
		int count);
int port_table_check(struct port_pair *table, __u8 port, int size);
char* get_masked_ip_addr(char *current, __u8 mask);


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

	if(strstr(argv[i],".pcap")!=NULL || strstr(argv[i],".pcapng"))
			filename = argv[i];

			if(!strcmp(argv[i],"-f"))
			{
				manual_mode=0;
				i++;
				modify_filename = argv[i];
			}

		}

		if (manual_mode == 1) {
			char ipA[20];
			char ipB[20];
			__u16 portA;
			__u16 portB;

			while (1) {
				char *ip_listA;
				char *ip_listB;
				short freeA_flag=0;
				short freeB_flag=0;
				__u8 availA=0;
				__u8 availB=0;

				char *slash_pointer;

				printf(
						"MODIFY IP\nIP-IP(0 for exit)\nEnter Orig IP(#.#.#.#/#) New IP(#.#.#.#/#)  :");
				scanf("%s %s", ipA,ipB);
				if (!strcmp(ipA, "0"))
					break;

				if( (slash_pointer=strstr(ipA,"/"))!=NULL)
				{
					freeA_flag=1;
					__u8 subnet = atoi(slash_pointer+1);
					memset(slash_pointer,0x0,4);
					ip_listA = get_masked_ip_addr(ipA,subnet);
					availA = pow(2,(32-subnet))-2;
				}
				else
				{
					freeA_flag=0;
					ip_listA = ipA;
					availA = 1;
				}

				if( (slash_pointer=strstr(ipB,"/"))!=NULL)
				{
					freeB_flag=1;
					__u8 subnet = atoi(slash_pointer+1);
					memset(slash_pointer,0x0,4);
					ip_listB = get_masked_ip_addr(ipB,subnet);
					availB = pow(2,(32-subnet))-2;
				}
				else
				{
					freeB_flag=0;
					ip_listB = ipB;
					availB = 1;
				}

				int a,b;
				for(a=0;a<availA;a++)
					for(b=0;b<availB;b++)
					{
						struct ip_pair tmp;
						strcpy(tmp.ip1,ip_listA + 16 * a);
						strcpy(tmp.ip2,ip_listB + 16 * b);
						if(ip_pair_exist_check(ip_table,tmp,ip_table_c)==-1)
						{
							strcpy(ip_table[ip_table_c].ip1, tmp.ip1);
							strcpy(ip_table[ip_table_c].ip2, tmp.ip2);
							printf("IP registered! %s->%s\n",ip_table[ip_table_c].ip1,ip_table[ip_table_c].ip2);
							ip_table_c++;
						}

					}
				if(freeA_flag)
					free(ip_listA);
				if(freeB_flag)
					free(ip_listB);

			}
			while (1) {
				printf(
						"MODIFY PORT\nPORT-PORT(0 for exit)\nEnter Orig PORT(#) New Port(#)  :");
				scanf("%hu %hu", &portA, &portB);
				if (portA == 0)
					break;


				if (port_table_check(port_table, portA, port_table_c) == -1) {
					port_table[port_table_c].port1 = portA;
					port_table[port_table_c].port2 = portB;
					port_table_c++;
				}

			}
		} else {
			FILE *f;
			f = fopen(modify_filename, "r+");
			char buffer[100];
			/*
			 * FILE in format
			 * port-port
			 * or
			 * ip-ip
			 * or
			 * ip/mask-ip/mask
			 */

			while (fgets(buffer, 100, f)) {

			}
		}
		//pointer to save packet data;
		struct sudo_packet_data packets[PACKET_NUM_MAXIMUM];
		struct sudo_packet_data new_packets[PACKET_NUM_MAXIMUM];

		struct session sessions[PACKET_NUM_MAXIMUM];
		//get file

		//errbuff used like stderr in pcap.h
		char errbuff[PCAP_ERRBUF_SIZE];

		//open .pcap/.pcapng file
		pcap_t *handler = pcap_open_offline(filename, errbuff);

		//to parse header;
		struct pcap_pkthdr *header;

		//actual packet;
		const u_char *packet;

		const char *tcp_payload;

		int packetCount = 0;

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
				exit(1);
			}

			//point to tcp part.
			tcp = (struct sniff_tcp*) (packet + sizeof(struct sniff_ethernet)
					+ size_ip);

			packets[packetCount - 1].sport = ntohs(tcp->th_sport);
			packets[packetCount - 1].dport = ntohs(tcp->th_dport);

			printf("src port: %u dest port: %u \n",
					packets[packetCount - 1].sport,
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
/*
		//make socket for new data send;
		int sock = make_socket(IPPROTO_TCP);
		struct sockaddr_in dest;
		dest.sin_family = AF_INET;

		dest.sin_port = htons(new_dport);

		dest.sin_addr.s_addr = inet_addr("192.168.56.1");

		for (i = 0; i < packetCount; i++) {
			ip = (struct sniff_ip*) (new_packets[i]);
			size_ip = IP_HL(ip) * 4;

			//modify ips
			struct ip_port_pair tmp_pair;
			strcpy(tmp_pair.ip1, packets[i].src_ip);
			strcpy(tmp_pair.ip2, packets[i].dest_ip);

			int loc = ip_pair_exist_check(pairs, tmp_pair, pair_count);
			if (ip_pair_match_check(pairs[loc], tmp_pair) == 1) {
				ip_table[ip_table_check(ip_table, pairs[loc].ip1, table_size)];

				ip->ip_src.s_addr = inet_addr(
						ip_table[ip_table_check(ip_table, pairs[loc].ip1,
								table_size)].ip2);
				ip->ip_dst.s_addr = inet_addr(
						ip_table[ip_table_check(ip_table, pairs[loc].ip2,
								table_size)].ip2);
			} else {
				ip->ip_src.s_addr = inet_addr(
						ip_table[ip_table_check(ip_table, pairs[loc].ip2,
								table_size)].ip2);
				ip->ip_dst.s_addr = inet_addr(
						ip_table[ip_table_check(ip_table, pairs[loc].ip1,
								table_size)].ip2);
			}

			tcp = (struct sniff_tcp*) (packet + +size_ip);

			tcp->th_sport = htons(new_sport);
			tcp->th_dport = htons(new_dport);

			if (sendto(sock, (void*) new_packets[i], new_packets_size[i], 0,
					(struct sockaddr*) &dest, sizeof(dest)) < 0) {
				perror("sendto() error");
				exit(-1);
			}

			//print sent data.
			printf("New Packet # %d\n", i + 1);
			printf("src port: %u dest port: %u \n", ntohs(tcp->th_sport),
					ntohs(tcp->th_dport));

			printf("src address: %s", inet_ntoa(ip->ip_src));
			printf(" dest address: %s \n", inet_ntoa(ip->ip_dst));

			printf("\n");

		}
*/
	}

	int session_exist_check(struct session *session_list, __u16 session_num,
			struct session a_session) {
		int i = 0;

		while (i < session_num) {
			//in right order
			if(session_list[i].sport == a_session.sport)
			if(!strcmp(session_list[i].ip_src,a_session.ip_src))
				if(session_list[i].dport == a_session.dport)
					if(!strcmp(session_list[i].ip_dst,a_session.ip_dst))
						return 1;

					//in reverse order
					if(session_list[i].sport == a_session.dport)
						if(!strcmp(session_list[i].ip_src,a_session.ip_dst))
							if(session_list[i].dport == a_session.sport)
								if(!strcmp(session_list[i].ip_dst,a_session.ip_src))
							return 2;

							i++;

						}

						//no match
						return 0;
					}

					//0 no match, 1 in right order, 2 in reverse order.
					int ip_pair_match_check(struct ip_pair pair1,
							struct ip_pair pair2) {
						//right order check;
						if (!strcmp(pair1.ip1, pair2.ip1))
							if (!strcmp(pair1.ip2, pair2.ip2))
								return 1;
							else
								return 0;

						//reverse order check;
						if (!strcmp(pair1.ip1, pair2.ip2))
							if (!strcmp(pair1.ip2, pair2.ip1))
								return 2;
							else
								return 0;
					}

					//0 not exist -1 , else # = already exists;
					int ip_pair_exist_check(struct ip_pair *pairs,
							struct ip_pair new_pair, int count) {
						int i = 0;
						while (i < count) {

							if (ip_pair_match_check(pairs[i], new_pair) > 0)
								return i;

							i++;
						}
						return -1;
					}
					// returns location, -1 if no
					int ip_table_check(struct ip_pair *table, char ip[16],
							int size) {
						int i = 0;
						while (i < size) {
							if (!strcmp(table[i].ip1, ip))
								return i;

							i++;
						}

						return -1;
					}

					//0 no match, 1 in right order, 2 in reverse order.
					int port_pair_match_check(struct port_pair pair1,
							struct port_pair pair2) {
						//right order check;
						if (pair1.port1 == pair2.port1)
							if (pair1.port2 == pair2.port2)
								return 1;
							else
								return 0;

						//reverse order check;
						if (pair1.port2 == pair2.port1)
							if (pair1.port1 == pair2.port2)
								return 2;
							else
								return 0;
					}

					//0 not exist -1 , else # = already exists;
					int port_pair_exist_check(struct port_pair *pairs,
							struct port_pair new_pair, int count) {
						int i = 0;
						while (i < count) {

							if (port_pair_match_check(pairs[i], new_pair) > 0)
								return i;

							i++;
						}
						return -1;
					}
					// returns location, -1 if no
					int port_table_check(struct port_pair *table, __u8 port,
							int size) {
						int i = 0;
						while (i < size) {
							if (table[i].port1 == port)
								return i;

							i++;
						}

						return -1;
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
								ip_in_bits[i] = ip_in_bits[i]
										& ((1 << mask) - 1);
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

							sprintf(c_buffer, "%d.%d.%d.%d", ips[0], ips[1],
									ips[2], ips[3]);
							memcpy(buffer + 16 * j, c_buffer, 16);
						}

						return buffer;

					}
