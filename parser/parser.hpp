#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "../lexer/lexer.hpp"
#include "../ast/Program.hpp"
#include "../ast/ClassMember.hpp"
#include "../ast/ClassDeclaration.hpp"
#include "../ast/MainMethodDeclaration.hpp"

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

		// TODO: determine (in and out) parameters as well as the return type, e.g.: anchor set for panic mode, later on the AST data structure.
		std::unique_ptr<ast::Program> parseProgram();
		std::unique_ptr<std::vector<std::unique_ptr<ast::ClassMember>>> parseClassMembers();
		std::unique_ptr<ast::MainMethodDeclaration> parseMainMethod();

		std::unique_ptr<ast::TypeIdent> parseTypeIdent();
		std::pair<ast::TypeIdent::Primitive_type, std::string> parseBasicType();
		int parseArrayDecl();

		void parseFieldOrMethod();
		void parseOptionalParameters();
		void parseStatement();
		void parseBlock();
		void parseBlockStatement();
		void parseLocalVariableDeclarationStatement();
		void parseIfStatement();
		void parseWhileStatement();
		void parseReturnStatement();
		void parseExpression();
		void parseAssignmentExpression();
		void precedenceClimb(int minPrec);
		void parseUnaryExpression();
		void parsePostfixOps();
		void parsePrimaryExpression();
		void parseArguments();
		void parseMethodInvocation();
		void parseMethodInvocationOrFieldAccess();
		void parseNewArrayExpression();
		void parseNewObjectExpression();
		void parseNewObjectOrNewArrayExpression();
		void parseOptionalBrackets();
		void parseIdentOrIdentWithArguments();
};

#endif
