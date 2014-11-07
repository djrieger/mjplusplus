#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"

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

	private:
		Lexer& lexer;
		/**
		 * Whether parser prints error messages or suppresses them
		 */
		bool print_messages;
		bool error_mode = false;
		Token current;
		/*
		 * Precedences of all operators, as well as associativity
		 */
		static std::map<Token::Token_type, std::tuple<int, bool>> operator_precs;

		/**
		 * Get next token from lexer.
		 * Returns false if token is of type TOKEN_ERROR, else true.
		 */
		bool nextToken();
		/**
		 * Common functionality for all expect functions. See implementations of expect()
		 * for details.
		 */
		bool expectHelper(Token::Token_type tokenType, bool ret, std::string const& error_msg, bool report);
		/**
		 * Checks whether type of current token is tokenType.
		 * If this is the case, nextToken() is called to advance to the next token.
		 * @param	report	if true, an error is printed in case of a type mismatch
		 */
		bool expect(Token::Token_type tokenType, bool report = true);
		/**
		 * Same behavior as expect(Token::Token_type, bool).
		 * In addition, this method also compares the string_value of current
		 * to string_val and also returns false, if the string contents do not match.
		 */
		bool expect(Token::Token_type tokenType, std::string const& string_val, bool report = true);

		/**
		 * Prints an error message, prepending current token position
		 */
		void printError(std::string const& error_msg);

		// TODO: determine (in and out) parameters as well as the return type, e.g.: anchor set for panic mode, later on the AST data structure.
		bool parseProgram();
		bool parseClassDeclaration();
		bool parseClassMembers();
		bool parseClassMember();
		bool parseClassMember_();
		bool parseMainMethod();
		bool parseTypeIdent();
		bool parseType();
		bool parseBasicType();
		bool parseArrayDecl();
		bool parseFieldOrMethod();
		bool parseOptionalParameters();
		bool parseStatement();
		bool parseBlock();
		bool parseBlockStatements();
		bool parseBlockStatement();
		bool parseLocalVariableDeclarationStatement();
		bool parseIfStatement();
		bool parseWhileStatement();
		bool parseReturnStatement();
		bool parseExpressionStatement();
		bool parseExpression();
		bool parseAssignmentExpression();
		bool precedenceClimb(int minPrec);
		bool parseArguments();
		bool parseArgumentsExpressions();
		bool parseArrayAccess();
		bool parseExclMarkOrHyphen();
		bool parseMethodInvocation();
		bool parseMethodInvocationOrFieldAccess();
		bool parseNewArrayExpression();
		bool parseNewObjectExpression();
		bool parseNewObjectOrNewArrayExpression();
		bool parseOptionalBrackets();
		bool parsePostfixExpression();
		bool parsePostfixOps();
		bool parsePrimaryExpression();
		bool parseUnaryExpression();
		bool parseIdentOrIdentWithArguments();
};

#endif
