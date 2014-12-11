#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "symbol_table/symbol_table.hpp"
#include "../ast/Program.hpp"
#include "../ast/Type.hpp"
#include "../ast/PositionAwareNode.hpp"
#include "../util/ErrorReporter.hpp"
#include "FieldTable.hpp"
#include "MethodTable.hpp"

namespace ast
{
	class ClassDeclaration;
	class Program;
}

namespace semantic
{
	class SemanticAnalysis
	{
		public:
			struct ClassTableItem
			{
				shptr<ast::ClassDeclaration> classNode;
				shptr<symbol::SymbolTable> symbolTable;
				shptr<FieldTable> fieldTable;
				shptr<MethodTable> methodTable;
			};
		private:
			shptr<ErrorReporter> errorReporter;
			/*
			 * whether semantic errors have been detected, true initially
			 */
			bool valid;
			/*
			 * root AST node
			 */
			shptr<ast::Program> root;
			/*
			 * contains parsed classes
			 */
			std::unordered_map<std::string, ClassTableItem> classTable;

		public:
			SemanticAnalysis(shptr<ast::Program> program, shptr<ErrorReporter> errorReporter);
			/*
			 * Report an error without a source position using errorReporter
			 * @param message error message
			 */
			void reportError(std::string message);
			/*
			 * Report an error using errorReporter
			 * @param message error message
			 * @param node a PositionAwareNode used to extract the position for this error
			 */
			void reportError(std::string message, shptr<ast::PositionAwareNode> node);
			void reportError(std::string message, source_position_t position);

			bool start();
			bool insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node);
			std::unordered_map<std::string, ClassTableItem> const& getClassTable() const;
			bool isTypeDefined(shptr<ast::Type> type, bool isVoidAcceptable = false);

			static shptr<ast::TypeIdent> const systemTypeIdent;
	};
}

#endif
