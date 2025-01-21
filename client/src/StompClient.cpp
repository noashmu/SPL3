#include <iostream>
#include <string>
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include "CommandHandler.h"
#include "ResponseHandler.h"


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
    std::istringstream iss(str); // זרם קלט מתוך המחרוזת
    std::string word;
    while (iss >> word) { // קרא מילה אחת בכל פעם לפי רווחים
        result.push_back(word);
    }
}



int main(int argc, char *argv[]) {
	std::cout << "main: " << std::endl;

    ConnectionHandler* connectionHandler=new ConnectionHandler();
    StompProtocol protocol(connectionHandler, false);
    CommandHandler commandHandler(protocol);

    std::thread inputThread([&]() {
        std::string command;
        bool islogin = false;
        std::vector<std::string> hostPort;

        while (std::getline(std::cin, command)) {
			std::cout << "Received command: " << command << std::endl;
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


                // בדיקה אם יש צורך בחיבור חדש
                if (!connectionHandler->isConnected() || 
                    connectionHandler->getHost() != hostPort[0] || 
                    connectionHandler->getPort() != static_cast<short>(std::stoi(hostPort[1]))) {
                    
                    // אם הכתובת או הפורט השתנו - נאתחל את החיבור מחדש
                    connectionHandler->close();
                    delete connectionHandler;

                    connectionHandler = new ConnectionHandler(hostPort[0], static_cast<short>(std::stoi(hostPort[1])));
                    if (!connectionHandler->connect()) {
                        std::cout << "Could not connect to server" << std::endl;
                        continue;
                    }

                    protocol = StompProtocol(connectionHandler, true);
                    commandHandler = CommandHandler(protocol);
                    islogin = true;
                }


                
                // connectionHandler->close();
                // delete connectionHandler;

                // connectionHandler = new ConnectionHandler(hostPort[0], static_cast<short>(std::stoi(hostPort[1])));
                // if (!connectionHandler->connect()) {
                //     std::cout << "Could not connect to server" << std::endl;
                //     continue;
                // }

                // protocol = StompProtocol(connectionHandler, true);
                // commandHandler = CommandHandler(protocol);

                // protocol.login(hostPort[0], hostPort[1], tokens[2], tokens[3]);
                //islogin = true;
            } 
            else if (!islogin) {
                std::cout << "please login first" << std::endl;
            } 
            else {
                commandHandler.handleCommand(command);
                if (command == "logout") {
                    islogin = false;
                    break;
                }
            }
            ResponseHandler responseHandler(protocol);
            std::string frame;
            connectionHandler->getFrameAscii(frame, '\0');
            responseHandler.handleResponse(frame);
        }
    });

    ResponseHandler responseHandler(protocol);
    std::thread responseThread([&]() {
        std::string frame;
        while (connectionHandler->getFrameAscii(frame, '\0')) {
            std::cout<<"fdkcnkv"<<std::endl;
            responseHandler.handleResponse(frame);
        }
    });

    inputThread.join();
    responseThread.join();

    // connectionHandler->close();
    // delete connectionHandler;

    return 0;
}