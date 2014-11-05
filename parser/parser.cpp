#include "parser.hpp"

Parser::Parser(Lexer& lexer, bool print_messages) : lexer(lexer), print_messages(print_messages)
{
}

/* Max' proposal: use return type to indicate if token stream is not a part of the language induced by the grammar
*/
bool Parser::start()
{
	nextToken();
	return parseProgram();
}

Token Parser::nextToken()
{
	current = lexer.get_next_token();
	return current;
}

void Parser::printError()
{
	if (print_messages) // only print error messages if they are wanted
		std::cout << error_msg_buf;

	error_msg_buf = "";
}

bool Parser::expect(Token::Token_type tokenType)
{
	bool ret = current.token_type == tokenType;

	if (ret)
		nextToken();

	return ret;
}

bool Parser::expect(Token::Token_type tokenType, std::string string_val)
{
	bool ret = current.token_type == tokenType && current.string_value == string_val;

	if (ret)
		nextToken();

	return ret;
}

bool Parser::expect(std::set<Token::Token_type> token_types)
{
	bool ret = token_types.find(current.token_type) != token_types.end();

	if (ret)
		nextToken();

	return ret;
}


/*

ClassMember -> public ClassMember_ .
ClassMember_ -> TypeIdent FieldOrMethod | MainMethod .

*/

bool Parser::parseProgram()
{
	switch (current.token_type)
	{
		case Token::Token_type::TOKEN_EOF:
			return true;
			break; // empty file, that's ok.

		case Token::Token_type::KEYWORD_CLASS:
			if (parseClassDeclaration())
			{
				// call parseProgram again: either its EOF or another class declaration follows
				return parseProgram();
			} // else case: fall through to default/error case

		default:
			printError();
			return false;
	}
}

// ClassDeclaration -> class IDENT { ClassMembers } .
bool Parser::parseClassDeclaration()
{
	return expect(Token::Token_type::KEYWORD_CLASS) &&
	       expect(Token::Token_type::TOKEN_IDENT) &&
	       expect(Token::Token_type::OPERATOR_LBRACE) &&
	       parseClassMembers() &&
	       expect(Token::Token_type::OPERATOR_RBRACE);
}

// ClassMembers -> ClassMember ClassMembers | .
bool Parser::parseClassMembers()
{
	while (current.token_type == Token::Token_type::KEYWORD_PUBLIC)
	{
		if (!parseClassMember())
			return false;
	}

	return true;
}

// ClassMember -> public ClassMember_ .
bool Parser::parseClassMember()
{
	return expect(Token::Token_type::KEYWORD_PUBLIC) &&
	       parseClassMember_();
}

// ClassMember_ -> TypeIdent FieldOrMethod | MainMethod .
bool Parser::parseClassMember_()
{
	if (current.token_type == Token::Token_type::KEYWORD_STATIC)
		return parseMainMethod();
	else
	{
		return parseTypeIdent() &&
		       parseFieldOrMethod();
	}
}

// MainMethod -> static void IDENT ( String [ ] IDENT ) Block .
bool Parser::parseMainMethod()
{
	return expect(Token::Token_type::KEYWORD_STATIC) &&
	       expect(Token::Token_type::KEYWORD_VOID) &&
	       expect(Token::Token_type::TOKEN_IDENT) &&
	       expect(Token::Token_type::OPERATOR_LPAREN) &&
	       expect(Token::Token_type::TOKEN_IDENT, "String") &&
	       expect(Token::Token_type::OPERATOR_LBRACKET) &&
	       expect(Token::Token_type::OPERATOR_RBRACKET) &&
	       expect(Token::Token_type::TOKEN_IDENT) &&
	       expect(Token::Token_type::OPERATOR_RPAREN) &&
	       parseBlock();
}

// TypeIdent -> Type IDENT
bool Parser::parseTypeIdent()
{
	return parseType() &&
	       expect(Token::Token_type::TOKEN_IDENT);
}

// Type -> BasicType ArrayDecl .
bool Parser::parseType()
{
	return parseBasicType() &&
	       parseArrayDecl();
}

// BasicType -> int | boolean | void | IDENT .
bool Parser::parseBasicType()
{
	return expect({ Token::Token_type::KEYWORD_INT,
	                Token::Token_type::KEYWORD_BOOLEAN,
	                Token::Token_type::KEYWORD_VOID,
	                Token::Token_type::TOKEN_IDENT
	              });
}

// ArrayDecl -> [ ] ArrayDecl | .
bool Parser::parseArrayDecl()
{
	while (expect(Token::Token_type::OPERATOR_LBRACE))
	{
		if (!expect(Token::Token_type::OPERATOR_RBRACE))
			return false;
	}

	return true;
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
bool Parser::parseFieldOrMethod()
{
	if (expect(Token::Token_type::OPERATOR_SEMICOLON))
		return true;

	return expect(Token::Token_type::OPERATOR_LPAREN) &&
	       parseOptionalParameters() &&
	       expect(Token::Token_type::OPERATOR_RPAREN) &&
	       parseBlock();
}

// OptionalParameters -> Parameters | .
// Parameters -> Parameter FollowingParameters .
// FollowingParameters -> , Parameters
//     | .
// Parameter -> TypeIdent .
bool Parser::parseOptionalParameters()
{
	bool isFirstParameter = true;

	while (parseTypeIdent())
	{
		isFirstParameter = false;

		if (!expect(Token::Token_type::OPERATOR_COMMA))
			return true;
	}

	return isFirstParameter;
}

// Statement -> Block | EmptyStatement | IfStatement | ExpressionStatement | WhileStatement | ReturnStatement .
bool Parser::parseStatement()
{
	switch (current.token_type)
	{
		case Token::Token_type::OPERATOR_LBRACE:
			return parseBlock();
			break;

		case Token::Token_type::OPERATOR_SEMICOLON:
			return expect(Token::Token_type::OPERATOR_SEMICOLON);
			break;

		case Token::Token_type::KEYWORD_IF:
			return parseIfStatement();
			break;

		case Token::Token_type::KEYWORD_WHILE:
			return parseWhileStatement();
			break;

		case Token::Token_type::KEYWORD_RETURN:
			return parseReturnStatement();
			break;

		// IDENT, (, -, !, null, false, true, INTEGER_LITERAL, this, new
		case Token::Token_type::TOKEN_IDENT:
		case Token::Token_type::TOKEN_INT_LIT:
		case Token::Token_type::OPERATOR_LPAREN:
		case Token::Token_type::OPERATOR_MINUS:
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::KEYWORD_NEW:
			return parseExpressionStatement();
			break;

		default:
			return false;
	}

	parseExpressionStatement();
}

// Block -> { BlockStatements } .
bool Parser::parseBlock()
{
	return expect(Token::Token_type::OPERATOR_LBRACE) &&
	       parseBlockStatements() &&
	       expect(Token::Token_type::OPERATOR_RBRACE);
}

// BlockStatements -> BlockStatement BlockStatements | .
bool Parser::parseBlockStatements()
{
	while (parseBlockStatement())
		;

	return true;
}

// TODO!!!
// BlockStatement -> Statement | LocalVariableDeclarationStatement .
bool Parser::parseBlockStatement()
{
	// Statement first = IDENT, {, (, ;, while, if, return, -, !, null, false, true, INTEGER_LITERAL, this, new
	// LVDS first =      IDENT, void, int, boolean
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
			return parseStatement();
			break;

		case Token::Token_type::KEYWORD_VOID:
		case Token::Token_type::KEYWORD_INT:
		case Token::Token_type::KEYWORD_BOOLEAN:
			return parseLocalVariableDeclarationStatement();
			break;

		case Token::Token_type::TOKEN_IDENT:
			// TODO!!!!
			return false;

		default:
			return false;
	}
}

// TODO: Implement
bool Parser::parseLocalVariableDeclarationStatement()
{
	return false;
}
bool Parser::parseIfStatement()
{
	return false;
}
bool Parser::parseWhileStatement()
{
	return false;
}
bool Parser::parseReturnStatement()
{
	return false;
}
bool Parser::parseExpressionStatement()
{
	return false;
}
//parseParameters