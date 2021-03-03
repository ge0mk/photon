#include <variant>
#include <regex>
#include <map>
#include <vector>
#include <istream>
#include <fstream>
#include <iostream>
#include <time.h>

#include <utils/string.hpp>

#pragma once

struct Token {
	enum {
		Space = 0,
		Colon = 1,
		Comma,
		String,
		Number,
		ObjectStart,
		ObjectEnd,
		ArrayStart,
		ArrayEnd,
		Null,
		True,
		False
	};
};

class token {
public:
	int type;
	inline token(int type) : type(type) {}
	virtual inline ~token() {}
	virtual operator bool() = 0;
	virtual bool matches(const std::string &str) = 0;
};

template <char C, bool b = true>
class tokenChar : public token {
public:
	tokenChar(int type) : token(type) {}
	char get() {return C;}
	operator bool() { return b; }
	bool matches(const std::string &str) {
		return str.length() == 1 && str[0] == C;
	}
};

template <bool b = true>
class tokenString : public token {
public:
	tokenString(std::string str, int type) : token(type), str(str) {}
	operator bool() { return b; }
	bool matches(const std::string &str) {
		return str == this->str;
	}
private:
	std::string str;
};

template <bool b = true>
class tokenStringExpr : public token {
public:
	tokenStringExpr(int type) : token(type) {}
	operator bool() { return b; }
	bool matches(const std::string &str) {
		return str.length() >= 2 && str[0] == '"' && str.back() == '"';
	}
};

template <bool b = true>
class tokenNumberExpr : public token {
public:
	tokenNumberExpr(int type) : token(type) {}
	operator bool() { return b; }
	bool matches(const std::string &str) {
		std::smatch match;
		return std::regex_match(str.begin(), str.end(), match, r) && match[0].str().size() == str.size();
	}
private:
	const std::regex r = std::regex(R"(-?[0-9]*\.?[0-9]*([eE][+-]?[0-9]*)?)");
};


template<class Stream>
class tokenstream {
public:
	tokenstream(Stream &input, std::vector<token*> tokens, bool deleteTokens = false) : input(input), tokens(tokens), deleteTokens(deleteTokens) {}
	~tokenstream() {
		if(deleteTokens) {
			for(token* t : tokens) {
				delete t;
			}
		}
	}
	std::pair<token*, std::string> next() {
		static char tmp = input.get();
		std::string match = "";
		while(tmp) {
			match.push_back(tmp);
			tmp = input.get();
			for(token *t : tokens) {
				if(t->matches(match) && !t->matches(match + tmp)) {
					if(*t) {
						return {t, match};
					}
					else {
						match = "";
						break;
					}
				}
			}
		}
		return {nullptr, match};
	}
private:
	Stream &input;
	std::vector<token*> tokens;
	bool deleteTokens;
};


class json {
public:
	enum type {
		null_t, object_t, array_t, string_t, boolean_t, number_t
	};

	typedef std::map<std::string, json> object;
	typedef std::vector<json> array;
	typedef std::variant<std::monostate, object, array, std::string, bool, double> value;

	inline json()											{this->val = std::monostate();}
	inline json(const value &val)							{this->val = val;}
	inline json(const double &val) 							{this->val = val;}
	inline json(const int &val)								{this->val = double(val);}
	inline json(const bool &val)							{this->val = val;}
	inline json(const std::string &val)						{this->val = val;}
	inline json(const char *val)							{this->val = std::string(val);}
	inline json(const std::vector<json> &val)				{this->val = val;}
	inline json(const std::map<std::string, json> &val)		{this->val = val;}

	static inline json parse(std::string &source) {
		return json(source.c_str(), source.length());
	}

	template<class Stream>
	static json parse(Stream &input) {
		tokenstream stream(input, {
			new tokenChar<'{'>(Token::ObjectStart),
			new tokenChar<'}'>(Token::ObjectEnd),
			new tokenChar<'['>(Token::ArrayStart),
			new tokenChar<']'>(Token::ArrayEnd),
			new tokenChar<','>(Token::Comma),
			new tokenChar<':'>(Token::Colon),
			new tokenChar<' ', false>(Token::Space),
			new tokenChar<'\t', false>(Token::Space),
			new tokenChar<'\n', false>(Token::Space),
			new tokenStringExpr(Token::String),
			new tokenString<false>("null", Token::Null),
			new tokenString<false>("true", Token::True),
			new tokenString<false>("false", Token::False),
			new tokenNumberExpr(Token::Number)
		}, true);
		return json(stream);
	}

	inline bool isNull()	const {return val.index() == null_t;	}
	inline bool isObject()	const {return val.index() == object_t;	}
	inline bool isArray()	const {return val.index() == array_t;	}
	inline bool isString()	const {return val.index() == string_t;	}
	inline bool isBool()	const {return val.index() == boolean_t;	}
	inline bool isNumber()	const {return val.index() == number_t;	}

	json& operator[](std::string key);
	json& operator[](size_t index);
	const json& at(std::string key) const;
	const json& at(size_t index) const;

	inline bool& getBool() 			{return std::get<bool>(val);}
	inline double& getNum() 		{return std::get<double>(val);}
	inline std::string& getStr() 	{return std::get<std::string>(val);}
	inline object& getObject() 		{return std::get<object>(val);}
	inline array& getArray() 		{return std::get<array>(val);}
	inline const object& getObject() 	const {return std::get<object>(val);}
	inline const array& getArray() 		const {return std::get<array>(val);}

	bool toBool() const;
	double toDouble() const;
	float toFloat() const;
	int toInt() const;
	std::string toString() const;

	std::string print(size_t offset = 1) const;
	void print(std::ostream &output, int offset = 1) const;

	static const int minified = -1;

private:
	value val;

	// parse a string
	//	+ really fast
	//	- entire source needs to be in memory at the same time
	json(const char *str, size_t length);

	// parse a tokenstream
	//	+ can parse without storing the entire source -> memory efficient
	//	- slow (reads 1 char, checks if it matches any token, parses it or reads next char)
	template<class Stream>
	json(tokenstream<Stream> &stream) {
		std::pair<token*, std::string> t = stream.next();
		if(t.first) {
			switch(t.first->type) {
				case Token::ObjectStart: {
					val = std::map<std::string, json>();
					std::pair<token*, std::string> tmp;
					do {
						std::pair<token*, std::string> key = stream.next();
						if(key.first->type == Token::String) {
							if(stream.next().first->type == Token::Colon) {
								std::get<std::map<std::string, json>>(val)[std::string(key.second.begin() + 1, key.second.end() - 1)] = json(stream);
							}
							else {
								val = std::monostate();
								break;
							}
						}
						else {
							tmp = key;
							break;
						}
					} while((tmp = stream.next()).first->type == Token::Comma);
					if(tmp.first->type != Token::ObjectEnd) {
						val = std::monostate();
					}
				} break;
				case Token::ArrayStart: {
					val = std::vector<json>();
					std::pair<token*, std::string> tmp;
					do {
						std::get<std::vector<json>>(val).push_back(json(stream));
					} while((tmp = stream.next()).first->type == Token::Comma);
					if(tmp.first->type != Token::ArrayEnd) {
						val = std::monostate();
					}
				} break;
				case Token::String: {
					val = std::string(t.second.begin() + 1, t.second.end() - 1);
				} break;
				case Token::Number: {
					val = std::stod(t.second);
				} break;
				case Token::True: {
					val = true;
				} break;
				case Token::False: {
					val = false;
				} break;
				case Token::Null:
				default:
					val = std::monostate();
			}
		}
		else {
			val = std::monostate();
		}
	}
};

std::ostream& operator<<(std::ostream &stream, json& data);

	/*
	std::ifstream f("mesh.json");
	std::string source((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	int count = 10000;
	int start = clock();
	for(int i = 0; i < count; i++)
		json mesh = json::parse(source);
	int end = clock();
	std::cout<<"parsing took "<<float(end-start)/count/CLOCKS_PER_SEC<<" s\n"<<"\n";
	//std::ofstream o("out.json");
	//o<<mesh;
	network::tcpsocket sock(AF_INET, SOCK_STREAM, 0);
	sock.connect("postman-echo.com");
	netstream stream(sock);
	stream<<"GET /get?name=hi&password=ho HTTP/1.1\r\nAccept: *\r\nConnection: close\r\nHost: postman-echo.com\r\n\r\n";
	std::string httpheader;
	std::getline(stream, httpheader);
	json response = json::object{
		{"HTTP header", httpheader.substr(0, httpheader.length() - 1)},	//remove "\r" at end
		{"headers", [&stream] () -> json::object {
			json::object object;
			std::string line;
			while(std::getline(stream, line)) {
				if(line[0] == '\r')
					break;
				std::string key = std::string(line.begin(), line.begin() + line.find(':'));
				std::string value = std::string(line.begin() + line.find(':') + 2, line.end() - 1);	//remove ": " at begining and "\r" at end
				object[key] = value;
			}
			return object;
		}()},
		{"numbers", []() -> json::array {
			json::array numbers;
			for(int i = 0; i < 10; i++) {
				numbers.push_back(i*i);
			}
			return numbers;
		}()},
		{"payload", json::parse(stream)}
	};
	json payload = json::parse(stream);
	std::cout<<response.print()<<"\n";
	*/