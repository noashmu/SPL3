#pragma once

#include "../include/ConnectionHandler.h"
#include "event.h"

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    ConnectionHandler* connectionHandler;
    bool loggedIn;
    std::string username;
    int reciptId=0;
    int subscriptionId=0;
   //std::unordered_map<std::string, std::vector<Event>> emergencyChannels;
    std::mutex stateMutex; // For thread safety
    std::thread inputThread, responseThread;
    std::map<std::string, std::map<std::string, std::vector<event>>> eventsByChannelAndUser;
    std::map<int, std::string> receiptActions; // Maps receipt-id to actions

    
    // Helper methods for creating STOMP frames
    std::string createConnectFrame(const std::string& host, const std::string& username, const std::string& password);
    std::string createSubscribeFrame(const std::string& channel, int id, int receipt);
    std::string createUnsubscribeFrame(int id, int receipt);
    std::string createSendFrame(const std::string& destination, event event); //we change it so it will get event
    std::string createDisconnectFrame(int receipt);

    void saveEvent(const std::string& channelName, const event& event);
    std::string epochToDate(time_t epochTime);
    std::string createSummary(const std::string& description);

    void splitBySpaces(const std::string& str, std::vector<std::string>& result);
    void split_str(const std::string& str, char delimiter, std::vector<std::string>& result);


public:
    StompProtocol(ConnectionHandler*,bool);
    ~StompProtocol();
    StompProtocol& operator=(const StompProtocol& other);
    void saveSummaryToFile(const std::string& channel, const std::string& user, const std::string& outputFile);
    void report(const std::string& filePath);
    void login(const std::string& host, const std::string& port, const std::string& user, const std::string& password);
    void joinChannel(const std::string& channelName);
    void exitChannel(const std::string& channelName);
    void logout();
    void handleResponse(const std::string& frame,const std::string& responseType);
    void handleCommand(const std::string& command);
    void processMessageFrame(const std::string& destination, const std::string& body);
    void run(); // Starts threads for input and response handling
};
