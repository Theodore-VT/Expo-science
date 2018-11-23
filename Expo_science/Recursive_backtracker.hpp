#ifndef RECURSIVE_BACKTRACKER_HPP
#define RECURSIVE_BACKTRACKER_HPP

#include "Algorithm.hpp"
#include <stack>

struct Neighbor
{
	Neighbor(int ind_, int Dir);
	int ind;
	int Direction;
	bool Visited;
};

class Recursive_backtracker : public Algorithm
{
public:
	Recursive_backtracker(int priority, int It_per_Step, int Width_nodes, int Height_nodes);
	virtual ~Recursive_backtracker() {};
protected:
	bool Update_core(std::vector<Node> &Nodes, Path &path_to_update);
	int Init(std::vector<Node> &Nodes);
	void Notify_node_core(int Node_ind) {};

private:

	void RemoveWall(int ind, int Wall);
	bool Has_unvisitedNodes();
	bool Check_Neighbors(int ind);
	int index(unsigned int ind);

	//_2d_array<Node> m_nodes;
	std::vector<Node> *m_nodes;
	std::vector<bool> visited_nodes;
	std::vector<Neighbor> Neighbors;
	std::stack<int> Stack;

	int Current;
	//std::vector<std::vector<Node_Recursive_backtracker>> m_nodes;

	int m_width, m_height;
};

#endif