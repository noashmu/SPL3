#include "StompProtocol.h"
#include "event.h"
#include "event.cpp"


    // Helper methods for creating STOMP frames
    std::string StompProtocol::createConnectFrame(const std::string& host, const std::string& username, const std::string& password){
        std::string frame = "CONNECT\n";
        frame += "accept-version:1.2\n";
        frame += "host:" + host + "\n";
        frame += "login:" + username + "\n";
        frame += "passcode:" + password + "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame

        return frame;
    }
    std::string createSubscribeFrame(const std::string& channel, int id, int receipt){
        std::string frame = "SUBSCRIBE\n";
        frame += "destination:" +channel+ "\n";
        frame += "id:" + std::to_string(id) +  "\n";
        frame += "receipt:" + std::to_string(receipt) + "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame
    }
    std::string createUnsubscribeFrame(int id, int receipt){
        std::string frame = "UNSUBSCRIBE\n";
        frame += "id:" +std::to_string(id)+  "\n";
        frame += "receipt:" + std::to_string(receipt) + "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame
    }
    std::string createSendFrame(const std::string& destination, event event){
        std::string frame = "SEND\n";
        frame += "destination:" +destination+ "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += "user:" +event.getEventOwnerUser()+ "\n";
        frame += "city:" + event.get_city() + "\n";
        frame += "event name:" + event.get_name() + "\n";
        frame += "date time:" + event.get_date_time() + "\n";
        frame += "general information:" + "\n";
        for (size_t i = 0; i < event.get_general_information().size; i++)
        {
            frame += event.get_general_information() + "\n";

        }
        
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame
    }
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