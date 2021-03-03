#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

class yaml {
public:
	yaml();

	static yaml parse(const std::string &src);

private:
};