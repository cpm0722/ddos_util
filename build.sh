#!/bin/bash
g++ Main.cpp make_ipv4.cpp make_tcp.cpp icmp_flood.cpp syn_flood.cpp conn_flood.cpp udp_flood.cpp receiver.cpp -lpthread -o ddos.out 
