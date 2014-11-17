#ifndef STATEOMAT_HPP
#define STATEOMAT_HPP

#include <unordered_map>
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
		/** Constructor. */
		Stateomat();
		/**
		 *  Returns whether the current state is accepting.
		 *  @param state the state to test
		 *  @returns true if state is accepting, else false
		 */
		bool state_is_accepting(unsigned int state);
		/**
		 * Writes the stateomat transition graph as a gml-file.
		 * @param file_name file to write the graph into
		 */
		void dump_graph(std::string const& file_name);

		//protected:
		/** transition map */
		std::vector<std::vector<unsigned int>> transitions;
		/** saves the token types corresponding to the different states */
		std::vector<Token::Token_type> state_type;
		/** maps string representation to token type for all keywords */
		std::unordered_map<std::string, Token::Token_type> keywords;
		/**
		 *  maps string representation to token type for operators.
		 *  indexed by end state to get the appropriate map
		 */
		std::vector<std::unordered_map<std::string, Token::Token_type>> operators;
		/** maps token type to its string representation (for printing purposes) */
		std::map<Token::Token_type, std::string> reverse;
};

#endif
