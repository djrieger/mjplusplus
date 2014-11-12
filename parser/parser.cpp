#include <iostream>
#include <algorithm>
#include "parser.hpp"

Parser::Parser(Lexer& lexer, bool print_messages) : lexer(lexer), print_messages(print_messages)
{
}

int Parser::operator_precs(Token::Token_type t)
{
	switch (t)
	{
		case Token::Token_type::OPERATOR_EQ:
			return 1;

		case Token::Token_type::OPERATOR_OROR:
			return 2;

		case Token::Token_type::OPERATOR_ANDAND:
			return 3;

		case Token::Token_type::OPERATOR_EQEQ:
		case Token::Token_type::OPERATOR_NOTEQ:
			return 4;

		case Token::Token_type::OPERATOR_LT:
		case Token::Token_type::OPERATOR_LTEQ:
		case Token::Token_type::OPERATOR_GT:
		case Token::Token_type::OPERATOR_GTEQ:
			return 5;

		case Token::Token_type::OPERATOR_PLUS:
		case Token::Token_type::OPERATOR_MINUS:
			return 6;

		case Token::Token_type::OPERATOR_MULT:
		case Token::Token_type::OPERATOR_SLASH:
		case Token::Token_type::OPERATOR_MOD:
			return 7;

		default:
			return -1;
	}
}

/* Max' proposal: use return type to indicate if token stream is not a part of the language induced by the grammar
*/
bool Parser::start()
{
	bool r = false;

	try
	{
		nextToken();
		parseProgram();
		r = true;
	}
	catch (char const* msg)
	{
		printError(msg);
	}
	catch (Token::Token_type tokenType)
	{
		printError("expected " + lexer.describe(tokenType));
	}
	catch (std::string string_val)
	{
		printError("expected \"" + string_val + '"');
	}

	if (!r)
		std::cerr << "Error during compilation." << std::endl;

	return r;
}

void Parser::nextToken()
{
	current = lexer.get_next_token();

	if (current.token_type == Token::Token_type::TOKEN_ERROR)
		throw "Error from lexer";
}

void Parser::printError(std::string const& error_msg)
{
	if (print_messages) // only print error messages if they are wanted
	{
		std::cerr << "at line " << current.position.first << ", column " << current.position.second <<
		          ", parsing \"" << current.string_value << '"' << (error_msg.empty() ? "" : ": ") <<
		          error_msg << std::endl;

		// read current line
		std::string line = lexer.getLine();
		std::replace(line.begin(), line.end(), '\t', ' ');

		// markerLine stores a position indicator like so: "     ^"
		std::string markerline(current.position.second - 1, ' ');
		markerline += '^';

		// output input line where error occurred and markerline
		// line already ends with \n so no additional std::endl needs to be added
		std::cerr << line << std::endl;
		std::cerr << markerline << std::endl;
	}
}

void Parser::expect(Token::Token_type const& tokenType)
{
	if (current.token_type != tokenType)
		throw tokenType;

	nextToken();
}

void Parser::expect(Token::Token_type const& tokenType, std::string const& string_val)
{
	if (current.token_type != tokenType || current.string_value != string_val)
		throw string_val;

	return nextToken();
}

// Program -> class ClassDeclaration Program | .
// ClassDeclaration -> IDENT { ClassMembers } .
void Parser::parseProgram()
{
	while (current.token_type == Token::Token_type::KEYWORD_CLASS)
	{
		nextToken();
		expect(Token::Token_type::TOKEN_IDENT);
		expect(Token::Token_type::OPERATOR_LBRACE);
		parseClassMembers();
		expect(Token::Token_type::OPERATOR_RBRACE);
	}

	expect(Token::Token_type::TOKEN_EOF);
}

// ClassMembers -> public ClassMember ClassMembers | .
// ClassMember -> TypeIdent FieldOrMethod | static MainMethod .
void Parser::parseClassMembers()
{
	while (current.token_type == Token::Token_type::KEYWORD_PUBLIC)
	{
		nextToken();

		if (current.token_type == Token::Token_type::KEYWORD_STATIC)
		{
			nextToken();
			parseMainMethod();
		}
		else
		{
			parseTypeIdent();
			parseFieldOrMethod();
		}
	}
}

// MainMethod -> void IDENT ( String [ ] IDENT ) Block .
void Parser::parseMainMethod()
{
	expect(Token::Token_type::KEYWORD_VOID);
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_LPAREN);
	expect(Token::Token_type::TOKEN_IDENT, "String");
	expect(Token::Token_type::OPERATOR_LBRACKET);
	expect(Token::Token_type::OPERATOR_RBRACKET);
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_RPAREN);
	parseBlock();
}

// TypeIdent -> Type IDENT
void Parser::parseTypeIdent()
{
	parseType();
	expect(Token::Token_type::TOKEN_IDENT);
}

// Type -> BasicType ArrayDecl .
void Parser::parseType()
{
	parseBasicType();
	parseArrayDecl();
}

// BasicType -> int | boolean | void | IDENT .
void Parser::parseBasicType()
{
	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_INT:
		case Token::Token_type::KEYWORD_BOOLEAN:
		case Token::Token_type::KEYWORD_VOID:
		case Token::Token_type::TOKEN_IDENT:
			nextToken();
			break;

		default:
			throw "expected Type";
	}
}

// ArrayDecl -> [ ] ArrayDecl | .
void Parser::parseArrayDecl()
{
	while (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();
		expect(Token::Token_type::OPERATOR_RBRACKET);
	}
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
void Parser::parseFieldOrMethod()
{
	if (current.token_type == Token::Token_type::OPERATOR_SEMICOLON)
		nextToken();
	else
	{
		expect(Token::Token_type::OPERATOR_LPAREN);
		parseOptionalParameters();
		expect(Token::Token_type::OPERATOR_RPAREN);
		parseBlock();
	}
}

// OptionalParameters -> Parameters | .
// Parameters -> Parameter FollowingParameters .
// FollowingParameters -> , Parameters
//     | .
// Parameter -> TypeIdent .
void Parser::parseOptionalParameters()
{
	bool isFirstParameter = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstParameter = false;
		parseTypeIdent();

		if (current.token_type != Token::Token_type::OPERATOR_COMMA)
			return;
		else
			nextToken();
	}

	if (!isFirstParameter)
		throw "trailing comma";
}

// Statement -> Block | EmptyStatement | if IfStatement | Expression ; | while WhileStatement | return ReturnStatement .
// EmptyStatement -> ; .
void Parser::parseStatement()
{
	switch (current.token_type)
	{
		case Token::Token_type::OPERATOR_LBRACE:
			parseBlock();
			break;

		case Token::Token_type::OPERATOR_SEMICOLON:
			nextToken();
			break;

		case Token::Token_type::KEYWORD_IF:
			nextToken();
			parseIfStatement();
			break;

		case Token::Token_type::KEYWORD_WHILE:
			nextToken();
			parseWhileStatement();
			break;

		case Token::Token_type::KEYWORD_RETURN:
			nextToken();
			parseReturnStatement();
			break;

		// IDENT, (, -, !, null, false, true, INTEGER_LITERAL, this, new
		case Token::Token_type::TOKEN_IDENT:
		case Token::Token_type::TOKEN_INT_LIT:
		case Token::Token_type::OPERATOR_LPAREN:
		case Token::Token_type::OPERATOR_MINUS:
		case Token::Token_type::OPERATOR_NOT:
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::KEYWORD_NEW:
			parseExpression();
			expect(Token::Token_type::OPERATOR_SEMICOLON);
			break;

		default:
			throw "expected Statement";
	}
}

// Block -> { BlockStatements } .
// BlockStatements -> BlockStatement BlockStatements | .
void Parser::parseBlock()
{
	expect(Token::Token_type::OPERATOR_LBRACE);

	while (current.token_type != Token::Token_type::OPERATOR_RBRACE)
		parseBlockStatement();

	nextToken();
}

// BlockStatement -> Statement | LocalVariableDeclarationStatement .
void Parser::parseBlockStatement()
{
	// Statement first = IDENT, {, (, ;, while, if, return, -, !, null, false, true, INTEGER_LITERAL, this, new
	// LVDS first =      IDENT, void, int, boolean

	Token idToken;
	Token maybeLBracketToken;
	Token maybeRBracketToken;


	switch (current.token_type)
	{
		case Token::Token_type::OPERATOR_LBRACE:
		case Token::Token_type::OPERATOR_LPAREN:
		case Token::Token_type::OPERATOR_SEMICOLON:
		case Token::Token_type::KEYWORD_WHILE:
		case Token::Token_type::KEYWORD_IF:
		case Token::Token_type::KEYWORD_RETURN:
		case Token::Token_type::OPERATOR_MINUS:
		case Token::Token_type::OPERATOR_NOT:
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::TOKEN_INT_LIT:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::KEYWORD_NEW:
			parseStatement();
			break;

		case Token::Token_type::KEYWORD_VOID:
		case Token::Token_type::KEYWORD_INT:
		case Token::Token_type::KEYWORD_BOOLEAN:
			parseLocalVariableDeclarationStatement();
			break;

		case Token::Token_type::TOKEN_IDENT:
			idToken = current;
			nextToken();
			maybeLBracketToken = current;

			if (maybeLBracketToken.token_type == Token::Token_type::TOKEN_IDENT)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				parseLocalVariableDeclarationStatement();
			}
			else if (maybeLBracketToken.token_type != Token::Token_type::OPERATOR_LBRACKET)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				parseStatement();
			}
			else
			{
				nextToken();
				maybeRBracketToken = current;
				bool isRBracket = maybeRBracketToken.token_type == Token::Token_type::OPERATOR_RBRACKET;
				lexer.unget_token(maybeRBracketToken);
				lexer.unget_token(maybeLBracketToken);
				current = idToken;

				if (isRBracket)
					parseLocalVariableDeclarationStatement();
				else
					parseStatement();
			}

			break;

		default:
			throw "expected Statement";
	}
}

// LocalVariableDeclarationStatement -> TypeIdent OptionalLVDSExpression ; .
// OptionalLVDSExpression -> = Expression
//	| .
void Parser::parseLocalVariableDeclarationStatement()
{
	parseTypeIdent();

	if (current.token_type == Token::Token_type::OPERATOR_EQ)
	{
		nextToken();
		parseExpression();
	}

	expect(Token::Token_type::OPERATOR_SEMICOLON);
}

// IfStatement -> ( Expression ) Statement OptionalElseStatement .
// OptionalElseStatement -> else Statement
// 	| .
void Parser::parseIfStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	parseStatement();

	if (current.token_type == Token::Token_type::KEYWORD_ELSE)
	{
		nextToken();
		parseStatement();
	}
}

// WhileStatement -> ( Expression ) Statement .
void Parser::parseWhileStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	parseStatement();
}

// ReturnStatement -> OptionalExpression ; .
// OptionalExpression -> Expression
//  	| .
void Parser::parseReturnStatement()
{
	if (current.token_type != Token::Token_type::OPERATOR_SEMICOLON)
		parseExpression();


	expect(Token::Token_type::OPERATOR_SEMICOLON);
}

/*
Expression -> AssignmentExpression .
AssignmentExpression -> LogicalOrExpression OptionalAssignmentExpression .
OptionalAssignmentExpression -> = AssignmentExpression
	| .
LogicalOrExpression -> OptionalLogicalOrExpression LogicalAndExpression .
OptionalLogicalOrExpression -> LogicalOrExpression DOUBLEPIPE
	| .
LogicalAndExpression ->  OptionalLogicalAndExpression EqualityExpression .
OptionalLogicalAndExpression -> LogicalAndExpression &&
	| .

EqualityExpression -> OptionalEqualityExpression RelationalExpression .
OptionalEqualityExpression -> EqualityExpression EqOrNeq
	| .
EqOrNeq -> == | != .

RelationalExpression -> OptionalRelationalExpression AdditiveExpression .
OptionalRelationalExpression -> RelationalExpression RelationalOperator
	| .
RelationalOperator -> < | <= | > | >= .

AdditiveExpression -> OptionalAdditiveExpression MultiplicativeExpression .
OptionalAdditiveExpression -> AdditiveExpression PlusOrMinus | .
PlusOrMinus -> + | - .

MultiplicativeExpression -> OptionalMultiplicativeExpression UnaryExpression .
OptionalMultiplicativeExpression -> MultiplicativeExpression MultSlashPercent
	| .
MultSlashPercent -> * | / | % .
*/
void Parser::parseExpression()
{
	precedenceClimb(1);
}

/*
parses an expression via precedence climb
*/
void Parser::precedenceClimb(int minPrec)
{
	parseUnaryExpression();
	int prec = operator_precs(current.token_type);

	while (prec >= minPrec)
	{
		if (prec > 1) //equivalent to the fact that operator is left associative
			prec++;

		nextToken();
		precedenceClimb(prec);
		prec = operator_precs(current.token_type);
	}
}

// UnaryExpression -> PostfixExpression | ExclMarkOrHyphen UnaryExpression .
// PostfixExpression -> PrimaryExpression PostfixOps .
void Parser::parseUnaryExpression()
{
	while (current.token_type == Token::Token_type::OPERATOR_NOT ||
	        current.token_type == Token::Token_type::OPERATOR_MINUS)
		nextToken();

	parsePrimaryExpression();
	parsePostfixOps();
}

// PrimaryExpression -> null | false | true | INTEGER_LITERAL | IDENT IdentOrIdentWithArguments | this | ( Expression ) | new NewObjectOrNewArrayExpression .
void Parser::parsePrimaryExpression()
{
	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::TOKEN_INT_LIT:
			nextToken();
			break;

		case Token::Token_type::TOKEN_IDENT:
			nextToken();
			parseIdentOrIdentWithArguments();
			break;

		case Token::Token_type::OPERATOR_LPAREN:
			nextToken();
			parseExpression();
			expect(Token::Token_type::OPERATOR_RPAREN);
			break;

		case Token::Token_type::KEYWORD_NEW:
			nextToken();
			parseNewObjectOrNewArrayExpression();
			break;

		default:
			throw "expected Expression";
	}
}

// IdentOrIdentWithArguments -> ( Arguments )
//     | .
void Parser::parseIdentOrIdentWithArguments()
{
	if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
	{
		nextToken();
		parseArguments();
		expect(Token::Token_type::OPERATOR_RPAREN);
	}
}

// NewObjectOrNewArrayExpression -> NewObjectExpression | NewArrayExpression .
void Parser::parseNewObjectOrNewArrayExpression()
{
	Token id = current;

	nextToken();
	Token next = current;
	lexer.unget_token(next);
	current = id;

	if (next.token_type == Token::Token_type::OPERATOR_LPAREN)
		parseNewObjectExpression();
	else
		parseNewArrayExpression();
}

// NewObjectExpression -> IDENT ( ) .
void Parser::parseNewObjectExpression()
{
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_LPAREN);
	expect(Token::Token_type::OPERATOR_RPAREN);
}

// NewArrayExpression -> BasicType [ Expression ] OptionalBrackets .
void Parser::parseNewArrayExpression()
{
	parseBasicType();
	expect(Token::Token_type::OPERATOR_LBRACKET);
	parseExpression();
	expect(Token::Token_type::OPERATOR_RBRACKET);
	parseOptionalBrackets();
}

// OptionalBrackets -> [ ] OptionalBrackets
//     | .
void Parser::parseOptionalBrackets()
{
	Token t = current;

	while (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();

		if (current.token_type == Token::Token_type::OPERATOR_RBRACKET)
		{
			nextToken();
			t = current;
		}
		else
		{
			lexer.unget_token(current);
			current = t;
			return;
		}
	}
}

// Arguments -> Expression ArgumentsExpressions | .
// ArgumentsExpressions -> , Expression ArgumentsExpressions | .
void Parser::parseArguments()
{
	bool isFirstArgument = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstArgument = false;
		parseExpression();

		if (current.token_type != Token::Token_type::OPERATOR_COMMA)
			return;
		else
			nextToken();
	}

	if (!isFirstArgument)
		throw "trailing comma";
}

// MethodInvocation -> ( Arguments ) .
void Parser::parseMethodInvocation()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	parseArguments();
	expect(Token::Token_type::OPERATOR_RPAREN);
}

// MethodInvocationOrFieldAccess -> MethodInvocation | .
void Parser::parseMethodInvocationOrFieldAccess()
{
	if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
		parseMethodInvocation();
}

// PostfixOps -> PostfixOp PostfixOps | .
// PostfixOp -> DOT IDENT MethodInvocationOrFieldAccess
//     | [ Expression ] .
void Parser::parsePostfixOps()
{
	while (true)
	{
		if (current.token_type == Token::Token_type::OPERATOR_DOT)
		{
			nextToken();
			expect(Token::Token_type::TOKEN_IDENT);
			parseMethodInvocationOrFieldAccess();
		}
		else if (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
		{
			nextToken();
			parseExpression();
			expect(Token::Token_type::OPERATOR_RBRACKET);
		}
		else
			return;
	}
}
