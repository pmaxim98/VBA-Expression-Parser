#ifndef INTEGER_H
#define INTEGER_H

#include "Operand.h"

namespace token::operand 
{
	class Integer : public Operand
	{
		public:
			Integer();
			explicit Integer(long long value);

			std::optional<std::string_view> parse(std::string_view expression, Context& context) override;
			Type getValue() const override;
			std::string toString() const override;

		private:
			long long value;
	};
}

#endif