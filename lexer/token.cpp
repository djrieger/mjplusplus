#include <iostream>
#include "token.hpp"

void print_token(token *t) {
    std::cout << t->position.first << "," << t->position.second << ":\t";

    switch (t->type) {
        case TOKEN_EOF:
            std::cout << "EOF" << std::endl;
            break;
        case TOKEN_INT_LIT:
            std::cout << "integer literal " << t->string_value << std::endl;
            break;
        case TOKEN_IDENT:
            std::cout << "identifier " << t->string_value << std::endl;
            break;
        case TOKEN_OPERATOR:
        case TOKEN_KEYWORD:
            std::cout << t->string_value << std::endl;
            break;
        case TOKEN_ERROR:
            break;
    }
}