#include "../src/header.h"
#include <pcap.h>
#include <netinet/in.h>

#define BUFFER_SIZE 8192
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN  6

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

	//get file
	char *filename= argv[1];

	//errbuff used like stderr in pcap.h
	char errbuff[PCAP_ERRBUF_SIZE];

	//open .pcap/.pcapng file
	pcap_t * handler = pcap_open_offline(filename, errbuff);


	//to parse header;
	struct pcap_pkthdr *header;


	//actual packet;
	const u_char *packet;

	const char *tcp_payload;


	int packetCount = 0;
	int i;

	//to save result...
	FILE *fp = fopen( "result.txt","wb+");

	//actually sniff ethernet, ip, tcp part.
	const struct sniff_ethernet *ethernet;
	const struct sniff_ip *ip;
	const struct sniff_tcp *tcp;

	//size ip, tcp.
	u_int size_ip;
	u_int size_tcp;

	//get next pcap(packet).
	while(pcap_next_ex(handler, &header, &packet) >= 0)
	{
		printf("Packet # %i\n", ++packetCount);
		printf("Packet size : %d bytes\n",header->len);

			//warning when capture size != packet size
		if(header->len != header->caplen)
			printf("Warning! Capture size != packet size\n");

		//printf("Epoch Time: %ld:%ld seconds\n",header->ts.tv_sec, header->ts.tv_usec);

		//point to packet(ethernet = beginning).
		ethernet = (struct sniff_ethernet*)(packet);

		//poitn to packet ip part.
		ip = (struct sniff_ip *)(packet + 	sizeof(struct sniff_ethernet));
		size_ip = IP_HL(ip)*4;

		if(size_ip < 20){
			printf("   * Invalid IP header length : %u bytes\n", size_ip);
			exit(1);
		}

		//point to tcp part.
		tcp = (struct sniff_tcp*)(packet + 	sizeof(struct sniff_ethernet) + size_ip);

		printf("src port: %u dest port: %u \n", ntohs(tcp->th_sport), ntohs(tcp->th_dport));

		printf("src address: %s",  inet_ntoa(ip->ip_src));
		printf(" dest address: %s \n",  inet_ntoa(ip->ip_dst));

		/*
		tcp_payload = (char *)(packet + sizeof(struct sniff_ethernet) + size_ip + sizeof(struct sniff_tcp));

		int i;
		printf("data :\n");
		for(i=sizeof(struct sniff_ethernet) + size_ip + sizeof(struct sniff_tcp) ; i< (header->len) ;i++)
			printf("%c",tcp_payload[i]);
*/
		printf("\n");
		printf("\n");
	}
	/*
	FILE *fp;

	int p = 0 ;
	//error buffer
	char buffer[BUFFER_SIZE];

	fp = fopen(filename, "rb");
	if(fp==NULL)
		perror("no such file exists\n");

	fread(buffer,BUFFER_SIZE,1,fp);

	struct pcap_hdr pcap_header;
	struct pcaprec_hdr record_header;
	struct sniff_ip ip;
	struct sniff_tcp tcp;


	printf("size of pcap_hdr = %d\n",sizeof(struct pcap_hdr));
	memcpy(&pcap_header,(buffer+p),sizeof(struct pcap_hdr));

	//printing pcap_hdr data

	printf("magic number : %u\n",pcap_header.magic_number);
	printf("version_major : %u\n",pcap_header.version_major);
	printf("version_minor : %u\n",pcap_header.version_minor);
	printf("thiszone : %d\n",pcap_header.thiszone);
	printf("sigfigs : %u\n",pcap_header.sigfigs);
	printf("snaplen : %u\n",pcap_header.snaplen);
	printf("network : %u\n",pcap_header.network);

	p += sizeof(struct pcap_hdr);

	memcpy(&record_header, (buffer+p), sizeof(struct pcaprec_hdr));

	p += sizeof(struct pcaprec_hdr);

	printf("\nts_sec : %d\n",record_header.ts_sec);
	printf("ts_usec : %d\n",record_header.ts_usec);
	printf("incl_len : %d\n",record_header.incl_len);
	printf("orig_len : %d\n",record_header.orig_len);
*/

}
