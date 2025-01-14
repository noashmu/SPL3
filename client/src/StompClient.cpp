#include <iostream>
#include <string>
#include "ConnectionHandler.h"

void split_str(std::string str, char delimiter, std::vector<std::string>& result) {
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
    }

int main(int argc, char *argv[]) {
	std::string command;
	while(std::getline(std::cin,command)){
		if(command.size()!=4){
            std::cout<< "login command needs 3 args: {host:port} {username} {password}" <<std::endl;
            
        }
		else if(command[0] != "login"){
			std::cout<< "please login first" <<std::endl;
		}
		else{
            std::vector<std::string> result;
            split_str(command[1],':', result);
            this->connectionHandler = new ConnectionHandler(result[0], static_cast<short>(std::stoi(result[1])));
            if(this->connectionHandler.connect()){
                frame = createConnectFrame(commandDetails[1], commandDetails[2], commandDetails[3]);
            }
            else{
                std::cout << "Cannot connect to host:7777 please try to login again" << std::endl;
            }
		}
	}
}