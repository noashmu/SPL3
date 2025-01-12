#pragma once

#include "../include/ConnectionHandler.h"

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    ConnectionHandler& connectionHandler;
    bool loggedIn;
    std::string username;
   //std::unordered_map<std::string, std::vector<Event>> emergencyChannels;
    std::mutex stateMutex; // For thread safety
    std::thread inputThread, responseThread;

    // Helper methods for creating STOMP frames
    std::string createConnectFrame(const std::string& host, const std::string& username, const std::string& password);
    std::string createSubscribeFrame(const std::string& channel, int id, int receipt);
    std::string createUnsubscribeFrame(int id, int receipt);
    std::string createSendFrame(const std::string& destination, const std::string& message);
    std::string createDisconnectFrame(int receipt);

    // Response handling
    void handleResponse();
    void handleCommand(const std::string& command);

    // Utility methods
    void parseAndStoreEvents(const std::string& jsonFile);
    void saveSummaryToFile(const std::string& channel, const std::string& user, const std::string& outputFile);

public:
    StompProtocol(ConnectionHandler& handler);
    ~StompProtocol();

    void run(); // Starts threads for input and response handling
};
