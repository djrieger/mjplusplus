#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"

class Parser
{
	private:
		// TODO: determine member variables.
		Lexer& lexer;
		bool print_messages;
		Token current;

		Token nextToken();
		bool expect(Token::Token_type tokenType, bool report = true);
		bool expect(Token::Token_type tokenType, std::string const& string_val, bool report = true);

		/**
		 * Prints an error message, prepending current token position
		 */
		void printError(std::string const& error_msg);

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

	private:
		static std::map<Token::Token_type, std::tuple<int, bool> > operator_precs;

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
