#include "../src/header.h"
#include "../src/base/make_ipv4.h"
#include "../src/base/make_tcp.h"
#include <pcap.h>
#include <netinet/in.h>

#define BUFFER_SIZE 8192
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN  6
#define PACKET_NUM_MAXIMUM 1024

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

int main(int argc, char *argv[]) {

	printf("--- Packet Parse Start ---\n");

	//pointer to save packet data;
	struct sudo_packet_data packets[PACKET_NUM_MAXIMUM];

	//get file
	char *filename = argv[1];

	//errbuff used like stderr in pcap.h
	char errbuff[PCAP_ERRBUF_SIZE];

	//open .pcap/.pcapng file
	pcap_t *handler = pcap_open_offline(filename, errbuff);

	//to parse header;
	struct pcap_pkthdr *header;

	//actual packet;
	const u_char *packet;
	u_char *new_packets[PACKET_NUM_MAXIMUM];
	int new_packets_size[PACKET_NUM_MAXIMUM];

	const char *tcp_payload;

	int packetCount = 0;
	int i;

	//to save result...
	FILE *fp = fopen("result.txt", "wb+");

	//actually sniff ethernet, ip, tcp part.
	struct sniff_ethernet *ethernet;
	struct sniff_ip *ip;
	struct sniff_tcp *tcp;

	//size ip, tcp.
	u_int size_ip;
	u_int size_tcp;

	//to be modified to below IPs and ports;
	char new_src_ip[16];
	strcpy(new_src_ip, "192.168.56.3");
	char new_dst_ip[16];
	strcpy(new_dst_ip, "192.168.56.1");
	int new_sport = 12345;
	int new_dport = 55555;

	//get next pcap(packet).
	while (pcap_next_ex(handler, &header, &packet) >= 0) {
		printf("Packet # %i\n", ++packetCount);
		printf("Packet size : %d bytes\n", header->len);

		//warning when capture size != packet size
		if (header->len != header->caplen)
			printf("Warning! Capture size != packet size\n");

		new_packets[packetCount - 1] = malloc(
				header->len - sizeof(struct sniff_ethernet));
		memcpy(new_packets[packetCount - 1],
				packet + sizeof(struct sniff_ethernet),
				header->len - sizeof(struct sniff_ethernet));
		new_packets_size[packetCount - 1] = header->len
				- sizeof(struct sniff_ethernet);

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

		printf("src port: %u dest port: %u \n", packets[packetCount - 1].sport,
				packets[packetCount - 1].dport);

		strcpy(packets[packetCount - 1].src_ip, inet_ntoa(ip->ip_src));
		strcpy(packets[packetCount - 1].dest_ip, inet_ntoa(ip->ip_dst));

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

	//make socket for new data send;
	int sock = make_socket(IPPROTO_TCP);
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;

	dest.sin_port = htons(new_dport);

	dest.sin_addr.s_addr = inet_addr(new_dst_ip);

	for (i = 0; i < packetCount; i++) {
		ip = (struct sniff_ip*) (new_packets[i]);
		size_ip = IP_HL(ip) * 4;
		ip->ip_src.s_addr = inet_addr(new_src_ip);
		ip->ip_dst.s_addr = inet_addr(new_dst_ip);

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

}
