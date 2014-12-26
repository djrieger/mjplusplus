#ifndef TARVAL_HPP
#define TARVAL_HPP

#include <libfirm/firm.h>
#include "../globals.hpp"

namespace firm
{
	class Node;

	class Tarval
	{
		protected:
			ir_tarval* tarval;
		public:
			Tarval(ir_tarval* tarval);
			Tarval(long value, ir_mode* mode = mode_Is);
			Tarval();
			ir_mode* getMode() const;
			bool isModeIs() const;
			bool isModeLu() const;
			bool isModeBu() const;
			bool isNumeric() const;
			bool isNumericOrBool() const;
			bool isBad() const;
			bool isUnknown() const;
			long getLong() const;
			bool isNumWithVal(long val) const;
			void setValue(ir_tarval* tarval);
			operator ir_tarval* ()
			{
				return tarval;
			}
			operator bool ()
			{
				return tarval != NULL;
			}
			bool operator==(Tarval const& otherTarval) const
			{
				return tarval == otherTarval.tarval;
			}
			bool operator==(ir_tarval* otherTarval) const
			{
				return tarval == otherTarval;
			}
			bool operator!=(Tarval const& otherTarval) const
			{
				return !operator==(otherTarval);
			}
			bool operator!=(ir_tarval* otherTarval) const
			{
				return !operator==(otherTarval);
			}
	};

	class BadTarval : public Tarval
	{
		public:
			BadTarval();
	};

	class UnknownTarval : public Tarval
	{
		public:
			UnknownTarval();
	};
}
#endif