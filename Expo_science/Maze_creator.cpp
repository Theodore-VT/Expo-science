
#include "Maze_creator.h"



Maze_generator::Maze_generator(int priority, int It_per_Step, int Width_nodes, int Height_nodes):
Algorithm(priority, It_per_Step),
//VisitedNodes_ind(Width_nodes * Height_nodes),
m_width(Width_nodes),
m_height(Height_nodes),
visited_nodes(Width_nodes * Height_nodes)
{
	m_ID = 2;

	std::fill(visited_nodes.begin(), visited_nodes.end(), false);
}

bool Maze_generator::Update_core(std::vector<Node>& Nodes)
{
	if (this->Has_unvisitedNodes())
	{
		this->Check_Neighbors(Current);
		
		if (Neighbors.size())
		{
			int ind_neigh = rand() % (Neighbors.size());
			
			Neighbor next = Neighbors[ind_neigh];
			
			visited_nodes[next.ind] = true;			
			Stack.push(Current);
			
			this->RemoveWall(Current, next.Direction);
			
			m_nodes->at(Current).SetIsWall(0.0f);
			Current = next.ind;
			m_nodes->at(Current).SetIsWall(1.0f);	
		}
		else if (!Stack.empty())
		{
			m_nodes->at(Current).SetIsWall(0.0f);
			Current = Stack.top();
			m_nodes->at(Current).SetIsWall(1.0f);
			Stack.pop();
		}
		
		return false;
	}

	m_nodes->at(Current).SetIsWall(0.0f);
	print("Finished maze!\n");
	return true;
}

void Maze_generator::Init(std::vector<Node>& Nodes)
{
	srand(time(NULL));

	m_nodes = &Nodes;//= _2d_array<Node>(m_width, m_height, Nodes);

	Current = 16;
	visited_nodes[Current] = true;	

	for (int i = 0; i < m_nodes->size(); ++i)
	{	
		if (m_nodes->at(i).IsWall(true) != PERMANENT_WALL)
		{
			m_nodes->at(i).SetIsWall(0.0f);
			m_nodes->at(i).SetWall(LEFT_WALL_, true);
			m_nodes->at(i).SetWall(BOTTOM_WALL_, true);
			m_nodes->at(i).SetWall(RIGHT_WALL_, true);
			m_nodes->at(i).SetWall(TOP_WALL_, true);
		}
	}
}

void Maze_generator::RemoveWall(int ind, int Wall)
{
	if (Wall == TOP_WALL_)
	{
		if(this->index(ind) != -1)
			m_nodes->at(ind).SetWall(TOP_WALL_, false);
		if(this->index(ind - m_width) != -1)
			m_nodes->at(ind - m_width).SetWall(BOTTOM_WALL_, false);
	}
	else if (Wall == BOTTOM_WALL_)
	{
		if (this->index(ind) != -1)
			m_nodes->at(ind).SetWall(BOTTOM_WALL_, false);
		if (this->index(ind + m_width) != -1)
			m_nodes->at(ind + m_width).SetWall(TOP_WALL_, false);
	}
	else if (Wall == RIGHT_WALL_)
	{
		if (this->index(ind) != -1)
			m_nodes->at(ind).SetWall(RIGHT_WALL_, false);
		if((this->index(ind + 1) != -1))
			m_nodes->at(ind + 1).SetWall(LEFT_WALL_, false);
	}
	else if (Wall == LEFT_WALL_)
	{
		if (this->index(ind) != -1)
			m_nodes->at(ind).SetWall(LEFT_WALL_, false);
		if( (this->index(ind - 1) != -1))
			m_nodes->at(ind - 1).SetWall(RIGHT_WALL_, false);
	}
}

bool Maze_generator::Has_unvisitedNodes()
{
	for (int i = 0; i < m_nodes->size(); ++i)
	{
		if (visited_nodes[i] == false && this->index(i) != -1)
			return true;
	}

	return false;
}

bool Maze_generator::Check_Neighbors(int ind)
{
	Neighbors.clear();

	// Top right bottom left
	if ((index(ind - m_width) != -1) && !visited_nodes[index(ind - m_width)])
		Neighbors.push_back(Neighbor(index(ind - m_width), TOP_WALL_));

	if ((index(ind + 1) != -1) && !visited_nodes[index(ind + 1)])
		Neighbors.push_back(Neighbor(index(ind + 1), RIGHT_WALL_));

	if ((index(ind + m_width)) != -1 && !visited_nodes[index(ind + m_width)])
		Neighbors.push_back(Neighbor(index(ind + m_width), BOTTOM_WALL_));

	if ((index(ind - 1) != -1) && !visited_nodes[index(ind - 1)])
		Neighbors.push_back(Neighbor(index(ind - 1), LEFT_WALL_));

	return false;
}

int Maze_generator::index(unsigned int ind)
{
	if (ind < m_nodes->size() && m_nodes->at(ind).IsWall(true) != PERMANENT_WALL)
		return ind;

	return -1;
}

Neighbor::Neighbor(int ind_, int Dir)
{
	Direction = Dir;
	ind = ind_;
}
