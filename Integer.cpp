#include "Integer.h"

#include "Utils.h"

#include <locale>

token::operand::Integer::Integer() : Integer(0ll)
{ }

token::operand::Integer::Integer(long long value) : value(value)
{ }

std::optional<std::string_view> token::operand::Integer::parse(std::string_view expression, Context& context)
{
	auto charactersParsed = 0u;
	auto parsedValue = 0ll;

	try { parsedValue = std::stoll(expression.data(), &charactersParsed); }
	catch (...) { return {}; }

	const auto suffix = expression.substr(charactersParsed);
	if (!suffix.empty() && (std::isalpha(suffix.front(), std::locale::classic()) || suffix.front() == '.'))
		return {};

	context.lastToken = Context::TokenType::Operand;
	value = parsedValue;
	return utils::str::skipWhitespace(expression.substr(charactersParsed));
}

token::operand::Type token::operand::Integer::getValue() const
{
	return value;
}

std::string token::operand::Integer::toString() const
{
	return std::to_string(value);
}
