
#ifndef TURN_RIHT_HPP
#define TURN_RIHT_HPP

#include "Algorithm.hpp"
#include <vector>

class Stay_left : public Algorithm
{
public:
	Stay_left(int priority, int It_per_Step, int Width_nodes, int Height_nodes, int path);
	~Stay_left() {};

protected:
	bool Update_core(std::vector<Node> &Nodes, Path &path_to_change);
	int Init(std::vector<Node> &Nodes);
	void Notify_node_core(int Node_ind);

private:

	bool CanGo(int From, int To, std::vector<Node> &Nodes);
	void StayLeft(std::vector<Node>& Nodes);

	int Current, Previous, Start, Goal;
	int m_width, m_height;
};

#endif // !A_STAR_HPP
