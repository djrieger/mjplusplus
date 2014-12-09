#include "FirmVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		void FirmVisitor::setOwner(ir_type* owner)
		{
			this->owner = owner;
		}

		FirmVisitor::FirmVisitor()
		{

		}

		ir_type* FirmVisitor::getOwner() const
		{
			return this->owner;
		}
	}
}
