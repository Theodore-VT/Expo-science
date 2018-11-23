
#ifndef RECURSIVE_DIVISION_HPP
#define RECURSIVE_DIVISION_HPP

#include "Algorithm.hpp"



struct Chamber
{
	Chamber(int x1_, int y1_, int x2_, int y2_, int original_width);

	bool Resolve(std::vector<Node> &Nodes);
	bool Divide(std::vector<Node> &Nodes);

	int x1, y1, x2, y2, m_area;
	bool Resolved;
	int Original_width;
	std::vector<Chamber*> m_childs;
};


class Recursive_division : public Algorithm
{
public:
	Recursive_division(int priority, int It_per_Step, int Width_nodes, int Height_nodes);
	virtual ~Recursive_division() {};
protected:
	bool Update_core(std::vector<Node> &Nodes, Path &path_to_update);
	int Init(std::vector<Node> &Nodes);
	void Notify_node_core(int Node_ind) {};

private:

	/*
	 * Function used to divide a space in 4
	 * 
	 * @Anarea : the index to the Already subdivised area (index of m_areas)
	 */
	void SubDivide(int Anarea);

	/*
	 * Function used to set the walls of the nodes
	 *
	 * @Anarea : the index to the Already subdivised area (index of m_areas)
	 * @Nodes : reference to array of nodes
	 */
	void SetWalls(int anArea, std::vector<Node> &Nodes);

	int _2d_2_1d_(int x, int y);

	Chamber m_chamber;

	int Width, Height;
};

#endif