#include "StompProtocol.h"
#include "event.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>

  

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

    std::string StompProtocol::createSubscribeFrame(const std::string& channel, int id, int receipt){
        std::string frame = "SUBSCRIBE\n";
        frame += "destination:" +channel+ "\n";
        frame += "id:" + std::to_string(id) +  "\n";
        frame += "receipt:" + std::to_string(receipt) + "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame

        return frame;
    }

    std::string StompProtocol::createUnsubscribeFrame(int id, int receipt){
        std::string frame = "UNSUBSCRIBE\n";
        frame += "id:" +std::to_string(id)+  "\n";
        frame += "receipt:" + std::to_string(receipt) + "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame

        return frame;
    }

    std::string StompProtocol::createSendFrame(const std::string& destination, event event){
        std::string frame = "SEND\n";
        frame += "destination:" +destination+ "\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += "user:" +event.getEventOwnerUser()+ "\n";
        frame += "city:" + event.get_city() + "\n";
        frame += "event name:" + event.get_name() + "\n";
        frame += "date time:" + std::to_string(event.get_date_time()) + "\n";
        frame += "general information:\n";
        for (const auto& pair : event.get_general_information()) {
            frame+= pair.first +":" + pair.second+"\n";
        }
        frame+= "description:\n" + event.get_description()+"\n";
        frame += "\n"; // Empty line indicating the end of the headers
        frame += '\0'; // Null character to terminate the STOMP frame


        return frame;
    }

    std::string StompProtocol::createDisconnectFrame(int receipt){
        std::string frame = "DISCONNECT\n";
        frame += "receipt:" +std::to_string(receipt)+"\n";

        return frame;
    }

    void StompProtocol::handleResponse(){

    }

    void StompProtocol::handleCommand(const std::string& command){
        std::vector<std::string> commandDetails;
        splitBySpaces(command,commandDetails);
        std::string frame;
        std::string commandName = commandDetails[0];
    

        if(commandName == "login"){
            if(commandDetails.size()!=4){
                std::cout<< "login command needs 3 args: {host:port} {username} {password}" <<std::endl;
                return;
            }
            std::vector<std::string> result;
            split_str(commandDetails[1],':', result);
            this->connectionHandler = new ConnectionHandler(result[0], static_cast<short>(std::stoi(result[1])));
            if(this->connectionHandler.connect()){
                frame = createConnectFrame(commandDetails[1], commandDetails[2], commandDetails[3]);
            }
            else{
                std::cout << "Cannot connect to host:7777 please try to login again" << std::endl;
            }

        }
        else if (commandName == "join")
        {
            if(commandDetails.size()!=2){
                std::cout<< "join command needs 1 arg: {channel_name}" <<std::endl;
                return;
            }

            frame = createSubscribeFrame(commandDetails[1],);
        } 
        else if(commandName == "exit"){
            if(commandDetails.size()!=2){
                std::cout<< "exit command needs 1 args: {channel_name}" <<std::endl;
                return;
            }

            frame = createUnsubscribeFrame();
        }
        else if(commandName == "report"){
            if(commandDetails.size()!=2){
                std::cout<< "report command needs 1 args: {file}" <<std::endl;
                return;
            }

            frame = createSendFrame();
        }
        else if(commandName == "summary"){
            if(commandDetails.size()!=4){
                std::cout<< "summary command needs 4 args: {channel_name} {user} {file}" <<std::endl;
                return;
            }

            saveSummaryToFile(commandDetails[1], commandDetails[2], commandDetails[3]);
        }
        else if(commandName == "logout"){
            if(commandDetails.size()!=1){
                std::cout<< "logout command needs 0 args" <<std::endl;
                return;
            }

            frame = createDisconnectFrame();
        }
    }


    void StompProtocol::report(const std::string& filePath) {
        // Parse the events file.
        names_and_events eventsData = parseEventsFile(filePath);
        const std::string& channelName = eventsData.channel_name;
        std::vector<event>& events = eventsData.events;

        if (events.empty()) {
            return;
        }
        // Save and send each event.
        for (const auto& event : events) {
            saveEvent(channelName, event);
            connectionHandler.sendFrameAscii(createSendFrame(channelName,event),'\0');

        }
    }
    void StompProtocol::saveEvent(const std::string& channelName, const event& event1) {
            if (eventsByChannelAndUser.find(username) == eventsByChannelAndUser.end()) {
                eventsByChannelAndUser[username] = std::map<std::string, std::vector<event>>();
            }
            if (eventsByChannelAndUser[username].find(channelName) == eventsByChannelAndUser[username].end()) {
                eventsByChannelAndUser[username][channelName] = std::vector<event>();
            }
            eventsByChannelAndUser[username][channelName].push_back(event1);

    }
    // Helper function: Convert epoch to date string
        std::string StompProtocol::epochToDate(time_t epochTime) {
            std::ostringstream oss;
            std::tm* tmPtr = std::localtime(&epochTime);
            oss << std::put_time(tmPtr, "%d/%m/%y %H:%M");
            return oss.str();
        }

    // Helper function: Create a summary from description
    std::string StompProtocol::createSummary(const std::string& description) {
        if (description.size() <= 27) {
            return description;
        }
        return description.substr(0, 27) + "...";
    }

    void StompProtocol::saveSummaryToFile(const std::string& channel, const std::string& user, const std::string& outputFile){
        auto userMap = eventsByChannelAndUser.find(user);
        auto channelMap = userMap->second.find(channel);

        std::vector<event>& events = channelMap->second;
        std::sort(events.begin(), events.end(), [](const event& a, const event& b) {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time();
        }
        return a.get_name() < b.get_name();
        });
        int activeCount = 0;
        int forcesArrivalCount = 0;
        for (const auto& event : events) {
            if (event.get_general_information().at("active") == "true") {
            activeCount++;
              }
             if (event.get_general_information().at("forces_arrival_at_scene") == "true") {
            forcesArrivalCount++;
             }
       }
        if (userMap == eventsByChannelAndUser.end()) {
            std::cerr << "Error: User not found: " << user << std::endl;
            return;
        }

        if (channelMap == userMap->second.end()) {
            std::cerr << "Error: Channel not found for user: " << channel << std::endl;
            return;
        }



        if (!std::filesystem::exists(outputFile)) { //if file is not exist, create it
            std::ofstream file(outputFile);
            if (!file.is_open()) {
                std::cerr << "Error: Could not create the file: " << outputFile << std::endl;
                return;
            }
        }

        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << "Channel: " << channel << std::endl;
            file << "Stats:"<< std::endl;
            file << "Total:" <<  events.size() << std::endl;
            file << "active: " << activeCount << "\n";
            file << "forces arrival at scene: " << forcesArrivalCount << "\n";
            file << "Event Reports:\n";
            
            int reportNumber=1;
            for (const auto& event : events) {
                file << "Report_" << reportNumber << ":\n";
                file << "city: " << event.get_city() << "\n";
                file << "date time: " << epochToDate(event.get_date_time()) << "\n";
                file << "event name: " << event.get_name() << "\n";
                file << "summary: " << createSummary(event.get_description()) << "\n";
                reportNumber++;
            }

            file.close();
        }
    }

    StompProtocol::StompProtocol() { //:loggedIn(false), username(""), inputThread(), responseThread() 
        this->connectionHandler = new connectionHandler();
        this->loggedIn =false;
        this->username = "";
    }



    StompProtocol::~StompProtocol(){

    }

    void StompProtocol::run() {

    }

    void StompProtocol::splitBySpaces(const std::string& str, std::vector<std::string>& result) {
        std::istringstream iss(str); // זרם קלט מתוך המחרוזת
        std::string word;
        while (iss >> word) { // קרא מילה אחת בכל פעם לפי רווחים
            result.push_back(word);
        }
    }

    void StompProtocol::split_str(const std::string& str, char delimiter, std::vector<std::string>& result) {
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
    }