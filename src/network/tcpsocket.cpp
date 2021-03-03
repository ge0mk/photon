#include <network/tcpsocket.hpp>

namespace network{
	void __attribute__((constructor)) initTCP(){
		#if defined(WINDOWS)
			WSADATA wsa;
			long r =  WSAStartup(MAKEWORD(2,0),&wsa);
			if(r != 0){
				exit(-1);
			}
		#endif
	}

	address::address(unsigned v) : val(v){}

#if defined(WINDOWS)
	tcpsocket::tcpsocket(int af, int type, int protocol){
		handle = socket(af, type, protocol);
	}
	tcpsocket::tcpsocket(SOCKET s, SOCKADDR_IN addr) : handle{s}, client{addr}{
		;
	}

	void tcpsocket::bind(address addr, unsigned short port){
		SOCKADDR_IN server = {AF_INET, htons(port)};
		server.sin_addr.s_addr = htonl(addr.val);
		memset(&server.sin_zero, 0, 8);
		if(::bind(handle, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR);
	}
	void tcpsocket::listen(){
		if(::listen(handle, 64) == SOCKET_ERROR);
	}
	tcpsocket* tcpsocket::accept(struct timeval timeout){
		int len = sizeof(client);
		SOCKET s = ::accept(handle, (SOCKADDR*)&client, &len);
		if(s == INVALID_SOCKET);
		else{
			std::string ipaddress = inet_ntoa(client.sin_addr);
			setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
		}
		return new tcpsocket(s, client);
	}
	std::string tcpsocket::getClientIP(){
		return inet_ntoa(client.sin_addr);
	}
	void tcpsocket::connect(std::string host, unsigned short port){
		unsigned long addr;

		if((addr = inet_addr(host.c_str())) != INADDR_NONE);
		else{
			hostent *host_info = gethostbyname(host.c_str());
			if(host_info)
				memcpy(&addr, host_info->h_addr, host_info->h_length);
			else{
				return;
			}
		}
		SOCKADDR_IN server = {AF_INET, htons(port)};
		server.sin_addr.s_addr = htonl(addr);
		if(::connect(handle, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR);
	}
	void tcpsocket::disconnect(){
		if(shutdown(handle, SD_SEND) < 0){
		}
		if(close(handle) < 0){
		}
	}

	bool tcpsocket::isConnected(){
		int error = 0;
		int errorlen = sizeof(error);
		if(getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &errorlen) != 0){
			return false;
		}
		return true;
	}

#else

	tcpsocket::tcpsocket(int af, int type, int protocol){
		handle = socket(af, type, protocol);
		setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, nullptr, 0);
	}
	tcpsocket::tcpsocket(int handle, struct sockaddr_in addr) : handle(handle), client{addr}{
		connected = true;
	}

	void tcpsocket::bind(address addr, unsigned short port){
		struct sockaddr_in server = {AF_INET, htons(port), {htonl(addr.val)}};
		memset(&server.sin_zero, 0, 8);
		if(::bind(handle, (struct sockaddr*)&server, sizeof(server)) < 0);
	}
	void tcpsocket::listen(){
		if(::listen(handle, 64) == -1) {
			spdlog::error("tcpsocket::listen()");
		}
		FD_ZERO(&master);
		FD_ZERO(&readfds);
		FD_SET(handle, &master);
	}
	tcpsocket* tcpsocket::accept(struct timeval timeout){
		memcpy(&readfds, &master, sizeof(master));
		int nready = select(handle + 1, &readfds, NULL, NULL, &timeout);
		for (int i = 0; i <= handle && nready > 0; i++) {
			if (FD_ISSET(i, &readfds)) {
				nready--;
				if(i == handle) {
					unsigned int len = sizeof(client);
					int new_socket = ::accept(handle, (struct sockaddr *)&client, &len);
					if(new_socket >= 0) {
						setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
						return new tcpsocket(new_socket, client);
					}
					spdlog::error("accept failed");
					return nullptr;
				}
			}
		}
		return nullptr;
	}
	std::string tcpsocket::getClientIP(){
		std::string ipaddress(16, ' ');
		struct in_addr ipAddr = client.sin_addr;
		inet_ntop(AF_INET, &ipAddr, &ipaddress[0], INET_ADDRSTRLEN);
		return ipaddress.c_str();
	}
	bool tcpsocket::connect(std::string host, unsigned short port){
		struct hostent *host_info;
		unsigned long addr;

		if((addr = inet_addr(host.c_str())) != INADDR_NONE);
		else{
			host_info = gethostbyname(host.c_str());
			if(host_info)
				memcpy(&addr, host_info->h_addr, host_info->h_length);
			else{
				return false;
			}
		}
		struct sockaddr_in server = {AF_INET, htons(port), {unsigned(addr)}};
		if(::connect(handle, (struct sockaddr*)&server, sizeof(server)) < 0){
			return false;
		}
		else{
			return true;
		}
	}
	void tcpsocket::disconnect(){
		if(shutdown(handle, SHUT_RDWR) < 0){
		}
		if(close(handle) < 0){
		}
	}

	bool tcpsocket::isConnected(){
		int error = 0;
		socklen_t errorlen = sizeof(error);
		if(getsockopt(handle, SOL_SOCKET, SO_ERROR, &error, &errorlen) != 0){
			return false;
		}
		return true;
	}

#endif
	tcpsocket::~tcpsocket(){
		if(isConnected())
			disconnect();
	}

	bool tcpsocket::send(std::string data){
		if(::send(handle, data.c_str(), (size_t)data.size(), 0) == -1){
			return false;
		}
		return true;
	}
	std::string tcpsocket::recv(int len){
		if(isConnected()){
			std::string data(len+1, '\0');
			int size = ::recv(handle, &data[0], len, 0);
			data[size] = '\0';
			data.resize(size);
			return data;
		}
		return "";
	}
	std::string tcpsocket::recvLine(char delim, size_t maxlen){;
		std::string line = "";
		if(isConnected()){
			char c;
			while(::recv(handle, &c, 1, 0) > 0 && c != delim && line.length() < maxlen){
				if(c != '\r')
					line.push_back(c);
			}
		}
		return line;
	}
}
