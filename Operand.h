#ifndef OPERAND_H
#define OPERAND_H

#include "Context.h"

#include <string_view>
#include <optional>
#include <variant>
#include <memory>

namespace token::operand
{
	using Type = std::variant<long long, double>;
		
	class Operand
	{
		public:
			virtual ~Operand() = default;
			virtual std::optional<std::string_view> parse(std::string_view expression, Context& context) = 0;
			virtual Type getValue() const = 0;
			virtual std::string toString() const = 0;
	};

	using Ptr = std::shared_ptr<Operand>;
}

#endif