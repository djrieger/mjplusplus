#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "../util/symbol_table/symbol_table.hpp"
#include "../ast/Program.hpp"

class SemanticAnalysis
{
	private:
		bool valid;
		shptr<ast::Program> root;
		shptr<SymbolTable> symboltable;
		//std::unordered_map<std::string, std::unordered_map<std::string, Symbol>> classMap;
		std::unordered_map<std::string, shptr<ast::ClassDeclaration>> classTable;

	public:
		SemanticAnalysis(shptr<ast::Program> program);
		void printError(std::string s);

		bool start();
		bool insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node);
		shptr<SymbolTable> getSymbolTable() const;
};

#endif