CFLAGS := -c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS := -lboost_system -lboost_filesystem -lpthread

all: bin/ bin/StompClient

# Ensure the bin directory exists
bin/:
	mkdir -p bin

# Build the StompClient executable
bin/StompClient: bin/ConnectionHandler.o bin/event.o bin/StompClient.o bin/StompProtocol.o bin/ResponseHandler.o bin/CommandHandler.o
	g++ -o bin/StompClient bin/ConnectionHandler.o bin/event.o bin/StompClient.o bin/StompProtocol.o bin/ResponseHandler.o bin/CommandHandler.o $(LDFLAGS)

# Compile ConnectionHandler.cpp into an object file
bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

# Compile event.cpp into an object file
bin/event.o: src/event.cpp include/event.h
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

# Compile StompClient.cpp into an object file
bin/StompClient.o: src/StompClient.cpp include/StompProtocol.h include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

# Compile StompProtocol.cpp into an object file
bin/StompProtocol.o: src/StompProtocol.cpp include/StompProtocol.h
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

# Compile ResponseHandler.cpp into an object file
bin/ResponseHandler.o: src/ResponseHandler.cpp include/ResponseHandler.h
	g++ $(CFLAGS) -o bin/ResponseHandler.o src/ResponseHandler.cpp

# Compile CommandHandler.cpp into an object file
bin/CommandHandler.o: src/CommandHandler.cpp include/CommandHandler.h
	g++ $(CFLAGS) -o bin/CommandHandler.o src/CommandHandler.cpp

# Clean all build artifacts
.PHONY: clean
clean:
	rm -rf bin/*
