#!/bin/bash
gcc pcap_test.c -lpcap ../srcs/base/make_ipv4.c ../srcs/base/make_tcp.c && sudo ./a.out conn_done.pcapng 


