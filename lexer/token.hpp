#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum token_type {
    TOKEN_EOF,
    TOKEN_INT_LIT,
    TOKEN_IDENT,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_ERROR,
};

struct token {
    token_type type;
    std::string string_value;
};

void print_token(struct token *t);

#endif