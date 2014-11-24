#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include "../globals.hpp"

#include "../util/ErrorReporter.hpp"
#include "../lexer/lexer.hpp"
#include "../ast/Program.hpp"
#include "../ast/ClassMember.hpp"
#include "../ast/ClassDeclaration.hpp"
#include "../ast/MainMethodDeclaration.hpp"
#include "../ast/FieldDeclaration.hpp"
#include "../ast/LVDStatement.hpp"
#include "../ast/Block.hpp"
#include "../ast/ExpressionStatement.hpp"
#include "../ast/Statement.hpp"
#include "../ast/IfStatement.hpp"
#include "../ast/WhileStatement.hpp"
#include "../ast/ReturnStatement.hpp"
#include "../ast/PostfixExpression.hpp"
#include "../ast/PostfixOp.hpp"
#include "../ast/ArrayAccess.hpp"
#include "../ast/FieldAccess.hpp"
#include "../ast/MethodInvocation.hpp"
#include "../ast/UnaryExpression.hpp"
#include "../ast/BinaryExpression.hpp"
#include "../ast/PrimaryExpression.hpp"
#include "../ast/Type.hpp"
#include "../ast/TypeIdent.hpp"

class Parser
{
	public:
		/**
		 * Constructor to the parser, needs at least a lexer.
		 * Since it's going to be handimplemented, the grammar is implicitly defined in parse*() functions below
		 * @param	lexer	instance of a lexer providing the tokens
		 */
		Parser(lexer::Lexer& lexer, shptr<ErrorReporter> errorReporter); // TODO: determine parameters

		/**
		 * Generic function to start the parser
		 */
		bool start();
		/**
		 * Reference to parsed Program
		 */
		shptr<ast::Program> getRoot();

	private:
		lexer::Lexer& lexer;
		shptr<ErrorReporter> errorReporter;
		shptr<ast::Program> astRoot;
		lexer::Token current;

		/**
		 * Whether we are in error mode (true between invalid token and next occurrence of expected token)
		 */
		bool error_mode = false;
		/**
		 * False at start. Set to true when at least one parser error has been detected.
		 */
		bool errors_found = false;


		/**
		 * Get next token from lexer.
		 */
		void nextToken();
		/**
		 * Checks whether type of current token is tokenType.
		 * If this is the case, nextToken() is called to advance to the next token.
		 */
		void expect(lexer::Token::Token_type const& tokenType);
		/**
		 * Same behavior as expect(lexer::Token::Token_type).
		 * In addition, this method also compares the string_value of current to string_val.
		 */
		void expect(lexer::Token::Token_type const& tokenType, std::string const& string_val);

		/**
		 * Prints an error message, prepending current token position
		 */
		void reportError(std::string const& error_msg);

		/*
		 * Precedences of all operators, associativity is stored implicitly
		 */
		int operator_precs(lexer::Token::Token_type t);

		shptr<ast::Program> parseProgram();
		shptr<vec<shptr<ast::ClassMember>>> parseClassMembers();
		shptr<ast::MainMethodDeclaration> parseMainMethod();

		shptr<ast::TypeIdent> parseTypeIdent();
		shptr<ast::Type> parseType();
		shptr<ast::Type> parseBasicType();
		int parseArrayDecl();
		shptr<ast::Expression> parseNewArrayExpression();
		int parseOptionalBrackets();

		shptr<ast::ClassMember> parseFieldOrMethod();
		shptr<vec<shptr<ast::TypeIdent>>> parseOptionalParameters();
		shptr<ast::Arguments> parseArguments();

		shptr<ast::Statement> parseStatement();
		shptr<ast::Statement> parseBlock();
		shptr<ast::Statement> parseBlockStatement();
		shptr<ast::LVDStatement> parseLocalVariableDeclarationStatement();
		shptr<ast::IfStatement> parseIfStatement();
		shptr<ast::WhileStatement> parseWhileStatement();
		shptr<ast::ReturnStatement> parseReturnStatement();

		shptr<ast::Expression> parseExpression();
		shptr<ast::Expression> precedenceClimb(int minPrec);
		shptr<ast::Expression> parseUnaryExpression();

		shptr<ast::Expression> parsePrimaryExpression();
		shptr<vec<shptr<ast::PostfixOp>>> parsePostfixOps();
		shptr<ast::PostfixOp> parseMethodInvocationOrFieldAccess();

		shptr<ast::Expression> parseNewObjectExpression();
		shptr<ast::Expression> parseNewObjectOrNewArrayExpression();

};

#endif
