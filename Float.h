#ifndef FLOAT_H
#define FLOAT_H

#include "Operand.h"

namespace token::operand 
{
	class Float : public Operand
	{
		public:
			Float();
			explicit Float(double value);

			std::optional<std::string_view> parse(std::string_view expression, Context& context) override;
			Type getValue() const override;
			std::string toString() const override;

		private:
			double value;
	};
}

#endif