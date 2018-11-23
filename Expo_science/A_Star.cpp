

#include "A_Star.hpp"
#include "guicon.hpp"
#include <algorithm>

A_Star::A_Star(int priority, int It_per_Step, int Width_nodes, int Height_nodes, int path):
	Algorithm(priority, It_per_Step),
	m_width(Width_nodes),
	m_height(Height_nodes)//,
	//m_path_IRL(path)
{
	m_path_IRL = path;
	m_ID = rand() % 25555892;

	Start = 0;
	Goal  = 0;
	//path->nodes.clear();
}

bool A_Star::Update_core(std::vector<Node>& Nodes, Path &path_to_change)
{
	if (!OpenSet.empty())
	{
		if(Current != Start && Current != Goal)
			m_nodes[Current].node->SetFlag("", 0.0f);
		
		Current = this->Get_Lowest_Fscore(OpenSet);
		
		if(Current != Start && Current != Goal)
			m_nodes[Current].node->SetFlag("CURRENT", 1.0f);

		if (Current == Goal)
			return true;
		
		ClosedSet.push_back(Current);
		OpenSet.erase(std::remove(OpenSet.begin(), OpenSet.end(), Current), OpenSet.end());

		std::vector<int> Neighbors;
		this->GetNeighbors(Current, Neighbors);
		
		for (int i = 0; i < Neighbors.size(); ++i)
		{
			// if the neighbor is alreadey in the closed set
			if (std::find(ClosedSet.begin(), ClosedSet.end(), Neighbors[i]) != ClosedSet.end())
				continue;

			// 1 because diagonals are disabled, therfore, the distance between a node and it's neighbors must be 1
			float Try_GScore = m_nodes[Current].Gscore + 1;
			
			// if the neighbor is not in the open set (new node)
			if (std::find(OpenSet.begin(), OpenSet.end(), Neighbors[i]) == OpenSet.end())
			{
				OpenSet.push_back(Neighbors[i]);
				//m_nodes[Neighbors[i]].node->SetIsWall(0.5f);
			}
			// if it is not a better path
			else if (Try_GScore >= m_nodes[Neighbors[i]].Gscore)
				continue;
			
			m_nodes[Neighbors[i]].Came_from = &m_nodes[Current];
			m_nodes[Neighbors[i]].Gscore = Try_GScore;
			m_nodes[Neighbors[i]].Fscore = Try_GScore + this->EstimateDistance(Neighbors[i], Goal); 
		}

		ReconstructPath(path_to_change);
		return false;
	}

	print("Finished A*\n");
	return true;
}

int A_Star::Init(std::vector<Node>& Nodes)
{

	m_nodes.clear();
	OpenSet.clear();
	ClosedSet.clear();
	
	for (int i = 0; i < Nodes.size(); ++i)
	{
		m_nodes.push_back(A_Star_node(&Nodes[i]));
	}

	if (Start == 0 || Goal == 0)
	{
		return INIT_CODE_NOT_READY_;
	}

	OpenSet.push_back(Start);
	m_nodes[Start].Gscore = 0;
	m_nodes[Start].Fscore = this->EstimateDistance(Start, Goal);
	print("Start & Goal node : " + std::to_string(Start) + ",  " + std::to_string(Goal) + "\tDist : " + std::to_string(m_nodes[Start].Fscore) + "\n");

	return INIT_CODE_SUCCES_;
}

void A_Star::Notify_node_core(int Node_ind)
{
	//if (this->Index(Node_ind))
	{
		if (Start == 0)
		{
			Start = Node_ind;
			m_nodes[Start].node->SetFlag("START", 1.0f);
			m_nodes[Start].node->SetIsWall(0.0f);
		}
		else if (Goal == 0)
		{
			Goal = Node_ind;
			m_nodes[Goal].node->SetFlag("GOAL", 1.0f);
			m_nodes[Goal].node->SetIsWall(0.0f);
		}
	}
}

float A_Star::Xpos(int ind)
{
	return ind % m_width;
}

float A_Star::Ypos(int ind)
{
	return static_cast<int>(ind / m_height);
}

void A_Star::ReconstructPath(Path &output_path)
{
	A_Star_node *Chain = &m_nodes[Current];

//	for (int i = 0; i < m_path->nodes.size(); ++i)
//		m_path->nodes[i]->SetFlag("CURRENT", 1.0f);//SetIsWall(0.2f);

	output_path.nodes.clear();
	output_path.nodes.push_back(m_nodes[Current].node);
	while (Chain->Came_from != nullptr)
	{
		output_path.nodes.push_back(Chain->Came_from->node);
		Chain = Chain->Came_from;
	}

	//m_nodes[Current].node->SetIsWall(0.75f);
}

bool A_Star::Index(unsigned int ind, bool Security)
{
	if (ind < m_nodes.size() && !m_nodes[ind].IsPermanentWall())
	{
		if(!Security || !(Security && m_nodes[ind].IsWall()))
			return true;
	}
	return false;
}

float A_Star::EstimateDistance(int nd1, int nd2)
{
	if (!this->Index(nd1) || !this->Index(nd2))
	{
		print("bad index!\n");
		return 0;
	}
	//return std::sqrtf(std::powf(this->Xpos(nd1) - this->Xpos(nd2), 2.0) +  
	//				  std::powf(this->Ypos(nd1) - this->Ypos(nd2), 2.0));

	return 1 * (std::fabsf(this->Xpos(nd1) - this->Xpos(nd2)) + std::fabsf(this->Ypos(nd1) - this->Ypos(nd2)));
}

int A_Star::Get_Lowest_Fscore(std::vector<int>& nodes_ind)
{
	int Lowest_Fscore_ind = 0;
	float Lowest_Fscore = POSITIVE_INFINITY+1; // Arbitrary large number
	
	for(int i = 0; i < nodes_ind.size(); ++i)
	{ 
		int ind = nodes_ind[i];
		if (m_nodes[ind].Fscore < Lowest_Fscore)
		{
			Lowest_Fscore = m_nodes[ind].Fscore;
			Lowest_Fscore_ind = ind;
		}
	}

	return Lowest_Fscore_ind;
}

void A_Star::GetNeighbors(int node_ind, std::vector<int> &Neighbors)
{
	// Top neighbor
	if (this->Index(node_ind - m_width, true) && !m_nodes[node_ind].node->GetWall(TOP_WALL_))
		Neighbors.push_back(node_ind - m_width);

	// Bottom neighbor
	if (this->Index(node_ind + m_width, true) && !m_nodes[node_ind].node->GetWall(BOTTOM_WALL_))
		Neighbors.push_back(node_ind + m_width);

	// Left neighbor
	if (this->Index(node_ind - 1, true) && !m_nodes[node_ind].node->GetWall(LEFT_WALL_))
		Neighbors.push_back(node_ind - 1);

	// Right neighbor
	if (this->Index(node_ind + 1, true) && !m_nodes[node_ind].node->GetWall(RIGHT_WALL_))
		Neighbors.push_back(node_ind + 1);
}

A_Star_node::A_Star_node(Node * node_)
{
	node = node_;

	Came_from = nullptr;

	Gscore = POSITIVE_INFINITY;
	Fscore = POSITIVE_INFINITY;
}

bool A_Star_node::IsPermanentWall()
{
	return node->IsWall(true) == PERMANENT_WALL;
}

bool A_Star_node::IsWall()
{
	return node->IsWall(true) == WALL;
}

bool A_Star_node::operator==(A_Star_node & Node_to_Compare)
{
	if (Node_to_Compare.node->GetPos(X_) == this->node->GetPos(X_) && Node_to_Compare.node->GetPos(Y_) == this->node->GetPos(Y_))
		return true;
	return false;
}
