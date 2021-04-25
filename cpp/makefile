CXX = g++
CPPFLAGS = -g -Wall

.PHONY: all server client mserver

all: server client mserver

mserver: mult_server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

server: server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

client: client.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

.PHONEY: clean
clean:
	rm *.o
