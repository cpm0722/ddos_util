udp.out: udp.o udp_flood.o make_ipv4.o
	g++ udp.o udp_flood.o make_ipv4.o -o udp.out -lpthread

udp.o: udp.cpp udp_flood.h
	g++ -c udp.cpp -lpthread

udp_flood.o: udp_flood.cpp udp_flood.h make_ipv4.h
	g++ -c udp_flood.cpp make_ipv4.cpp -lpthread

make_ipv4.o: make_ipv4.cpp make_ipv4.h
	g++ -c make_ipv4.cpp -lpthread
