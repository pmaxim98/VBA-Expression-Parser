#include "Float.h"

#include "Utils.h"

#include <iomanip>
#include <locale>
#include <sstream>

token::operand::Float::Float() : Float(0.0)
{ }

token::operand::Float::Float(double value) : value(value)
{ }

std::optional<std::string_view> token::operand::Float::parse(std::string_view expression, Context& context)
{
	auto charactersParsed = 0u;
	auto parsedValue = 0.0;

	try { parsedValue = std::stod(expression.data(), &charactersParsed); }
	catch (...) { return {}; }

	const auto dot = expression.substr(0, charactersParsed).find('.');
	if (dot == std::string_view::npos || expression.substr(dot + 1).empty() || std::isspace(expression[dot + 1], std::locale::classic()))
		return {};

	context.lastToken = Context::TokenType::Operand;
	value = parsedValue;
	return utils::str::skipWhitespace(expression.substr(charactersParsed));
}

token::operand::Type token::operand::Float::getValue() const
{
	return value;
}

std::string token::operand::Float::toString() const
{
	if (std::abs(value) < std::numeric_limits<double>::min())
		return "0";

	std::ostringstream oss;
	oss << std::setprecision(15) << value;

	auto number = oss.str();

	if (auto dot = number.find('.'); dot != std::string::npos)
		number[dot] = ',';

	auto signPosition = std::string::npos;

	if (std::abs(value) < 1e-4)
		signPosition = number.rfind('-');
	else if (std::abs(value) >= 1e+15)
		signPosition = number.rfind('+');
	else
		return number;

	number.at(signPosition - 1) = 'E';

	return number;
}
