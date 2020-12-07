
void* receive_icmp_reply(void *data) {
	int thread_id = *((int*) data);

	while (1) {
		pthread_mutex_lock(&mutex);

		if (icmp_received >= icmp_total) {
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return 0;
		}

		icmp_received++;

		pthread_mutex_unlock(&mutex);

		/*
		 sl = sizeof(from);
		 ret = recvfrom(icmp_socket, buffer, 1024, 0, (struct sockaddr*) &from, &sl);
		 if (ret < 0) {
		 printf("%d %d %d\n", ret, errno, EAGAIN);
		 perror("recvfrom error : ");
		 }

		 ip = (struct ip*) buffer;
		 hlen = ip->ip_hl * 4;
		 rp = (struct icmp*) (buffer + hlen);
		 printf("reply from %s, ", inet_ntoa(from.sin_addr));
		 printf("Type : %d, ", rp->icmp_type);
		 printf("Code : %d, ", rp->icmp_code);
		 printf("Seq  : %d, ", rp->icmp_seq);
		 printf("Iden : %d \n", rp->icmp_id);
		 */
	}
}
