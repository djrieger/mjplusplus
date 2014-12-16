#include "Tarval.hpp"

namespace firm
{
	Tarval::Tarval(ir_tarval* tarval): tarval(tarval) {}

	Tarval::Tarval(long value): tarval(new_tarval_from_long(value, mode_Is)) {}

	BadTarval::BadTarval(): Tarval(tarval_bad)
	{

	}

	UnknownTarval::UnknownTarval(): Tarval(tarval_unknown)
	{

	}

	bool Tarval::isModeIs() const
	{
		return get_tarval_mode(tarval) == mode_Is;
	}

	long Tarval::getLong() const
	{
		return get_tarval_long(tarval);
	}

	void Tarval::setValue(ir_tarval* tarval)
	{
		this->tarval = tarval;
	}

}