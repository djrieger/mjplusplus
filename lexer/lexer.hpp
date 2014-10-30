#include "token.hpp"

class Lexer {
public:
	Lexer(bool debug=false);
	token_t get_next_token();
}
