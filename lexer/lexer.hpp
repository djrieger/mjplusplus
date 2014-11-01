#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>

#include "token.hpp"
#include "stateomat.hpp"

class Lexer {
	protected:
		// line, column
		std::pair<unsigned int, unsigned int> position;
		std::istream &input;
		Stateomat stateomat;
		bool debug;

	public:
		Lexer(std::istream &input, Stateomat const &stateomat, bool debug = false);
		Token get_next_token();
		bool good();
};

#endif
