#include "MainMethodDeclaration.hpp"

ast::MainMethodDeclaration::MainMethodDeclaration(Statement& block)
	: ast::MethodDeclaration::MethodDeclaration(block);
{

}

std::string ast::MainMethodDeclaration::toString()
{
	std::string ret = "public static void main(String args[])" + std::endl;
	ret += block.toString();

	return ret;
}


