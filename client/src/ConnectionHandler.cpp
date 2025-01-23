#include "../include/ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_),isConnect(false) {}
ConnectionHandler::ConnectionHandler():host_(""), port_(0), io_service_(), socket_(io_service_),isConnect(false){}
ConnectionHandler::~ConnectionHandler() {
	close();
}

bool ConnectionHandler::connect() {
	std::cout << "Starting connect to "
	          << host_ << ":" << port_ << std::endl;
	try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);		
		if (error){
			isConnect = false;
			throw boost::system::system_error(error);
		}
	}
	catch (std::exception &e) {
		std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
		isConnect = false;
		return false;
	}
	isConnect = true;
	return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
	size_t tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToRead > tmp) {
			tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
	int tmp = 0;
	boost::system::error_code error;
	// std::cout << "Socket " << socket_.native_handle() << std::endl;	

	// if (!this->isConnected())
    //             {
    //                 std::cout<<"Connection lost or not established."<<std::endl;
    //             }
	// if (!socket_.is_open()) {
    // std::cout << "Socket is not open." << std::endl;
	// }
	// if (bytes == nullptr || bytesToWrite <= 0) {
    // std::cerr << "Invalid input to sendBytes: bytes is null or bytesToWrite <= 0" << std::endl;
	// return false;
	// }

	try {
		while (!error && bytesToWrite > tmp) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}



bool ConnectionHandler::getLine(std::string &line) {
	return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string &line) {
	return sendFrameAscii(line, '\n');
}



bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
	char ch;
	// Stop when we encounter the null character.
	// Notice that the null character is not appended to the frame string.

	try {
		do {
			if (!getBytes(&ch, 1)) {
				return false;
			}
			if (ch != '\0')
				frame.append(1, ch);
		} while (delimiter != ch);
	} catch (std::exception &e) {
		std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
	if (!this->isConnected())
	{
		std::cout<<"disconnected from send frame ascii"<<std::endl;
	}
	std::cout << "Frame to send: " << frame << std::endl;

	bool result = sendBytes(frame.c_str(), frame.length());
	if (!result) return false;
	return sendBytes(&delimiter, 1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
	try {
		socket_.close();
	} catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
}

    // // Move constructor
    // ConnectionHandler::ConnectionHandler(ConnectionHandler&& other) 
    //     : host_(std::move(other.host_)), port_(other.port_),
    //       io_service_(std::move(other.io_service_)), socket_(std::move(other.socket_)) {}

    // // Move assignment operator
    // void ConnectionHandler::operator=(ConnectionHandler&& other) {
    //     if (this != &other) {
    //         // Clean up existing resources
    //         close();

    //         // Transfer ownership
    //         const_cast<std::string&>(host_) = std::move(other.host_); // Host and port need casting due to const
    //         const_cast<short&>(port_) = other.port_;
    //         io_service_ = std::move(other.io_service_);
    //         socket_ = std::move(other.socket_);
    //     }
    //     return *this;
    // }


    ConnectionHandler::ConnectionHandler(const ConnectionHandler& other):host_(other.host_),port_(other.port_),io_service_(),
                                                                socket_(io_service_),isConnect(false)
	{

	}



	std::string ConnectionHandler::getHost(){
		return this->host_;
	}

	short ConnectionHandler::getPort(){
		return this->port_;
	}

	bool ConnectionHandler::isConnected(){
		return isConnect;
	}

