#ifndef TARVAL_HPP
#define TARVAL_HPP

#include <libfirm/firm.h>
#include "../globals.hpp"

namespace firm
{
	class Node;

	class Tarval {
		protected:
			ir_tarval* tarval;
		public:
			Tarval(ir_tarval* tarval);
			Tarval(long value);
			Tarval() {}
			bool isModeIs() const;
			long getLong() const;
			void setValue(ir_tarval* tarval);
			operator ir_tarval*() { return tarval; }
			bool operator==(Tarval const& otherTarval) const { return tarval == otherTarval.tarval; }
			bool operator==(ir_tarval * otherTarval) const { return tarval == otherTarval; }
			bool operator!=(Tarval const& otherTarval) const { return !operator==(otherTarval); }
			bool operator!=(ir_tarval * otherTarval) const { return !operator==(otherTarval); }
	};

	class BadTarval : public Tarval {
		public:
			BadTarval();		
	};

	class UnknownTarval : public Tarval {
		public:
			UnknownTarval();		
	};	
}
#endif