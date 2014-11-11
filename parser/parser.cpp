#include <iostream>
#include <algorithm>
#include "parser.hpp"

Parser::Parser(Lexer& lexer, bool print_messages) : lexer(lexer), print_messages(print_messages)
{
}

std::map<Token::Token_type, std::tuple<int, bool> > Parser::operator_precs =
{
	{Token::Token_type::OPERATOR_EQ, std::make_tuple(1, false)},
	{Token::Token_type::OPERATOR_OROR, std::make_tuple(2, true)},
	{Token::Token_type::OPERATOR_ANDAND, std::make_tuple(3, true)},
	{Token::Token_type::OPERATOR_EQEQ, std::make_tuple(4, true)},
	{Token::Token_type::OPERATOR_NOTEQ, std::make_tuple(4, true)},
	{Token::Token_type::OPERATOR_LT, std::make_tuple(5, true)},
	{Token::Token_type::OPERATOR_LTEQ, std::make_tuple(5, true)},
	{Token::Token_type::OPERATOR_GT, std::make_tuple(5, true)},
	{Token::Token_type::OPERATOR_GTEQ, std::make_tuple(5, true)},
	{Token::Token_type::OPERATOR_PLUS, std::make_tuple(6, true)},
	{Token::Token_type::OPERATOR_MINUS, std::make_tuple(6, true)},
	{Token::Token_type::OPERATOR_MULT, std::make_tuple(7, true)},
	{Token::Token_type::OPERATOR_SLASH, std::make_tuple(7, true)},
	{Token::Token_type::OPERATOR_MOD, std::make_tuple(7, true)}
};

/* Max' proposal: use return type to indicate if token stream is not a part of the language induced by the grammar
*/
bool Parser::start()
{
	if (!nextToken())
		return false;

	bool r = parseProgram();

	if (!r)
		std::cerr << "Error during compilation." << std::endl;

	return r;
}

bool Parser::nextToken()
{
	current = lexer.get_next_token();

	if (current.token_type == Token::Token_type::TOKEN_ERROR)
	{
		printError("Error from lexer");
		return false;
	}
	else
		return true;
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

bool Parser::expectHelper(bool ret, Token::Token_type const& tokenType, std::string const& error_msg, bool report)
{
	if (ret)
		return nextToken();
	else if (report)
	{
		if (error_msg.empty())
			printError("expected " + lexer.describe(tokenType));
		else
			printError(error_msg);
	}

	return ret;
}

bool Parser::expect(Token::Token_type const& tokenType, bool report)
{
	bool condition = current.token_type == tokenType;
	return expectHelper(condition, tokenType, "", report);
}

bool Parser::expect(Token::Token_type const& tokenType, std::string const& string_val, bool report)
{
	bool condition = current.token_type == tokenType && current.string_value == string_val;
	return expectHelper(condition, tokenType, (current.string_value == string_val ? "" : '"' + string_val + '"'), report);
}

// Program -> ClassDeclaration Program | .
bool Parser::parseProgram()
{
	while (current.token_type == Token::Token_type::KEYWORD_CLASS)
	{
		if (!parseClassDeclaration())
			return false;
	}

	return expect(Token::Token_type::TOKEN_EOF);
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
// ClassMember_ -> TypeIdent FieldOrMethod | MainMethod .
bool Parser::parseClassMember()
{
	if (!expect(Token::Token_type::KEYWORD_PUBLIC))
		return false;

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
	       expect(Token::Token_type::TOKEN_IDENT, std::string("String")) &&
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
	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_INT:
		case Token::Token_type::KEYWORD_BOOLEAN:
		case Token::Token_type::KEYWORD_VOID:
		case Token::Token_type::TOKEN_IDENT:
			return nextToken();
			break;

		default:
			printError("expected Type");
			return false;
	}
}

// ArrayDecl -> [ ] ArrayDecl | .
bool Parser::parseArrayDecl()
{
	while (expect(Token::Token_type::OPERATOR_LBRACKET, false))
	{
		if (!expect(Token::Token_type::OPERATOR_RBRACKET))
			return false;
	}

	return true;
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
bool Parser::parseFieldOrMethod()
{
	if (expect(Token::Token_type::OPERATOR_SEMICOLON, false))
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

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstParameter = false;

		if (!parseTypeIdent())
			return false;

		if (!expect(Token::Token_type::OPERATOR_COMMA, false))
			return true;
	}

	if (!isFirstParameter)
		printError("trailing comma");

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
		case Token::Token_type::OPERATOR_NOT:
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::KEYWORD_NEW:
			return parseExpressionStatement();
			break;

		default:
			printError("expected Statement");
			return false;
	}
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
	while (current.token_type != Token::Token_type::OPERATOR_RBRACE)
	{
		if (!parseBlockStatement())
			return false;
	}

	return true;
}
/*
 Expression -> AssignmentExpression .
*/
bool Parser::parseExpression()
{
	return parseAssignmentExpression();
}

/*
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
bool Parser::parseAssignmentExpression()
{
	return precedenceClimb(1);
}

/*
parses an expression via precedence climb
*/
bool Parser::precedenceClimb(int minPrec)
{
	bool result = parseUnaryExpression();
	int prec;
	bool left_assoc;
	Token::Token_type op = current.token_type;

	if (!result || operator_precs.find(op) == operator_precs.end())
		return result;

	std::tie(prec, left_assoc) = operator_precs[op];

	while (prec >= minPrec)
	{
		if (left_assoc)
			prec = prec + 1;

		if (!nextToken())
			return false;

		bool rhs = precedenceClimb(prec);
		result = result && rhs;
		op = current.token_type;

		if (!result || operator_precs.find(op) == operator_precs.end())
			return result;

		std::tie(prec, left_assoc) = operator_precs[op];
	}

	return result;
}


// PrimaryExpression -> null | false | true | INTEGER_LITERAL | IDENT IdentOrIdentWithArguments | this | ( Expression ) | new NewObjectOrNewArrayExpression .
bool Parser::parsePrimaryExpression()
{
	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_NULL:
		case Token::Token_type::KEYWORD_FALSE:
		case Token::Token_type::KEYWORD_TRUE:
		case Token::Token_type::KEYWORD_THIS:
		case Token::Token_type::TOKEN_INT_LIT:
			return nextToken();
			break;

		case Token::Token_type::TOKEN_IDENT:
			return nextToken() && parseIdentOrIdentWithArguments();
			break;

		case Token::Token_type::OPERATOR_LPAREN:
			return nextToken() && parseExpression() && expect(Token::Token_type::OPERATOR_RPAREN);
			break;

		case Token::Token_type::KEYWORD_NEW:
			return nextToken() && parseNewObjectOrNewArrayExpression();
			break;

		default:
			printError("expected Expression");
			return false;
	}
}

// IdentOrIdentWithArguments -> ( Arguments )
//     | .
bool Parser::parseIdentOrIdentWithArguments()
{
	if (expect(Token::Token_type::OPERATOR_LPAREN, false))
		return parseArguments() && expect(Token::Token_type::OPERATOR_RPAREN);

	return true;
}

// NewObjectOrNewArrayExpression -> NewObjectExpression | NewArrayExpression .
bool Parser::parseNewObjectOrNewArrayExpression()
{
	Token id = current;

	if (!nextToken())
		return false;

	Token next = current;
	lexer.unget_token(next);
	current = id;

	if (next.token_type == Token::Token_type::OPERATOR_LPAREN)
		return parseNewObjectExpression();
	else
		return parseNewArrayExpression();
}

// NewObjectExpression -> IDENT ( ) .
bool Parser::parseNewObjectExpression()
{
	return expect(Token::Token_type::TOKEN_IDENT) &&
	       expect(Token::Token_type::OPERATOR_LPAREN) &&
	       expect(Token::Token_type::OPERATOR_RPAREN);
}

// NewArrayExpression -> BasicType [ Expression ] OptionalBrackets .
bool Parser::parseNewArrayExpression()
{
	return parseBasicType() &&
	       expect(Token::Token_type::OPERATOR_LBRACKET) &&
	       parseExpression() &&
	       expect(Token::Token_type::OPERATOR_RBRACKET) &&
	       parseOptionalBrackets();
}

// OptionalBrackets -> [ ] OptionalBrackets
//     | .
bool Parser::parseOptionalBrackets()
{
	Token t = current;

	while (expect(Token::Token_type::OPERATOR_LBRACKET, false))
	{
		if (expect(Token::Token_type::OPERATOR_RBRACKET, false))
			t = current;
		else
		{
			lexer.unget_token(current);
			current = t;
			return true;
		}
	}

	return true;
}

// BlockStatement -> Statement | LocalVariableDeclarationStatement .
bool Parser::parseBlockStatement()
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
			return parseStatement();
			break;

		case Token::Token_type::KEYWORD_VOID:
		case Token::Token_type::KEYWORD_INT:
		case Token::Token_type::KEYWORD_BOOLEAN:
			return parseLocalVariableDeclarationStatement();
			break;

		case Token::Token_type::TOKEN_IDENT:
			idToken = current;

			if (!nextToken())
				return false;

			maybeLBracketToken = current;

			if (maybeLBracketToken.token_type == Token::Token_type::TOKEN_IDENT)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				return parseLocalVariableDeclarationStatement();
			}
			else if (maybeLBracketToken.token_type != Token::Token_type::OPERATOR_LBRACKET)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				return parseStatement();
			}
			else
			{
				if (!nextToken())
					return false;

				maybeRBracketToken = current;

				bool isRBracket = maybeRBracketToken.token_type == Token::Token_type::OPERATOR_RBRACKET;

				lexer.unget_token(maybeRBracketToken);
				lexer.unget_token(maybeLBracketToken);
				current = idToken;

				if (isRBracket)
					return parseLocalVariableDeclarationStatement();
				else
					return parseStatement();
			}

			break;

		default:
			printError("expected Statement");
			return false;
	}

	/* unreachable */
	printError("expected Statement -- unreachable");
	return false;
}

// LocalVariableDeclarationStatement -> TypeIdent OptionalLVDSExpression ; .
// OptionalLVDSExpression -> = Expression
//	| .
bool Parser::parseLocalVariableDeclarationStatement()
{
	if (!parseTypeIdent())
		return false;

	if (expect(Token::Token_type::OPERATOR_EQ, false))
	{
		if (!parseExpression())
			return false;
	}

	return expect(Token::Token_type::OPERATOR_SEMICOLON);
}

// IfStatement -> if ( Expression ) Statement OptionalElseStatement .
// OptionalElseStatement -> else Statement
// 	| .
bool Parser::parseIfStatement()
{
	bool isValidIf = expect(Token::Token_type::KEYWORD_IF) &&
	                 expect(Token::Token_type::OPERATOR_LPAREN) &&
	                 parseExpression() &&
	                 expect(Token::Token_type::OPERATOR_RPAREN) &&
	                 parseStatement();

	if (!isValidIf)
		return false;

	if (expect(Token::Token_type::KEYWORD_ELSE, false))
		return parseStatement();
	else
		return true;
}

// WhileStatement -> while ( Expression ) Statement .
bool Parser::parseWhileStatement()
{
	return expect(Token::Token_type::KEYWORD_WHILE) &&
	       expect(Token::Token_type::OPERATOR_LPAREN) &&
	       parseExpression() &&
	       expect(Token::Token_type::OPERATOR_RPAREN) &&
	       parseStatement();
}

// ReturnStatement -> return OptionalExpression ; .
// OptionalExpression -> Expression
//  	| .
bool Parser::parseReturnStatement()
{
	bool hasReturnKeyword = expect(Token::Token_type::KEYWORD_RETURN);

	if (!hasReturnKeyword)
		return false;

	if (!expect(Token::Token_type::OPERATOR_SEMICOLON, false))
	{
		return parseExpression() &&
		       expect(Token::Token_type::OPERATOR_SEMICOLON);
	}

	return true;
}

// ExpressionStatement -> Expression ; .
bool Parser::parseExpressionStatement()
{
	return parseExpression() &&
	       expect(Token::Token_type::OPERATOR_SEMICOLON);
}

// ArrayAccess -> [ Expression ] .
bool Parser::parseArrayAccess()
{
	return expect(Token::Token_type::OPERATOR_LBRACKET) &&
	       parseExpression() &&
	       expect(Token::Token_type::OPERATOR_RBRACKET);
}

// Arguments -> Expression ArgumentsExpressions
//     | .
// ArgumentsExpressions -> , Expression ArgumentsExpressions
//     | .
bool Parser::parseArguments()
{
	bool isFirstArgument = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstArgument = false;

		if (!parseExpression())
			return false;

		if (!expect(Token::Token_type::OPERATOR_COMMA, false))
			return true;
	}

	if (!isFirstArgument)
		printError("trailing comma");

	return isFirstArgument;
}

// MethodInvocation -> ( Arguments ) .
bool Parser::parseMethodInvocation()
{
	return expect(Token::Token_type::OPERATOR_LPAREN) &&
	       parseArguments() &&
	       expect(Token::Token_type::OPERATOR_RPAREN);
}

// MethodInvocationOrFieldAccess -> MethodInvocation | .
bool Parser::parseMethodInvocationOrFieldAccess()
{
	if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
		return parseMethodInvocation();

	return true;
}

// ExclMarkOrHyphen -> ! | - .
bool Parser::parseExclMarkOrHyphen()
{
	return expect(Token::Token_type::OPERATOR_NOT, false) ||
	       expect(Token::Token_type::OPERATOR_MINUS);
}

// PostfixExpression -> PrimaryExpression PostfixOps .
bool Parser::parsePostfixExpression()
{
	return parsePrimaryExpression() && parsePostfixOps();
}

// PostfixOps -> PostfixOp PostfixOps
//     | .
// PostfixOp -> DOT IDENT MethodInvocationOrFieldAccess
//     | ArrayAccess .
bool Parser::parsePostfixOps()
{
	while (true)
	{
		if (expect(Token::Token_type::OPERATOR_DOT, false))
		{
			if (!(expect(Token::Token_type::TOKEN_IDENT) && parseMethodInvocationOrFieldAccess()))
				return false;
		}
		else if (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
		{
			if (!parseArrayAccess())
				return false;
		}
		else
			return true;
	}
}

// UnaryExpression -> PostfixExpression | ExclMarkOrHyphen UnaryExpression .
bool Parser::parseUnaryExpression()
{
	if (current.token_type == Token::Token_type::OPERATOR_NOT ||
	        current.token_type == Token::Token_type::OPERATOR_MINUS)
		return nextToken() && parseUnaryExpression();
	else
		return parsePostfixExpression();
}
