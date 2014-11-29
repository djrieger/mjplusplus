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

shptr<ast::Type> ast::MethodInvocation::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<ast::Type> callingType)
{
	if (callingType->isClassType())
	{
		auto class_table = sa.getClassTable();
		auto class_it = class_table.find(callingType->getClassName());

		if (class_it != class_table.end())
		{
			auto& class_item = class_it->second;
			auto method_table = class_item.methodTable->getMethodTable();
			auto method_it = method_table.find(method_name->getName());

			if (method_it != method_table.end())
			{
				auto method_item = method_it->second;
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
						this->callTarget = method_item.methodNode;
						return method_item.returnType;
					}
					else
						sa.reportError("Arguments do not match parameter types.", method_name);
				}
				else
					sa.reportError("Wrong number of arguments.", method_name);
			}
			else
			{
				sa.reportError("$type{" + callingType->getName() + "} has no method named $ident{" + method_name->getName() + "}",
				               method_name);
			}
		}
		else
			sa.reportError("No such class: $type{" + callingType->getClassName() + "}", method_name);
	}
	else
		sa.reportError("Cannot invoke a method on a primitive or array type.", method_name);


	return shptr<ast::Type>();
}

shptr<const ast::MethodDeclaration> ast::MethodInvocation::getTarget() const
{
	return this->callTarget;
}

bool ast::MethodInvocation::lValueHelp() const
{
	return false;
}

void ast::MethodInvocation::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<MethodInvocation const>(shared_from_this()));
}
