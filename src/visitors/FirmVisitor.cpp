#include "FirmVisitor.hpp"

void FirmVisitor::setOwner(ir_type* owner)
{
	this->owner = owner;
}

FirmVisitor::FirmVisitor(FirmInterface& firmInterface): firmInterface(firmInterface)
{

}