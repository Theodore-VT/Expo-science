#ifndef DUMMY_HPP
#define DUMMY_HPP

#include "Algorithm.hpp"

class Dummy : public Algorithm
{
public:
	Dummy(int priority, int It_per_Step);
	virtual ~Dummy() { };
protected:
	bool Update_core(std::vector<Node> &Nodes, Path &path_to_update);
	int  Init(std::vector<Node> &Nodes) { return INIT_CODE_SUCCES_; };
	void Notify_node_core(int Node_ind) {};
};

#endif