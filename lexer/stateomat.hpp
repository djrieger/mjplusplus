#ifndef STATEOMAT_HPP
#define STATEOMAT_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include "token.hpp"

#define STATE_START 0
#define STATE_STOP 19

class Stateomat
{
public:
	Stateomat();
    bool state_is_accepting(unsigned state);
    void dump_graph(std::string file_name);

//protected:
	std::vector<std::vector<unsigned>> transitions;
	std::map<unsigned, token_type> state_type;
	std::set<std::string> keywords;
	std::set<unsigned> non_accepting_states;
};

#endif
