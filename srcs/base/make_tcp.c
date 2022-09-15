#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"

extern int g_recv_flag;

void PrepareEmptyTcphdr(struct tcphdr *tcp_head)
{
    memset(tcp_head, 0, sizeof(struct tcphdr));
    tcp_head->source = 0;   // port
    tcp_head->dest = 0;     // port
    tcp_head->seq = 0;      // fill in later;
    tcp_head->ack_seq = 0;  // fill in later
    tcp_head->doff = 5;

    tcp_head->ack = 0;
    tcp_head->syn = 0;
    tcp_head->fin = 0;

    tcp_head->res1 = 0;
    tcp_head->urg = 0;
    tcp_head->psh = 0;
    tcp_head->rst = 0;
    tcp_head->res2 = 0;

    tcp_head->window = htons(5840);
    tcp_head->check = 0;
}

void TcphdrSetSrcPort(struct tcphdr *tcph, uint16_t src_port)
{
    tcph->source = htons(src_port);
}

void TcphdrSetDestPort(struct tcphdr *tcph, uint16_t dest_port)
{
    tcph->dest = htons(dest_port);
}

void TcphdrSetSeq(struct tcphdr *tcph, uint32_t seq)
{
    tcph->seq = htonl(seq);
}

void TcphdrSetAckSeq(struct tcphdr *tcph, uint32_t ack_seq)
{
    tcph->ack_seq = htonl(ack_seq);
}

void TcphdrSetSynFlag(struct tcphdr *tcph)
{
    tcph->syn = 1;
}

void TcphdrSetAckFlag(struct tcphdr *tcph)
{
    tcph->ack = 1;
}

void TcphdrSetPshFlag(struct tcphdr *tcph)
{
    tcph->psh = 1;
}

void TcphdrSetWindowSize(struct tcphdr *tcph, uint16_t window_size)
{
    tcph->window = htons(window_size);
}

void TcphdrGetChecksum(struct iphdr *ipv4h,
        struct tcphdr *tcph,
        void *data,
        int datasize)
{
    struct PseudoTcphdr psh;
    memset(&psh, 0, sizeof(struct PseudoTcphdr));
    psh.source_address = ipv4h->saddr;
    psh.dest_address = ipv4h->daddr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + datasize);
    int psize = sizeof(struct PseudoTcphdr) +
        sizeof(struct tcphdr) +
        datasize;
    char assembled[BUFFER_SIZE] = {0, };
    memcpy(assembled, &psh, sizeof(struct PseudoTcphdr));
    memcpy(assembled + sizeof(struct PseudoTcphdr),
            tcph,
            sizeof(struct tcphdr));
    if (data != NULL && datasize != 0) {
        memcpy(assembled +
                sizeof(struct PseudoTcphdr) +
                sizeof(struct tcphdr), data, datasize);
    }
    tcph->check = IphdrGetChecksum((uint16_t*) assembled, psize);
}

// 3way handshake completed socket, returns socket;
void MakeTcpConnection(int sock,uint32_t src_ip,
        uint32_t dest_ip,
        int *src_port_copy,
        int dest_port,
        int *seq_copy,
        int *ack_copy,
        uint16_t window_size)
{
    // in make_ipv4.c -> make tcp socket via raw socket.
    struct iphdr ipv4_h;

    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.protocol = IPPROTO_TCP;
    ipv4_h.saddr = src_ip;
    ipv4_h.daddr = dest_ip;
    ipv4_h.tot_len += sizeof(struct tcphdr);
    ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
            sizeof(struct iphdr) + sizeof(struct tcphdr));

    // make tcp header.
    struct tcphdr tcp_h;
    PrepareEmptyTcphdr(&tcp_h);
    if (window_size != 0) {
        tcp_h.window = htons(window_size);
    }
    // set src port number random
    int src_port = rand() % 63535 + 1500;
    *src_port_copy = src_port;
    tcp_h.source = htons(src_port);
    tcp_h.dest = htons(dest_port);
    int seq = rand() % 10000000;
    tcp_h.seq = htonl(seq);
    seq++;
    
    tcp_h.syn = 1;
    TcphdrGetChecksum(&ipv4_h, &tcp_h, NULL, 0);

    char packet[BUFFER_SIZE] = {0, };
    AssembleIphdrWithData(packet ,&ipv4_h, &tcp_h, sizeof(tcp_h));
    SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, dest_port);
    uint32_t req_seq;

    if (g_recv_flag == 1) {
        unsigned char buffer[1000];
        recv(sock, buffer, 1000, 0);

        memcpy(&req_seq, buffer + 24, 4);
        req_seq = ntohl(req_seq);
    } else {
        req_seq = rand() % 10000000;
    }
    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.protocol = IPPROTO_TCP;
    ipv4_h.saddr = src_ip;
    ipv4_h.daddr = dest_ip;
    ipv4_h.tot_len += sizeof(struct tcphdr);
    ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
            sizeof(struct iphdr) + sizeof(struct tcphdr));
    // make tcp header.
    PrepareEmptyTcphdr(&tcp_h);
    // set src port number random
    if (window_size != 0) {
        tcp_h.window = htons(window_size);
    }
    tcp_h.source = htons(src_port);
    tcp_h.dest = htons(dest_port);

    tcp_h.seq = htonl(seq);
    tcp_h.ack_seq = htonl(req_seq+1);

    tcp_h.ack = 1;
    TcphdrGetChecksum(&ipv4_h, &tcp_h, NULL, 0);

    memset(packet ,0x00, sizeof(packet));
    AssembleIphdrWithData(packet, &ipv4_h, &tcp_h, sizeof(tcp_h));
    SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, dest_port);

    *seq_copy = seq;
    *ack_copy = req_seq + 1;

}

void TcpSocketSendData(int sock,
        uint32_t src_ip,
        uint32_t dest_ip,
        int src_port,
        int dest_port,
        char *data,
        int data_size,
        int seq,
        int ack,
        uint16_t window_size)
{
    struct iphdr ipv4_h;
    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.protocol = IPPROTO_TCP;
    ipv4_h.saddr = src_ip;
    ipv4_h.daddr = dest_ip;


    // make tcp header.
    struct tcphdr tcp_h;
    PrepareEmptyTcphdr(&tcp_h);
    if (window_size != 0) {
        tcp_h.window = htons(window_size);
    }
    // set src port number random

    tcp_h.source = htons(src_port);
    tcp_h.dest = htons(dest_port);
    tcp_h.seq = htonl(seq);
    tcp_h.ack_seq = htonl(ack);
    seq++;

    tcp_h.psh = 1;
    tcp_h.ack = 1;

    ipv4_h.tot_len += sizeof(struct tcphdr) + data_size;
    ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
            sizeof(struct iphdr) + sizeof(struct tcphdr)+data_size);

    TcphdrGetChecksum(&ipv4_h, &tcp_h, data, data_size);

    char tcp_with_data[BUFFER_SIZE] = {0, };
    memcpy(tcp_with_data, &tcp_h, sizeof(tcp_h));
    memcpy(tcp_with_data + sizeof(tcp_h), data, data_size);

    char packet[BUFFER_SIZE] = {0, };
    AssembleIphdrWithData(packet, &ipv4_h, tcp_with_data,
            sizeof(struct tcphdr) + data_size);

    SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, dest_port);
    return;
}

void TckSocketSendDataWithoutAck(int sock,
        uint32_t src_ip,
        uint32_t dest_ip,
        int src_port,
        int dest_port,
        char *data,
        int data_size,
        int seq,
        int ack,
        uint16_t window_size)
{
    struct iphdr ipv4_h;
    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.protocol = IPPROTO_TCP;
    ipv4_h.saddr = src_ip;
    ipv4_h.daddr = dest_ip;

    // make tcp header.
    struct tcphdr tcp_h;
    PrepareEmptyTcphdr(&tcp_h);
    if (window_size != 0) {
        tcp_h.window = htons(window_size);
    }
    // set src port number random
    tcp_h.source = htons(src_port);
    tcp_h.dest = htons(dest_port);
    tcp_h.seq = htonl(seq);
    tcp_h.ack_seq = htonl(ack);

    tcp_h.psh = 1;

    ipv4_h.tot_len += sizeof(struct tcphdr) + data_size;
    ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
            sizeof(struct iphdr) + sizeof(struct tcphdr)+data_size);
    TcphdrGetChecksum(&ipv4_h, &tcp_h, data, data_size);

    char tcp_with_data[BUFFER_SIZE] = {0, };
    memcpy(tcp_with_data, &tcp_h, sizeof(tcp_h));
    memcpy(tcp_with_data + sizeof(tcp_h), data, data_size);

    char packet[BUFFER_SIZE] = {0 ,};
    AssembleIphdrWithData(packet, &ipv4_h, tcp_with_data,
            sizeof(struct tcphdr) + data_size);

    SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, dest_port);
    return;
}
