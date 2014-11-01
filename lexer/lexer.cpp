#include "lexer.hpp"
#include "token.hpp"

Lexer::Lexer(std::istream& input, Stateomat const &stateomat, bool debug)
	: input(input), stateomat(stateomat), debug(debug)
{;}

token Lexer::get_next_token() {
	struct token t;
	t.string_value = "";

    int state = STATE_START;

    while(1) {
        int c = input.get();
        int new_state = stateomat.transitions[state][c == EOF ? 128 : c];

        if (!stateomat.state_is_accepting(new_state))
            t.string_value = "";
        else if (new_state == STATE_STOP) {
            if (stateomat.state_is_accepting(state)) {
                input.unget();
                if (stateomat.keywords.find(t.string_value) != stateomat.keywords.end())
                    t.type = TOKEN_KEYWORD;
                if (debug)
              		print_token(&t);
                return t;
            }
            else 
            {
            	t.type = TOKEN_ERROR;
                return t;
            }
        }
        else {
            t.string_value.push_back(c);
            t.type = stateomat.state_type[new_state];
        }

        state = new_state;
    }

    t.type = TOKEN_ERROR;
    return t;
}

bool Lexer::good() {
	return input.good();
}
