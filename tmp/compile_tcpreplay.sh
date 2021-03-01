#!/bin/bash
gcc pcap_test.c -lpcap ../src/base/make_ipv4.c ../src/base/make_tcp.c && sudo ./a.out bitt.pcapng


