#include <iostream>
#include <algorithm>
#include <memory>
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
std::unique_ptr<ast::Program> Parser::parseProgram()
{
	auto classes = std::make_unique<std::vector<ast::ClassDeclaration>>();

	while (current.token_type == Token::Token_type::KEYWORD_CLASS)
	{
		nextToken();
		// now current.string_value contains the name of the class:
		auto className = std::make_unique<ast::Ident>(current.string_value);
		expect(Token::Token_type::TOKEN_IDENT);
		expect(Token::Token_type::OPERATOR_LBRACE);
		auto members = parseClassMembers();
		expect(Token::Token_type::OPERATOR_RBRACE);
		classes->emplace_back(className, members);
	}

	expect(Token::Token_type::TOKEN_EOF);
	return std::make_unique<ast::Program>(classes);
}

// ClassMembers -> public ClassMember ClassMembers | .
// ClassMember -> TypeIdent FieldOrMethod | static MainMethod .
std::unique_ptr<std::vector<std::unique_ptr<ast::ClassMember>>> Parser::parseClassMembers()
{
	auto classMembers = std::make_unique<std::vector<std::unique_ptr<ast::ClassMember>>>();

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
			// TODO
			parseFieldOrMethod(std::move(typeIdent));
		}
	}

	return classMembers;
}

// MainMethod -> void IDENT ( String [ ] IDENT ) Block .
std::unique_ptr<ast::MainMethodDeclaration> Parser::parseMainMethod()
{
	expect(Token::Token_type::KEYWORD_VOID);
	auto mainMethodName = std::make_unique<ast::Ident>(current.string_value);
	auto typeIdent = std::make_unique<ast::TypeIdent>(
	                     mainMethodName,
	                     ast::TypeIdent::Primitive_type::VOID);

	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_LPAREN);

	auto parameterName = std::make_unique<ast::Ident>(current.string_value);
	auto parameters = std::make_unique<std::vector<std::unique_ptr<ast::TypeIdent>>>();
	parameters->push_back(std::make_unique<ast::TypeIdent>(parameterName, ast::TypeIdent::Primitive_type::VOID));

	expect(Token::Token_type::TOKEN_IDENT, "String");
	expect(Token::Token_type::OPERATOR_LBRACKET);
	expect(Token::Token_type::OPERATOR_RBRACKET);
	expect(Token::Token_type::TOKEN_IDENT);
	expect(Token::Token_type::OPERATOR_RPAREN);
	// TODO: Implemented Statement AST Node, update parseBlock() to return AST Node and then:
	// auto block = parseBlock();
	// instead of this:
	parseBlock();
	std::unique_ptr<ast::Statement> emptyStatement;
	// TODO: return block instead of emptyStatement:
	return std::make_unique<ast::MainMethodDeclaration>(typeIdent, parameters, emptyStatement);
}

// TypeIdent -> Type IDENT
// Type -> BasicType ArrayDecl .
std::unique_ptr<ast::TypeIdent> Parser::parseTypeIdent()
{
	auto basicType = parseBasicType();
	int dimension = parseArrayDecl();

	// retrieve variable name:
	auto variable_name = std::make_unique<ast::Ident>(current.string_value);
	expect(Token::Token_type::TOKEN_IDENT);

	auto typeIdent = std::make_unique<ast::TypeIdent>(variable_name, basicType.first, basicType.second, dimension);
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

// BasicType -> int | boolean | void | IDENT .
std::pair<ast::TypeIdent::Primitive_type, std::string> Parser::parseBasicType()
{
	std::string class_name = "";
	ast::TypeIdent::Primitive_type primitive_type = ast::TypeIdent::Primitive_type::NONE;

	switch (current.token_type)
	{
		case Token::Token_type::KEYWORD_INT:
			primitive_type = ast::TypeIdent::Primitive_type::INT;
			break;

		case Token::Token_type::KEYWORD_BOOLEAN:
			primitive_type = ast::TypeIdent::Primitive_type::BOOLEAN;
			break;

		case Token::Token_type::KEYWORD_VOID:
			primitive_type = ast::TypeIdent::Primitive_type::VOID;
			break;

		case Token::Token_type::TOKEN_IDENT:
			class_name = current.string_value;
			break;

		default:
			throw "expected Type";
	}

	nextToken();
	return std::make_pair(primitive_type, class_name);
}

// FieldOrMethod -> Field | Method .
// Field -> ; .
// Method -> ( OptionalParameters ) Block .
std::unique_ptr<ast::ClassMember> Parser::parseFieldOrMethod(std::unique_ptr<ast::TypeIdent> typeIdent)
{
	std::unique_ptr<ast::ClassMember> classMember;

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
		// TODO: auto block = parseBlock();
		parseBlock();
		// TODO: remove this:
		std::unique_ptr<ast::Statement> emptyStatement;

		classMember = std::make_unique<ast::MethodDeclaration>(typeIdent, parameters, emptyStatement);
	}

	return classMember;
}

// OptionalParameters -> Parameters | .
// Parameters -> Parameter FollowingParameters .
// FollowingParameters -> , Parameters
//     | .
// Parameter -> TypeIdent .
std::unique_ptr<std::vector<std::unique_ptr<ast::TypeIdent>>> Parser::parseOptionalParameters()
{
	auto parameters = std::make_unique<std::vector<std::unique_ptr<ast::TypeIdent>>>();
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
std::unique_ptr<ast::Statement> Parser::parseStatement()
{
	switch (current.token_type)
	{
		case Token::Token_type::OPERATOR_LBRACE:
			return parseBlock();
			break;

		case Token::Token_type::OPERATOR_SEMICOLON:
			nextToken();
			return std::unique_ptr<ast::Statement>;
			break;

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
			auto expr = parseExpression();
			expect(Token::Token_type::OPERATOR_SEMICOLON);
			return std::make_unique<ast::ExpressionStatement>(expr);
			break;

		default:
			throw "expected Statement";
	}
}

// Block -> { BlockStatements } .
// BlockStatements -> BlockStatement BlockStatements | .
std::unique_ptr<ast::Statement> Parser::parseBlock()
{
	expect(Token::Token_type::OPERATOR_LBRACE);

	auto statements = std::make_unique<std::vector<std::unique_ptr<ast::Statement>>>();

	while (current.token_type != Token::Token_type::OPERATOR_RBRACE)
		auto block_statement = parseBlockStatement();

	if (block_statement)
		statements->push_back(std::move(block_statement));

	nextToken();

	//If there are no block_statements we can skip the block.
	if (!statements.empty())
		return std::make_unique<ast::Block>(statements);
	else
		return std::unique_ptr<ast::Statement>;
}

// BlockStatement -> Statement | LocalVariableDeclarationStatement .
std::unique_ptr<ast::Statement> Parser::parseBlockStatement()
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
std::unique_ptr<ast::LVDStatement> Parser::parseLocalVariableDeclarationStatement()
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
std::unique_ptr<ast::IfStatement> Parser::parseIfStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	auto then = parseStatement();

	if (current.token_type == Token::Token_type::KEYWORD_ELSE)
	{
		nextToken();
		return std::make_unique<ast::IfStatement>(cond, then, parseStatement());
	}
	else
		return std::make_unique<ast::IfStatement>(cond, then);
}

// WhileStatement -> ( Expression ) Statement .
std::unique_ptr<ast::WhileStatement> Parser::parseWhileStatement()
{
	expect(Token::Token_type::OPERATOR_LPAREN);
	auto cond = parseExpression();
	expect(Token::Token_type::OPERATOR_RPAREN);
	return std::make_unique<ast::WhileStatement>(cond, parseStatement());
}

// ReturnStatement -> OptionalExpression ; .
// OptionalExpression -> Expression
//  	| .
std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement()
{
	if (current.token_type != Token::Token_type::OPERATOR_SEMICOLON)
	{
		expect(Token::Token_type::OPERATOR_SEMICOLON);
		return std::make_unique<ast::ReturnStatement>(parseExpression());
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
std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>> Parser::parseIdentOrIdentWithArguments()
{
	std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>> arguments;

	if (current.token_type == Token::Token_type::OPERATOR_LPAREN)
	{
		nextToken();
		arguments = parseArguments();
		expect(Token::Token_type::OPERATOR_RPAREN);
	}

	return arguments;
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
std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>> Parser::parseArguments()
{
	auto arguments = std::make_unique<std::vector<std::unique_ptr<ast::Ident>>>();
	bool isFirstArgument = true;

	while (current.token_type != Token::Token_type::OPERATOR_RPAREN)
	{
		isFirstArgument = false;
		// TODO:
		// parameters->push_back(std::move(parseExpression());
		// instead of
		parseExpression();

		if (current.token_type != Token::Token_type::OPERATOR_COMMA)
			return arguments;
		else
			nextToken();
	}

	if (!isFirstArgument)
		throw "trailing comma";

	return arguments;
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
