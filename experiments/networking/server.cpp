#include "server.hpp"

Server::Server(network::address addr, unsigned short port, std::string passPhrase) : port(port), passPhrase(passPhrase){
	socket = new network::tcpsocket(AF_INET, SOCK_STREAM, 0);
	socket->bind(addr, port);

	plugins.push_back({ANY - GET, "default", "/", [](json request, Server*, tcpsocket*){
		spdlog::error("no plugin found");
		return json::object{
			{"request", request},
			{"version", "HTTP/1.1"},
			{"status", 501},
			{"header", json::object{}},
			{"data", request.print()}
		};
	}});
}

Server::~Server(){
	for(auto &[key, dll] : libs) {
		delete dll;
	}
}

void Server::start(){
	socket->listen();
	while(!quit){
		tcpsocket *client = socket->accept({5,0});
		if(client && !quit) {
			std::thread([this, client]() {
				try {
					handleClient(client);
				} catch(std::exception &e) {
					spdlog::error("exception while handling client: \'{}\'", e.what());
				}
				delete client;
			}).detach();
		}
		else if(client) {
			delete client;
		}
	}
}

void Server::handleClient(tcpsocket *client){
	json request = json::object{
		{"origin", client->getClientIP()},
		{"method", parseMethod(client->recvLine(' ', 8))},
		{"url", client->recvLine(' ', 2048)},
		{"version", client->recvLine('\n', 16)},
		{"header", [&client]() -> json::object {
			json::object headers;
			std::string line;
			do {
				line = client->recvLine();
				size_t pos = line.find(':');
				if(line.size() >= 3 && pos != std::string::npos && pos > 0 && pos < line.size() - 1) {
					headers[toLower(std::string(line.begin(), line.begin() + pos))] = toLower(std::string(line.begin() + pos + 1, line.end()));
				}
			} while(line.size() >= 3);
			return headers;
		}()}
	};
	if(request["version"].toString() != "HTTP/1.1" || request["method"].toInt() == NONE) {
		return;
	}
	std::string url = decodeUrl(request["url"].toString());
	if(url.find('?') < url.length() - 1) {
		request["url"] = url.substr(0, url.find('?'));
		std::string parameters = url.substr(url.find('?') + 1);
		request["parameters"] = [&parameters]() -> json {
			json::object result;
			for(std::string &param : split(parameters, '&')) {
				std::string key = param.substr(0, param.find('='));
				std::string val = "";
				if(param.find('=') < param.length() - 1) {
					val = param.substr(param.find('=') + 1);
				}
				result[key] = val;
			}
			return result;
		}();
	}
	if(request["header"]["cookie"].isString()) {
		json::object cookies;
		for(std::string &cookie : split(request["header"]["cookie"].toString(), ';')) {
			if(isspace(cookie[0]))
				cookie.erase(0, 1);
			std::string key = cookie.substr(0, cookie.find('='));
			std::string val = "";
			if(cookie.find('=') < cookie.length() - 1) {
				val = cookie.substr(cookie.find('=') + 1);
			}
			cookies[key] = val;
		}
		request["cookies"] = cookies;
	}
	else {
		request["cookies"] = json::object{};
	}
	spdlog::info("request: {}", request.print(json::minified));

	int method = request["method"].toInt();
	std::string response;
	for(const Plugin &p : plugins){
		if(p.active && (p.method & method) && (request["url"].toString().find(p.suburl) == 0)){
			try {
				response = responseToString(p.callback(request, this, client));
			}
			catch (std::exception &e) {
				spdlog::error("exception thrown in plugin {} : {}", p.name, e.what());
				response = responseToString(json::object{
					{"request", request},
					{"version", "HTTP/1.1"},
					{"status", HttpStatus_InternalServerError},
					{"header", json::object{
						{"Content-Type", "application/json"}
					}}
				});
			}
			break;
		}
	}
	spdlog::info("response: {}", inflate(response.substr(0, response.find("\n\n"))));
	if(response.length()){
		client->send(response);
	}
}

std::string Server::responseToString(json response, bool payload){
	if(response.isNull())
		return "";
	std::stringstream result;
	result<<response["version"].toString()<<" ";
	result<<int(response["status"].toInt())<<" ";
	result<<HttpStatusReasonPhrase(response["status"].isNumber() ? response["status"].toInt() : 500)<<"\n";

	json header = response["header"].isObject() ? response["header"] : json::object{};
	for(auto & [key, val] : header.getObject()){
		result<<key<<": "<<val.toString()<<"\n";
	}

	if(options["cors"]){
		result<<"Access-Control-Allow-Origin: \n";
		result<<"Access-Control-Allow-Methods: *\n";
		result<<"Access-Control-Allow-Headers: *\n";
		result<<"Access-Control-Allow-Credentials: true\n";
	}
	result<<"Connection: close\n";

	std::string data = response["data"].toString();
	if(data.size() > 0 && payload){
		result<<"Content-Length: "<<data.size()<<"\n";
		result<<"\n";
		result<<data;
	}
	return result.str();
}

std::string Server::requestToString(json data){
	std::stringstream result;
	result<<data["method"].toString()<<" "<<data["url"].toString()<<" "<<data["version"].toString()<<"\n";
	for(auto & [key, val] : data["header"].getObject()){
		result<<key<<": "<<val.toString()<<"\n";
	}
	std::cout<<result.str()<<"\n";
	return result.str();
}

int Server::parseMethod(std::string method){
	if(method == "CONNECT")
		return CONNECT;
	else if(method == "DELETE")
		return DELETE;
	else if(method == "GET")
		return GET;
	else if(method == "HEAD")
		return HEAD;
	else if(method == "OPTIONS")
		return OPTIONS;
	else if(method == "PATCH")
		return PATCH;
	else if(method == "POST")
		return POST;
	else if(method == "PUT")
		return PUT;
	else if(method == "TRACE")
		return TRACE;
	return NONE;
}

std::string Server::decodeUrl(std::string url){
	unsigned int len = url.length();
	std::string decoded(len, 0);
	for(unsigned int i = 0, k = 0; i < len; i++, k++){
		int c = url[i];
		if(c == '%' && i < len - 2){
			std::string byte = &url[i+1];
			byte.resize(2);
			std::stringstream ss;
			ss<<std::hex<<byte;
			ss>>c;
			i += 2;
		}
		decoded[k] = char(c);
	}
	return decoded.data();
}

dll& Server::loadLib(std::string name){
	if(libs.count(name) == 0){
		libs[name] = new dll(name);
		spdlog::info("loaded shared library \"{}\"", name);
	}
	return *libs[name];
}

void Server::loadPlugins(std::string fileName){
	std::ifstream f(fileName);
	std::string source((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	json data = json::parse(source);
	//std::cout<<data.print()<<"\n";
	for(auto &e : data.getArray()){
		try {
			std::string libname = e["dll"].toString();
			std::string func = e["function"].toString();
			std::string method = e["method"].toString();
			std::string name = e["name"].toString();
			std::string suburl = e["suburl"].toString();
			dll &lib = loadLib(libname);
			if(lib.get<json(*)(json, Server*, tcpsocket*)>(func)) {
				plugins.push_back(Plugin{
					parseMethod(method), name, suburl,
					lib.get<json(*)(json, Server*, tcpsocket*)>(func), true
				});
			}
		}
		catch (std::exception &exception) {
			spdlog::error("couldn't load plugin (exception(\'{}\')): \'{}\'", exception.what(), e.print(json::minified));
		}
	}
	sortPlugins();
	spdlog::info("loaded {} plugins: ", plugins.size());
	for(Plugin p : plugins){
		spdlog::info("    {} : {}", p.name, p.suburl);
	}
}

void Server::registerPlugin(const Plugin &plugin) {
	plugins.push_back(plugin);
}

void Server::sortPlugins(){
	std::sort(plugins.begin(), plugins.end(), [](Plugin &a, Plugin &b) -> bool {
		std::vector<std::string> la = split(a.suburl, '/');
		std::vector<std::string> lb = split(b.suburl, '/');
		if(la.size() == lb.size()){
			for(unsigned i = 0; i < la.size(); i++){
				std::string sa = la[i], sb = lb[i];
				if(sa[i] != sb[i]){
					if(sa.find(sb) == 0){
						return false;
					}
					else if(sb.find(sa) == 0){
						return true;
					}
					else{
						return sa > sb;
					}
				}
			}
			return a.suburl > b.suburl;
		}
		else{
			return la.size() > lb.size();
		}
	});
}

void Server::setOption(std::string key, int val) {
	options[key] = val;
}

void Server::setPluginActive(std::string name, bool active) {
	for(Plugin &p : plugins) {
		if(p.name == name) {
			p.active = active;
		}
	}
}

std::string Server::getPassPhrase() {
	return passPhrase;
}

void Server::stop(){
	quit = true;
}

std::stringstream buffer;

void print(std::string str) { buffer<<str; }
void print(int i) { buffer<<i; }
void print(double d) { buffer<<d; }

void MessageCallback(const asSMessageInfo *msg) {
	switch(msg->type) {
		case asMSGTYPE_ERROR: 		spdlog::error("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
		case asMSGTYPE_WARNING: 	spdlog::warn("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
		case asMSGTYPE_INFORMATION: spdlog::info("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
	}
	buffer<<(msg->type == asMSGTYPE_ERROR ? "[ERROR]" : msg->type == asMSGTYPE_WARNING ? "[WARNING]" : "[INFO]");
	buffer<<"["<<msg->row<<":"<<msg->col<<"] "<<msg->section<<" : "<<msg->message<<"\n";
}

int main(int argc, char *argv[]){
	struct sigaction s{[](int i){
		spdlog::warn("received SIGPIPE: {}", i);
	}};
	sigaction(SIGPIPE, &s, NULL);
	srand(unsigned(time(nullptr)));
	std::vector<std::string> args(argv + 1, argv + argc);

	//auto logger = spdlog::daily_logger_mt("logger", "logs/daily.txt", 0, 0);
	//spdlog::set_default_logger(logger);
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$]\t%v");
	std::string passPhrase = "";

	for(unsigned i = 0; i < args.size(); i++) {
		if(args[i] == "-p" && i < args.size() - 1)
			passPhrase = args[i + 1];
	}

	asIScriptEngine *engine = asCreateScriptEngine();
	engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
	RegisterScriptAny(engine);
	RegisterScriptMath(engine);
	RegisterScriptArray(engine, true);
	RegisterStdString(engine);
	RegisterStdStringUtils(engine);

	Server *server = new Server(address::Any, 80, passPhrase.length() ? stringFromHex(crypt::sha256::hash(passPhrase)) : "");

	engine->RegisterGlobalFunction("void setOption(string name, int state)", 
		asMETHODPR(Server, setOption, (std::string, int), void), asCALL_THISCALL_ASGLOBAL, server);
	engine->RegisterGlobalFunction("void setPluginActive(string name, bool active)", 
		asMETHODPR(Server, setPluginActive, (std::string, bool), void), asCALL_THISCALL_ASGLOBAL, server);
	engine->RegisterGlobalFunction("void stop()", 
		asMETHODPR(Server, stop, (void), void), asCALL_THISCALL_ASGLOBAL, server);
	engine->RegisterGlobalFunction("void print(string val)", asFUNCTIONPR(print, (std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void print(int val)", asFUNCTIONPR(print, (int), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void print(double val)", asFUNCTIONPR(print, (double), void), asCALL_CDECL);
	server->registerPlugin({Server::GET, "call as function", "/api/server", 
		[engine, server](json request, Server*, tcpsocket*){
			buffer.str("");	//clear buffer
			std::string code = request["parameters"]["code"].toString();
			if(server->getPassPhrase().length())
				code = crypt::rijndael(code, server->getPassPhrase(), crypt::decrypt);
			spdlog::info("running as code \'{}\'", code);

			ExecuteString(engine, code.c_str());
			asThreadCleanup();
			return json::object{
				{"request", request},
				{"version", "HTTP/1.1"},
				{"status", 200},
				{"header", json::object{}},
				{"data", buffer.str()}
			};
		}, true
	});
	server->loadPlugins("./plugins.json");
	server->setOption("cors", 1);
	server->start();
	usleep(1000000);
	delete server;
	engine->ShutDownAndRelease();
	return 0;
}
