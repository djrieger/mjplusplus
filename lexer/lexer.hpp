#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
//#include <string>
#include "token.hpp"
#include "stateomat.hpp"

#define N_STATES 23
#define STATE_START 0
#define STATE_STOP 19

class Lexer {
	protected:
		// line, column
		std::pair<unsigned int, unsigned int> position;
		bool debug;
		std::istream& input;
		Stateomat stateomat;
		bool debug;

		int is_accepting(int state);
	public:
		Lexer(std::istream& input, Stateomat const &stateomat, bool debug = false);
		token get_next_token();
		bool good();
};

#endif
