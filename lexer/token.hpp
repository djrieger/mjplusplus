struct token {
    enum token_type type;
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
    }
}

