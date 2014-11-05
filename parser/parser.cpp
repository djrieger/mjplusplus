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
    nextToken();
    return ret;
}

bool Parser::expect(Token::Token_type tokenType, std::string string_val) {
    bool ret = current.token_type == tokenType && current.string_val == string_val;
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