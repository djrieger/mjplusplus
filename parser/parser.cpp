#include "parser.hpp"

Parser::Parser(Lexer& lexer, bool print_messages) : lexer(lexer), print_messages(print_messages) {
}

/* Max' proposal: use return type to indicate if token stream is not a part of the language induced by the grammar
*/
int Parser::start() {
    return parseProgram();
}

void Parser::printError() {
    if (print_messages) // only print error messages if they are wanted
        std::cout << error_msg_buf;
    error_msg_buf = "";
}

int Parser::parseProgram() {
    current = lexer.get_next_token();
    switch (current.token_type)
    {
        case Token::Token_type::TOKEN_EOF: 
            return 0; break; // empty file, that's ok.
        case Token::Token_type::TOKEN_KEYWORD: 
            if ( current.keyword_type == Token::Keyword_type::KEYWORD_CLASS ) { 
                if (parseClassDeclaration() == 0) {
                    // call parseProgram again: either its EOF or another class declaration follows
                    return parseProgram();
                } // else case: fall through to default/error case
            } // else case: fall through to default/error case
        default: 
            printError();
            return 1;	
    }
}

int Parser::parseClassDeclaration() {
    return 0;
}




