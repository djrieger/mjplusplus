#include "MainMethodDeclaration.hpp"

ast::MainMethodDeclaration::MainMethodDeclaration(Statement& block)
	: MethodDeclaration(block)
{
	;
}

std::string ast::MainMethodDeclaration::toString()
{
	std::string ret = "public static void main(String args[])\n";
	ret += block.toString();

	return ret;
}


