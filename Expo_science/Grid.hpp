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

#define LAST_PATH	   -1
#define LAST_ELEM	   -1

class Grid
{
friend class Grid_manager;

public:

	// Width in nodes
	Grid(int Width_nodes, int Height_nodes, int Width_px, int Height_px, PAINTSTRUCT *ps_, std::string path_to_maze = "");

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
	void Add_algortihm_notifier(int ID, int Number_of_times);

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

	/*std::shared_ptr<Path>*/int GetPath(int ind);
	void AddPath(unsigned int Start_node);

	int GetAlgorithmID(int ind);

private:

	void Notify_algorithm_on_click(int Node_ind);
	std::vector<int> Get_ChangedNodes_ind();
	void Node_Color(int node_ind, int &R, int &G, int &B);

	POINT rotate_point(float cx, float cy, float angle, POINT p);
	void Draw_Line(HDC hdc, int x1, int y1, int x2, int y2, int thinkness);
	void Draw_Path(int ind, HWND window_handle);

	int Last_node_shifted;
	int m_width_nodes, m_width_px, m_height_nodes, m_height_px;
	int m_real_width_px, m_real_height_px;
	int m_width_px_per_node, m_height_px_per_node;
	Node m_NULL_node;
	int Highest_priority;
	
	float m_shift_Right_Const;
	float m_shift_Bottom_Const;

	int m_algorithm_to_notify, m_notify_steps;
	std::vector<int> All_indexes;
	std::vector<Algorithm*> Algorithms_to_update;
	std::vector<Node> Nodes, Nodes_SnapShot;
	std::vector<Path> Resolve_paths;
	std::vector<Path> Resolve_paths_snapshots;
	bool Just_finished_algo;
	bool Finished = true;
	int Path_stroke;
	//PAINTSTRUCT *ps;
	Path Null_Path;
};

#endif // !GRID_HPP