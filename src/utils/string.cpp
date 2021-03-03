#include <utils/string.hpp>

std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

std::vector<std::string> split(const std::string& str, std::string delimiter) {
	std::string s = str;
	std::vector<std::string> tokens;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	return tokens;
}

std::string join(const std::vector<std::string> parts, const char delimiter) {
	std::stringstream stream;
	for(const std::string &e : parts) {
		stream<<e<<delimiter;
	}
	std::string tmp = stream.str();
	if(tmp.length() >= 2)
		return std::string(tmp.begin(), tmp.end() - 1);
	return "";
}

std::string removeAll(std::string str, std::string chars) {
	str.erase(std::remove_if(str.begin(), str.end(), [&](unsigned char c) -> bool {return chars.find(c) != std::string::npos;}), str.end());
	return str;
}

std::string stringToHex(std::string str, bool space) {
	if(str.length() > 0){
		std::string hex(str.size()*(2+space)-space, ' ');
		for(unsigned int i = 0; i < str.size(); i++){
			unsigned char c = static_cast<unsigned char>(str[i]);
			unsigned char higher = c>>4;
			unsigned char lower = static_cast<unsigned char>(c<<4);
			lower >>= 4;
			higher += higher > 9 ? 7 : 0;
			lower += lower > 9 ? 7 : 0;
			hex[i*(2+space)] = char(higher+48);
			hex[i*(2+space)+1] = char(lower+48);
		}
		return hex;
	}
	return "00";
}

std::string stringFromHex(std::string hex) {
	std::string str(hex.size()/2, '\0');
	for(unsigned int i = 0; i < str.size(); i++){
		int higher = hex[i*2] - 48;
		int lower = hex[i*2+1] - 48;
		higher -= higher > 9 ? 7 : 0;
		lower -= lower > 9 ? 7 : 0;

		int c = (higher<<4) + lower;
		str[i] = char(c);
	}
	return str;
}

std::string replace(std::string &str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

std::string inflate(std::string str) {
	replace(str, "\a", "\\a");
	replace(str, "\b", "\\b");
	replace(str, "\f", "\\f");
	replace(str, "\n", "\\n");
	replace(str, "\r", "\\r");
	replace(str, "\t", "\\t");
	replace(str, "\v", "\\v");
    return str;
}
std::string deflate(std::string str) {
	replace(str, "\\a", "\a");
	replace(str, "\\b", "\b");
	replace(str, "\\f", "\f");
	replace(str, "\\n", "\n");
	replace(str, "\\r", "\r");
	replace(str, "\\t", "\t");
	replace(str, "\\v", "\v");
    return str;
}