#include "Operator.h"

#include "Utils.h"
#include "Boolean.h"
#include "Float.h"
#include "Integer.h"

#include <algorithm>
#include <cctype>
#include <vector>
#include <utility>
#include <cinttypes>

namespace
{
	using namespace std::string_view_literals;

	const std::vector<std::pair<std::string, token::Operator>> Operators
	{
		{"^",		{token::Operator::Power, 2, 2}},
		{"*",		{token::Operator::Multiplication, 4, 2}},
		{"/",		{token::Operator::FloatDivision, 4, 2}},
		{"\\",		{token::Operator::IntegerDivision, 5, 2}},
		{"Mod",		{token::Operator::Mod, 6, 2}},
		{"+",		{token::Operator::Sum, 7, 2}},
		{"-",		{token::Operator::Difference, 7, 2}},
		{"<<",		{token::Operator::LeftBitshift, 9, 2}},
		{">>",		{token::Operator::RightBitshift, 9, 2}},
		{"=",		{token::Operator::Equality, 10, 2}},
		{"<>",		{token::Operator::Inequality, 10, 2}},
		{"<=",		{token::Operator::LessThanEqual, 10, 2}},
		{">=",		{token::Operator::GreaterThanEqual, 10, 2}},
		{"<",		{token::Operator::LessThan, 10, 2}},
		{">",		{token::Operator::GreaterThan, 10, 2}},
		{"AndAlso", {token::Operator::AndAlso, 12, 2}},
		{"And",		{token::Operator::And, 12, 2}},
		{"OrElse",	{token::Operator::OrElse, 13, 2}},
		{"Or",		{token::Operator::Or, 13, 2}},
		{"Xor",		{token::Operator::Xor, 14, 2}},
		{"(",		{token::Operator::LeftParanthesis, 0, 1}},
		{")",		{token::Operator::RightParanthesis, 0, 1}},
		{"Abs",		{token::Operator::Abs, 1, 1}},
		{"Acos",	{token::Operator::Acos, 1, 1}},
		{"Asin",	{token::Operator::Asin, 1, 1}},
		{"Atan",	{token::Operator::Atan, 1, 1}},
		{"Ceiling", {token::Operator::Ceil, 1, 1}},
		{"Cos",		{token::Operator::Cos, 1, 1}},
		{"Exp",		{token::Operator::Exp, 1, 1}},
		{"Floor",	{token::Operator::Floor, 1, 1}},
		{"Log10",	{token::Operator::Log10, 1, 1}},
		{"Log",		{token::Operator::Log, 1, 1}},
		{"Round",	{token::Operator::Round, 1, 1}},
		{"Sin",		{token::Operator::Sin, 1, 1}},
		{"Sqrt",	{token::Operator::Sqrt, 1, 1}},
		{"Tan",		{token::Operator::Tan, 1, 1}},
		{"Truncate",{token::Operator::Truncate, 1, 1}},
		{"Not",		{token::Operator::Not, 11, 1}}
	};

	constexpr token::Operator UnaryPlus {token::Operator::Positive, 3, 1};
	constexpr token::Operator UnaryMinus {token::Operator::Negative, 3, 1};
}

std::optional<std::string_view> token::Operator::parse(std::string_view expression, Context& context)
{
	auto found = utils::str::findPrefix(expression, Operators);
	if (!found) return {};

	auto [prefix, op] = found.value();

	if (prefix.length() > 1 && utils::str::charAfterPrefix(expression, prefix, [] (char ch) { return !utils::contains("\n\v\f\r\t(+- ", ch); } ))
		return {};

	if (context.lastToken == Context::TokenType::Operator) {
		if (op.type == Type::Sum)
			op = UnaryPlus;
		else if (op.type == Type::Difference)
			op = UnaryMinus;
	}

	context.lastToken = Context::TokenType::Operator;

	if (op.type == Type::LeftParanthesis)
		++context.parenthesesOpen;
	else if (op.type == Type::RightParanthesis) {
		--context.parenthesesOpen;
		context.lastToken = Context::TokenType::Operand;
	}

	*this = op;
	return utils::str::skipWhitespace(expression.substr(prefix.length()));
}

token::operand::Ptr token::Operator::compute(token::operand::Ptr operand)
{
	switch (type)
	{
		case token::Operator::Positive:
			return operand;

		case token::Operator::Negative: {
			return std::visit([] (auto&& val) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val)>;

				if constexpr (std::is_same_v<T, double>)
					return std::make_unique<token::operand::Float>(-val);
				else
					return std::make_unique<token::operand::Integer>(-val);

			}, operand->getValue());
		}

		case token::Operator::Not: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Integer>(~std::llrint(val));
			}, operand->getValue());
		}

		case token::Operator::Abs: {
			return std::visit([] (auto&& val) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val)>;

				if constexpr (std::is_same_v<T, double>)
					return std::make_unique<token::operand::Float>(std::abs(val));
				else
					return std::make_unique<token::operand::Integer>(std::abs(val));
			}, operand->getValue());
		}

		case token::Operator::Acos: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::acos(val));
			}, operand->getValue());
		}

		case token::Operator::Asin: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::asin(val));
			}, operand->getValue());
		}

		case token::Operator::Atan: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::atan(val));
			}, operand->getValue());
		}

		case token::Operator::Cos: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::cos(val));
			}, operand->getValue());
		}

		case token::Operator::Sin: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::sin(val));
			}, operand->getValue());
		}

		case token::Operator::Tan: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::tan(val));
			}, operand->getValue());
		}

		case token::Operator::Exp: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::exp(val));
			}, operand->getValue());
		}

		case token::Operator::Log: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::log(val));
			}, operand->getValue());
		}

		case token::Operator::Log10: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::log10(val));
			}, operand->getValue());
		}

		case token::Operator::Sqrt: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::sqrt(val));
			}, operand->getValue());
		}

		case token::Operator::Ceil: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::ceil(val));
			}, operand->getValue());
		}

		case token::Operator::Floor: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::floor(val));
			}, operand->getValue());
		}

		case token::Operator::Round: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Integer>(std::llrint(val));
			}, operand->getValue());
		}

		case token::Operator::Truncate: {
			return std::visit([] (auto&& val) {
				return std::make_unique<token::operand::Float>(std::trunc(val));
			}, operand->getValue());
		}
	}
}

token::operand::Ptr token::Operator::compute(token::operand::Ptr leftOperand, token::operand::Ptr rightOperand)
{
	switch (type)
	{
		case token::Operator::Power: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::pow(val_l, val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Float>(result);
		}
			
		case token::Operator::Multiplication: {
			return std::visit([&rightOperand] (auto&& val_l) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val_l)>;

				if constexpr (!std::is_same_v<T, double>)
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						using M = std::decay_t<decltype(val_r)>;

						if constexpr (!std::is_same_v<M, double>)
							return std::make_unique<token::operand::Integer>(val_l * val_r);
						else
							return std::make_unique<token::operand::Float>(val_l * val_r);

				}, rightOperand->getValue());
				else
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						return std::make_unique<token::operand::Float>(val_l * val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());
		}

		case token::Operator::FloatDivision: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return 1.0 * val_l / val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Float>(result);
		}

		case token::Operator::IntegerDivision: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::llrint(val_l) / std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}

		case token::Operator::Mod: {
			return std::visit([&rightOperand] (auto&& val_l) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val_l)>;

				if constexpr (std::is_same_v<T, double>)
					return std::visit([&val_l] (auto&& val_r) { 
						return std::make_unique<token::operand::Float>(std::fmod(val_l, val_r));
				}, rightOperand->getValue());
				else
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						using M = std::decay_t<decltype(val_r)>;

						if constexpr (std::is_same_v<M, double>)
							return std::make_unique<token::operand::Float>(std::fmod(val_l, val_r));
						else
							return std::make_unique<token::operand::Integer>(std::lldiv(val_l, val_r).rem);

					}, rightOperand->getValue());
			}, leftOperand->getValue());
		}

		case token::Operator::Sum: {
			return std::visit([&rightOperand] (auto&& val_l) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val_l)>;

				if constexpr (!std::is_same_v<T, double>)
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						using M = std::decay_t<decltype(val_r)>;

						if constexpr (!std::is_same_v<M, double>)
							return std::make_unique<token::operand::Integer>(val_l + val_r);
						else
							return std::make_unique<token::operand::Float>(val_l + val_r);

					}, rightOperand->getValue());
				else
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						return std::make_unique<token::operand::Float>(val_l + val_r);
					}, rightOperand->getValue());
			}, leftOperand->getValue());
		}

		case token::Operator::Difference: {
			return std::visit([&rightOperand] (auto&& val_l) -> token::operand::Ptr {
				using T = std::decay_t<decltype(val_l)>;

				if constexpr (!std::is_same_v<T, double>)
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						using M = std::decay_t<decltype(val_r)>;

						if constexpr (!std::is_same_v<M, double>)
							return std::make_unique<token::operand::Integer>(val_l - val_r);
						else
							return std::make_unique<token::operand::Float>(val_l - val_r);

					}, rightOperand->getValue());
				else
					return std::visit([&val_l] (auto&& val_r) -> token::operand::Ptr { 
						return std::make_unique<token::operand::Float>(val_l - val_r);
					}, rightOperand->getValue());
			}, leftOperand->getValue());
		}

		case token::Operator::LeftBitshift: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					if (static_cast<long long>(std::log2(val_l)) + static_cast<long long>(std::log2(val_r)) > 63ll)
						return std::llrint(val_l) << (std::llrint(val_r) && 63ll);
					return std::llrint(val_l) << std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}

		case token::Operator::RightBitshift: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::llrint(val_l) >> std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}

		case token::Operator::Equality: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l == val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::Inequality: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l != val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::LessThan: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l < val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::LessThanEqual: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l <= val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::GreaterThan: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l > val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::GreaterThanEqual: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l >= val_r;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::And: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::llrint(val_l) & std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}

		case token::Operator::AndAlso: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l != 0 && val_r != 0;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::Or: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::llrint(val_l) | std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}

		case token::Operator::OrElse: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return val_l != 0 || val_r != 0;
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Boolean>(result);
		}

		case token::Operator::Xor: {
			auto result = std::visit([&rightOperand] (auto&& val_l) {
				return std::visit([val_l] (auto&& val_r) {
					return std::llrint(val_l) ^ std::llrint(val_r);
				}, rightOperand->getValue());
			}, leftOperand->getValue());

			return std::make_unique<token::operand::Integer>(result);
		}
	}
}
