#!/bin/bash
g++ Main.cpp make_ipv4.cpp make_tcp.cpp icmp_flood.cpp syn_flood.cpp -lpthread -o ddos.out 
