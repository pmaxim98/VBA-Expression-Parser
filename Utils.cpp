#include "Utils.h"

#include <locale>

bool utils::str::isPrefix(std::string_view lhs, std::string_view rhs)
{
	return !rhs.compare(0, lhs.size(), lhs) && !lhs.empty();
}

bool utils::str::inString(std::string_view str, char character)
{
	return str.find(character) != std::string_view::npos;
}

std::string_view utils::str::skipWhitespace(std::string_view str)
{
	const auto found = std::find_if(str.begin(), str.end(), [] (char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	return str.substr(std::distance(str.begin(), found));
}