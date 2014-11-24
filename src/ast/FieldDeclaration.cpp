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
		sa.printError("Field with name \033[1m" + type_and_name->getName() + "\033[0m already declared.", type_and_name->getIdent());

	auto type = type_and_name->getType();
	auto primitiveType = type->getPrimitiveType();

	// We have a reference type. Find corresponding class in class table:
	if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(type->getClassName()) == sa.getClassTable().end())
		sa.printError("Type \033[1m" + type->getClassName() + "\033[0m used as field type undeclared.", type->getClassNameIdent());

	if (primitiveType == Type::Primitive_type::VOID)
		sa.printError("Cannot have a field with void as base type.");

	// insert this field into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, type);
	symbolTable->insert(symbol, definition);

	// insert this field into the method table in the class table
	auto ct = sa.getClassTable();
	//shptr<FieldDeclaration> fd_node;
	//fd_node.reset(this->enable_shared_from_this());
	auto const foo = shared_from_this();
	auto fd_node = std::static_pointer_cast<const FieldDeclaration>(foo);
	//	const std::shared_ptr<FieldDeclaration> fd_node = this; //auto fd_node = std::make_shared<FieldDeclaration>(this);
	//fd_node.reset(this);
	ct[class_name].fieldTable->insertField(type_and_name->getName(), fd_node, type);


	//	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	//	// check if a field with the same name already exists
	//	if (symbolTable->definedInCurrentScope(symbol))
	//		sa.printError("Field with name \033[1m" + type_and_name->getName() + "\033[0m already declared.", type_and_name->getIdent());

	//	auto type = type_and_name->getType();
	//	auto primitiveType = type->getPrimitiveType();

	//	if (primitiveType == Type::Primitive_type::VOID)
	//		sa.printError("Field " + type_and_name->getName() + " cannot have type void.", type_and_name->getIdent());
	//	else
	//	{
	//		if (primitiveType == Type::Primitive_type::NONE)
	//		{
	//			// We have a reference type. Find corresponding class in class table:
	//			auto iter = sa.getClassTable().find(type->getClassName());

	//			// not in class table:
	//			if (iter == sa.getClassTable().end())
	//				sa.printError("Type " + type->getClassName() + " undeclared.", type->getClassNameIdent());
	//		}
	//	}

	//	auto definition = std::make_shared<Definition>(symbol, type);
	//	symbolTable->insert(symbol, definition);
}

void ast::FieldDeclaration::analyze(SemanticAnalysis&, shptr<SymbolTable>) const
{
	/* does nothing */
}
