#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include "../globals.hpp"

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
#include "../ast/BasicType.hpp"
#include "../ast/TypeIdent.hpp"
#include "../ast/ArrayType.hpp"

class Parser
{
	public:
		/**
		 * Constructor to the parser, needs at least a lexer.
		 * Since it's going to be handimplemented, the grammar is implicitly defined in parse*() functions below
		 * @param	lexer	instance of a lexer providing the tokens
		 * @param   print_messages   sets whether error messages will be printed, default: true
		 */
		Parser(Lexer& lexer, bool print_messages = true); // TODO: determine parameters

		/**
		 * Generic function to start the parser
		 */
		bool start();
		/**
		 * Reference to parsed Program
		 */
		std::shared_ptr<ast::Program> getRoot();

	private:
		std::shared_ptr<ast::Program> astRoot;

		Lexer& lexer;
		/**
		 * Whether parser prints error messages or suppresses them
		 */
		bool print_messages;
		Token current;
		/*
		 * Precedences of all operators, associativity is stored implicitly
		 */
		int operator_precs(Token::Token_type t);

		/**
		 * Get next token from lexer.
		 * Returns false if token is of type TOKEN_ERROR, else true.
		 */
		void nextToken();
		/**
		 * Checks whether type of current token is tokenType.
		 * If this is the case, nextToken() is called to advance to the next token.
		 */
		void expect(Token::Token_type const& tokenType);
		/**
		 * Same behavior as expect(Token::Token_type).
		 * In addition, this method also compares the string_value of current to string_val.
		 */
		void expect(Token::Token_type const& tokenType, std::string const& string_val);

		/**
		 * Prints an error message, prepending current token position
		 */
		void printError(std::string const& error_msg);

		uptr<ast::Program> parseProgram();
		uptr<vec<uptr<ast::ClassMember>>> parseClassMembers();
		uptr<ast::MainMethodDeclaration> parseMainMethod();

		uptr<ast::TypeIdent> parseTypeIdent();
		uptr<ast::Type> parseType();
		uptr<ast::BasicType> parseBasicType();
		int parseArrayDecl();
		uptr<ast::Expression> parseNewArrayExpression();
		int parseOptionalBrackets();

		uptr<ast::ClassMember> parseFieldOrMethod(uptr<ast::TypeIdent> typeIdent);
		uptr<vec<uptr<ast::TypeIdent>>> parseOptionalParameters();
		uptr<ast::Arguments> parseArguments();

		uptr<ast::Statement> parseStatement();
		uptr<ast::Statement> parseBlock();
		uptr<ast::Statement> parseBlockStatement();
		uptr<ast::LVDStatement> parseLocalVariableDeclarationStatement();
		uptr<ast::IfStatement> parseIfStatement();
		uptr<ast::WhileStatement> parseWhileStatement();
		uptr<ast::ReturnStatement> parseReturnStatement();

		uptr<ast::Expression> parseExpression();
		uptr<ast::Expression> precedenceClimb(int minPrec);
		uptr<ast::Expression> parseUnaryExpression();

		uptr<ast::Expression> parsePrimaryExpression();
		uptr<vec<uptr<ast::PostfixOp>>> parsePostfixOps();
		uptr<ast::PostfixOp> parseMethodInvocationOrFieldAccess();

		uptr<ast::Expression> parseNewObjectExpression();
		uptr<ast::Expression> parseNewObjectOrNewArrayExpression();
};

#endif
