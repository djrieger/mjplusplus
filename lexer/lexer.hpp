#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "token.hpp"
#include "stateomat.hpp"

class Lexer {
	protected:

		std::tuple<unsigned int, unsigned int> position;
		std::istream& input;
		Stateomat stateomat;
		bool debug;

	public:
		Lexer(std::istream& input, Stateomat const &stateomat, bool debug = false);
		token get_next_token();
		bool good();
};

#endif
