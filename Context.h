#ifndef CONTEXT_H
#define CONTEXT_H

struct Context
{
	enum class TokenType { Operator, Operand, Variable };

	Context();

	TokenType lastToken;
	int parenthesesOpen;
};

#endif