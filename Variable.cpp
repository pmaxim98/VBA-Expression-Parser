#include "Variable.h"

#include "Utils.h"
#include "Boolean.h"
#include "Integer.h"
#include "Float.h"

#include <cctype>

using namespace std::string_view_literals;

std::optional<std::string_view> token::Variable::parse(std::string_view expression)
{
	auto parsed = parseAlias(expression);
	if (!parsed) return {};

	parsed = parseAsignment(parsed.value());
	if (!parsed) return {};

	return parseValue(parsed.value());
}

std::optional<std::string_view> token::Variable::parseAlias(std::string_view expression, Context& context)
{
	if (expression.empty() || !std::isalpha(expression.front()))
		return {};

	context.lastToken = Context::TokenType::Variable;

	static const auto delimiters = "abcdefghijklmnopqrstuvwxyz0123456789_";
	alias = expression.substr(0, expression.find_first_not_of(delimiters, 1));
	return utils::str::skipWhitespace(expression.substr(alias.length()));
}

std::optional<std::string_view> token::Variable::parseAlias(std::string_view expression)
{
	return parseAlias(expression, std::move(Context()));
}

std::optional<std::string_view> token::Variable::parseAsignment(std::string_view expression)
{
	if (expression.empty() || expression.front() != '=')
		return {};

	return utils::str::skipWhitespace(expression.substr(1));
}

std::optional<std::string_view> token::Variable::parseValue(std::string_view expression)
{
	if (expression.empty()) return {};

	auto isPositive = 0u;
	if (isPositive = "-+"sv.find(expression.front()); isPositive != std::string_view::npos)
		expression = utils::str::skipWhitespace(expression.substr(1));

	std::optional<std::string_view> parsed;
	if (token::operand::Boolean currentBool; parsed = currentBool.parse(expression, std::move(Context())))
		operand = std::make_unique<token::operand::Boolean>(currentBool);
	else if (token::operand::Float currentFloat; parsed = currentFloat.parse(expression, std::move(Context())))
		operand = std::make_unique<token::operand::Float>(currentFloat);
	else if (token::operand::Integer currentInt; parsed = currentInt.parse(expression, std::move(Context())))
		operand = std::make_unique<token::operand::Integer>(currentInt);
	else
		return  {};

	if (!isPositive) {
		operand = std::visit([] (auto&& val) -> token::operand::Ptr {
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, double>)
				return std::make_unique<token::operand::Float>(-val);
			else
				return std::make_unique<token::operand::Integer>(-val);

		}, operand->getValue());
	}

	return utils::str::skipWhitespace(parsed.value());
}