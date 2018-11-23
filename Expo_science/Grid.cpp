
#include "Grid.hpp"
#include "guicon.hpp"

#include <algorithm>
#include <string>
#include <fstream>


#define PI 3.14159265359

#define R_REGULAR_ 255
#define G_REGULAR_ 120
#define B_REGULAR_ 45

#define R_START_ 25
#define G_START_ 255
#define B_START_ 120

#define R_GOAL_ 100
#define G_GOAL_ 50
#define B_GOAL_ 140

#undef max
#undef min

Grid::Grid(int Width_nodes, int Height_nodes, int Width_px, int Height_px, PAINTSTRUCT *ps_, std::string path_to_maze) :
	m_NULL_node(0, 0, 0, 0),
	Last_node_shifted(-1),
	Highest_priority(200),	// Arbitrary big number
	Null_Path(nullptr),

	m_algorithm_to_notify(0),
	m_notify_steps(0),
	Just_finished_algo(false)//,
	//ps(ps_)
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
		if (this->operator()(ind).IsWall() != 0.0f && this->operator()(ind).IsWall() != 1.0f)
			this->operator()(ind).SetIsWall(1.0f);
		else
			this->operator()(ind).SetIsWall((1 - this->operator()(ind).IsWall()));
		Last_node_shifted = ind;

		this->Notify_algorithm_on_click(ind);
	}
}

float Grid::GetColor(int ind)
{
	return this->operator()(ind).IsWall();
}

bool Grid::IsNode_shiftable(int ind)
{
	return !((this->operator()(ind).IsWall(true)) == PERMANENT_WALL || this->operator()(ind).GetFlag() != "");
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
			float IsWall = 0.0f;
			bool  TopWall = false;
			bool  BottomWall = false;
			bool  RightWall = false;
			bool  LeftWall = false;


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

	this->Reset();
}

void Grid::Reset(int Width_nodes, int Height_nodes, int Width_px, int Height_px)
{
	Resolve_paths.clear();
	Algorithms_to_update.clear();

	if ((Width_nodes + Height_nodes + Width_px + Height_px) == -4)
	{
		for (int i = 0; i < Nodes.size(); ++i)
			if (Nodes[i].IsWall(true) != PERMANENT_WALL)
			{
				Nodes[i].SetFlag("", 0.0f);
				//if(i % 2 == 0)
				Nodes[i].SetIsWall(0.0f);
				//else
				//	Nodes[i].SetIsWall(0.5f);
			}
	}
	else
	{
		m_width_nodes = Width_nodes;
		m_width_px = Width_px;
		m_height_nodes = Height_nodes;
		m_height_px = Height_px;

		m_width_px_per_node = Width_px / Width_nodes;
		m_height_px_per_node = Height_px / Height_nodes;

		m_real_width_px = m_width_px_per_node * Width_nodes;
		m_real_height_px = m_height_px_per_node * Height_nodes;

		Path_stroke = std::min(m_width_px_per_node, m_height_px_per_node) * 0.6;

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

void Grid::Add_algortihm_notifier(int ID, int Number_of_times)
{
	m_algorithm_to_notify = ID;
	m_notify_steps = Number_of_times;
}

void Grid::Notify_algorithm_on_click(int Node_ind)
{
	if (m_algorithm_to_notify && m_notify_steps)
	{
		bool Did_something = false;
		for (int i = 0; i < Algorithms_to_update.size(); ++i)
		{
			if (Algorithms_to_update[i]->GetID() == m_algorithm_to_notify)
			{
				Did_something = true;
				Algorithms_to_update[i]->Notify_node(Node_ind);
			}
		}
		if(Did_something)
			m_notify_steps--;
	}
	else
	{
		m_notify_steps = 0;
		m_algorithm_to_notify = 0;
	}
}

void Grid::Update()
{
	int Highest_priority_found = 200; // Arbitrary big number
	Just_finished_algo = false;

	for (int i = 0; i < Algorithms_to_update.size(); ++i)
	{
		int Algorithm_priority = Algorithms_to_update[i]->GetPriority();

		if (Algorithm_priority == Highest_priority)
		{
			int Path_IRL = Algorithms_to_update[i]->GetPathIRL();
			if (Path_IRL < 0 || Path_IRL > Resolve_paths.size() - 1)
				Algorithms_to_update[i]->Update(Nodes, Null_Path);
			else
				Algorithms_to_update[i]->Update(Nodes, Resolve_paths[Path_IRL]);

			if (Algorithms_to_update[i]->Is_Finished())
			{
				Just_finished_algo = true;
				Algorithms_to_update.erase(Algorithms_to_update.begin() + i);
			}
		}
		if (Algorithm_priority < Highest_priority_found)
			Highest_priority_found = Algorithm_priority;
	}

	if (Highest_priority_found > Highest_priority)
		Highest_priority = Highest_priority_found;
}

void Grid::Show(HWND window_handle, bool Force_Redraw)
{
	static int num_show = 0;
	if (Finished)
	{
		//OutputDebugStringA(std::string("Shows : " + std::to_string(num_show) + "\n").c_str());
		Finished = false;
		std::vector<int> indexes_to_update;

		if (Force_Redraw || Just_finished_algo)
		{
			Just_finished_algo = false;
			InvalidateRect(window_handle, NULL, TRUE);
			//RedrawWindow(window_handle, NULL, NULL, RDW_ERASE);
			indexes_to_update = All_indexes;
			num_show = 0;
		}
		else
		{
			indexes_to_update = this->Get_ChangedNodes_ind();
			InvalidateRect(window_handle, NULL, FALSE);
		}
		
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window_handle, &ps);
		HBRUSH invalidate_hbr = CreateSolidBrush(RGB(0, 0, 0));
		//HBRUSH hbr = CreateSolidBrush(RGB(255, 100, 25));
		for (int i = 0; i < indexes_to_update.size(); ++i)
		{
			if (i == 0)
				num_show++;
			int ind = indexes_to_update[i];
			Node tmp_node = this->operator()(ind);

			float Shift_Right = (tmp_node.GetWall(LEFT_WALL_) * m_shift_Right_Const);
			float Shift_Bottom = -(tmp_node.GetWall(BOTTOM_WALL_) * m_shift_Bottom_Const);

			RECT lpcr = { tmp_node.GetVertex(0, X_) + Shift_Right, tmp_node.GetVertex(0, Y_), tmp_node.GetVertex(2, X_), tmp_node.GetVertex(2, Y_) + Shift_Bottom };
			RECT invalidate_lpcr = { tmp_node.GetVertex(0, X_), tmp_node.GetVertex(0, Y_), tmp_node.GetVertex(2, X_), tmp_node.GetVertex(2, Y_) };

			FillRect(hdc, &invalidate_lpcr, invalidate_hbr);

			int R, G, B;
			this->Node_Color(ind, R, G, B);

			HBRUSH hbr = CreateSolidBrush(RGB(R, G, B));
			//Sleep(0.5);
			FillRect(hdc, &lpcr, hbr);
			::DeleteObject(hbr);
		}
		::DeleteObject(invalidate_hbr);
		for(int i = 0; i < Nodes.size(); ++i)
			if(Nodes[i].GetFlag() == "IN_PATH")
				Nodes[i].SetFlag("", 0.0f);

		for (int ind = 0; ind < Resolve_paths.size(); ++ind)
		{
			Path *path = &Resolve_paths[ind];

			HPEN pen = CreatePen(PS_SOLID, Path_stroke, RGB(path->R, path->G, path->B));
			SelectObject(hdc, pen);

			if (path->nodes.size() > 1)
			{
				//path->nodes[0]->SetFlag("IN_PATH", 1.0f);
				::MoveToEx(hdc, path->nodes[0]->GetPos(X_), path->nodes[0]->GetPos(Y_), NULL);

				for (int i = 1; i < path->nodes.size(); ++i)
				{
					path->nodes[i]->SetFlag("IN_PATH", 1.0f);

					LineTo(hdc, path->nodes[i]->GetPos(X_), path->nodes[i]->GetPos(Y_));
				}
			}
			DeleteObject(pen);
		}

		EndPaint(window_handle, &ps);
		
		::DeleteObject(hdc);
		//::DeleteDC(hdc);

		Finished = true;
		//OutputDebugStringA("!_Showing!\n");
	}
}

int Grid::GetInd_from_2dPos(int x, int y)
{
	if (x <= 0 || y <= 0 || x >= m_width_px || y >= m_height_px)
		return -1;
	
	float Node_x = std::floor(static_cast<float>(x) / m_real_width_px * m_width_nodes);
	float Node_y = std::floor(static_cast<float>(y) / m_real_height_px * m_height_nodes);


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

int Grid::GetPath(int ind)
{
	if (ind >= (int)Resolve_paths.size())
		return -1;//std::shared_ptr<Path>(&Null_Path);
	else if (ind == LAST_PATH)
	{
		print("Return last path !\n");
		return /*this->GetPath(*/Resolve_paths.size() -1;
	}
	else if (ind >= 0)
	{
		//Path path = Resolve_paths[ind];
		return /*std::shared_ptr<Path>(/*&Resolve_paths[ind]&Resolve_paths.data()[*/ind;// ]);
	}
}

void Grid::AddPath(unsigned int Start_node)
{
	if (Start_node > Nodes.size())
		return;

	Resolve_paths.push_back(Path());

	print("Number of paths : " + std::to_string(Resolve_paths.size()) + "\n");
}

int Grid::GetAlgorithmID(int ind)
{
	if (ind >= (int)Algorithms_to_update.size())
		return 0;
	if (ind == LAST_ELEM)
		return this->GetAlgorithmID(Algorithms_to_update.size() - 1);

	return Algorithms_to_update[ind]->GetID();
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
			Nodes[i].GetWall(TOP_WALL_) != Nodes_SnapShot[i].GetWall(TOP_WALL_) ||
			Nodes[i].GetFlag() != Nodes_SnapShot[i].GetFlag())
			indexes.push_back(i);

		//if (Nodes[i].GetWall(LEFT_WALL_, true) != Nodes_SnapShot[i].GetWall(LEFT_WALL_, true) ||
		//	Nodes[i].GetWall(BOTTOM_WALL_, true) != Nodes_SnapShot[i].GetWall(BOTTOM_WALL_, true) ||
		//	Nodes[i].GetWall(RIGHT_WALL_, true) != Nodes_SnapShot[i].GetWall(RIGHT_WALL_, true) ||
		//	Nodes[i].GetWall(TOP_WALL_, true) != Nodes_SnapShot[i].GetWall(TOP_WALL_, true))
		//{
		//	OutputDebugStringA(std::string(std::to_string(Nodes[i].GetWall(LEFT_WALL_, true)) + ",  " +
		//								   std::to_string(Nodes[i].GetWall(BOTTOM_WALL_, true)) + ",  " +
		//								   std::to_string(Nodes[i].GetWall(RIGHT_WALL_, true)) + ",  " +
		//								   std::to_string(Nodes[i].GetWall(TOP_WALL_, true)) + "\n").c_str());
		//}
	}

	Nodes_SnapShot = Nodes;
	return indexes;
}

void Grid::Node_Color(int node_ind, int & R, int & G, int & B)
{
	std::string flag = Nodes[node_ind].GetFlag();
	if (flag != "")
	{
		if (flag == "START")
		{
			print("Update the start node!\n");
			R = R_START_;
			G = G_START_;
			B = B_START_;
			return;
		}
		else if (flag == "GOAL")
		{
			R = R_GOAL_;
			G = G_GOAL_;
			B = B_GOAL_;
			return;
		}
	}
	
	{
		float Color_modifier = 1.0f - Nodes[node_ind].IsWall();

		if (flag == "CURRENT")
			Color_modifier = 0.5f;

		R = R_REGULAR_ * Color_modifier;
		G = G_REGULAR_ * Color_modifier;
		B = B_REGULAR_ * Color_modifier;
	}
}

POINT Grid::rotate_point(float cx, float cy, float angle, POINT p)
{
	float s = sin(angle * PI / 180);
	float c = cos(angle * PI / 180);

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;
	return p;
}

void Grid::Draw_Line(HDC hdc, int x1, int y1, int x2, int y2, int thinkness)
{

}

void Grid::Draw_Path(int ind, HWND window_handle)
{
	//print("Drawing path! " + std::to_string(Resolve_paths[ind].nodes.size()) + "\n");
	if (ind > Resolve_paths.size())
		return;

	Path *path = &Resolve_paths[ind];

	InvalidateRect(window_handle, NULL, FALSE);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(window_handle, &ps);

	HPEN pen = CreatePen(PS_SOLID, 25, RGB(path->R, path->G, path->B));
	SelectObject(hdc, pen);
	
	for (int i = 0; i < Nodes.size(); ++i)
		if (Nodes[i].GetFlag() == "IN_PATH")
			Nodes[i].SetFlag("", 0.0f);

	if (path->nodes.size() > 1)
	{
		//path->nodes[0]->SetFlag("IN_PATH", 1.0f);
		::MoveToEx(hdc, path->nodes[0]->GetPos(X_), path->nodes[0]->GetPos(Y_), NULL);

		for (int i = 1; i < path->nodes.size(); ++i)
		{
			path->nodes[i]->SetFlag("IN_PATH", 1.0f);

			::LineTo(hdc, path->nodes[i]->GetPos(X_), path->nodes[i]->GetPos(Y_));
		}
	}
	EndPaint(window_handle, &ps);
	//::DeleteObject(hdc);
	::DeleteDC(hdc);
}
