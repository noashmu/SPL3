#include <iostream>
#include <string>
#include "ConnectionHandler.h"
#include <StompProtocol.h>
#include <CommandHandler.h>
#include <ResponseHandler.h>


std::vector<std::string> split_str(std::string command) {
   std::vector<std::string> result;
   size_t delimiterPos = command.find(':');

        if (delimiterPos != std::string::npos) {
            // Extract the part after the colon (second word)
			std::string firstWord = command.substr(delimiterPos - 1);

            std::string secondWord = command.substr(delimiterPos + 1);

            // Optional: Trim any leading/trailing whitespace from the second word
            secondWord.erase(0, secondWord.find_first_not_of(" \t")); // Trim leading spaces
            secondWord.erase(secondWord.find_last_not_of(" \t") + 1); // Trim trailing spaces
			result.push_back(firstWord);
			result.push_back(secondWord);	
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
	ConnectionHandler connectionHandler;
	StompProtocol protocol(connectionHandler,false);
	CommandHandler commandHandler(protocol);
    // Create threads for user input and server response handling
    std::thread inputThread([&]() {
        std::string command;
		bool islogin =false;
		std::vector<std::string> hostPort;
        while (std::getline(std::cin, command)) {
			std::istringstream iss(command);
			std::vector<std::string> tokens;
			std::string token;
			// Split the command into tokens (words)
			while (iss >> token) {
				tokens.push_back(token);
			}

			// Check if the command has exactly 4 parts
			if (tokens[0]== "login")
			{
				if (tokens.size() != 4) {
					std::cout << "login command needs 3 args: {host:port} {username} {password}" << std::endl;
					islogin = false;
				} 
				else if (tokens[0] != "login") {
					std::cout << "please login first" << std::endl;
					islogin = false;
				}
				else{
					hostPort=split_str(tokens[1]);
					islogin=true;
					connectionHandler = ConnectionHandler(hostPort[0], static_cast<short>(std::stoi(hostPort[1])));
                    protocol = StompProtocol(connectionHandler, true);
                    commandHandler = CommandHandler(protocol);
                    islogin = true;
			 	
			 }
			 if(islogin){
				commandHandler.handleCommand(command);
				if (command == "logout") {
					islogin = false;
                	break;
            	}
			}
			}
        }
    });
	ResponseHandler responseHandler(protocol);
    std::thread responseThread([&]() {
        std::string frame;
        while (connectionHandler.getFrameAscii(frame, '\0')) {
           responseHandler.handleResponse(frame);
        }
    });

    // Wait for threads to finish
    inputThread.join();
   	responseThread.join();
}