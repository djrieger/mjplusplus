#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"
#include "precedences.hpp"

class Parser
{
	protected:
		// TODO: determine member variables.
		Lexer& lexer;
		bool print_messages;
		Token current;
		std::string error_msg_buf;

		Token nextToken();
		bool expect(Token::Token_type tokenType);
	public:
		/**
		 * Constructor to the parser, needs at least a lexer.
		 * Since it's going to be handimplemented, the grammar is implicitly defined in the the functions
		 * @param	lexer	instance of a lexer providing the tokens
		 * @param   print_messages   sets whether error messages will be printed, default: true
		 */
		Parser(Lexer& lexer, bool print_messages = true); // TODO: determine parameters

		/**
		 * Generic function to start the parser
		 */
		bool start();

		/**
		 * Prints the content of the error message buffer and resets it to an empty string. TODO: private?
		 */
		void printError();

	private:
		// TODO: parseNT() function for each non terminal
		// TODO: determine (in and out) parameters as well as the return type, e.g.: anchor set for panic mode, later on the AST data structure.
		bool parseProgram();
		bool parseClassDeclaration();
		bool parseExpression();
		bool parseAssignmentExpression();
		bool precedenceClimb(int minPrec);
		// ..
		bool parsePrimaryExpression();
		bool parseNewObjectOrNewArrayExpression();
		bool parseNewObjectExpression();
		bool parseNewArrayExpression();
		bool parseOptionalBrackets();
		bool parseNewKeyword();
		bool parseArgumentsExpressions();
		bool parseArrayAccess();
		bool parseArguments();
		bool parseMethodInvocation();
		bool parseMethodInvocationOrFieldAccess();
		bool parseExclMarkOrHyphen();
		bool parsePostfixExpression();
		bool parsePostfixOps();
		bool parsePostfixOp();
		bool parseUnaryExpression();
};

#endif
