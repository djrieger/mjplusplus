#include "FirmNodeVisitor.hpp"

void FirmNodeVisitor::visit(ir_node* node) 
{
	if (is_Const(node))
		visitConstNode(node);
	else if (is_Add(node))
		visitAddNode(node);
	// TODO: Implement remaining node types
	// TODO: Replace if cascade with a switch/case
}

void FirmNodeVisitor::visitConstNode(ir_node* node)
{

}

void FirmNodeVisitor::visitAddNode(ir_node* node)
{

}