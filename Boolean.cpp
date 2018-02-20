#include "Boolean.h"

#include "Utils.h"

#include <vector>
#include <cctype>

token::operand::Boolean::Boolean() : Boolean(false)
{ }

token::operand::Boolean::Boolean(bool value) : value(value)
{ }

std::optional<std::string_view> token::operand::Boolean::parse(std::string_view expression, Context& context)
{
	static const std::vector<std::pair<std::string_view, int>> map {{"True", true}, {"False", false}};

	const auto found = utils::str::findPrefix(expression, map);
	if (!found) return {};

	const auto& [prefix, boolean] = found.value();

	if (utils::str::charAfterPrefix(expression, prefix, std::isalnum)) 
		return {};

	context.lastToken = Context::TokenType::Operand;
	value = boolean;
	return utils::str::skipWhitespace(expression.substr(prefix.length()));
}

token::operand::Type token::operand::Boolean::getValue() const
{
	return -static_cast<long long>(value);
}

std::string token::operand::Boolean::toString() const
{
	return value ? "True" : "False";
}
