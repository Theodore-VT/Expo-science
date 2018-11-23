
#ifndef A_STAR_HPP
#define A_STAR_HPP

#include "Algorithm.hpp"
#include <vector>

#define POSITIVE_INFINITY 99999999

struct A_Star_node
{
	A_Star_node(Node *node_);

	Node* node; // Pointer to the actual node
	A_Star_node *Came_from; // Pointer to the previous node on the path

	float Gscore; // Cost to go to this node from the start using the path
	float Fscore; // Cost to go from this node to the end using heuristic estimation

	bool IsPermanentWall();
	bool IsWall();

	bool operator ==(A_Star_node &Node_to_Compare);
};

class A_Star : public Algorithm
{
public:
	A_Star(int priority, int It_per_Step, int Width_nodes, int Height_nodes, int path);
	~A_Star() {};

protected:
	bool Update_core(std::vector<Node> &Nodes, Path &path_to_change);
	int Init(std::vector<Node> &Nodes);
	void Notify_node_core(int Node_ind);

private:



	float Xpos(int ind);
	float Ypos(int ind);

	void ReconstructPath(Path &output_path);

	bool Index(unsigned int ind, bool Security = 0);
	
	float EstimateDistance(int nd1, int nd2);
	int Get_Lowest_Fscore(std::vector<int>& nodes_ind);

	void GetNeighbors(int node_ind, std::vector<int> &Neighbors);

	std::vector<A_Star_node> m_nodes;
	std::vector<int> OpenSet, ClosedSet;

	int Start, Goal, Current;
	int m_path;
	int m_width, m_height;
};

#endif // !A_STAR_HPP
