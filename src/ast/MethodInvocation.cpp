#include "../globals.hpp"
#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(uptr<Ident> method_name, uptr<Arguments> arguments)
	: method_name(std::move(method_name)), arguments(std::move(arguments))
{

}

void ast::MethodInvocation::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << '.';
	method_name->toString(out, indent);
	arguments->toString(out, indent);
}
