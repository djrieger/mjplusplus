#include "parser.hpp"

Parser::Parser(Lexer& lexer, bool print_messages) : lexer(lexer), print_messages(print_messages) {
}

/* Max' proposal: use return type to indicate if token stream is not a part of the language induced by the grammar
*/
bool Parser::start() {
    nextToken();
    return parseProgram();
}

Token Parser::nextToken() {
    current = lexer.get_next_token();
    return current;
}

void Parser::printError() {
    if (print_messages) // only print error messages if they are wanted
        std::cout << error_msg_buf;
    error_msg_buf = "";
}

bool Parser::expect(Token::Token_type tokenType) {
    bool ret = current.token_type == tokenType;
    if (ret) 
        nextToken();
    return ret;
}

bool Parser::expect(Token::Token_type tokenType, std::string string_val) {
    bool ret = current.token_type == tokenType && current.string_val == string_val;
    if (ret)
        nextToken();
    return ret;
}

bool Parser::expect(std::set<Token::Token_type> token_types) {
    bool ret = token_types.find(current.token_type) != token_types.end();
    if (ret)
        nextToken();
    return ret;
}

bool Parser::parseProgram() {
    switch (current.token_type)
    {
        case Token::Token_type::TOKEN_EOF: 
            return true; break; // empty file, that's ok.
        case Token::Token_type::KEYWORD_CLASS: 
            if (parseClassDeclaration()) {
                // call parseProgram again: either its EOF or another class declaration follows
                return parseProgram();
            } // else case: fall through to default/error case
        default: 
            printError();
            return false;	
    }
}

bool Parser::parseClassDeclaration() {
    return true;
}

/*
 Expression -> AssignmentExpression .
*/
bool Parser::parseExpression() {
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
bool Parser::parseAssignmentExpression() {
	return precedenceClimb(1);
}

/*
parses an expression via precedence climb
*/
bool Parser::precedenceClimb(int minPrec) {
	bool result = parseUnaryExpression();
	int prec;
	bool left_assoc;
	Token::Token_type op = current.token_type;
	if (!result || operator_precs.find(op) == operator_precs.end()) {
		return result;
	}
	std::tie(prec, left_assoc) = operator_precs.find(current.token_type);
	while (prec >= minPrec) {
		if (left_assoc) {
			prec = prec + 1;
		}
		next_token();
		rhs = precedenceClimb(prec);
		result = result && rhs;
		op = current.token_type;
		if (!result || operator_precs.find(op) == operator_precs.end()) {
			return result;
		}
		std::tie(prec, left_assoc) = operator_precs.find(current.token_type);
	}
	return result;
}
