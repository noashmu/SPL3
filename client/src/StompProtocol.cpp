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

        totalReport++;

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

    }

    void StompProtocol::parseAndStoreEvents(const std::string& jsonFile){

    }

    void StompProtocol::saveSummaryToFile(const std::string& channel, const std::string& user, const std::string& outputFile){
        if (!std::filesystem::exists(outputFile)) { //if file is not exist, create it
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file.close(); // סגור את הקובץ אחרי יצירתו
            } else {
                std::cerr << "Error: Could not create the file: " << outputFile << std::endl;
                return;
            }
        }

        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << "Channel: " << channel << std::endl;
            file << "Stas:"<< std::endl;
            file << "Total:" << this->totalReport << std::endl;


            ////////////////////////
            ////////need to continue, need to add data
            ///////////////////////



            file.close();
            std::cout << "Summary saved to file: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Could not write to the file: " << outputFile << std::endl;
        }
    }

    StompProtocol::StompProtocol(ConnectionHandler& handler){
        this->totalReport = 0;
        //this->connectionHandler = handler;
    }

    StompProtocol::~StompProtocol(){

    }

    void StompProtocol::run() {

    }