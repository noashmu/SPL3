#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include <vector>

CommandHandler::CommandHandler(StompProtocol& protocol) : protocol(protocol) {}


void CommandHandler::handleCommand(const std::string& command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;

    // Split the command into tokens
    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        std::cerr << "Error: Empty command." << std::endl;
        return;
    }

    const std::string& action = tokens[0];

    // Process specific commands
    if (action == "login") {
			std::vector<std::string> commandDetails;
			splitBySpaces(command,commandDetails);
            std::vector<std::string> hostPort=split_str(commandDetails[1]);
            protocol.login(hostPort[0],hostPort[1], tokens[2], tokens[3]);
    } 
    else if (action == "join") {
        if (tokens.size() != 2) {
                std::cout<< "join command needs 1 arg: {channel_name}" <<std::endl;
        } else {
            protocol.joinChannel(tokens[1]);
        }
    } 
    else if (action == "exit") {
        if (tokens.size() != 2) {
                std::cout<< "exit command needs 1 args: {channel_name}" <<std::endl;
        } else {
            protocol.exitChannel(tokens[1]);
        }
    } 
    else if (action == "report") {
        if (tokens.size() != 2) {
                std::cout<< "report command needs 1 args: {file}" <<std::endl;
        } else {
            protocol.report(tokens[1]);
        }
    } 
    else if (action == "summary") {
        if (tokens.size() != 4) {
                std::cout<< "summary command needs 4 args: {channel_name} {user} {file}" <<std::endl;
        } else {
            protocol.saveSummaryToFile(tokens[1], tokens[2], tokens[3]);
        }
    } 
    else if (action == "logout") {
        if (tokens.size() != 1) {
                std::cout<< "logout command needs 0 args" <<std::endl;
        } else {
            protocol.logout();
        }
    } 
    else {
        std::cout << "Unknown command: " << action << std::endl;
    }
}

    std::vector<std::string> CommandHandler::split_str(std::string command) {
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

    void CommandHandler::splitBySpaces(const std::string& str, std::vector<std::string>& result) {
        std::istringstream iss(str); // זרם קלט מתוך המחרוזת
        std::string word;
        while (iss >> word) { // קרא מילה אחת בכל פעם לפי רווחים
            result.push_back(word);
        }
    }
        CommandHandler& CommandHandler::operator=(const CommandHandler& other)
        {
            if (this!= &other)
            {
                this->protocol=other.protocol;
            }
            
            return *this;
        }
