#include "FirmNodeVisitor.hpp"
#include <iostream>
void FirmNodeVisitor::visit(ir_node* node)
{
	std::cout << "visiting node at " << node << "\tof type ";

	if (is_Const(node))
	{
		std::cout << "CONST";
		visitConstNode(node);
	}
	else if (is_Add(node))
	{
		std::cout << "ADD";
		visitAddNode(node);
	}

	std::cout << std::endl;
	// TODO: Implement remaining node types
	// TODO: Replace if cascade with a switch/case
}

void FirmNodeVisitor::visitConstNode(ir_node* node)
{

}

void FirmNodeVisitor::visitAddNode(ir_node* node)
{

}