#include <vector>
#include <set>
#include <map>
#include <string>
#include "token.hpp"

#define N_STATES 23
#define STATE_START 0
#define STATE_STOP 19

class Stateomat {

    public:
        Stateomat();
	
	

    protected:

	std::vector<std::vector<int>> transitions;
	std::map<unsigned int, token_type> state_type;
	std::set<std::string> keywords;
};

