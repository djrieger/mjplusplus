#ifndef TOKEN_HPP
#define TOKEN_HPP

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

void print_token(struct token *t) {
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

#endif