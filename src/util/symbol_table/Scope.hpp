#ifndef SCOPE_HPP
#define SCOPE_HPP

class Scope
{
private:
	Scope const &parent;
	int oldSize;
public:
	Scope(Scope const &parent, int oldSize);

	int getOldSize() const;
};

#endif