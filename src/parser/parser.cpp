#include <iostream>
#include <algorithm>
#include <memory>
#include "../globals.hpp"
#include "parser.hpp"

Parser::Parser(lexer::Lexer& lexer, shptr<ErrorReporter> errorReporter) : lexer(lexer), errorReporter(errorReporter), current {lexer::Token::Token_type::TOKEN_ERROR, "", { -1, 0}}
{
}

int Parser::operator_precs(lexer::Token::Token_type t)
{
	switch (t)
	{
		case lexer::Token::Token_type::OPERATOR_EQ:
			return 1;

		case lexer::Token::Token_type::OPERATOR_OROR:
			return 2;

		case lexer::Token::Token_type::OPERATOR_ANDAND:
			return 3;

		case lexer::Token::Token_type::OPERATOR_EQEQ:
		case lexer::Token::Token_type::OPERATOR_NOTEQ:
			return 4;

		case lexer::Token::Token_type::OPERATOR_LT:
		case lexer::Token::Token_type::OPERATOR_LTEQ:
		case lexer::Token::Token_type::OPERATOR_GT:
		case lexer::Token::Token_type::OPERATOR_GTEQ:
			return 5;

		case lexer::Token::Token_type::OPERATOR_PLUS:
		case lexer::Token::Token_type::OPERATOR_MINUS:
			return 6;

		case lexer::Token::Token_type::OPERATOR_MULT:
		case lexer::Token::Token_type::OPERATOR_SLASH:
		case lexer::Token::Token_type::OPERATOR_MOD:
			return 7;

		default:
			return -1;
	}
}

bool Parser::start()
{
	bool r = false;

	try
	{
		nextToken();
		astRoot = parseProgram();
		r = true;
	}
	catch (char const* msg)
	{
		reportError(msg);
	}
	catch (lexer::Token::Token_type tokenType)
	{
		if (tokenType != lexer::Token::Token_type::TOKEN_ERROR)
			/* no need to produce two messages for lexer errors */
			reportError("expected " + lexer.describe(tokenType));
	}
	catch (std::string string_val)
	{
		reportError("expected \"" + string_val + '"');
	}

	return r && !errors_found;
}

shptr<ast::Program> Parser::getRoot()
{
	return astRoot;
}

void __attribute__ ((noinline)) Parser::nextToken()
{
	current = lexer.get_next_token();

	if (current.token_type == lexer::Token::Token_type::TOKEN_ERROR)
		throw current.token_type;
}

void Parser::reportError(std::string const& error_msg)
{
	errorReporter->recordError(ErrorReporter::ErrorType::PARSER,
	                           "parsing \"" + *current.string_value + '"' + (error_msg.empty() ? "" : ": " + error_msg),
	                           current.position);
}

void Parser::expect(lexer::Token::Token_type const& tokenType)
{
	if (current.token_type != tokenType)
	{
		errors_found = true;

		if (!error_mode)
		{
			reportError("expected " + lexer.describe(tokenType));

			error_mode = true;
		}

		// skip until token found
		do
		{
			if (current.token_type != lexer::Token::Token_type::TOKEN_EOF)
				return;

			nextToken();
		}
		while (current.token_type != tokenType);

		nextToken();
		error_mode = false;

	}

	nextToken();
}

void Parser::expect(lexer::Token::Token_type const& tokenType, std::string const& string_val)
{
	if (current.token_type != tokenType || *current.string_value != string_val)
	{
		errors_found = true;

		if (!error_mode)
		{
			reportError("expected \"" + string_val + '"');

			error_mode = true;
		}

		// skip until token found
		do
		{
			if (current.token_type != lexer::Token::Token_type::TOKEN_EOF)
				return;

			nextToken();
		}
		while (current.token_type != tokenType || *current.string_value != string_val);

		nextToken();
		error_mode = false;

	}

	nextToken();
}

// Program -> class ClassDeclaration Program | .
// ClassDeclaration -> IDENT { ClassMembers } .
shptr<ast::Program> Parser::parseProgram()
{
	auto classes = std::make_shared<vec<shptr<ast::ClassDeclaration>>>();

	while (current.token_type == lexer::Token::Token_type::KEYWORD_CLASS)
	{
		nextToken();
		// now current.string_value contains the name of the class:
		auto className = std::make_shared<ast::Ident>(current);
		expect(lexer::Token::Token_type::TOKEN_IDENT);
		expect(lexer::Token::Token_type::OPERATOR_LBRACE);
		classes->push_back(std::make_shared<ast::ClassDeclaration>(className, parseClassMembers()));
		expect(lexer::Token::Token_type::OPERATOR_RBRACE);
	}

	expect(lexer::Token::Token_type::TOKEN_EOF);
	return std::make_shared<ast::Program>(classes);
}

// ClassMembers -> public ClassMember ClassMembers | .
// ClassMember -> TypeIdent FieldOrMethod | static MainMethod .
shptr<vec<shptr<ast::ClassMember>>> Parser::parseClassMembers()
{
	auto classMembers = std::make_shared<vec<shptr<ast::ClassMember>>>();

	while (current.token_type == lexer::Token::Token_type::KEYWORD_PUBLIC)
	{
		nextToken();

		if (current.token_type == lexer::Token::Token_type::KEYWORD_STATIC)
		{
			nextToken();
			classMembers->push_back(parseMainMethod());
		}
		else
			classMembers->push_back(parseFieldOrMethod());
	}

	return classMembers;
}

// MainMethod -> void IDENT ( String [ ] IDENT ) Block .
shptr<ast::MainMethodDeclaration> Parser::parseMainMethod()
{
	// build "void METHODNAME"
	std::shared_ptr<ast::Type> voidType( new ast::Type(ast::Type::Primitive_type::VOID));
	expect(lexer::Token::Token_type::KEYWORD_VOID);
	auto mainMethodName = std::make_shared<ast::Ident>(current);
	auto typeIdent = std::make_shared<ast::TypeIdent>(voidType, mainMethodName);
	expect(lexer::Token::Token_type::TOKEN_IDENT);
	expect(lexer::Token::Token_type::OPERATOR_LPAREN);

	// build "String[] PARAMETERNAME"
	auto stringType = std::make_shared<ast::Ident>(current);
	auto type = std::make_shared<ast::Type>(stringType, 1);
	expect(lexer::Token::Token_type::TOKEN_IDENT, "String");
	expect(lexer::Token::Token_type::OPERATOR_LBRACKET);
	expect(lexer::Token::Token_type::OPERATOR_RBRACKET);
	auto parameterName = std::make_shared<ast::Ident>(current);
	expect(lexer::Token::Token_type::TOKEN_IDENT);
	auto parameters = std::make_shared<vec<shptr<ast::TypeIdent>>>();
	parameters->push_back(std::make_shared<ast::TypeIdent>(type, parameterName));
	expect(lexer::Token::Token_type::OPERATOR_RPAREN);

	return std::make_shared<ast::MainMethodDeclaration>(typeIdent, parameters, parseBlock());
}

// TypeIdent -> Type IDENT
shptr<ast::TypeIdent> Parser::parseTypeIdent()
{
	auto type = parseType();

	// retrieve variable name:
	auto variable_name = std::make_shared<ast::Ident>(current);
	expect(lexer::Token::Token_type::TOKEN_IDENT);

	return std::make_shared<ast::TypeIdent>(type, variable_name);
}

// ArrayDecl -> [ ] ArrayDecl | .
int Parser::parseArrayDecl()
{
	int dimension = 0;

	while (current.token_type == lexer::Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();
		expect(lexer::Token::Token_type::OPERATOR_RBRACKET);
		dimension++;
	}

	return dimension;
}

// BasicType -> int | boolean | void | IDENT .
shptr<ast::Type> Parser::parseBasicType()
{
	ast::Type::Primitive_type primitive_type = ast::Type::Primitive_type::NONE;

	switch (current.token_type)
	{
		case lexer::Token::Token_type::KEYWORD_INT:
			primitive_type = ast::Type::Primitive_type::INT;
			break;

		case lexer::Token::Token_type::KEYWORD_BOOLEAN:
			primitive_type = ast::Type::Primitive_type::BOOLEAN;
			break;

		case lexer::Token::Token_type::KEYWORD_VOID:
			primitive_type = ast::Type::Primitive_type::VOID;
			break;

		case lexer::Token::Token_type::TOKEN_IDENT:
		{
			auto class_name = std::make_shared<ast::Ident>(current);
			nextToken();
			return std::make_shared<ast::Type>(class_name);
			break;
		}

		default:
			throw "expected Type";
	}

	nextToken();
	return  std::make_shared<ast::Type>(primitive_type);
}

// Type -> BasicType ArrayDecl .
shptr<ast::Type> Parser::parseType()
{
	auto type = parseBasicType();
	int dimension = parseArrayDecl();
	type->setDimension(dimension);
	return type;
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
shptr<ast::ClassMember> Parser::parseFieldOrMethod()
{
	auto typeIdent = parseTypeIdent();

	if (current.token_type == lexer::Token::Token_type::OPERATOR_SEMICOLON)
	{
		nextToken();
		return std::make_shared<ast::FieldDeclaration>(typeIdent);
	}
	else
	{
		expect(lexer::Token::Token_type::OPERATOR_LPAREN);
		auto parameters = parseOptionalParameters();
		expect(lexer::Token::Token_type::OPERATOR_RPAREN);
		return std::make_shared<ast::MethodDeclaration>(typeIdent, parameters, parseBlock());
	}
}

// OptionalParameters -> Parameters | .
// Parameters -> Parameter FollowingParameters .
// FollowingParameters -> , Parameters
//     | .
// Parameter -> TypeIdent .
shptr<vec<shptr<ast::TypeIdent>>> Parser::parseOptionalParameters()
{
	auto parameters = std::make_shared<vec<shptr<ast::TypeIdent>>>();
	bool isFirstParameter = true;

	while (current.token_type != lexer::Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstParameter = false;
		parameters->push_back(parseTypeIdent());

		if (current.token_type != lexer::Token::Token_type::OPERATOR_COMMA)
			return parameters;
		else
			nextToken();
	}

	if (!isFirstParameter)
		throw "trailing comma";

	return parameters;
}

// Statement -> Block | EmptyStatement | if IfStatement | Expression ; | while WhileStatement | return ReturnStatement .
// EmptyStatement -> ; .
shptr<ast::Statement> Parser::parseStatement()
{
	switch (current.token_type)
	{
		case lexer::Token::Token_type::OPERATOR_LBRACE:
			return parseBlock();
			break;

		case lexer::Token::Token_type::OPERATOR_SEMICOLON:
		{
			nextToken();
			shptr<ast::Statement> stmt;
			return stmt;
			break;
		}

		case lexer::Token::Token_type::KEYWORD_IF:
			nextToken();
			return parseIfStatement();
			break;

		case lexer::Token::Token_type::KEYWORD_WHILE:
			nextToken();
			return parseWhileStatement();
			break;

		case lexer::Token::Token_type::KEYWORD_RETURN:
		{
			auto returnKeywordPosition = current.position;
			nextToken();
			return parseReturnStatement(returnKeywordPosition);
			break;
		}

		// IDENT, (, -, !, null, false, true, INTEGER_LITERAL, this, new
		case lexer::Token::Token_type::TOKEN_IDENT:
		case lexer::Token::Token_type::TOKEN_INT_LIT:
		case lexer::Token::Token_type::OPERATOR_LPAREN:
		case lexer::Token::Token_type::OPERATOR_MINUS:
		case lexer::Token::Token_type::OPERATOR_NOT:
		case lexer::Token::Token_type::KEYWORD_NULL:
		case lexer::Token::Token_type::KEYWORD_FALSE:
		case lexer::Token::Token_type::KEYWORD_TRUE:
		case lexer::Token::Token_type::KEYWORD_THIS:
		case lexer::Token::Token_type::KEYWORD_NEW:
		{
			auto expr = parseExpression();
			expect(lexer::Token::Token_type::OPERATOR_SEMICOLON);
			return std::make_shared<ast::ExpressionStatement>(expr);
			break;
		}

		default:
			throw "expected Statement";
	}
}

// Block -> { BlockStatements } .
// BlockStatements -> BlockStatement BlockStatements | .
shptr<ast::Statement> Parser::parseBlock()
{
	expect(lexer::Token::Token_type::OPERATOR_LBRACE);

	auto statements = std::make_shared<vec<shptr<ast::Statement>>>();

	while (current.token_type != lexer::Token::Token_type::OPERATOR_RBRACE)
	{
		auto block_statement = parseBlockStatement();

		//skip emtpy statements
		if (block_statement)
			statements->push_back(block_statement);
	}

	nextToken();

	//If there are no block_statements we can skip the block.
	if (!statements->empty())
		return std::make_shared<ast::Block>(statements);
	else
	{
		shptr<ast::Statement> stmt;
		return stmt;
	}
}

// BlockStatement -> Statement | LocalVariableDeclarationStatement .
shptr<ast::Statement> Parser::parseBlockStatement()
{
	// Statement first = IDENT, {, (, ;, while, if, return, -, !, null, false, true, INTEGER_LITERAL, this, new
	// LVDS first =      IDENT, void, int, boolean

	switch (current.token_type)
	{
		case lexer::Token::Token_type::OPERATOR_LBRACE:
		case lexer::Token::Token_type::OPERATOR_LPAREN:
		case lexer::Token::Token_type::OPERATOR_SEMICOLON:
		case lexer::Token::Token_type::KEYWORD_WHILE:
		case lexer::Token::Token_type::KEYWORD_IF:
		case lexer::Token::Token_type::KEYWORD_RETURN:
		case lexer::Token::Token_type::OPERATOR_MINUS:
		case lexer::Token::Token_type::OPERATOR_NOT:
		case lexer::Token::Token_type::KEYWORD_NULL:
		case lexer::Token::Token_type::KEYWORD_FALSE:
		case lexer::Token::Token_type::KEYWORD_TRUE:
		case lexer::Token::Token_type::TOKEN_INT_LIT:
		case lexer::Token::Token_type::KEYWORD_THIS:
		case lexer::Token::Token_type::KEYWORD_NEW:
			return parseStatement();
			break;

		case lexer::Token::Token_type::KEYWORD_VOID:
		case lexer::Token::Token_type::KEYWORD_INT:
		case lexer::Token::Token_type::KEYWORD_BOOLEAN:
			return parseLocalVariableDeclarationStatement();
			break;

		case lexer::Token::Token_type::TOKEN_IDENT:
		{
			lexer::Token idToken = current;
			nextToken();
			lexer::Token maybeLBracketToken = current;

			if (maybeLBracketToken.token_type == lexer::Token::Token_type::TOKEN_IDENT)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				return parseLocalVariableDeclarationStatement();
			}
			else if (maybeLBracketToken.token_type != lexer::Token::Token_type::OPERATOR_LBRACKET)
			{
				lexer.unget_token(maybeLBracketToken);
				current = idToken;
				return parseStatement();
			}
			else
			{
				nextToken();
				lexer::Token maybeRBracketToken = current;
				bool isRBracket = maybeRBracketToken.token_type == lexer::Token::Token_type::OPERATOR_RBRACKET;
				lexer.unget_token(maybeRBracketToken);
				lexer.unget_token(maybeLBracketToken);
				current = idToken;

				if (isRBracket)
					return parseLocalVariableDeclarationStatement();
				else
					return parseStatement();
			}

			break;
		}

		default:
			throw "expected Statement";
	}

	throw "unreachable";
}

// LocalVariableDeclarationStatement -> TypeIdent OptionalLVDSExpression ; .
// OptionalLVDSExpression -> = Expression
//	| .
shptr<ast::LVDStatement> Parser::parseLocalVariableDeclarationStatement()
{
	auto type_ident = parseTypeIdent();
	shptr<ast::LVDStatement> lvdStatement;

	if (current.token_type == lexer::Token::Token_type::OPERATOR_EQ)
	{
		nextToken();
		lvdStatement = std::make_shared<ast::LVDStatement>(type_ident, parseExpression());
	}
	else
		lvdStatement = std::make_shared<ast::LVDStatement>(type_ident);

	expect(lexer::Token::Token_type::OPERATOR_SEMICOLON);
	return lvdStatement;
}

// IfStatement -> ( Expression ) Statement OptionalElseStatement .
// OptionalElseStatement -> else Statement
// 	| .
shptr<ast::IfStatement> Parser::parseIfStatement()
{
	expect(lexer::Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(lexer::Token::Token_type::OPERATOR_RPAREN);
	auto then = parseStatement();

	if (current.token_type == lexer::Token::Token_type::KEYWORD_ELSE)
	{
		nextToken();
		return std::make_shared<ast::IfStatement>(cond, then, parseStatement());
	}
	else
		return std::make_shared<ast::IfStatement>(cond, then);
}

// WhileStatement -> ( Expression ) Statement .
shptr<ast::WhileStatement> Parser::parseWhileStatement()
{
	expect(lexer::Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(lexer::Token::Token_type::OPERATOR_RPAREN);
	return std::make_shared<ast::WhileStatement>(cond, parseStatement());
}

// ReturnStatement -> OptionalExpression ; .
// OptionalExpression -> Expression
//  	| .
shptr<ast::ReturnStatement> Parser::parseReturnStatement(source_position_t returnKeywordPosition)
{
	if (current.token_type != lexer::Token::Token_type::OPERATOR_SEMICOLON)
	{
		auto expr = parseExpression();
		expect(lexer::Token::Token_type::OPERATOR_SEMICOLON);
		return std::make_shared<ast::ReturnStatement>(expr);
	}

	expect(lexer::Token::Token_type::OPERATOR_SEMICOLON);
	return std::make_shared<ast::ReturnStatement>(returnKeywordPosition);
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
shptr<ast::Expression> Parser::parseExpression()
{
	return precedenceClimb(1);
}

/*
parses an expression via precedence climb
*/
shptr<ast::Expression> Parser::precedenceClimb(int minPrec)
{
	shptr<ast::Expression> expr = parseUnaryExpression();
	int prec = operator_precs(current.token_type);

	while (prec >= minPrec)
	{
		if (prec > 1) //equivalent to the fact that operator is left associative
			prec++;

		lexer::Token::Token_type t = current.token_type;
		nextToken();

		expr = ast::be::BinaryExpression::createBinaryExpr(expr, precedenceClimb(prec), t);
		prec = operator_precs(current.token_type);
	}

	return expr;
}

// UnaryExpression -> PostfixExpression | ExclMarkOrHyphen UnaryExpression .
// PostfixExpression -> PrimaryExpression PostfixOps .
shptr<ast::Expression> Parser::parseUnaryExpression()
{
	auto unary_operators = std::make_shared<vec<lexer::Token::Token_type>>();

	while (current.token_type == lexer::Token::Token_type::OPERATOR_NOT ||
	        current.token_type == lexer::Token::Token_type::OPERATOR_MINUS)
	{
		unary_operators->push_back(current.token_type);
		nextToken();
	}

	auto primaryExpr = parsePrimaryExpression();
	auto postfix_ops = parsePostfixOps();
	shptr<ast::Expression> postfixExpr;

	// skip creating postfixExpr if no postfix operators present
	if (postfix_ops->empty())
		postfixExpr = primaryExpr;
	else
		postfixExpr = std::make_shared<ast::PostfixExpression>(primaryExpr, postfix_ops);

	//dito for unary expressions
	if (unary_operators->empty())
		return postfixExpr;
	else
		return ast::ue::UnaryExpression::createUnaryExpr(postfixExpr, unary_operators);
}

// PrimaryExpression -> null | false | true | INTEGER_LITERAL | IDENT IdentOrIdentWithArguments | this | ( Expression ) | new NewObjectOrNewArrayExpression .
// IdentOrIdentWithArguments -> ( Arguments )
//     | .
shptr<ast::Expression> Parser::parsePrimaryExpression()
{
	shptr<ast::Expression> pe;

	switch (current.token_type)
	{
		case lexer::Token::Token_type::KEYWORD_FALSE:
			pe = std::make_shared<ast::pe::Bool>(false, current.position);
			nextToken();
			break;

		case lexer::Token::Token_type::KEYWORD_TRUE:
			pe = std::make_shared<ast::pe::Bool>(true, current.position);
			nextToken();
			break;

		case lexer::Token::Token_type::KEYWORD_NULL:
			pe = std::make_shared<ast::pe::Object>(ast::pe::Object::Object_Type::NULL_OBJECT, current.position);
			nextToken();
			break;

		case lexer::Token::Token_type::KEYWORD_THIS:
			pe = std::make_shared<ast::pe::Object>(ast::pe::Object::Object_Type::THIS_OBJECT, current.position);
			nextToken();
			break;

		case lexer::Token::Token_type::TOKEN_INT_LIT:
			pe = std::make_shared<ast::pe::Integer>(*current.string_value, current.position);
			nextToken();
			break;

		case lexer::Token::Token_type::TOKEN_IDENT:
		{
			auto ident = std::make_shared<ast::Ident>(current);
			nextToken();

			if (current.token_type == lexer::Token::Token_type::OPERATOR_LPAREN)
			{
				nextToken();
				auto arguments = parseArguments();
				expect(lexer::Token::Token_type::OPERATOR_RPAREN);
				pe = std::make_shared<ast::pe::MethodInvocation>(ident, arguments);
			}
			else
				pe = std::make_shared<ast::pe::Ident>(ident);

			break;
		}

		case lexer::Token::Token_type::OPERATOR_LPAREN:
		{
			nextToken();
			pe = parseExpression();
			expect(lexer::Token::Token_type::OPERATOR_RPAREN);
			break;
		}

		case lexer::Token::Token_type::KEYWORD_NEW:
			nextToken();
			pe = parseNewObjectOrNewArrayExpression();
			break;

		default:
			throw "expected Expression";
	}

	return pe;
}

// NewObjectOrNewArrayExpression -> NewObjectExpression | NewArrayExpression .
shptr<ast::Expression> Parser::parseNewObjectOrNewArrayExpression()
{
	lexer::Token id = current;

	nextToken();
	lexer::Token next = current;
	lexer.unget_token(next);
	current = id;

	if (next.token_type == lexer::Token::Token_type::OPERATOR_LPAREN)
		return parseNewObjectExpression();
	else
		return parseNewArrayExpression();
}

// NewObjectExpression -> IDENT ( ) .
shptr<ast::Expression> Parser::parseNewObjectExpression()
{
	auto ident = std::make_shared<ast::Ident>(current);
	expect(lexer::Token::Token_type::TOKEN_IDENT);
	expect(lexer::Token::Token_type::OPERATOR_LPAREN);
	expect(lexer::Token::Token_type::OPERATOR_RPAREN);
	return std::make_shared<ast::pe::NewObjectExpression>(ident);
}

// NewArrayExpression -> BasicType [ Expression ] OptionalBrackets .
shptr<ast::Expression> Parser::parseNewArrayExpression()
{
	auto type = parseBasicType();
	expect(lexer::Token::Token_type::OPERATOR_LBRACKET);
	auto expression = parseExpression();
	expect(lexer::Token::Token_type::OPERATOR_RBRACKET);
	type->setDimension(parseOptionalBrackets() + 1);

	return std::make_shared<ast::pe::NewArrayExpression>(type, expression);
}

// OptionalBrackets -> [ ] OptionalBrackets
//     | .
int Parser::parseOptionalBrackets()
{
	int dimension = 0;
	lexer::Token t = current;

	while (current.token_type == lexer::Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();

		if (current.token_type == lexer::Token::Token_type::OPERATOR_RBRACKET)
		{
			dimension++;
			nextToken();
			t = current;
		}
		else
		{
			lexer.unget_token(current);
			current = t;
			return dimension;
		}
	}

	return dimension;
}

// Arguments -> Expression ArgumentsExpressions | .
// ArgumentsExpressions -> , Expression ArgumentsExpressions | .
shptr<ast::Arguments> Parser::parseArguments()
{
	auto args = std::make_shared<vec<shptr<ast::Expression>>>();
	bool isFirstArgument = true;

	while (current.token_type != lexer::Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstArgument = false;
		args->push_back(parseExpression());

		if (current.token_type != lexer::Token::Token_type::OPERATOR_COMMA)
			return std::make_shared<ast::Arguments>(args);
		else
			nextToken();
	}

	if (!isFirstArgument)
		throw "trailing comma";

	return std::make_shared<ast::Arguments>(args);
}

// MethodInvocationOrFieldAccess -> IDENT MethodInvocation | .
// MethodInvocation -> ( Arguments ) .
std::shared_ptr<ast::PostfixOp> Parser::parseMethodInvocationOrFieldAccess()
{
	auto id = std::make_shared<ast::Ident>(current);
	expect(lexer::Token::Token_type::TOKEN_IDENT);

	if (current.token_type == lexer::Token::Token_type::OPERATOR_LPAREN)
	{
		nextToken();
		auto args = parseArguments();
		expect(lexer::Token::Token_type::OPERATOR_RPAREN);
		return std::make_shared<ast::MethodInvocation>(id, args);
	}
	else
		return std::make_shared<ast::FieldAccess>(id);
}

// PostfixOps -> PostfixOp PostfixOps | .
// PostfixOp -> DOT MethodInvocationOrFieldAccess
//     | [ Expression ] .
shptr<vec<shptr<ast::PostfixOp>>> Parser::parsePostfixOps()
{
	auto postfixops = std::make_shared<std::vector<std::shared_ptr<ast::PostfixOp>>>();

	while (true)
	{
		if (current.token_type == lexer::Token::Token_type::OPERATOR_DOT)
		{
			nextToken();
			postfixops->push_back(parseMethodInvocationOrFieldAccess());
		}
		else if (current.token_type == lexer::Token::Token_type::OPERATOR_LBRACKET)
		{
			nextToken();
			postfixops->push_back(std::make_shared<ast::ArrayAccess>(parseExpression()));
			expect(lexer::Token::Token_type::OPERATOR_RBRACKET);
		}
		else
			return postfixops;
	}
}
