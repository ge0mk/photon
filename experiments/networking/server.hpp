#include <map>
#include <thread>
#include <ctime>
#include <functional>
#include <csignal>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <network/statuscodes.hpp>
#include <network/tcpsocket.hpp>
#include <network/address.hpp>
#include <utils/json.hpp>
#include <utils/dll.hpp>
#include <utils/string.hpp>
#include <angelscript/angelscript.h>
#include <angelscript/plugins/all.hpp>
#include <crypt/sha.hpp>
#include <crypt/crypt.hpp>
#include <crypt/rijndael.hpp>

typedef inet::ipv4address address;
typedef inet::tcpserver<address> tcpserver;
typedef inet::tcpclient<address> tcpclient;

class Server{
public:
	enum methods{
		NONE = 		0,
		CONNECT = 	1<<0,
		DELETE = 	1<<1,
		GET = 		1<<2,
		HEAD = 		1<<3,
		OPTIONS = 	1<<4,
		PATCH = 	1<<5,
		POST = 		1<<6,
		PUT = 		1<<7,
		TRACE = 	1<<8,
		ANY = (1<<0) + (1<<1) + (1<<2) + (1<<3) + (1<<4) + (1<<5) + (1<<6) + (1<<7) + (1<<8)
	};

	struct Plugin {
		int method;
		std::string name;
		std::string suburl;
		std::function<json(json, Server*, tcpclient&)> callback;
		bool active;
	};

	Server(std::string passPhrase = "");
	~Server();
	void run(address addr);
	void stop();
	void handleClient(tcpclient &client);

	std::string requestToString(json data);
	std::string responseToString(json response, bool payload = true);
	
	static int parseMethod(std::string method);
	static std::string decodeUrl(std::string url);

	utils::dll& loadLib(std::string name);
	void loadPlugins(std::string fileName);
	void registerPlugin(const Plugin &plugin);
	void sortPlugins();

	std::string getPassPhrase();

	void setOption(std::string key, int val);
	void setPluginActive(std::string name, bool active);

private:
	bool quit = false;
	unsigned short port = 0;
	std::map<std::string, int> options;
	std::map<std::string, utils::dll*> libs;
	std::vector<Plugin> plugins;
	std::string passPhrase;
};