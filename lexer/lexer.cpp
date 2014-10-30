#include "lexer.hpp"
#include "token.hpp"

Lexer::Lexer(std::istream& input, 
			std::set<std::string> keywords,
			std::map<unsigned int, token_type> state_type,
			std::vector<std::vector<int>> transitions,
			std::set<unsigned int> non_accepting_states,
			bool debug)
	 : input(input),
	  keywords(keywords), 
	  state_type(state_type), 
	  transitions(transitions),
	  non_accepting_states(non_accepting_states),
	  debug(debug) {
};

token Lexer::get_next_token() {
	struct token t;
	t.string_value = "";

    int state = STATE_START;

    while(1) {
        int c = input.get();
        int new_state = transitions[state][c == EOF ? 128 : c];

        if (!is_accepting(new_state))
            t.string_value = "";
        else if (new_state == STATE_STOP) {
            if (is_accepting(state)) {
                input.unget();
                if (keywords.find(t.string_value) != keywords.end())
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
            t.type = state_type[new_state];
        }

        state = new_state;
    }

    t.type = TOKEN_ERROR;
    return t;
}

int Lexer::is_accepting(int state) {
	return non_accepting_states.find(state) == non_accepting_states.end();
}

bool Lexer::good() {
	return input.good();
}