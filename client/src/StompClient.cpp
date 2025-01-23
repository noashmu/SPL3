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
    std::istringstream iss(str); // זרם קלט מתוך המחרוזת
    std::string word;
    while (iss >> word) { // קרא מילה אחת בכל פעם לפי רווחים
        result.push_back(word);
    }
}



int main(int argc, char *argv[]) {

    ConnectionHandler* connectionHandler=nullptr;

    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    bool islogin = false;
    bool shouldTerminate = false;

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


                // בדיקה אם יש צורך בחיבור חדש
                if (connectionHandler==nullptr){
               //    connectionHandler->getHost() != hostPort[0] || 
               //    connectionHandler->getPort() != static_cast<short>(std::stoi(hostPort[1]))) {
                    
                    // אם הכתובת או הפורט השתנו - נאתחל את החיבור מחדש
                 //   connectionHandler->close();
                  //  delete connectionHandler;
                    std::cout << hostPort[0]<< "  start "<<hostPort[1]<< std::endl;
                    connectionHandler = new ConnectionHandler(hostPort[0], static_cast<short>(std::stoi(hostPort[1])));
                    if (!connectionHandler->connect()) {
                        std::cout << "Could not connect to server" << std::endl;
                        continue;
                    }

             
                }

                messageQueue.push(command);
                queueCV.notify_all();

    
            } 
       
            else {
                messageQueue.push(command);
                queueCV.notify_all();
                //commandHandler.handleCommand(command);
                if (command == "logout") {
                    islogin = false;
                    shouldTerminate = true;
                }
            }

        }
    });

    
    std::thread responseThread([&]() {
        StompProtocol protocol(connectionHandler, false);
        CommandHandler commandHandler(protocol);
        ResponseHandler responseHandler(protocol);

        while (!shouldTerminate) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [&]() { return !messageQueue.empty() || shouldTerminate; });

            while (!messageQueue.empty()) {
                std::string frame;
                std::string message = messageQueue.front();
                messageQueue.pop();
                frame = commandHandler.handleCommand(message);
                lock.unlock();

                // שליחת הודעה לשרת
                std::cout << "Frame being sent:\n" << frame << std::endl;
            if (message.substr(0,message.find(' '))!="report"){
                if (!connectionHandler->sendFrameAscii(frame,'\0')){
                    std::cout << "Error sending message: " << message << std::endl;
                    shouldTerminate = true;
                    break;
                }
            }
                // קבלת הודעות מהשרת
                std::string responseFrame;
                if (!connectionHandler->isConnected())
                {
                    std::cout<<"Connection lost or not established."<<std::endl;
                }
                
                if (connectionHandler != nullptr && connectionHandler->getFrameAscii(responseFrame, '\0')) {
                    std::cout << "Received from server: " << responseFrame << std::endl;
                    responseHandler.handleResponse(responseFrame);
                }

                lock.lock();
            }
        }

    });

    inputThread.join();
    responseThread.join();

    // connectionHandler->close();
    // delete connectionHandler;

    return 0;
}