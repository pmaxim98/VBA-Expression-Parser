#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cassert>

#include "Context.h"
#include "Variable.h"
#include "Float.h"
#include "Integer.h"
#include "Boolean.h"
#include "Operator.h"
#include "Utils.h"

void processTopOperation(std::stack<token::Operator>& operators, std::stack<token::operand::Ptr>& operands)
{
	auto currentOperator = operators.top();
	operators.pop();
	auto currentOperand = operands.top();
	operands.pop();

	if (currentOperator.arity <= 1)
		operands.push(currentOperator.compute(currentOperand));
	else {
		auto prevOperand = operands.top();
		operands.pop();
		operands.push(currentOperator.compute(prevOperand, currentOperand));
	}
}

std::vector<token::Variable> parseVariables(std::string_view expression)
{
	expression = utils::str::skipWhitespace(expression);

	std::vector<token::Variable> vars;

	for (token::Variable variable; auto parsed = variable.parse(expression); expression = parsed.value())
		vars.push_back(variable);

	return vars;
}

std::shared_ptr<token::operand::Operand> parseStatement(std::string_view expression, const std::vector<token::Variable>& vars)
{
	std::stack<token::Operator> operators;
	std::stack<token::operand::Ptr> operands;
	Context context;

	if (const auto expressionStart = expression.find(';'); expressionStart != std::string::npos)
		expression.remove_prefix(expressionStart + 1);
	expression = utils::str::skipWhitespace(expression);

	for (std::optional<std::string_view> parsed; !expression.empty(); expression = parsed.value())
	{
		if (token::Operator currentOperator; parsed = currentOperator.parse(expression, context)) {
			if (currentOperator.type == token::Operator::Type::RightParanthesis) {
				while (operators.top().type != token::Operator::Type::LeftParanthesis)
					processTopOperation(operators, operands);

				operators.pop();
			}
			else {
				while (currentOperator.arity > 1 
					   && !operators.empty() 
					   && currentOperator.precedence >= operators.top().precedence 
					   && operators.top().type != token::Operator::Type::LeftParanthesis)
				{
					processTopOperation(operators, operands);
				}

				operators.push(currentOperator);
			}

			continue;
		}

		if (token::operand::Boolean currentBool; parsed = currentBool.parse(expression, context)) {
			operands.push(std::make_unique<token::operand::Boolean>(currentBool));
			continue;
		}

		if (token::operand::Float currentFloat; parsed = currentFloat.parse(expression, context)) {
			operands.push(std::make_unique<token::operand::Float>(currentFloat));
			continue;
		}

		if (token::operand::Integer currentInt; parsed = currentInt.parse(expression, context)) {
			operands.push(std::make_unique<token::operand::Integer>(currentInt));
			continue;
		}

		if (token::Variable currentVariable; parsed = currentVariable.parseAlias(expression, context)) {
			auto found = std::find_if(std::begin(vars), std::end(vars),
											[&currentVariable] (const token::Variable& var) { return currentVariable.alias == var.alias; });

			if (found != std::end(vars))
				operands.push(found->operand);

			continue;
		}
	}

	while (!operators.empty())
		processTopOperation(operators, operands);

	return operands.top();
}

std::string evaluate(const std::string& expression) 
{
	std::shared_ptr<token::operand::Operand> result;

	try {
		const std::vector<token::Variable> vars = parseVariables(expression);
		result = parseStatement(expression, vars);
	}
	catch (const std::exception& e) {
		return "Error(s):\n\n" + std::string(e.what()) + "\n";
	}

	return result->toString();
}

void tests()
{
	//	Integers
	assert(evaluate("0") == "0");
	assert(evaluate("-0") == "0");
	assert(evaluate("1234") == "1234");
	assert(evaluate("-9999") == "-9999");
	assert(evaluate("0") == "0");
	assert(evaluate("2147483648") == "2147483648");
	assert(evaluate("4294967295") == "4294967295");
	assert(evaluate("4728542116258799982") == "4728542116258799982");
	assert(evaluate("9223372036854775807") == "9223372036854775807");
	assert(evaluate("-9223372036854775807") == "-9223372036854775807");

	//	Floats
	assert(evaluate("-0.0") == "0");
	assert(evaluate("0.0") == "0");
	assert(evaluate("9.0") == "9");
	assert(evaluate("-0.0001") == "-0,0001");
	assert(evaluate("-0.00001") == "-1E-05");
	assert(evaluate("-0.0000001") == "-1E-07");
	assert(evaluate("0.0000000000025") == "2,5E-12");
	assert(evaluate("123456789.0") == "123456789");
	assert(evaluate("1000000000000000000000000000.0000123") == "1E+27");
	assert(evaluate("-1124124.253452645745729995600000001") == "-1124124,25345265");

	//	Power
	assert(evaluate("2 ^ -3 ^ 4") == "4,13590306276514E-25");
	assert(evaluate("3 * (((4 ^ 2.3) ^ 9.1) ^ -3.512)") == "1,66725950718577E-44");
	assert(evaluate("2^8 - 3 * -4.1 ^ 2.3 ^ 9.1 ^ -3.512 + -4 ^ -3") == "255,984375");
	assert(evaluate("4 ^ True ^ False + True ^ False + False") == "2");
	assert(evaluate("(1.25 ^ 1.333005) + (0.5 ^ 3.5) + (-4.5 * -2) - (9.5 ^ 0.123456789) ^ 2 -2^3^4^0.2^-23") == "8,691346377692");

	//	Multiplication
	assert(evaluate("5*5*5*5*5-2*2*2*2*2+3*3*3*3*3") == "3336");
	assert(evaluate("3.2 * 2 + 3 * 3.5 + 3.8 * 2 + 4 * 3.7 + 2 * 9.3 + 5 * 9.5 + 9.8 * 7") == "174");
	assert(evaluate("0.1234 * 0.5678 - 0.0004 * 5 * 13.31 + True * True + False*False + False*True + True*False") == "1,04344652");
	assert(evaluate("True*True*12*0.5*2.5*1.5*1*0.0001*True+2*False-100000*100") == "-10000000,00225");
	assert(evaluate("7000 * 0.00000001 * 1000000 * 0.01111111 + 9.999999 * 2 * 3 * 4") == "240,7777537");

	//	Float Division
	assert(evaluate("5 / 3 + 1023 / 324234 - 124124 / 433 + 1000000.0235 / 32414 + 123 / 1 / 2 / 3 / -3 / -5/-9") == "-254,291670505267");
	assert(evaluate("0.123 / 50034.00000001 + 5.5 / 2.5 + 123.5 - 92312 / 4124 - 2/-4/-5/-9/-20") == "103,315353789291");
	assert(evaluate("1234567890123456.123456789101112 / 2.123456789 / -5235235 / 123 / 2 /1 / 0.9999") == "-451485,38129948");
	assert(evaluate("1234567890123456.123456789101112 / 2.123456789 / -5235235 / 123 / 2 /1 / 0.9999") == "-451485,38129948");
	assert(evaluate("-0.5 / 1.5 / 2.5 / 3.5 + 1000 / 10000 + 20000 / 200000 + 300000000/0.0000001") == "3E+15");

	//	Integer Division
	assert(evaluate("3.1 \\ 2 + 3.6 \\ 2 + 3.5 \\ 2 + 9.3 \\ 3 + 9.5 \\ 3 + 9.99 \\ 3") == "14");
	assert(evaluate("3 \\ 2.5 + 3 \\ 2.2 + 3 \\ 2.7 + 9 \\ 3.3 + 9 \\ 3.5 + 9 \\ 3.8") == "10");
	assert(evaluate("3 / 2.5 * 3 \\ 2.2 + 3 / 2.7 * 9 \\ 3.3 + 9 / 3.5 * 9 \\ 3.8") == "10");
	assert(evaluate("(3.5 ^ -4) - (123.4567 - 10 ^ 2 / 3) \\ 1.42 - 3 * 2.546 + 312 / (3 / 2.3) \\ 1.34 + 0.123") == "141,491663890046");
	assert(evaluate("123 \\ 2 \\ 1.5 + 53.5 \\ 4.5 - 2 \\ 4 + 0 \\ 4") == "43");

	//	Mod
	assert(evaluate("4.7 Mod 2.4 Mod 3 Mod True Mod -25") == "0,3");
	assert(evaluate("-4.3 Mod -9 Mod 2.5 Mod True Mod 0.987654321") == "-0,8");
	assert(evaluate("-4.5 Mod -3.5 Mod 2 Mod 1.5") == "-1");
	assert(evaluate(" -+--+123 Mod -+-(+-++(2)+--++ +(+-1)+ + -24)+   -2^3 ") == "-23");
	assert(evaluate("124215.55 Mod 1234.5 + 0.5 Mod -1.5 - 2.5 Mod 9.0 + (-+3 Mod 1.5)") == "763,550000000003");
	assert(evaluate("True Mod 3.5 + True Mod 2 + True Mod -3 + True Mod 123 + True Mod 2.5 * True Mod 125.5 + True Mod -0.5") == "-5");
	assert(evaluate("1.54 Mod 7.5 ^ 3 - 2 - (-3) * (0.34) / 2.55 Mod 3.7 + (2.1 * 3) Mod 2") == "0,240000000000001");
	
	//	Left Bitshift
	assert(evaluate("3 << 2 + 3.2 << 2 + 3.5 << 2 + 3.7 << 2 - 2 << 2.3 - 2 << 2.5 - 2 << 2.7 + 2 << 4 - 4.5 << 3.5") == "201326592");
	assert(evaluate("0.005 << 0.2 + 4 << 2 - 0.1234 << 9.5534 + 123124125 << 0.1 << 0.0005") == "0");

	//	Right Bitshift
	assert(evaluate("(2383.5 >> 2 + 2) + (123.3 >> 3) - (2451.8 >> 3.4) + (394.3 >> 3.6)") == "-118");
	assert(evaluate("(123 >> 2.5 >> 0.51234 >> 0.3) + (124125523 >> 2 >> 3.7 >> 1.231245) - (12315.5 >> 3.5123 >> 0.99)") == "969361");

	//	Comparisons
	assert(evaluate("(3 / 5 = 6 / 10) = (9 / 15 = 0.6)") == "True");
	assert(evaluate("4 / 9 < 2.4 > 1.3 <= 424 >= 1 <> 32 = 12 <> 1 <= 312 >= 23 < 1 < 2 > 3 <> False > True") == "True");
	assert(evaluate("False <> True > False < True < False = False > True = False < False") == "True");

	//	Not
	assert(evaluate("Not -0.12345 + Not Not 3.4 + Not Not Not 2 - Not -3 + 2 + (Not 9) + Not (-3.123) + Not -3 / Not 2 + Not False + Not True") == "-6");
	assert(evaluate("Not Not Not Not -4 * 3 + (Not 5.2 / 3.5 * Not 1.02) - Not 1 \\ 3 / 5 + (Not Not 0) + Not (0.1 - 0.5)") ==  "-9");
	assert(evaluate("Not ((Not True + Not False) - Not Not Not False + Not Not True * Not True) + Not Not False * True") == "-3");
	assert(evaluate("(Not 1.25) - (Not -2.25) + (Not 0.5) * (Not -32.5) + (Not -2.5) + (Not 3.5) + (Not 12.4)") == "-51");

	//	And
	assert(evaluate("3.5 And 2.5 + 3.6 And 2 - 3.2 And 4 + 2.5 And 4 + 2.3 And 7 + 3 And 2.8 + 2.3 And 9.6 And 5.5") == "0");
	assert(evaluate("(3.5 And 2.5) + (3.6 And 2 - 3.2 And 4) + (2.5 And 4) + ((2.3 And 7) + (3 And 2.8)) + (2.3 And 9.6 And 5.5)") == "11");

	//	Or
	assert(evaluate("True And 3 * 5 And 23 ^ 1.2 Or -32 Or 0.0 Or True - 2 And False And True + 2 Or True") == "-1");
	assert(evaluate("False Or (True And True) Or ((False And True) + (False And False) + True Or False Or False) - True * False") == "-1");

	//	AndAlso	/ OrElse
	assert(evaluate("3.5 AndAlso 1.2 OrElse 0.1 + (False OrElse -0.5) + True AndAlso True OrElse False + True") == "True");
	assert(evaluate("(0.1234 AndAlso -234.123 AndAlso 0.0023) + 0.5 OrElse 1.4 + -24 OrElse -2 OrElse 0 + (124 AndAlso -0.123)") == "True");

	//	Xor
	assert(evaluate("2.1 Xor 3 Xor 1 + 2.5 Xor 3 + 2.8 Xor 4 + 3.4 Xor 2 + 3.5 Xor 9 + 3.9 Xor 4 + 3.4 Xor 2.7 - 3.5 Xor 9.5") == "-3");
	assert(evaluate("(0.25 Xor -0.123) Xor 0.67 + (0.23 Xor -9.02) Xor (-0.5 Xor 0.5) Xor 1.4 - (123 Xor 321)") == "319");

	//	Variables
	assert(evaluate("x = 3 y=5; -x * y + 3") == "-12");
	assert(evaluate("x=-9 y=45 z=1000 x1=3 xx=4.5 yy = False; y / x + z + (x1 + x + x1 \\ xx) - xx And yy OrElse yy") == "False");
	assert(evaluate("x1 = 5 x2 = 3 y1 = 1; (x1+x2*2)+y1*2+Sin(y1-1)") == "13");
	assert(evaluate("x = 5 y=20 a = 24 ; Cos(a * y >> 2.3) ^ (x / Abs(Not 5))") == "0,842559907342032");

	//	Functions
	assert(evaluate("Sin(30)") == "-0,988031624092862");
	assert(evaluate("Sin(30) + Cos(0.25) - Abs(3.0 + True) + Acos(0.53) * Atan(0.93) + Ceiling(0.2) -"
					"Floor(5.8) + Exp(2.2) + Round(16) + Log10(3 + 5 / 4) + Sqrt(4) - Truncate(23.98)") == "-0,607435759156953");
}

int main()
{
	tests();
	return 0;
}
