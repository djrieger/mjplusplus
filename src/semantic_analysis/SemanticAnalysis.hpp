#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "../util/symbol_table/symbol_table.hpp"
#include "../ast/Program.hpp"
#include "../ast/Type.hpp"
#include "../ast/PositionAwareNode.hpp"
#include "../util/ErrorReporter.hpp"

namespace ast
{
	class ClassDeclaration;
	class Program;
}

class SemanticAnalysis
{
	public:
		struct ClassTableItem
		{
			shptr<ast::ClassDeclaration> classNode;
			shptr<SymbolTable> symbolTable;
		};
		ErrorReporter reporter;
	private:

		bool valid;
		shptr<ast::Program> root;
		shptr<SymbolTable> symboltable;
		std::unordered_map<std::string, ClassTableItem> classTable;

	public:
		SemanticAnalysis(shptr<ast::Program> program);
		void printError(std::string s);
		void printError(std::string s, shptr<ast::PositionAwareNode> node);

		bool start();
		bool insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node);
		shptr<SymbolTable> getSymbolTable() const;
		std::unordered_map<std::string, ClassTableItem> const& getClassTable() const;
		bool isTypeDefined(shptr<ast::Type> type, bool isVoidAcceptable = false);
};

#endif
