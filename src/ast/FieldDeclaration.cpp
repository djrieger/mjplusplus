#include "../globals.hpp"
#include "FieldDeclaration.hpp"
#include "../lexer/token.hpp"

ast::FieldDeclaration::FieldDeclaration(shptr<TypeIdent> type_and_name) : type_and_name(type_and_name)
{

}

void ast::FieldDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	out << "public ";
	type_and_name->toString(out, indent);
	out << ";\n";
}

std::string ast::FieldDeclaration::getName() const
{
	return '%' + type_and_name->getName();
}

void ast::FieldDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const
{
	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.reportError("Field with name $ident{" + type_and_name->getName() + "} already declared.", type_and_name->getIdent());

	auto type = type_and_name->getType();
	auto primitiveType = type->getPrimitiveType();

	// We have a reference type. Find corresponding class in class table:
	if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(type->getClassName()) == sa.getClassTable().end())
		sa.reportError("Type $type{" + type->getClassName() + "} used as field type undeclared.", type->getClassNameIdent());

	if (primitiveType == Type::Primitive_type::VOID)
		sa.reportError("Cannot have a field with void as base type.", type_and_name->getIdent());

	// insert this field into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, type);
	symbolTable->insert(symbol, definition);

	// insert this field into the method table in the class table
	auto ct = sa.getClassTable();
	auto const foo = shared_from_this();
	auto fd_node = std::static_pointer_cast<const FieldDeclaration>(foo);
	ct[class_name].fieldTable->insertField(type_and_name->getName(), fd_node, type);
}

void ast::FieldDeclaration::analyze(SemanticAnalysis&, shptr<SymbolTable>) const
{
	/* does nothing */
}

void ast::FieldDeclaration::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<FieldDeclaration const>(shared_from_this()));
}

shptr<ast::Type> ast::FieldDeclaration::getType() const
{
	return this->type_and_name->getType();
}
