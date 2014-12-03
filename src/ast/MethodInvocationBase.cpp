#include "MethodInvocationBase.hpp"
#include "Arguments.hpp"

ast::MethodInvocationBase::MethodInvocationBase(shptr<Ident> method_name, shptr<Arguments> arguments)
	: method_name(method_name), arguments(arguments)
{

}

std::string const& ast::MethodInvocationBase::getIdentifier() const
{
	return method_name->getName();
}

shptr<ast::Arguments const> ast::MethodInvocationBase::getArguments() const
{
	return arguments;
}


shptr<ast::MethodDeclaration const> ast::MethodInvocationBase::getDeclaration() const
{
	return declaration;
}

shptr<ast::Type> ast::MethodInvocationBase::performTypeChecks(shptr<ast::Ident> method_ident, MethodTable::MethodTableItem const& method_item, SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	declaration = method_item.methodNode;
	shptr<vec<shptr<ast::Type>>> declarationTypes = method_item.parameterTypes;
	shptr<vec<shptr<ast::Expression>>> invokedExpressions = arguments->getArgumentExpressions();

	int decSize = declarationTypes->size();
	int invSize = invokedExpressions->size();

	if (decSize == invSize)
	{
		bool validArguments = true;
		auto decIt = declarationTypes->begin();
		auto invIt = invokedExpressions->begin();

		for (int i = 0; i < invSize; i++)
		{
			auto decType = *decIt;
			auto invType = (*invIt)->get_type(sa, symbolTable);

			//TODO: check if invType is non-empty pointer
			if (!invType || !(*decType == *invType ||
			                  (decType->isRefType() && invType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE)))
			{
				validArguments = false;
				break;
			}

			decIt++;
			invIt++;
		}

		if (validArguments)
		{
			//this->callTarget = method_item.methodNode;
			return method_item.returnType;
		}
		else
			sa.reportError("Arguments do not match parameter types.", method_ident);
	}
	else
		sa.reportError("Wrong number of arguments.", method_ident);

	return shptr<ast::Type>();
}
