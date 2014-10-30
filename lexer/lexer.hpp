#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
//#include <string>
#include "token.hpp"

#define N_STATES 23
#define STATE_START 0
#define STATE_STOP 19

class Lexer {
	protected:

		std::tuple<unsigned int, unsigned int> position;
		bool debug;
		std::istream& input;
		std::set<std::string> keywords;
		std::map<unsigned int, token_type> state_type;
		std::vector<std::vector<int>> transitions;
		std::set<unsigned int> non_accepting_states;

		int is_accepting(int state);
	public:
		Lexer(std::istream& input, 
			std::set<std::string> keywords,
			std::map<unsigned int, token_type> state_type,
			std::vector<std::vector<int>> transitions,
			std::set<unsigned int> non_accepting_states,
			bool debug=false);
		token get_next_token();
		bool good();
};

#endif