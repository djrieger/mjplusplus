#include <iostream>
#include <algorithm>
#include <memory>
#include "parser.hpp"

template <class T> using uptr = std::unique_ptr<T>;
template <class T> using vec = std::vector<T>;

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
		astRoot = std::move(parseProgram());
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

std::shared_ptr<ast::Program> Parser::getRoot()
{
	return astRoot;
}

void Parser::nextToken()
{
	current = lexer.get_next_token();

	if (current.token_type == Token::Token_type::TOKEN_ERROR)
		throw "Error from lexer";
}

void Parser::printError(std::string const& error_msg)
{
	if (print_messages)   // only print error messages if they are wanted
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
uptr<ast::Program> Parser::parseProgram()
{
	auto classes = std::make_unique<vec<uptr<ast::ClassDeclaration>>>();

	while (current.token_type == Token::Token_type::KEYWORD_CLASS)
	{
		nextToken();
		// now current.string_value contains the name of the class:
		auto className = std::make_unique<ast::Ident>(current.string_value);
		expect(Token::Token_type::TOKEN_IDENT);
		expect(Token::Token_type::OPERATOR_LBRACE);
		auto members = parseClassMembers();
		expect(Token::Token_type::OPERATOR_RBRACE);
		classes->push_back(std::make_unique<ast::ClassDeclaration>(className, members));
	}

	expect(Token::Token_type::TOKEN_EOF);
	return std::make_unique<ast::Program>(classes);
}

// ClassMembers -> public ClassMember ClassMembers | .
// ClassMember -> TypeIdent FieldOrMethod | static MainMethod .
uptr<vec<uptr<ast::ClassMember>>> Parser::parseClassMembers()
{
	auto classMembers = std::make_unique<vec<uptr<ast::ClassMember>>>();

	while (current.token_type == Token::Token_type::KEYWORD_PUBLIC)
	{
		nextToken();

		if (current.token_type == Token::Token_type::KEYWORD_STATIC)
		{
			nextToken();
			classMembers->push_back(std::move(parseMainMethod()));
		}
		else
		{

			auto typeIdent = parseTypeIdent();
			classMembers->push_back(std::move(parseFieldOrMethod(std::move(typeIdent))));
		}
	}

	return classMembers;
}

// MainMethod -> void IDENT ( String [ ] IDENT ) Block .
uptr<ast::MainMethodDeclaration> Parser::parseMainMethod()
{
	// build "void METHODNAME"
	std::unique_ptr<ast::Type> voidType( new ast::BasicType(ast::Type::Primitive_type::VOID));
	expect(Token::Token_type::KEYWORD_VOID);
	auto mainMethodName = std::make_unique<ast::Ident>(current.string_value);
	auto typeIdent = std::make_unique<ast::TypeIdent>(voidType, mainMethodName);

	// expect "METHODNAME (String[]"
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_LPAREN);
	expect(Token::Token_type::TOKEN_IDENT, "String");
	expect(Token::Token_type::OPERATOR_LBRACKET);
	expect(Token::Token_type::OPERATOR_RBRACKET);

	// build "String[] PARAMETERNAME"
	auto parameterName = std::make_unique<ast::Ident>(current.string_value);
	auto parameters = std::make_unique<vec<uptr<ast::TypeIdent>>>();
	auto basicType = std::make_unique<ast::BasicType>("String");
	std::unique_ptr<ast::Type> parType( new ast::ArrayType(basicType, 1) );
	parameters->push_back(std::make_unique<ast::TypeIdent>(parType, parameterName));

	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_RPAREN);

	auto block = parseBlock();

	return std::make_unique<ast::MainMethodDeclaration>(typeIdent, parameters, block);
}

// TypeIdent -> Type IDENT
// Type -> BasicType ArrayDecl .
uptr<ast::TypeIdent> Parser::parseTypeIdent()
{
	auto type = parseType();

	// retrieve variable name:
	auto variable_name = std::make_unique<ast::Ident>(current.string_value);
	expect(Token::Token_type::TOKEN_IDENT);

	auto typeIdent = std::make_unique<ast::TypeIdent>(type, variable_name);
	return typeIdent;
}

// ArrayDecl -> [ ] ArrayDecl | .
int Parser::parseArrayDecl()
{
	int dimension = 0;

	while (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();
		expect(Token::Token_type::OPERATOR_RBRACKET);
		dimension++;
	}

	return dimension;
}

uptr<ast::BasicType> Parser::parseBasicType()
{
	std::string class_name = "";
	ast::Type::Primitive_type primitive_type = ast::Type::Primitive_type::NONE;

	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_INT:
			primitive_type = ast::Type::Primitive_type::INT;
			break;

		case Token::Token_type::KEYWORD_BOOLEAN:
			primitive_type = ast::Type::Primitive_type::BOOLEAN;
			break;

		case Token::Token_type::KEYWORD_VOID:
			primitive_type = ast::Type::Primitive_type::VOID;
			break;

		case Token::Token_type::TOKEN_IDENT:
			class_name = current.string_value;
			break;

		default:
			throw "expected Type";

	}

	nextToken();

	if (class_name.empty())
		return  std::make_unique<ast::BasicType>(primitive_type);
	else
		return std::make_unique<ast::BasicType>(class_name);
}

// BasicType -> int | boolean | void | IDENT .
uptr<ast::Type> Parser::parseType()
{
	auto basicType = parseBasicType();

	int dimension = parseArrayDecl();
	uptr<ast::Type> type;

	if (dimension > 0)
		type = std::make_unique<ast::ArrayType>(basicType, dimension);
	else
		type = std::move(basicType);

	return type;
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
uptr<ast::ClassMember> Parser::parseFieldOrMethod(uptr<ast::TypeIdent> typeIdent)
{
	uptr<ast::ClassMember> classMember;

	if (current.token_type == Token::Token_type::OPERATOR_SEMICOLON)
	{
		classMember = std::make_unique<ast::FieldDeclaration>(typeIdent);
		nextToken();
	}
	else
	{
		expect(Token::Token_type::OPERATOR_LPAREN);
		auto parameters = parseOptionalParameters();
		expect(Token::Token_type::OPERATOR_RPAREN);
		auto block = parseBlock();

		classMember = std::make_unique<ast::MethodDeclaration>(typeIdent, parameters, block);;
	}

	return classMember;
}

// OptionalParameters -> Parameters | .
// Parameters -> Parameter FollowingParameters .
// FollowingParameters -> , Parameters
//     | .
// Parameter -> TypeIdent .
uptr<vec<uptr<ast::TypeIdent>>> Parser::parseOptionalParameters()
{
	auto parameters = std::make_unique<vec<uptr<ast::TypeIdent>>>();
	bool isFirstParameter = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstParameter = false;
		parameters->push_back(std::move(parseTypeIdent()));

		if (current.token_type != Token::Token_type::OPERATOR_COMMA)
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
uptr<ast::Statement> Parser::parseStatement()
{
	switch (current.token_type)
	{
		case Token::Token_type::OPERATOR_LBRACE:
			return parseBlock();
			break;

		case Token::Token_type::OPERATOR_SEMICOLON:
		{
			nextToken();
			uptr<ast::Statement> stmt;
			return stmt;
			break;
		}

		case Token::Token_type::KEYWORD_IF:
			nextToken();
			return parseIfStatement();
			break;

		case Token::Token_type::KEYWORD_WHILE:
			nextToken();
			return parseWhileStatement();
			break;

		case Token::Token_type::KEYWORD_RETURN:
			nextToken();
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
		{
			auto expr = parseExpression();
			expect(Token::Token_type::OPERATOR_SEMICOLON);
			return std::make_unique<ast::ExpressionStatement>(expr);
			break;
		}

		default:
			throw "expected Statement";
	}
}

// Block -> { BlockStatements } .
// BlockStatements -> BlockStatement BlockStatements | .
uptr<ast::Statement> Parser::parseBlock()
{
	expect(Token::Token_type::OPERATOR_LBRACE);

	auto statements = std::make_unique<vec<uptr<ast::Statement>>>();

	while (current.token_type != Token::Token_type::OPERATOR_RBRACE)
	{
		auto block_statement = parseBlockStatement();

		//skip emtpy statements
		if (block_statement)
			statements->push_back(std::move(block_statement));
	}

	nextToken();

	//If there are no block_statements we can skip the block.
	if (!statements->empty())
		return std::make_unique<ast::Block>(statements);
	else
	{
		uptr<ast::Statement> stmt;
		return stmt;
	}
}

// BlockStatement -> Statement | LocalVariableDeclarationStatement .
uptr<ast::Statement> Parser::parseBlockStatement()
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
			nextToken();
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
				nextToken();
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
			throw "expected Statement";
	}

	throw "unreachable";
}

// LocalVariableDeclarationStatement -> TypeIdent OptionalLVDSExpression ; .
// OptionalLVDSExpression -> = Expression
//	| .
uptr<ast::LVDStatement> Parser::parseLocalVariableDeclarationStatement()
{
	auto type_ident = parseTypeIdent();
	uptr<ast::LVDStatement> lvdStatement;

	if (current.token_type == Token::Token_type::OPERATOR_EQ)
	{
		nextToken();
		auto expression = parseExpression();
		lvdStatement = std::make_unique<ast::LVDStatement>(type_ident, expression);
	}
	else
		lvdStatement = std::make_unique<ast::LVDStatement>(type_ident);

	expect(Token::Token_type::OPERATOR_SEMICOLON);
	return lvdStatement;
}

// IfStatement -> ( Expression ) Statement OptionalElseStatement .
// OptionalElseStatement -> else Statement
// 	| .
uptr<ast::IfStatement> Parser::parseIfStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	auto then = parseStatement();

	if (current.token_type == Token::Token_type::KEYWORD_ELSE)
	{
		nextToken();
		auto elseStmt = parseStatement();
		return std::make_unique<ast::IfStatement>(cond, then, elseStmt);
	}
	else
		return std::make_unique<ast::IfStatement>(cond, then);
}

// WhileStatement -> ( Expression ) Statement .
uptr<ast::WhileStatement> Parser::parseWhileStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	auto stmt = parseStatement();
	return std::make_unique<ast::WhileStatement>(cond, stmt);
}

// ReturnStatement -> OptionalExpression ; .
// OptionalExpression -> Expression
//  	| .
uptr<ast::ReturnStatement> Parser::parseReturnStatement()
{
	if (current.token_type != Token::Token_type::OPERATOR_SEMICOLON)
	{
		auto expr = parseExpression();
		expect(Token::Token_type::OPERATOR_SEMICOLON);
		return std::make_unique<ast::ReturnStatement>(expr);
	}

	expect(Token::Token_type::OPERATOR_SEMICOLON);
	return std::make_unique<ast::ReturnStatement>();
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
uptr<ast::Expression> Parser::parseExpression()
{
	return precedenceClimb(1);
}

/*
parses an expression via precedence climb
*/
uptr<ast::Expression> Parser::precedenceClimb(int minPrec)
{
	uptr<ast::Expression> expr = parseUnaryExpression();
	int prec = operator_precs(current.token_type);

	while (prec >= minPrec)
	{
		if (prec > 1) //equivalent to the fact that operator is left associative
			prec++;

		Token::Token_type t = current.token_type;
		nextToken();
		auto exprRHS = precedenceClimb(prec);
		expr = std::make_unique<ast::BinaryExpression>(expr, exprRHS, t);

		prec = operator_precs(current.token_type);
	}

	return expr;
}

// UnaryExpression -> PostfixExpression | ExclMarkOrHyphen UnaryExpression .
// PostfixExpression -> PrimaryExpression PostfixOps .
uptr<ast::Expression> Parser::parseUnaryExpression()
{
	auto unary_operators = std::make_unique<vec<ast::UnaryExpression::Unary_Operator>>();

	while (current.token_type == Token::Token_type::OPERATOR_NOT ||
	        current.token_type == Token::Token_type::OPERATOR_MINUS)
	{
		switch (current.token_type)
		{
			case Token::Token_type::OPERATOR_NOT:
				unary_operators->push_back(ast::UnaryExpression::Unary_Operator::UNARY_NOT);
				break;

			case Token::Token_type::OPERATOR_MINUS:
				unary_operators->push_back(ast::UnaryExpression::Unary_Operator::UNARY_MINUS);
				break;

			default:
				break;
		}

		nextToken();
	}

	auto primaryExpr = parsePrimaryExpression();
	auto postfix_ops = parsePostfixOps();
	uptr<ast::Expression> postfixExpr;

	// skip creating postfixExpr if no postfix operators present
	if (postfix_ops->empty())
		postfixExpr = std::move(primaryExpr);
	else
		postfixExpr = std::make_unique<ast::PostfixExpression>(primaryExpr, postfix_ops);

	//dito for unary expressions
	if (unary_operators->empty())
		return postfixExpr;
	else
		return std::make_unique<ast::UnaryExpression>(postfixExpr, unary_operators);
}

// PrimaryExpression -> null | false | true | INTEGER_LITERAL | IDENT IdentOrIdentWithArguments | this | ( Expression ) | new NewObjectOrNewArrayExpression .
// IdentOrIdentWithArguments -> ( Arguments )
//     | .
uptr<ast::Expression> Parser::parsePrimaryExpression()
{
	uptr<ast::Expression> pe;

	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_FALSE:
			pe = std::make_unique<ast::pe::Bool>(false);
			nextToken();
			break;

		case Token::Token_type::KEYWORD_TRUE:
			pe = std::make_unique<ast::pe::Bool>(true);
			nextToken();
			break;

		case Token::Token_type::KEYWORD_NULL:
			pe = std::make_unique<ast::pe::Object>(ast::pe::Object::Object_Type::NULL_OBJECT);
			nextToken();
			break;

		case Token::Token_type::KEYWORD_THIS:
			pe = std::make_unique<ast::pe::Object>(ast::pe::Object::Object_Type::THIS_OBJECT);
			nextToken();
			break;

		case Token::Token_type::TOKEN_INT_LIT:
			pe = std::make_unique<ast::pe::Integer>(current.string_value);
			nextToken();
			break;

		case Token::Token_type::TOKEN_IDENT:
		{
			auto ident = std::make_unique<ast::Ident>(current.string_value);
			nextToken();

			if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
			{
				nextToken();
				auto arguments = parseArguments();
				expect(Token::Token_type::OPERATOR_RPAREN);
				pe = std::make_unique<ast::pe::MethodInvocation>(ident, arguments);
			}
			else
				pe = std::make_unique<ast::pe::Ident>(ident);

			break;
		}

		case Token::Token_type::OPERATOR_LPAREN:
		{
			nextToken();
			pe = parseExpression();
			expect(Token::Token_type::OPERATOR_RPAREN);
			break;
		}

		case Token::Token_type::KEYWORD_NEW:
			nextToken();
			pe = parseNewObjectOrNewArrayExpression();
			break;

		default:
			throw "expected Expression";
	}

	return pe;
}

// NewObjectOrNewArrayExpression -> NewObjectExpression | NewArrayExpression .
uptr<ast::Expression> Parser::parseNewObjectOrNewArrayExpression()
{
	Token id = current;

	nextToken();
	Token next = current;
	lexer.unget_token(next);
	current = id;

	uptr<ast::Expression> pe;

	if (next.token_type == Token::Token_type::OPERATOR_LPAREN)
	{
		auto newObjectExpression = parseNewObjectExpression();
		return std::move(newObjectExpression);
	}
	else
		pe = parseNewArrayExpression();

	return pe;
}

// NewObjectExpression -> IDENT ( ) .
uptr<ast::Expression> Parser::parseNewObjectExpression()
{
	auto ident = std::make_unique<ast::Ident>(current.string_value);
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_LPAREN);
	expect(Token::Token_type::OPERATOR_RPAREN);
	return std::make_unique<ast::pe::NewObjectExpression>(ident);
}

// NewArrayExpression -> BasicType [ Expression ] OptionalBrackets .
uptr<ast::Expression> Parser::parseNewArrayExpression()
{
	auto basicType = parseBasicType();
	expect(Token::Token_type::OPERATOR_LBRACKET);
	auto expression = parseExpression();
	expect(Token::Token_type::OPERATOR_RBRACKET);
	int dimension = parseOptionalBrackets();

	return std::make_unique<ast::pe::NewArrayExpression>(basicType, expression, dimension + 1);
}

// OptionalBrackets -> [ ] OptionalBrackets
//     | .
int Parser::parseOptionalBrackets()
{
	int dimension = 0;
	Token t = current;

	while (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
	{
		nextToken();

		if (current.token_type == Token::Token_type::OPERATOR_RBRACKET)
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
uptr<ast::Arguments> Parser::parseArguments()
{
	auto args = std::make_unique<vec<uptr<ast::Expression>>>();
	bool isFirstArgument = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstArgument = false;
		args->push_back(std::move(parseExpression()));

		if (current.token_type != Token::Token_type::OPERATOR_COMMA)
			return std::make_unique<ast::Arguments>(args);
		else
			nextToken();
	}

	if (!isFirstArgument)
		throw "trailing comma";

	return std::make_unique<ast::Arguments>(args);
}

// MethodInvocationOrFieldAccess -> IDENT MethodInvocation | .
// MethodInvocation -> ( Arguments ) .
std::unique_ptr<ast::PostfixOp> Parser::parseMethodInvocationOrFieldAccess()
{
	auto id = std::make_unique<ast::Ident>(current.string_value);
	expect(Token::Token_type::TOKEN_IDENT);

	if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
	{
		nextToken();
		auto args = parseArguments();
		expect(Token::Token_type::OPERATOR_RPAREN);
		return std::make_unique<ast::MethodInvocation>(id, args);
	}
	else
		return std::make_unique<ast::FieldAccess>(id);
}

// PostfixOps -> PostfixOp PostfixOps | .
// PostfixOp -> DOT MethodInvocationOrFieldAccess
//     | [ Expression ] .
uptr<vec<uptr<ast::PostfixOp>>> Parser::parsePostfixOps()
{
	auto postfixops = std::make_unique<std::vector<std::unique_ptr<ast::PostfixOp>>>();

	while (true)
	{
		if (current.token_type == Token::Token_type::OPERATOR_DOT)
		{
			nextToken();
			postfixops->push_back(parseMethodInvocationOrFieldAccess());
		}
		else if (current.token_type == Token::Token_type::OPERATOR_LBRACKET)
		{
			nextToken();
			auto expr = parseExpression();
			postfixops->push_back(std::make_unique<ast::ArrayAccess>(expr));
			expect(Token::Token_type::OPERATOR_RBRACKET);
		}
		else
			return postfixops;
	}
}
