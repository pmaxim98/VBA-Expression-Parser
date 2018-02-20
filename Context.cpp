#include "Context.h"

Context::Context()
	:
	lastToken(TokenType::Operator),
	parenthesesOpen(0)
{ }
