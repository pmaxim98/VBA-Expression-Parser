#ifndef VARIABLE_H
#define VARIABLE_H

#include "Context.h"
#include "Operand.h"

#include <string_view>
#include <optional>

namespace token
{
	struct Variable
	{
			std::optional<std::string_view> parse(std::string_view expression);
			std::optional<std::string_view> parseAlias(std::string_view expression, Context& context);

			std::string_view alias;
			operand::Ptr operand;

		private:
			std::optional<std::string_view> parseAlias(std::string_view expression);
			std::optional<std::string_view> parseAsignment(std::string_view expression);
			std::optional<std::string_view> parseValue(std::string_view expression);
	};
}

#endif
