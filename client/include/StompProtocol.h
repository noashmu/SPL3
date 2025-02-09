#pragma once

#include "../include/ConnectionHandler.h"
#include "event.h"

class StompProtocol
{
private:
    ConnectionHandler* connectionHandler;
    bool loggedIn;
    std::string username;
    int reciptId;
    int subscriptionId;

    std::map<std::string, std::map<std::string, std::vector<event>>> eventsByChannelAndUser;
    std::map<int, std::string> receiptActions; 
    std::map<std::string, std::map<std::string, int>> subscriptionById;

    
    std::string createConnectFrame(const std::string& host, const std::string& username, const std::string& password);
    std::string createSubscribeFrame(const std::string& channel, int id, int receipt);
    std::string createUnsubscribeFrame(int id, int receipt);
    std::string createSendFrame(const std::string& destination, event event); 
    std::string createDisconnectFrame(int receipt);

    void saveEvent(const std::string& channelName, const event& event);
    std::string epochToDate(time_t epochTime);
    std::string createSummary(const std::string& description);

    void splitBySpaces(const std::string& str, std::vector<std::string>& result);
    void split_str(const std::string& str, char delimiter, std::vector<std::string>& result);

    bool isAlreadySubForJoin(const std::string &channelName);
    bool isAlreadySubForExit(const std::string &channelName);
    void addSubscriptionByUser(const std::string &channelName);


public:
    StompProtocol(ConnectionHandler*,bool);
    ~StompProtocol();
    StompProtocol(const StompProtocol&) = delete;
    StompProtocol& operator=(const StompProtocol&) = delete;
    void saveSummaryToFile(const std::string& channel, const std::string& user, const std::string& outputFile);
    std::string report(const std::string& filePath);
    std::string login(const std::string& host, const std::string& port, const std::string& user, const std::string& password);
    std::string joinChannel(const std::string& channelName);
    std::string exitChannel(const std::string& channelName);
    std::string logout();
    void handleResponse(const std::string& frame,const std::string& responseType);
    void handleCommand(const std::string& command);
    void processMessageFrame(const std::string& destination, const std::string& body);
    void run(); 
    void SetIsLogin(bool);
    void setConnectionHandler(ConnectionHandler*);

};
