#include <iostream>
#include <string>
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include "CommandHandler.h"
#include "ResponseHandler.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>


std::vector<std::string> split_str(const std::string& command) {
    std::vector<std::string> result;
    size_t delimiterPos = command.find(':');
    if (delimiterPos != std::string::npos) {
        result.push_back(command.substr(0, delimiterPos)); // Host
        result.push_back(command.substr(delimiterPos + 1)); // Port
    }
    return result;
}


void splitBySpaces(const std::string& str, std::vector<std::string>& result) {
    std::istringstream iss(str); 
    std::string word;
    while (iss >> word) { 
        result.push_back(word);
    }
}



int main(int argc, char *argv[]) {
    ConnectionHandler* connectionHandler = nullptr; // Pointer for ConnectionHandler
    StompProtocol protocol(connectionHandler, false); // Protocol initialized with a placeholder
    CommandHandler commandHandler(protocol);
    ResponseHandler responseHandler(protocol);

    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    bool isLoggedIn = false;
    bool shouldTerminate = false;

    // Input thread
    std::thread inputThread([&]() {
        std::string command;
        std::vector<std::string> hostPort;

        while (std::getline(std::cin, command)) {
            std::unique_lock<std::mutex> lock(queueMutex);
            std::istringstream iss(command);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 1 && tokens[0] == "login") {
                if (tokens.size() != 4) {
                    std::cout << "login command needs 3 args: {host:port} {username} {password}" << std::endl;
                    continue;
                }

                hostPort = split_str(tokens[1]);
                if (hostPort.size() != 2) {
                    std::cout << "Invalid host:port format" << std::endl;
                    continue;
                }

                // Check if re-login is needed
                if (isLoggedIn) {
                    std::cout << "You are already logged in. Please logout first." << std::endl;
                    continue;
                }

                if (connectionHandler != nullptr) {
                    connectionHandler->close();
                    delete connectionHandler; // Clean up the old connection handler
                }

                connectionHandler = new ConnectionHandler(hostPort[0], static_cast<short>(std::stoi(hostPort[1])));
                if (!connectionHandler->connect()) {
                    std::cout << "Could not connect to server" << std::endl;
                    delete connectionHandler;
                    connectionHandler = nullptr;
                    continue;
                }

                // Update protocol with the new connection handler
                protocol.setConnectionHandler(connectionHandler);
                CommandHandler commandHandler(protocol);
                ResponseHandler responseHandler(protocol);

                isLoggedIn = true;
                std::cout << "Connected to server successfully." << std::endl;
            }

            messageQueue.push(command);
            queueCV.notify_all();
        }
    });

    // Response thread
    std::thread responseThread([&]() {
        while (!shouldTerminate) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [&]() { return !messageQueue.empty() || shouldTerminate; });

            while (!messageQueue.empty()) {
                std::string frame;
                std::string message = messageQueue.front();
                messageQueue.pop();
                frame = commandHandler.handleCommand(message);
                lock.unlock();

                // Send frame to server
              //  std::cout << "Frame being sent:\n" << frame << std::endl;
                if (message.substr(0, message.find(' ')) != "report" && message.substr(0, message.find(' ')) != "summary") {
                    if (!connectionHandler->sendFrameAscii(frame, '\0')) {
                        std::cout << "Error sending message: " << message << std::endl;
                       // shouldTerminate = true;
                       // break;
                    }
                

                // Receive responses from the server
                std::string responseFrame;
                if (connectionHandler->isConnected() && connectionHandler->getFrameAscii(responseFrame, '\0')) {
               //     std::cout << "Received from server: " << responseFrame << std::endl;
                    responseHandler.handleResponse(responseFrame);

                    // Handle logout case
                    if (message == "logout") {
                        isLoggedIn = false;
                        connectionHandler->close();
                        delete connectionHandler;
                        connectionHandler = nullptr;
                    }
                }
                }
                lock.lock();
            }
        }
    });

    inputThread.join();
    responseThread.join();

    if (connectionHandler != nullptr) {
        connectionHandler->close();
        delete connectionHandler;
    }

    return 0;
}

