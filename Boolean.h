#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "Operand.h"

namespace token::operand 
{
	class Boolean : public Operand
	{
		public:
			Boolean();
			explicit Boolean(bool value);

			std::optional<std::string_view> parse(std::string_view expression, Context& context) override;
			Type getValue() const override;
			std::string toString() const override;

		private:
			bool value;
	};
}

#endif