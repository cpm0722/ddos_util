#include "header.h"
#include "base/make_ipv4.h"

int g_pid_list[100];
int g_num_cores;

void PreparePid(int *pid_list, int num)
{
  int i;
  for (i = 0; i < num; i++) {
    g_pid_list[i] = pid_list[i];
  }
  g_num_cores = num;
  return;
}

void PrepareEmptyIphdr(struct iphdr *ip_head)
{
    memset(ip_head, 0x00, sizeof(struct iphdr));
    ip_head->ihl = 5;
    ip_head->version = 4;
    ip_head->tos = 0;
    ip_head->tot_len = sizeof(struct iphdr);
    unsigned int seed = time(NULL);
    srand(seed);
    ip_head->id = htons(31337 + (rand_r(&seed) % 100));
    ip_head->frag_off = 0;
    ip_head->ttl = 255;
    ip_head->protocol = IPPROTO_TCP;
    ip_head->check = 0;
    ip_head->saddr = 0;
    ip_head->daddr = 0;
    ip_head->check = IphdrGetChecksum((uint16_t *) &ip_head, sizeof(struct iphdr));
}


void IphdrSetProtocol(struct iphdr *ip_head, unsigned char protocol)
{
    ip_head->protocol = protocol;
}

void IphdrSetSrcAddr(struct iphdr *ip_head, uint32_t saddr)
{
    ip_head->saddr = saddr;
}

void IphdrSetDestAddr(struct iphdr *ip_head, uint32_t daddr)
{
    ip_head->daddr = daddr;
}

void IphdrAddSize(struct iphdr *ip_head, uint32_t data_size)
{
    ip_head->tot_len += data_size;
    ip_head->check = IphdrGetChecksum((uint16_t *) ip_head,
            sizeof(struct iphdr) + data_size);
}

void AssembleIphdrWithData(char *packet, struct iphdr *ip_head, void *data, uint32_t data_size)
{
    memcpy(packet, ip_head, sizeof(struct iphdr));
    memcpy(packet + sizeof(struct iphdr), data, data_size);
}

int MakeRawSocket(int protocol)
{
    int sock;
    sock = socket(AF_INET, SOCK_RAW, protocol);
    if (sock < 0) {
        perror("socket() error");
        exit(-1);
    }
    int one = 1;
    const int *val = &one;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt() error");
        exit(-1);
    }
    return sock;
}

void SendPacket(int sock, char *packet, uint32_t daddr, uint16_t tot_len, int port)
{
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = daddr;

    if (sendto(sock, packet, tot_len, 0,  (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) < 0) {
        perror("sendto() error");
        int i;
        for (i = 0; i < g_num_cores; i++) {
            printf("Cleaning...");
            kill(g_pid_list[i], SIGKILL);
        }
        exit(-1);
    }
    return;
}

uint16_t IphdrGetChecksum(uint16_t *ptr, int nbytes)
{
    register uint32_t sum;
    uint16_t oddbyte;
    register uint16_t answer;
    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}
