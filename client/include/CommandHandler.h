#include "StompProtocol.h"
#include <string>

class CommandHandler {
private:
    StompProtocol& protocol;

public:
    CommandHandler(StompProtocol& protocol);
    CommandHandler(const CommandHandler& other);
    std::string handleCommand(const std::string& command);
    std::vector<std::string> split_str(std::string command);
    void splitBySpaces(const std::string& str, std::vector<std::string>& result);
};