#include "FirmVisitor.hpp"

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