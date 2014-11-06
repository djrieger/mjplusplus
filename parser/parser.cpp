#include <iostream>

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
	nextToken();
	bool r = parseProgram();

	if (!r)
		std::cerr << "parsing failed at '" << current.string_value << std::endl;

	return r;
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

// Program -> ClassDeclaration Program | .
bool Parser::parseProgram()
{
	while (current.token_type == Token::Token_type::KEYWORD_CLASS)
	{
		if (!parseClassDeclaration())
			return false;
	}

	return true;
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

		nextToken();
		bool rhs = precedenceClimb(prec);
		result = result && rhs;
		op = current.token_type;

		if (!result || operator_precs.find(op) == operator_precs.end())
			return result;

		std::tie(prec, left_assoc) = operator_precs[op];
	}

	return result;
}


// PrimaryExpression -> null | false | true | INTEGER_LITERAL | IDENT IdentOrIdentWithArguments | this | ( Expression ) | new NewObjectOrNewArrayExpression .↩
bool Parser::parsePrimaryExpression()
{
	if (expect(Token::Token_type::KEYWORD_NULL))
		return true;
	else if (expect(Token::Token_type::KEYWORD_FALSE))
		return true;
	else if (expect(Token::Token_type::KEYWORD_TRUE))
		return true;
	else if (expect(Token::Token_type::TOKEN_INT_LIT))
		return true;
	else if (expect(Token::Token_type::KEYWORD_THIS))
		return true;
	else if (expect(Token::Token_type::TOKEN_IDENT))
		return parseIdentOrIdentWithArguments();
	else if (expect(Token::Token_type::OPERATOR_LPAREN))
	{
		if (parseExpression())
			return expect(Token::Token_type::OPERATOR_RPAREN);
	}
	else if (expect(Token::Token_type::KEYWORD_NEW))
		return parseNewObjectOrNewArrayExpression();

	return false;
}

// IdentOrIdentWithArguments -> ( Arguments )
//     | .
bool Parser::parseIdentOrIdentWithArguments()
{
	if (expect(Token::Token_type::OPERATOR_LPAREN))
	{
		if (parseArguments())
			return expect(Token::Token_type::OPERATOR_RPAREN);
	}

	return true;
}

// NewObjectOrNewArrayExpression -> NewObjectExpression | NewArrayExpression .
bool Parser::parseNewObjectOrNewArrayExpression()
{
	return parseNewObjectExpression() || parseNewArrayExpression();
}

// NewObjectExpression -> IDENT ( ) .
bool Parser::parseNewObjectExpression()
{
	if (expect(Token::Token_type::TOKEN_IDENT))
	{
		if (expect(Token::Token_type::OPERATOR_LPAREN))
			return expect(Token::Token_type::OPERATOR_RPAREN);
	}

	return false;
}

// NewArrayExpression -> BasicType [ Expression ] OptionalBrackets .
bool Parser::parseNewArrayExpression()
{
	if (parseBasicType())
	{
		if (expect(Token::Token_type::OPERATOR_LBRACKET))
		{
			if (parseExpression())
			{
				if (expect(Token::Token_type::OPERATOR_RBRACKET))
					return parseOptionalBrackets();
			}
		}
	}

	return false;
}

// OptionalBrackets -> [ ] OptionalBrackets
//     | .
bool Parser::parseOptionalBrackets()
{
	if (expect(Token::Token_type::OPERATOR_LBRACKET))
	{
		if (expect(Token::Token_type::OPERATOR_RBRACKET))
			return parseOptionalBrackets();
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
			maybeLBracketToken = nextToken();

			if (!expect(Token::Token_type::OPERATOR_LBRACKET))
			{
				//lexer.unget_token(maybeLBracketToken);
				current = idToken;
				return parseStatement();
			}
			else
			{
				maybeRBracketToken = current;

				bool isRBracket = expect(Token::Token_type::OPERATOR_RBRACKET);

				if (isRBracket)
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
			return false;
	}

	/* unreachable */
	return false;
}

// LocalVariableDeclarationStatement -> TypeIdent OptionalLVDSExpression ; .
// OptionalLVDSExpression -> = Expression
//	| .
bool Parser::parseLocalVariableDeclarationStatement()
{
	if (!parseTypeIdent())
		return false;

	if (expect(Token::Token_type::OPERATOR_EQ))
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

	if (expect(Token::Token_type::KEYWORD_ELSE))
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

// ReturnStatement -> return OptionalExpression .
bool Parser::parseReturnStatement()
{
	bool hasReturnKeyword = expect(Token::Token_type::KEYWORD_RETURN);

	if (!hasReturnKeyword)
		return false;

	if (!expect(Token::Token_type::OPERATOR_SEMICOLON))
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


// NewKeyword -> new .
bool Parser::parseNewKeyword()
{
	return expect(Token::Token_type::KEYWORD_NEW);
}

// ArgumentsExpressions -> , Expression ArgumentsExpressions
//     | .
bool Parser::parseArgumentsExpressions()
{
	if (expect(Token::Token_type::OPERATOR_COMMA))
		return parseExpression() && parseArgumentsExpressions();

	return true;
}

// ArrayAccess -> [ Expression ] .
bool Parser::parseArrayAccess()
{
	if (expect(Token::Token_type::OPERATOR_LBRACKET))
	{
		if (parseArguments())
			return expect(Token::Token_type::OPERATOR_RBRACKET);
		else
			return false;
	}

	return false;
}

// Arguments -> Expression ArgumentsExpressions
//     | .
bool Parser::parseArguments()
{
	if (parseExpression())
		return parseArgumentsExpressions();

	return true;
}

// MethodInvocation -> ( Arguments ) .
bool Parser::parseMethodInvocation()
{
	if (expect(Token::Token_type::OPERATOR_LPAREN))
	{
		if (parseArguments())
			return expect(Token::Token_type::OPERATOR_RPAREN);
	}

	return false;
}

// MethodInvocationOrFieldAccess -> MethodInvocation | .
bool Parser::parseMethodInvocationOrFieldAccess()
{
	parseMethodInvocation();
	return true;
}

// ExclMarkOrHyphen -> ! | - .
bool Parser::parseExclMarkOrHyphen()
{
	if (expect(Token::Token_type::OPERATOR_NOT))
		return true;
	else if (expect(Token::Token_type::OPERATOR_MINUS))
		return true;

	return false;
}

// PostfixExpression -> PrimaryExpression PostfixOps .
bool Parser::parsePostfixExpression()
{
	if (parsePrimaryExpression() && parsePostfixOps())
		return true;
	else
		return false;
}

// PostfixOps -> PostfixOp PostfixOps
//     | .
bool Parser::parsePostfixOps()
{
	if (parsePostfixOp())
		return parsePostfixOps();

	return true;
}

// PostfixOp -> DOT IDENT MethodInvocationOrFieldAccess
//     | ArrayAccess .
bool Parser::parsePostfixOp()
{
	if (parseArrayAccess())
		return true;
	else
	{
		if (expect(Token::Token_type::OPERATOR_DOT))
		{
			if (expect(Token::Token_type::TOKEN_IDENT))
				return parseMethodInvocationOrFieldAccess();
		}
	}

	return false;
}

// UnaryExpression -> PostfixExpression | ExclMarkOrHyphen UnaryExpression .
bool Parser::parseUnaryExpression()
{
	if (parsePostfixExpression())
		return true;
	else if (parseExclMarkOrHyphen())
		return parseUnaryExpression();
	else
		return false;
}
