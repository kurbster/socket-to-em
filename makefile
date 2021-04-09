CXX = g++
CPPFLAGS = -g -Wall $(EXTRA_INCLUDE_DIR)

.PHONY: all server client


all: server client

server: server.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

client: client.cc
	$(CXX) $(CPPFLAGS) -o $@ $^

.PHONEY: clean
clean:
	rm $(SRC_DIR)/*.o
