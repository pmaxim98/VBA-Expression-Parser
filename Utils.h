#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string_view>
#include <optional>

namespace utils
{
	//	return true if the value is in the container
	template <typename Container, typename T = Container::value_type>
	bool contains(const Container& container, const T& value)
	{
		return std::find(std::begin(container), std::end(container), value) != std::end(container);
	}

	namespace str
	{
		//	returns true if lhs is prefix of rhs
		bool isPrefix(std::string_view lhs, std::string_view rhs);

		//	checks if there is a string in the 'container' that is a prefix of 'str'
		//	in case a match is found, a std::optional<> is returned containing a pair with the prefix as key
		template <typename Container>
		auto findPrefix(std::string_view str, const Container& container) -> std::optional<typename Container::value_type>
		{
			const auto found = std::find_if(std::begin(container), std::end(container), 
											[&str] (const auto& pair) { return utils::str::isPrefix(pair.first, str); });
			
			if (found != std::end(container))
				return *found;
			return {};
		}

		//	checks if the first character immediately after the prefix of the expression matches the condition of the predicate supplied
		template <typename UnaryPredicate>
		bool charAfterPrefix(std::string_view str, std::string_view prefix, UnaryPredicate&& predicate)
		{
			const auto afterPrefix = str.substr(prefix.length());
			return !afterPrefix.empty() && predicate(afterPrefix.front());
		}

		//	returns true if the string contains the character
		bool inString(std::string_view str, char character);

		//	returns the string_view without any whitespaces at the start of the string
		std::string_view skipWhitespace(std::string_view str);

	}

}

#endif