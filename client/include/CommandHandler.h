#include "StompProtocol.h"
#include <string>

class CommandHandler {
private:
    StompProtocol& protocol;

public:
    CommandHandler(StompProtocol& protocol);
    void handleCommand(const std::string& command);
};