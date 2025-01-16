
#include "StompProtocol.h"
#include <string>

class ResponseHandler {
private:
    StompProtocol& protocol;

public:
    ResponseHandler(StompProtocol& protocol);
    void handleResponse(const std::string& frame);
};

