CXX = g++
CPPFLAGS = -g -Wall

.PHONY: all server client mserver spoofClient spoofServer

all: server client mserver spoofClient spoofServer

mserver: mult_server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

server: server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

client: client.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

spoofClient: spoof_client.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

spoofServer: spoof_server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

.PHONEY: clean
clean:
	rm *.o
