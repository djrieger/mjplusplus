#include "Tarval.hpp"

namespace firm
{
	Tarval::Tarval(ir_tarval* tarval): tarval(tarval) {}

	Tarval::Tarval(long value, ir_mode* mode): tarval(new_tarval_from_long(value, mode)) {}

	Tarval::Tarval(): tarval(NULL) {}

	BadTarval::BadTarval(): Tarval(tarval_bad)
	{

	}

	UnknownTarval::UnknownTarval(): Tarval(tarval_unknown)
	{

	}
	ir_mode* Tarval::getMode() const
	{
		return get_tarval_mode(tarval);
	}

	bool Tarval::isModeIs() const
	{
		return get_tarval_mode(tarval) == mode_Is;
	}

	bool Tarval::isModeLu() const
	{
		return get_tarval_mode(tarval) == mode_Lu;
	}

	bool Tarval::isModeBu() const
	{
		return get_tarval_mode(tarval) == mode_Bu;
	}

	bool Tarval::isNumeric() const
	{
		return get_mode_arithmetic(get_tarval_mode(tarval)) == irma_twos_complement;
	}

	bool Tarval::isBad() const
	{
		return tarval == tarval_bad;
	}

	bool Tarval::isUnknown() const
	{
		return tarval == tarval_unknown;
	}

	bool Tarval::isNumericOrBool() const
	{
		return isNumeric() || isModeBu();
	}

	long Tarval::getLong() const
	{
		return get_tarval_long(tarval);
	}

	void Tarval::setValue(ir_tarval* tarval)
	{
		this->tarval = tarval;
	}

	bool Tarval::isNumWithVal(long val) const
	{
		return isNumeric() && getLong() == val;
	}

	std::string Tarval::toString() const
	{
		if (isNumericOrBool())
		{
			std::stringstream s;
			s << getLong();
			return s.str();
		}
		else if (isBad())
			return "Bad";
		else if (isUnknown())
			return "Unknown";
		else
			throw "Error";
	}
}
