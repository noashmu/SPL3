CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
# TODO change the makefile for StompEMIlient
all: StompClient

StompClient: bin/ConnectionHandler.o bin/event.o bin/StompClient.o bin/StompProtocol.o bin/ResponseHandler.o
	g++ -o bin/StompClient bin/ConnectionHandler.o bin/event.o bin/StompClient.o bin/StompProtocol.o bin/ResponseHandler.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp include/event.h
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp include/StompProtocol.h include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp include/StompProtocol.h
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/ResponseHandler.o: src/ResponseHandler.cpp include/ResponseHandler.h
	g++ $(CFLAGS) -o bin/ResponseHandler.o src/ResponseHandler.cpp

.PHONY: clean
clean:
	rm -f bin/*
