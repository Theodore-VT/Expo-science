#include "Recursive_division.hpp"

#include <ctime>

Recursive_division::Recursive_division(int priority, int It_per_Step, int Width_nodes, int Height_nodes):
	Algorithm(priority, It_per_Step),
	Width(Width_nodes),
	Height(Height_nodes),
	m_chamber(1, 1, Width_nodes - 2, Height_nodes - 2, Width_nodes)
{
	m_ID = 8;

	srand(time(NULL));
}

bool Recursive_division::Update_core(std::vector<Node>& Nodes, Path & path_to_update)
{
	int it = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (!m_chamber.Resolved)
		{
			m_chamber.Resolve(Nodes);
		}
	}

	if (!m_chamber.Resolved)
	{
		m_chamber.Resolve(Nodes);
		return false;
	}

	return true;
}

int Recursive_division::Init(std::vector<Node>& Nodes)
{
	for (int i = 0; i < Nodes.size(); ++i)
	{
		if (Nodes.at(i).IsWall(true) != PERMANENT_WALL)
		{
		//	Nodes.at(i).SetIsWall(0.0f);
			Nodes.at(i).SetWall(LEFT_WALL_	, false);
			Nodes.at(i).SetWall(BOTTOM_WALL_, false);
			Nodes.at(i).SetWall(RIGHT_WALL_	, false);
			Nodes.at(i).SetWall(TOP_WALL_	, false);
		}
	}

	if (Width > 0 && Height > 0)
		return INIT_CODE_SUCCES_;
	else
		return INIT_CODE_FAILED_;
}

Chamber::Chamber(int x1_, int y1_, int x2_, int y2_, int original_width)
{
	x1 = x1_;
	y1 = y1_;
	x2 = x2_;
	y2 = y2_;

	if (x1 > x2)
		std::swap(x1, x2);
	if (y1 > y2)
		std::swap(y1, y2);

	m_area = (x2 - (x1 - 1)) * (y2 - (y1 - 1));
	Resolved = false;

	Original_width = original_width;
}

bool Chamber::Resolve(std::vector<Node> &Nodes)
{
	if (m_area <= 1)
	{
		Resolved = true;
		return true;
	}

	if (m_childs.empty())
	{
		this->Divide(Nodes);
		Resolved = false;
		return false;
	}

	for (int i = 0; i < m_childs.size(); ++i)
	{
		if (!m_childs[i]->Resolved)
		{
			m_childs[i]->Resolve(Nodes);
			Resolved = false;
			return false;
		}
	}

	Resolved = true;
	return true;
}

bool Chamber::Divide(std::vector<Node> &Nodes)
{
	if ((x2 - x1) > (y2 - y1))
	{
		int Separation = (rand() % (x2 - x1)) + x1;
		m_childs.push_back(new Chamber(x1, y1, Separation, y2, Original_width));
		m_childs.push_back(new Chamber(Separation+1, y1, x2, y2, Original_width));

		//print("Separation (x) : " + std::to_string(Separation) + "\n");

		int Hole = (rand() % (y2 - (y1 - 1))) + y1;
		for (int i = y1; i <= y2; ++i)
		{
			int Left  = _2D_2_1D_ind(Separation, i, Original_width);
			int Right = _2D_2_1D_ind(Separation+1, i, Original_width);

			if (Left < 0 || Left >= Nodes.size())
				Left = 0;
			if (Right < 0 || Right >= Nodes.size())
				Right = 0;

			Nodes[Left].SetWall(RIGHT_WALL_, true);
			Nodes[Right].SetWall(LEFT_WALL_, true);

			if (i == Hole)
			{
				Nodes[Left].SetWall(RIGHT_WALL_, false);
				Nodes[Right].SetWall(LEFT_WALL_, false);
			}
		}
		
		return 0;
	}
	else
	{
		int Separation = (rand() % (y2 - y1)) + y1;
		m_childs.push_back(new Chamber(x1, y1, x2, Separation, Original_width));
		m_childs.push_back(new Chamber(x1, Separation+1, x2, y2, Original_width));

		int Hole = (rand() % (x2 - (x1 - 1))) + x1;

		for (int i = x1; i <= x2; ++i)
		{
			int Top    = _2D_2_1D_ind(i, Separation, Original_width);
			int Bottom = _2D_2_1D_ind(i, Separation+1, Original_width);

			if (Top < 0 || Top >= Nodes.size())
				Top = 0;
			if (Bottom < 0 || Bottom >= Nodes.size())
				Bottom = 0;

			Nodes[Top].SetWall(BOTTOM_WALL_, true);
			Nodes[Bottom].SetWall(TOP_WALL_, true);

			if (i == Hole)
			{
				Nodes[Top].SetWall(BOTTOM_WALL_, false);
				Nodes[Bottom].SetWall(TOP_WALL_, false);
			}
		}
		return 1;
	}
}


