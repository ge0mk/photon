#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> split(const std::string& str, std::string delimiter);
std::string join(const std::vector<std::string> parts, const char delimiter);
std::string removeAll(std::string str, std::string chars);
std::string stringToHex(std::string str, bool space = false);
std::string stringFromHex(std::string hex);

std::string replace(std::string &str, const std::string& from, const std::string& to);

std::string inflate(std::string str);
std::string deflate(std::string str);

inline std::string toLower(std::string str){
	std::transform(str.begin(), str.end(), str.begin(), [](char c) -> char{{ return (c <= 'Z' && c >= 'A') ? c - ('Z' - 'z') : c;}});
	return str;
}

inline std::string toUpper(std::string str){
	std::transform(str.begin(), str.end(), str.begin(), [](char c) -> char{{ return (c <= 'z' && c >= 'a') ? c + ('Z' - 'z') : c;}});
	return str;
}