

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
	__u16 sport;
	__u16 dport;

	//to be used with modification count;
	__u16 modify_id;

};

struct sudo_packet_data {
	__u16 sport;
	__u16 dport;
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
u_short tcp_get_checksum_for_tcp_replay( struct sniff_tcp *, char *data, int datasize);




//Function prototypes
int session_match_check(struct session a_session, struct session b_session);
int session_exist_check(struct session *session_list, __u16 session_num,
		struct session a_session);
int session_table_check(struct session *session_list, __u16 session_num,
		struct session a_session);

int ip_pair_match_check(struct ip_pair pair1, struct ip_pair pair2);
int ip_pair_exist_check(struct ip_pair *pairs, struct ip_pair new_pair,
		int count);
int ip_table_check(struct ip_pair *table, char ip[16], int size);
struct ip_pair* ip_table_element_to_end(struct ip_pair *table_p, int index,
		int size);

int port_pair_match_check(struct port_pair pair1, struct port_pair pair2);
int port_pair_exist_check(struct port_pair *pairs, struct port_pair new_pair,
		int count);
int port_table_check(struct port_pair *table, __u8 port, int size);
struct port_pair* port_table_element_to_end(struct port_pair *table_p,
		int index, int size);
char* get_masked_ip_addr(char *current, __u8 mask);

void *tcpreplay_thread();
