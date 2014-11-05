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
	bool ret = current.token_type == tokenType && current.string_val == string_val;

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

bool Parser::parseClassDeclaration()
{
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
