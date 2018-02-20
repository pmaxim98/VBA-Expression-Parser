#ifndef OPERATOR_H
#define OPERATOR_H

#include "Context.h"
#include "Operand.h"

#include <optional>
#include <string_view>

namespace token
{
	struct Operator
	{
		enum Type
		{
			LeftParanthesis,
			RightParanthesis,
			Power,
			Positive,
			Negative,
			Multiplication,
			FloatDivision,
			IntegerDivision,
			Mod,
			Sum,
			Difference,
			LeftBitshift,
			RightBitshift,
			Equality,
			Inequality,
			LessThan,
			LessThanEqual,
			GreaterThan,
			GreaterThanEqual,
			Not,
			And,
			AndAlso,
			Or,
			OrElse,
			Xor,
			Abs,
			Acos,
			Asin,
			Atan,
			Ceil,
			Cos,
			Exp,
			Floor,
			Log,
			Log10,
			Round,
			Sin,
			Sqrt,
			Tan,
			Truncate,
			Total
		};
 
		std::optional<std::string_view> parse(std::string_view expression, Context& context);

		operand::Ptr compute(operand::Ptr operand);
		operand::Ptr compute(operand::Ptr leftOperand, operand::Ptr rightOperand);

		Type type;
		int precedence, arity;
	};
}

#endif