#ifndef NODE_HPP
#define NODE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <tuple>

#include <windows.h>

#define REVERSE_VALUE -1

#define R_ 0
#define G_ 1
#define B_ 2

#define X_ 0
#define Y_ 1
#define Z_ 1

#define WALL 1.0f
#define NOT_WALL 0.0f
#define PERMANENT_WALL 2.0f

#define LEFT_WALL_		0
#define BOTTOM_WALL_	1
#define RIGHT_WALL_		2
#define TOP_WALL_		3

#define NOT_A_WALL_ 0.2f

class Node
{
public:
	Node(float x, float y, float Width, float Height);
	Node();
	~Node();

	void Change_Color(int R, int G, int B);
	float GetColor(int Component, bool GetRawColor = false);

	float GetPos(int Component);
	float GetVertex(int ind, int Component);
	float IsWall(bool Raw_val = false);
	void SetIsWall(float Value);
	float GetWall(unsigned int Wall_ind, bool Raw_val = false);
	void SetWall(int Wall_ind, float Value);

	void SetFlag(std::string Key, float Value);
	std::string GetFlag();
	float GetFlagVal();
	bool Has_flag();

private:

	std::vector<float> Vertices;

	float Is_wall, Is_wall_updated;
	float m_x, m_y;
	float m_R, m_G, m_B;
	static size_t Index;

	std::tuple<std::string, float> m_additional_flag;

	// Node configuration is |_
	float Walls[4];
};

struct Path
{
	Path(Node *startNode);
	Path();
	
	std::vector<Node*> nodes;
	int R, G, B;
};

int _2D_2_1D_ind(int x, int y, int width);

#endif // ! NODE_H

