#ifndef GRID_HPP
#define GRID_HPP

#include <Windows.h>
#include "Node.hpp"
#include "Algorithm.hpp"
#include <string>
#include <vector>
#include <Windows.h>

#define CREATE_MAZE		0
#define A_STAR			1
#define RANDOM			2

class Grid
{
friend class Grid_manager;

public:

	// Width in nodes
	Grid(int Width_nodes, int Height_nodes, int Width_px, int Height_px, std::string path_to_maze = "");

	// The () operator will be using OpenGL's coordinates system to make everything simpler
	Node &operator ()(float x, float y);
	Node &operator ()(int ind);

	void Notify_wall_shift(int ind, bool overide_previous);
	float GetColor(int ind);
	bool IsNode_shiftable(int ind);

	void Save(const char * Filename);
	void Load(const char * Filename);
	void Reset(int Width_nodes = -1, int Height_nodes = -1, int Width_px = -1, int Height_px = -1);


	void Add_algorithm_to_queue(Algorithm * new_Algorithm);
	void Update();
	void Show(HWND window_handle, bool Force_Redraw = false);

	// This function expects input from glfw's mousse tracker
	int GetInd_from_2dPos(int x, int y);

	int Num_nodes();

	// Dimensions in pixels
	int Width_px();
	int Height_px();

	// Dimensions in nodes
	int Width_nd();
	int Height_nd();

private:

	std::vector<int> Get_ChangedNodes_ind();

	int Last_node_shifted;
	int m_width_nodes, m_width_px, m_height_nodes, m_height_px;
	int m_width_px_per_node, m_height_px_per_node;
	Node m_NULL_node;
	int Highest_priority;
	
	std::vector<int> All_indexes;
	std::vector<Algorithm*> Algorithms_to_update;
	std::vector<Node> Nodes, Nodes_SnapShot;
};

#endif // !GRID_HPP