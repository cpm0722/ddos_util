#include "header.h"
#include "make_ipv4.h"
#include "make_tcp.h"
#include "receiver.h"

int main(void) {
	char conn_src_ip[] = "192.168.56.3";
	char conn_dest_ip[] = "192.168.56.1";
	int conn_src_port = 14879;
	int conn_dest_port = 55555;
	char msg[8001];
	char randoms[10];
	char Request[8092];

	int length=0;
	int i, j;

	while(1){
	for (i = 0; i < 30; i++) {
		for (j = 0; j < 10; j++) {
			randoms[j] = rand() % 26;
			length += sprintf(msg+length, "%c", randoms[j]+97);
		}
		if (i == 29) {
			length +=sprintf(msg+length, "=1%c", '\0');
			break;
		}
		length +=sprintf(msg+length, "=1&");

	}
	length =0;
	length +=sprintf(Request+length,
			"POST / HTTP/1.1\r\nHost: www.somesite.so.kr\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n",
			sizeof(Request));
	length +=sprintf(Request+length, "%s\r\n", msg);

	printf("Sending msg : %s\n",Request);
	int s;
	s = tcp_make_connection(inet_addr(conn_src_ip), inet_addr(conn_dest_ip),
			conn_src_port, conn_dest_port);
	conn_src_port++;
	send(s, Request, sizeof(Request)-1, 0);

	//get_response(s);
	close(s);

	sleep(5);
	}
	//get_response(s);

	//close(s);

}
