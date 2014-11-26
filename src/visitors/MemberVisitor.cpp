#include "MemberVisitor.hpp"

void MemberVisitor::visit(shptr<const ast::Node> node) {

}

void MemberVisitor::visit(shptr<const ast::MethodDeclaration> node) {
	function_graph = firmInterface.generateMethod(owner, node);
}

void MemberVisitor::visit(shptr<const ast::FieldDeclaration> node) {

}

ir_graph *MemberVisitor::getGraph() const {
	return function_graph;
}