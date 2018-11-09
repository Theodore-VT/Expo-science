
#include "Grid.hpp"
#include "guicon.hpp"

#include <string>
#include <fstream>

#define BUTTON_ID_OFFSET 200

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Grid::Grid(int Width_nodes, int Height_nodes, int Width_px, int Height_px, std::string path_to_maze) :
	m_NULL_node(0, 0, 0, 0),
	Last_node_shifted(-1),
	Highest_priority(200)	// Arbitrary big number
{
	this->Reset(Width_nodes, Height_nodes, Width_px, Height_px);
}

Node & Grid::operator()(float x, float y)
{
	int index = this->GetInd_from_2dPos(x, y);

	return  this->operator()(index);
}

Node & Grid::operator()(int ind)
{
	if (ind >= 0 && ind < Nodes.size())
		return Nodes[ind];

	return m_NULL_node;
}

void Grid::Notify_wall_shift(int ind, bool overide_previous)
{
	if ((ind != Last_node_shifted || overide_previous) && this->IsNode_shiftable(ind))
	{
		this->operator()(ind).SetIsWall((1 - this->operator()(ind).IsWall()));
		Last_node_shifted = ind;
	}
}

float Grid::GetColor(int ind)
{
	return this->operator()(ind).IsWall();
}

bool Grid::IsNode_shiftable(int ind)
{
	return !((this->operator()(ind).IsWall(true)) == PERMANENT_WALL);
}

void Grid::Save(const char * Filename)
{
	std::ofstream Save_stream(std::string(std::string(Filename) + ".lab").c_str());

	Save_stream << m_width_nodes<<"\t"<<m_height_nodes<<"\n";

	for (int i = 0; i < Nodes.size(); ++i)
		Save_stream << Nodes[i].IsWall(true) << "\t" << Nodes[i].GetWall(TOP_WALL_) << "\t" << Nodes[i].GetWall(BOTTOM_WALL_) << "\t" << Nodes[i].GetWall(RIGHT_WALL_) << "\t" << Nodes[i].GetWall(LEFT_WALL_)<<"\n";

	Save_stream.close();
}

void Grid::Load(const char * Filename)
{
	std::string Filename_ = Filename;

	if (Filename_[0] == '\"')
	{
		Filename_.erase(Filename_.begin() + 0);
		Filename_.erase(Filename_.begin() + (Filename_.size() - 1));
	}

	std::ifstream Load_stream(Filename_.c_str());

	if (Load_stream)
	{
		int width = 0, height = 0;
		Load_stream >> width;
		Load_stream >> height;

		this->Reset(width, height, m_width_px, m_height_px);

		for (int i = 0; i < (m_width_nodes * m_height_nodes); ++i)
		{
			float IsWall     = 0.0f;
			bool  TopWall    = false;
			bool  BottomWall = false;
			bool  RightWall  = false;
			bool  LeftWall    = false;
			
			
			Load_stream >> IsWall;
			Load_stream >> TopWall;
			Load_stream >> BottomWall;
			Load_stream >> RightWall;
			Load_stream >> LeftWall;

			Nodes[i].SetIsWall(IsWall);
			Nodes[i].SetWall(TOP_WALL_, TopWall);
			Nodes[i].SetWall(BOTTOM_WALL_, BottomWall);
			Nodes[i].SetWall(RIGHT_WALL_, RightWall);
			Nodes[i].SetWall(LEFT_WALL_, LeftWall);
		}
	}
	else
		print(std::string("Failed to open " + Filename_ + "\n"));
	Load_stream.close();
}

void Grid::Reset(int Width_nodes, int Height_nodes, int Width_px, int Height_px)
{
	if ((Width_nodes + Height_nodes + Width_px + Height_px) == -4)
	{
		for (int i = 0; i < Nodes.size(); ++i)
			if(Nodes[i].IsWall(true) != PERMANENT_WALL)
				Nodes[i].SetIsWall(0.0f);
	}
	else
	{
		m_width_nodes = Width_nodes;
		m_width_px = Width_px;
		m_height_nodes = Height_nodes;
		m_height_px = Height_px;

		m_width_px_per_node = Width_px / Width_nodes;
		m_height_px_per_node = Height_px / Height_nodes;

		m_width_px = m_width_px_per_node * Width_nodes;
		m_height_px = m_height_px_per_node * Height_nodes;

		m_shift_Right_Const = m_width_px_per_node * 0.15;
		m_shift_Bottom_Const = m_height_px_per_node * 0.15;

		Nodes.clear();
		All_indexes.clear();
		Nodes_SnapShot.clear();

		for (int i = 0; i < Height_nodes; i++)
			for (int j = 0; j < Width_nodes; j++)
			{
				Nodes.push_back(Node(j * m_width_px_per_node, i * m_height_px_per_node, m_width_px_per_node, m_height_px_per_node));

				if (i == 0 || j == 0 || i == Height_nodes - 1 || j == Width_nodes - 1)
					Nodes.back().SetIsWall(PERMANENT_WALL);
				else
					Nodes.back().SetIsWall(NOT_WALL);
			}

		Nodes_SnapShot = Nodes;
		for (int i = 0; i < Nodes.size(); ++i)
			All_indexes.push_back(i);
	}
}

void Grid::Add_algorithm_to_queue(Algorithm * new_Algorithm)
{
	for (int i = 0; i < Algorithms_to_update.size(); ++i)
	{
		if (Algorithms_to_update[i]->GetID() == new_Algorithm->GetID() && 
			Algorithms_to_update[i]->GetPriority() == new_Algorithm->GetPriority() &&
			Algorithms_to_update[i]->GetSpeed() == new_Algorithm->GetSpeed())
		{
			print("Refused new algorithm!\n");
			return;
		}
	}

	if (new_Algorithm->GetPriority() < Highest_priority)
		Highest_priority = new_Algorithm->GetPriority();

	Algorithms_to_update.push_back(new_Algorithm);

	print("Number of algorithms : " + std::to_string(Algorithms_to_update.size()) + "\n");
}

void Grid::Update()
{
	int Highest_priority_found = 200; // Arbitrary big number
	
	for (int i = 0; i < Algorithms_to_update.size(); ++i)
	{
		//print("Algorithm update!\n");
		int Algorithm_priority = Algorithms_to_update[i]->GetPriority();

		if (Algorithm_priority == Highest_priority)
		{
			Algorithms_to_update[i]->Update(Nodes);

			if (Algorithms_to_update[i]->Is_Finished())
				Algorithms_to_update.erase(Algorithms_to_update.begin() + i);
		}
		if (Algorithm_priority < Highest_priority_found)
			Highest_priority_found = Algorithm_priority;
	}

	if (Highest_priority_found > Highest_priority)
		Highest_priority = Highest_priority_found;
}

void Grid::Show(HWND window_handle, bool Force_Redraw)
{
	static int it = 0;

	std::vector<int> indexes_to_update;
	
	if (Force_Redraw)// || it >= 3000)
	{
		InvalidateRect(window_handle, NULL, TRUE);
		it = 0;
		indexes_to_update = All_indexes;
	}
	else
	{
		
		indexes_to_update = this->Get_ChangedNodes_ind();

		if (!indexes_to_update.empty())
			it++;
			//print(std::to_string(it++) + "\n");
	}
	
	InvalidateRect(window_handle, NULL, FALSE);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(window_handle, &ps);
	HBRUSH invalidate_hbr = CreateSolidBrush(RGB(0, 0, 0));



	for (int i = 0; i < indexes_to_update.size(); ++i)
	{
		
		it++;
		int ind = indexes_to_update[i];
		Node tmp_node = this->operator()(ind);

		float Shift_Right = (tmp_node.GetWall(LEFT_WALL_) * m_shift_Right_Const);
		float Shift_Bottom = -(tmp_node.GetWall(BOTTOM_WALL_) * m_shift_Bottom_Const);

		RECT lpcr = { tmp_node.GetVertex(0, X_) + Shift_Right, tmp_node.GetVertex(0, Y_), tmp_node.GetVertex(2, X_), tmp_node.GetVertex(2, Y_) + Shift_Bottom };
		RECT invalidate_lpcr = { tmp_node.GetVertex(0, X_), tmp_node.GetVertex(0, Y_), tmp_node.GetVertex(2, X_), tmp_node.GetVertex(2, Y_) };
		
		float Color_modifier = 1.0f - tmp_node.IsWall();

		int R = 255 * Color_modifier;
		int G = 120 * Color_modifier;
		int B = 45 * Color_modifier;

		
		FillRect(hdc, &invalidate_lpcr, invalidate_hbr);

		HBRUSH hbr = CreateSolidBrush(RGB(R, G, B));
		FillRect(hdc, &lpcr, hbr);
	}
	EndPaint(window_handle, &ps);
	//Sleep(20);
}

int Grid::GetInd_from_2dPos(int x, int y)
{
	if (x <= 0 || y <= 0 || x >= m_width_px || y >= m_height_px)
		return -1;
	
	float Node_x = std::floor(static_cast<float>(x) / m_width_px * m_width_nodes);
	float Node_y = std::floor(static_cast<float>(y) / m_height_px * m_height_nodes);


	return Node_y * m_width_nodes + Node_x;
}

int Grid::Num_nodes()
{
	return m_width_nodes * m_height_nodes;
}

int Grid::Width_px()
{
	return m_width_px;
}

int Grid::Height_px()
{
	return m_height_px;
}

int Grid::Width_nd()
{
	return m_width_nodes;
}

int Grid::Height_nd()
{
	return m_height_nodes;
}

std::vector<int> Grid::Get_ChangedNodes_ind()
{
	std::vector<int> indexes;

	for (int i = 0; i < Nodes.size(); ++i)
	{
		if (Nodes[i].IsWall() != Nodes_SnapShot[i].IsWall() || 
			Nodes[i].GetWall(LEFT_WALL_) != Nodes_SnapShot[i].GetWall(LEFT_WALL_) || 
			Nodes[i].GetWall(BOTTOM_WALL_) != Nodes_SnapShot[i].GetWall(BOTTOM_WALL_) ||
			Nodes[i].GetWall(RIGHT_WALL_) != Nodes_SnapShot[i].GetWall(RIGHT_WALL_) ||
			Nodes[i].GetWall(TOP_WALL_) != Nodes_SnapShot[i].GetWall(TOP_WALL_))
			indexes.push_back(i);
	}

	Nodes_SnapShot = Nodes;
	return indexes;
}
