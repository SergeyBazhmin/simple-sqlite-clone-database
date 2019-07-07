#include "utils.h"
#include <vector>

namespace sqlite {
	std::vector<std::string> split(std::string & str)
	{
		std::istringstream is(str);
		std::string token;
		std::vector<std::string> tokens;
		while (std::getline(is, token, ' ')) tokens.emplace_back(token);
		return tokens;
	}
}