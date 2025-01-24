
#include "StompProtocol.h"
#include "event.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip> // Add this for std::put_time
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


std::string StompProtocol::createConnectFrame(const std::string &host, const std::string &username, const std::string &password)
{
    std::string frame = "CONNECT\n";
    frame += "accept-version:1.2\n";
    frame += "host:stomp.cs.bgu.ac.il\n";
    frame += "login:" + username + "\n";
    frame += "passcode:" + password + "\n";
    frame += "\n"; // Empty line indicating the end of the headers
    // frame += '\0'; // Null character to terminate the STOMP frame

    return frame;
}

std::string StompProtocol::createSubscribeFrame(const std::string &channel, int id, int receipt)
{
    std::string frame = "SUBSCRIBE\n";
    frame += "destination:" + channel + "\n";
    frame += "id:" + std::to_string(id) + "\n";
    frame += "receipt:" + std::to_string(receipt) + "\n";
    frame += "\n"; // Blank line after headers
 //   frame += '\0'; // Null character to terminate the frame

    return frame;
}

std::string StompProtocol::createUnsubscribeFrame(int id, int receipt)
{
    std::string frame = "UNSUBSCRIBE\n";
    frame += "id:" + std::to_string(id) + "\n";
    frame += "receipt:" + std::to_string(receipt) + "\n";
    frame += "\n"; // Empty line indicating the end of the headers
 //   frame += '\0'; // Null character to terminate the STOMP frame

    return frame;
}

std::string StompProtocol::createSendFrame(const std::string &destination, event event)
{
    std::string frame = "SEND\n";
    frame += "destination:" + destination + "\n";
    frame += "\n"; // Empty line indicating the end of the headers
    frame += "user:" + event.getEventOwnerUser() + "\n";
    frame += "city:" + event.get_city() + "\n";
    frame += "event name:" + event.get_name() + "\n";
    frame += "date time:" + std::to_string(event.get_date_time()) + "\n";
    frame += "general information:\n";
    for (const auto &pair : event.get_general_information())
    {
        frame += pair.first + ":" + pair.second + "\n";
    }
    frame += "description:\n" + event.get_description() + "\n";
    frame += "\n"; // Empty line indicating the end of the headers
 //   frame += '\0'; // Null character to terminate the STOMP frame

    return frame;
}

std::string StompProtocol::createDisconnectFrame(int receipt)
{
    std::string frame = "DISCONNECT\n";
    frame += "receipt:" + std::to_string(receipt) + "\n" +"\n";

    return frame;
}

std::string StompProtocol::report(const std::string &filePath)
{
    // Parse the events file.
    names_and_events eventsData = parseEventsFile(filePath);
    const std::string &channelName = eventsData.channel_name;
    std::vector<event> &events = eventsData.events;
    std::string frame="";

    if (events.empty())
    {
       return "";
    }
    // Save and send each event.
    for (const auto &event : events)
    {
        saveEvent(channelName, event);
        connectionHandler->sendFrameAscii(createSendFrame(channelName, event), '\0');
    }

    return frame;
}

void StompProtocol::saveEvent(const std::string &channelName, const event &event1)
{
    if (eventsByChannelAndUser.find(username) == eventsByChannelAndUser.end())
    {
        eventsByChannelAndUser[username] = std::map<std::string, std::vector<event>>();
    }
    if (eventsByChannelAndUser[username].find(channelName) == eventsByChannelAndUser[username].end())
    {
        eventsByChannelAndUser[username][channelName] = std::vector<event>();
    }
    eventsByChannelAndUser[username][channelName].push_back(event1);
}

// Helper function: Convert epoch to date string
std::string StompProtocol::epochToDate(time_t epochTime)
{
    std::ostringstream oss;
    std::tm *tmPtr = std::localtime(&epochTime);
    oss << std::put_time(tmPtr, "%d/%m/%y %H:%M");
    return oss.str();
}

// Helper function: Create a summary from description
std::string StompProtocol::createSummary(const std::string &description)
{
    if (description.size() <= 27)
    {
        return description;
    }
    return description.substr(0, 27) + "...";
}

void StompProtocol::saveSummaryToFile(const std::string &channel, const std::string &user, const std::string &outputFile)
{
    auto userMap = eventsByChannelAndUser.find(user);
    auto channelMap = userMap->second.find(channel);

    std::vector<event> &events = channelMap->second;
    std::sort(events.begin(), events.end(), [](const event &a, const event &b)
              {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time();
        }
        return a.get_name() < b.get_name(); });
    int activeCount = 0;
    int forcesArrivalCount = 0;
    for (const auto &event : events)
    {
        if (event.get_general_information().at("active") == "true")
        {
            activeCount++;
        }
        if (event.get_general_information().at("forces_arrival_at_scene") == "true")
        {
            forcesArrivalCount++;
        }
    }
    if (userMap == eventsByChannelAndUser.end())
    {
        std::cerr << "Error: User not found: " << user << std::endl;
        return;
    }

    if (channelMap == userMap->second.end())
    {
        std::cerr << "Error: Channel not found for user: " << channel << std::endl;
        return;
    }

    if (!fs::exists(outputFile))
    { // if file is not exist, create it
        std::ofstream file(outputFile);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not create the file: " << outputFile << std::endl;
            return;
        }
    }

    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open())
    {
        file << "Channel: " << channel << std::endl;
        file << "Stats:" << std::endl;
        file << "Total:" << events.size() << std::endl;
        file << "active: " << activeCount << "\n";
        file << "forces arrival at scene: " << forcesArrivalCount << "\n";
        file << "Event Reports:\n";

        int reportNumber = 1;
        for (const auto &event : events)
        {
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

StompProtocol::StompProtocol(ConnectionHandler* c, bool islogin) : connectionHandler(c), loggedIn(islogin), username(""),reciptId(1),subscriptionId(1),eventsByChannelAndUser(),receiptActions(),subscriptionById()
{ //: loggedIn(false), username(""), inputThread(), responseThread()

}

StompProtocol::~StompProtocol()
{
}

void StompProtocol::run()
{
}

void StompProtocol::splitBySpaces(const std::string &str, std::vector<std::string> &result)
{
    std::istringstream iss(str); // זרם קלט מתוך המחרוזת
    std::string word;
    while (iss >> word)
    { // קרא מילה אחת בכל פעם לפי רווחים
        result.push_back(word);
    }
}

void StompProtocol::split_str(const std::string &str, char delimiter, std::vector<std::string> &result)
{
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
        result.push_back(item);
    }
}

std::string StompProtocol::login(const std::string &host, const std::string &port, const std::string &user, const std::string &password)
{
    if (loggedIn)
    {
        std::cout << "The client is already logged in, log out before trying again" << std::endl;
        return "";
    }

    // Create the CONNECT frame
    std::string frame = createConnectFrame(host, user, password);
    std::cout<<frame<<std::endl;

    username = user;

    return frame;
}
std::string StompProtocol::joinChannel(const std::string &channelName)
{
    if (!loggedIn)
    {
        std::cout << "Please login first" << std::endl;
        return "";
    }
    receiptActions[reciptId] = "join:" + channelName; // Track the action

    std::string frame;
    if(isAlreadySubForJoin(channelName)){
        frame = createSubscribeFrame(channelName, subscriptionId, reciptId);
        std::cout<<"THE FRAME IS: "+ frame + "    JUST TO CHECK"<<std::endl;
        reciptId++;
    }
    else{         std::cout<<"ENTERD ELSE"<<std::endl;

        frame = "";}

    return frame;
}

void StompProtocol::addSubscriptionByUser(const std::string &channelName){
    subscriptionById[channelName].insert({username,subscriptionId});
}

bool StompProtocol::isAlreadySubForJoin(const std::string &channelName){
    auto userSubscriptions = subscriptionById.find(channelName);
    auto& channelsMap = userSubscriptions->second; // Inner map of channels

    if(subscriptionById.empty()){ //if no one is subscribe we can do join
        subscriptionId++; 
        addSubscriptionByUser(channelName);
        return true;
    }

    if (userSubscriptions == subscriptionById.end() || //if channel is not found
        channelsMap.find(username) == channelsMap.end()) { // if user is not found
            subscriptionId++; 
            addSubscriptionByUser(channelName);
            return false;
    }

    return true;
}

bool StompProtocol::isAlreadySubForExit(const std::string &channelName){
     // Check if the channel exists in the subscriptionById map
    auto channelIt = subscriptionById.find(channelName);

    // If the channel does not exist, return false
    if (channelIt == subscriptionById.end()) {
        return false;
    }

    // Access the inner map (users subscribed to this channel)
    auto& usersMap = channelIt->second;

    // Check if the user exists in the inner map
    auto userIt = usersMap.find(username);

    // If the user exists, erase them from the map and return true
    if (userIt != usersMap.end()) {
        usersMap.erase(userIt);
        // If the inner map becomes empty after removal, erase the outer map entry (channel)
        if (usersMap.empty()) {
            subscriptionById.erase(channelIt);
        }
        return true;
    }

    // Return false if the user is not found
    return false;

}


std::string StompProtocol::exitChannel(const std::string &channelName)
{
    if (!loggedIn)
    {
        std::cout << "Please login first" << std::endl;
        return "";
    }
    receiptActions[reciptId] = "exit:" + channelName; // Track the action

    std::string frame;
    if(isAlreadySubForExit(channelName)){
        frame = createUnsubscribeFrame(subscriptionId - 1, reciptId);
        reciptId++;
    }
    else { frame = "";}

    return frame;
}

std::string StompProtocol::logout()
{
    if (!loggedIn)
    {
        std::cout << "You are not logged in" << std::endl;
        return "";
    }
    receiptActions[reciptId] = "logout"; // Track the logout action
    std::string frame = createDisconnectFrame(reciptId);
    reciptId++;

    this->loggedIn=false;

    return frame;
}

void StompProtocol::handleResponse(const std::string &frame, const std::string &responseType)
{
    if (responseType == "RECEIPT")
    {
   // Parse the receipt-id from the frame
        size_t receiptPos = frame.find("receipt-id:");
        if (receiptPos != std::string::npos)
        {
            size_t start = receiptPos + std::string("receipt-id:").length();
            size_t end = frame.find('\n', start);
            int receiptId = std::stoi(frame.substr(start, end - start));

            

            // Find and process the action
            auto actionIt = receiptActions.find(receiptId);
            if (actionIt != receiptActions.end())
            {
                std::string action = actionIt->second;

                receiptActions.erase(actionIt); // Remove the processed receipt

                if (action == "logout")
                {
                    std::cout << "Logout successful!" << std::endl;
                    // if (connectionHandler!=nullptr)
                    // {
                    //     connectionHandler->close();
                    // }
                    
                }
                else if (action.rfind("join:", 0) == 0)
                {
                    std::string channel = action.substr(5);
                    std::cout << "Joined channel: " << channel << std::endl;
                }
                else if (action.rfind("exit:", 0) == 0)
                {
                    std::string channel = action.substr(5);
                    std::cout << "Exited channel: " << channel << std::endl;
                }
                else
                {
                    std::cerr << "Unknown action: " << action << std::endl;
                }
            }
    }
    }
    else if (responseType == "MESSAGE")
    {
        size_t destinationPos = frame.find("destination:");
        size_t bodyStart = frame.find("\n\n") + 2;

        if (destinationPos != std::string::npos && bodyStart != std::string::npos)
        {
            size_t destinationEnd = frame.find('\n', destinationPos);
            std::string destination = frame.substr(destinationPos + std::string("destination:").length(),
                                                   destinationEnd - (destinationPos + std::string("destination:").length()));
            std::string body = frame.substr(bodyStart);

            // Process the message to extract event information
            processMessageFrame(destination, body);
        }
    }
    else if (responseType == "ERROR")
    {
        // Handle ERROR frames
        size_t messagePos = frame.find("message:");
        size_t bodyStart = frame.find("\n\n") + 2;

        if (messagePos != std::string::npos)
        {
            size_t messageEnd = frame.find('\n', messagePos);
            std::string errorMessage = frame.substr(messagePos + std::string("message:").length(), messageEnd - (messagePos + std::string("message:").length()));
            std::cerr << "Error received: " << errorMessage << std::endl;

            if (bodyStart != std::string::npos)
            {
                std::string body = frame.substr(bodyStart);
                std::cerr << "Error details: " << body << std::endl;
            }
        }
    }
}


void StompProtocol::processMessageFrame(const std::string &destination, const std::string &body)
{
    // Parse the body of the MESSAGE frame to extract event details
    std::map<std::string, std::string> eventDetails;
    std::istringstream stream(body);
    std::string line;

    while (std::getline(stream, line))
    {
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos)
        {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            eventDetails[key] = value;
        }
    }

    if (eventDetails.find("user") == eventDetails.end() || eventDetails.find("event name") == eventDetails.end())
    {
        return;
    }

    const std::string &reporter = eventDetails["user"];
    const std::string &channelName = destination;

    event newEvent(
        destination,
        eventDetails["city"],
        eventDetails["event name"],
        std::stol(eventDetails["date time"]),
        eventDetails["description"],
        {{"active", eventDetails["active"]},
         {"forces_arrival_at_scene", eventDetails["forces_arrival_at_scene"]}});

    if (eventsByChannelAndUser[reporter].find(channelName) == eventsByChannelAndUser[reporter].end())
    {
        eventsByChannelAndUser[reporter][channelName] = std::vector<event>();
    }
    eventsByChannelAndUser[reporter][channelName].push_back(newEvent);
}
//   StompProtocol& StompProtocol::operator=(const StompProtocol& other)
//         {

//         }
    void StompProtocol::SetIsLogin(bool islogin)
    {
        this->loggedIn=islogin;
    }

