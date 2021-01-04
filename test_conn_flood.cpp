#include "header.h"
#include "make_ipv4.h"
#include "make_tcp.h"
#include "receiver.h"

int main(void) {
	char conn_src_ip[] = "192.168.56.3";
	char conn_dest_ip[] = "192.168.56.1";
	int conn_src_port = 22222;
	int conn_dest_port = 55555;

	char Request[]
 = "GET / HTTP/1.1\r\nHost: www.google.co.kr\r\n\r\n";

	while (1) {
		int s;
		s = tcp_make_connection(inet_addr(conn_src_ip), inet_addr(conn_dest_ip),
				conn_src_port, conn_dest_port);
		conn_src_port++;
		send(s,Request,100,0);
		sleep(5);

		close(s);
	}

}
