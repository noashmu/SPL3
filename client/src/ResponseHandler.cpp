#include "ResponseHandler.h"
#include <iostream>

ResponseHandler::ResponseHandler(StompProtocol& protocol) : protocol(protocol) {}

void ResponseHandler::handleResponse(const std::string& frame) {
    if (frame.find("CONNECTED") == 0) { // Frame starts with "CONNECTED"
        std::cout << "login successful" << std::endl;
    } else if (frame.find("RECEIPT") == 0) { // Frame starts with "RECEIPT"
         protocol.handleResponse(frame,"RECEIPT");
    } else if (frame.find("MESSAGE") == 0) {
         protocol.handleResponse(frame,"MESSAGE");
    } else if (frame.find("ERROR") == 0) { 
        protocol.handleResponse(frame,"ERROR");

    } 
}
