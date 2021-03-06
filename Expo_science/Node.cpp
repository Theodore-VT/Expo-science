#include "Node.hpp"
#include <iostream>

Node::Node(float x, float y, float Width, float Height) :
	m_x(x),
	m_y(y)
{
	for (int i = 0; i < 4; ++i)
		Walls[i] = false;

	// Set all vertices position	the Z compoenent is "deprecated", will be removed
	// Corner left-up
	Vertices.push_back(x);					// X
	Vertices.push_back(y);					// Y
	Vertices.push_back(0.0f);				// Z

	// Corner right-up
	Vertices.push_back(x + (Width));		// X
	Vertices.push_back(y);					// Y
	Vertices.push_back(0.0f);				// Z

	// Corner right-down
	Vertices.push_back(x + Width);			// X
	Vertices.push_back(y + Height);			// Y
	Vertices.push_back(0.0f);				// Z

	// Corner left-down
	Vertices.push_back(x);					// X
	Vertices.push_back(y + Height);			// Y
	Vertices.push_back(0.0f);				// Z

	this->Change_Color(255, 0, 255);

	m_x = m_x + Width / 2;
	m_y = m_y + Height / 2;
}

Node::Node():
	m_x(0),
	m_y(0)
{
	for (int i = 0; i < 4; ++i)
		Walls[i] = false;
}

Node::~Node()
{
}

void Node::Change_Color(int R, int G, int B)
{
	m_R = R;
	m_G = G;
	m_B = B;
}

float Node::GetColor(int Component, bool GetRawColor)
{
	float Modificator = 1.0f;
	if (!GetRawColor)
		Modificator = 1.0f - Is_wall;

	if (Component == R_)
		return m_R * Modificator;
	else if (Component == G_)
		return m_G * Modificator;
	else if (Component == B_)
		return m_B * Modificator;
	else
		return 0;
}

float Node::GetPos(int Component)
{
	if (Component == X_)
		return m_x;
	else if (Component == Y_)
		return m_y;
	else
		return 0;
}

float Node::GetVertex(int ind, int Component)
{
	if (ind > 3)
		return 0;

	if (Component == X_)
		return Vertices[ind * 3];
	else if (Component == Y_)
		return Vertices[ind * 3 + 1];
	else if (Component == Z_)
		return Vertices[ind * 3 + 2];
	else
		return 0;
}

float Node::IsWall(bool Raw_val)
{
	if (Raw_val || Is_wall <= WALL)
		return Is_wall;
	else if (Is_wall > WALL)
		return WALL;

	return 0;
}

void Node::SetIsWall(float Value)
{
	Is_wall = Value;

	if (Is_wall == PERMANENT_WALL)
	{
		for(int i = 0; i < 4; ++i)
			Walls[i] = false;
	}

}

float Node::GetWall(unsigned int Wall_ind, bool Raw_val)
{
	if (Wall_ind > 3)
		return 0;

	if (Raw_val)
		return Walls[Wall_ind];
	else
		return (bool)(Walls[Wall_ind] > NOT_A_WALL_);
}

void Node::SetWall(int Wall_ind, float Value)
{
	if (Wall_ind > 3)
		return;

	Walls[Wall_ind] = Value;
}

void Node::SetFlag(std::string Key, float Value)
{
	std::get<0>(m_additional_flag) = Key;
	std::get<1>(m_additional_flag) = Value;
}

std::string Node::GetFlag()
{
	return std::get<0>(m_additional_flag);
}

float Node::GetFlagVal()
{
	return std::get<1>(m_additional_flag);
}

bool Node::Has_flag()
{
	if(std::get<0>(m_additional_flag) == "")
		return false;

	return true;
}
/*
Path::Path(Node * node_)
{
	node = node_;
	Came_From = nullptr;

	R = rand() % 255;
	G = rand() % 255;
	B = rand() % 255;
}*/

Path::Path(Node *startNode)
{
	nodes.push_back(startNode);
	R = rand() % 255;
	G = rand() % 255;
	B = rand() % 255;
}

Path::Path()
{
	R = rand() % 255;
	G = rand() % 255;
	B = rand() % 255;
}

int _2D_2_1D_ind(int x, int y, int width)
{
	return y * width + x;
}

void _1D_2_2D_ind(int ind, int height, int width, int & x, int & y)
{
	x = ind % width;

	y = (ind / height);
}
