#ifndef BODY_BUFFERING

#define BODY_BUFFERING

#define __BODY_BUFFERING_REQUEST_MSG_SIZE__ 100

void body_buffering_print_usage(void);
void* generate_body_buffering(void *data);
void* bodybuffer_time_check(void *data);
void body_buffering_main(char *argv[]);
#endif
