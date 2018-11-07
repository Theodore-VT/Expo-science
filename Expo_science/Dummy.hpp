#ifndef DUMMY_HPP
#define DUMMY_HPP

#include "Algorithm.hpp"

class Dummy : public Algorithm
{
public:
	Dummy(int priority, int It_per_Step);
	virtual ~Dummy() {};
protected:
	bool Update_core(std::vector<Node> &Nodes);
	void Init(std::vector<Node> &Nodes) {};
};

#endif