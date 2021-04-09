typedef struct
{
	unsigned char src[IPV4_STRLEN];
	unsigned char dest[IPV4_STRLEN];
	__u32 src_mask;
	__u32 dest_mask;
	__u32 port_start;
	__u32 port_end;
	__u32 request_per_sec;
} InputArguments;

typedef struct
{
	unsigned char src[IPv4_STRLEN];
	unsigned char dest[IPv4_STRLEN];
	__u32 port;
} MaskingArguments;

int argv_to_input_arguments(char *argv[], InputArguments *inputs)
{
	__uc *src = argv[0];
	__uc *dest = argv[1];
	__uc *port = argv[2];
	get_ip_from_ip_addr(src, inputs->src);
	get_ip_from_ip_addr(dest, inputs->dest);
	inputs->src_mask = get_mask_from_ip_addr(src);
	inputs->dest_mask = get_mask_from_ip_addr(dest);

	__uc *ptr = strtok(port, "-");
	*inputs->port_start = atoi(ptr);
	ptr = strtok(NULL, "-");
	if (ptr) {
		*inputs->port_end = atoi(ptr);
	} else {
		*inputs->port_end = *port_start;
	}
	return 0;
}

argv_to_input_arguments(argv, &g_bodybuf_inputs);
