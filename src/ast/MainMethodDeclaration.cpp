#include "../globals.hpp"
#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"
#include "../firm_interface/FirmInterface.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent> > > parameters, shptr<Block> block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

void ast::MainMethodDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	// Prints this method including the "static" modifier
	MethodDeclaration::toString(out, indent, true);
}



void ast::MainMethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const
{
	auto symbol = Symbol::makeSymbol(this->getNameForSort(), shptr<Scope>());

	if (return_type_and_name->getName() != "main")
		sa.reportError("Main method has name $ident{" + return_type_and_name->getName() + "} instead of 'main'.", return_type_and_name->getIdent());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.reportError("Method with name $ident{" + return_type_and_name->getName() + "} already declared.", return_type_and_name->getIdent());

	auto returnType = return_type_and_name->getType();//type is void

	//	symbolTable->enterScope();
	//	// foo is not necessary as the main method is not inserted into the method table
	//	auto foo = collectParameters(sa, symbolTable);
	//	symbolTable->leaveScope();

	// insert this method into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, nullptr);
	symbolTable->insert(symbol, definition);

	// insert this method into the method table in the class table
	auto ct = sa.getClassTable();
	//shptr<MethodDeclaration> md_node;
	//md_node.reset(this->enable_shared_from_this());
	auto const foo = shared_from_this();
	auto md_node = std::static_pointer_cast<const MethodDeclaration>(foo);
	//	const std::shared_ptr<MethodDeclaration> md_node = this; //auto md_node = std::make_shared<MethodDeclaration>(this);
	//md_node.reset(this);
	auto param_types = std::make_shared<vec<shptr<ast::Type>>>();

	lexer::Token dit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("void"), { -1, 0}};
	auto p = std::make_shared<Ident>(dit);
	param_types->push_back(std::make_shared<ast::Type>(p));
	ct[class_name].methodTable->insertMethod(return_type_and_name->getName(), md_node, returnType, param_types);
}

shptr<vec<shptr<ast::Type>>> ast::MainMethodDeclaration::collectParameters(SemanticAnalysis&, shptr<SymbolTable> symbolTable) const
{
	//we know we only have one parameter, which is of a pseudo-type String[]
	auto parameter = (*parameters)[0];
	auto paramSymbol = Symbol::makeSymbol(parameter->getName(), shptr<Scope>());
	auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter);
	symbolTable->insert(paramSymbol, paramDefinition);
	auto params_type = std::make_shared<vec<shptr<ast::Type>>>();
	params_type->push_back(parameter->getType());
	return params_type;
}

void ast::MainMethodDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	//std::cout << "copying " << return_type_and_name->getName() << std::endl;
	symbolTable->enterScope();
	auto ts = Symbol::makeSymbol("this");
	auto td = shptr<Definition>();
	symbolTable->insert(ts, td);

	auto s = Symbol::makeSymbol("return", symbolTable->getCurrentScope());
	auto d = std::make_shared<Definition>(s, return_type_and_name);
	symbolTable->insert(s, d);
	//collectParameters(sa, symbolTable);
	auto parameter = (*parameters)[0];
	auto paramSymbol = Symbol::makeSymbol(parameter->getName(), shptr<Scope>());
	lexer::Token dit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("void"), { -1, 0}};
	auto p = std::make_shared<Ident>(dit);
	auto param_type = std::make_shared<ast::Type>(p);
	auto param_type_ident = std::make_shared<TypeIdent>(param_type, parameter->getIdent());
	auto paramDefinition = std::make_shared<Definition>(paramSymbol, param_type_ident);
	symbolTable->insert(paramSymbol, paramDefinition);

	auto system_s = Symbol::makeSymbol("System");

	if (!symbolTable->definedInCurrentScope(system_s))
	{
		auto system_d = std::make_shared<Definition>(system_s, SemanticAnalysis::systemTypeIdent);
		symbolTable->insert(system_s, system_d);
	}

	if (!block)
	{
		if (*return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
			sa.reportError("Method $ident{" + return_type_and_name->getName() + "} returns non-void but body is empty", return_type_and_name->getIdent());
	}
	else if (!block->analyze(sa, symbolTable) && *return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
		sa.reportError("Method $ident{" + return_type_and_name->getName() + "} returns non-void but not all paths return", return_type_and_name->getIdent());

	symbolTable->leaveScope();
	//std::cout << "done" << std::endl;
}


void ast::MainMethodDeclaration::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<MainMethodDeclaration const>(shared_from_this()));
}

std::string ast::MainMethodDeclaration::mangle(std::string) const
{
	return "main";
}

shptr<std::map<std::string, int>> ast::MainMethodDeclaration::setVariablePositions() const
{

	auto var2pos = std::make_shared<std::map<std::string, int>>();
	FirmInterface::getInstance().setVarMap(var2pos);
	int pos = 0;

	if (block)
		pos = block->setVariablePositions(pos);

	return var2pos;
}
