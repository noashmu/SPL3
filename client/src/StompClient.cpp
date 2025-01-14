#include <iostream>
#include <string>
#include "ConnectionHandler.h"
#include <StompProtocol.h>

std::vector<std::string> split_str(std::string command) {
   std::vector<std::string> result;
   size_t delimiterPos = command.find(':');

        if (delimiterPos != std::string::npos) {
            // Extract the part after the colon (second word)
			std::string firstWord = command.substr(delimiterPos - 1);

            std::string secondWord = command.substr(delimiterPos + 1);

            // Optional: Trim any leading/trailing whitespace from the second word
            secondWord.erase(0, secondWord.find_first_not_of(" \t")); // Trim leading spaces
            secondWord.erase(secondWord.find_last_not_of(" \t") + 1); // Trim trailing spaces
			result.push_back(firstWord);
			result.push_back(secondWord);	
    }
	return result;
}

    void splitBySpaces(const std::string& str, std::vector<std::string>& result) {
        std::istringstream iss(str); // זרם קלט מתוך המחרוזת
        std::string word;
        while (iss >> word) { // קרא מילה אחת בכל פעם לפי רווחים
            result.push_back(word);
        }
    }

int main(int argc, char *argv[]) {
	std::string command;
	while(std::getline(std::cin,command)){
		if(command.size()!=4){
            std::cout<< "login command needs 3 args: {host:port} {username} {password}" <<std::endl;
            
        }
		else if(command[0]!= "login"){
			std::cout<< "please login first" <<std::endl;
		}
		else{
			std::vector<std::string> commandDetails;
			splitBySpaces(command,commandDetails);
           std::vector<std::string> result=split_str(commandDetails[1]);

            ConnectionHandler connectionHandler(result[0], static_cast<short>(std::stoi(result[1])));
			StompProtocol protocol(connectionHandler);
		}
	}
}