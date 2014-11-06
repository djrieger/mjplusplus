#ifndef STATEOMAT_HPP
#define STATEOMAT_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "token.hpp"

#define STATE_START 0
#define STATE_STOP 19

class Stateomat
{
	public:
		Stateomat();
		bool state_is_accepting(unsigned int state);
		void dump_graph(std::string const& file_name);

		//protected:
		std::vector<std::vector<unsigned int>> transitions;
		std::map<unsigned int, Token::Token_type> state_type;
		std::set<unsigned int> non_accepting_states;

		std::map<std::string, Token::Token_type> keywords;
		std::map<std::string, Token::Token_type> operators;
};

#endif
