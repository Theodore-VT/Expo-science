#ifndef MAZE_CREATOR_HPP
#define MAZE_CREATOR_HPP

#include "Algorithm.hpp"
#include <stack>

struct Neighbor
{
	Neighbor(int ind_, int Dir);
	int ind;
	int Direction;
	bool Visited;
};

class Maze_generator : public Algorithm
{
public:
	Maze_generator(int priority, int It_per_Step, int Width_nodes, int Height_nodes);
	virtual ~Maze_generator() {};
protected:
	bool Update_core(std::vector<Node> &Nodes);
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
	//std::vector<std::vector<Node_maze_generator>> m_nodes;

	int m_width, m_height;
};

#endif