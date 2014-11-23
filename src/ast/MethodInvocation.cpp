#include "../globals.hpp"
#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(shptr<Ident> method_name, shptr<Arguments> arguments)
	: method_name(method_name), arguments(arguments)
{

}

void ast::MethodInvocation::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << '.';
	method_name->toString(out, indent);
	arguments->toString(out, indent);
}

shptr<ast::Type> ast::MethodInvocation::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<ast::Type> callingType) const
{
	if (callingType->isClassType())
	{
		auto class_table = sa.getClassTable();
		auto class_item = class_table[callingType->getClassName()];
		auto method_table = class_item.methodTable->getMethodTable();
		auto method_it = method_table.find(method_name->getName());

		if (method_it != method_table.end())
		{
			auto method_item = method_it->second;
			shptr<vec<shptr<ast::Type>>> declarationTypes = method_item.parameterTypes;
			shptr<vec<shptr<ast::Expression>>> invokedExpressions = arguments->getArgumentTypes();

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
					if (*decType != *invType)
					{
						validArguments = false;
						break;
					}

					decIt++;
					invIt++;
				}

				if (validArguments)
					return method_item.returnType;
				else
					sa.printError("The arguments do not match the parameter types.", method_name);
			}
			else
				sa.printError("Wrong number of arguments.", method_name);
		}
		else
		{
			sa.printError(callingType->getName() + " has no method with the name " + method_name->getName(),
			              method_name);
		}
	}
	else
		sa.printError("Cannot invoke a method on a primitive or array type.", method_name);


	return shptr<ast::Type>();
}