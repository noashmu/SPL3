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
        if (tokens.size() != 4) {
            std::cout<< "login command needs 3 args: {host:port} {username} {password}" <<std::endl;
        } else {
            protocol.login(tokens[1], tokens[2], tokens[3]);
        }
    } else if (action == "join") {
        if (tokens.size() != 2) {
                std::cout<< "join command needs 1 arg: {channel_name}" <<std::endl;
        } else {
            protocol.joinChannel(tokens[1]);
        }
    } else if (action == "exit") {
        if (tokens.size() != 2) {
                std::cout<< "exit command needs 1 args: {channel_name}" <<std::endl;
        } else {
            protocol.exitChannel(tokens[1]);
        }
    } else if (action == "report") {
        if (tokens.size() != 2) {
                std::cout<< "report command needs 1 args: {file}" <<std::endl;
        } else {
            protocol.report(tokens[1]);
        }
    } else if (action == "summary") {
        if (tokens.size() != 4) {
                std::cout<< "summary command needs 4 args: {channel_name} {user} {file}" <<std::endl;
        } else {
            protocol.saveSummaryToFile(tokens[1], tokens[2], tokens[3]);
        }
    } else if (action == "logout") {
        if (tokens.size() != 1) {
                std::cout<< "logout command needs 0 args" <<std::endl;
        } else {
            protocol.logout();
        }
    } else {
        std::cout << "Unknown command: " << action << std::endl;
    }
}