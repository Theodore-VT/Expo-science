
#include "Stay_left.hpp"

Stay_left::Stay_left(int priority, int It_per_Step, int Width_nodes, int Height_nodes, int path):
	Algorithm(priority, It_per_Step),
	m_width(Width_nodes),
	m_height(Height_nodes)
{
	m_path_IRL = path;
	m_ID = rand() % 25555892;

	Start = -1;
	Goal = -1;
}

bool Stay_left::Update_core(std::vector<Node>& Nodes, Path & path_to_change)
{
	bool Is_current_start_or_goal = false;
	if (Current == Start || Current == Goal)
		Is_current_start_or_goal = true;
	
	if (!Is_current_start_or_goal)
		Nodes[Current].SetFlag("", false);

	if (Current != Goal)
	{
		path_to_change.nodes.push_back(&Nodes[Current]);
		this->StayLeft(Nodes);
		
		if(Current != Goal)
			Nodes[Current].SetFlag("CURRENT", true);
		return false;
	}

	//Nodes[Current].SetFlag("GOAL", 1.0f);
	path_to_change.nodes.push_back(&Nodes[Current]);

	return true;
}

int Stay_left::Init(std::vector<Node>& Nodes)
{
	if (Start != -1)
	{
		Nodes[Start].SetIsWall(false);
		Nodes[Start].SetFlag("START", 1.0f);
	}
	if (Goal != -1)
	{
		Nodes[Goal].SetIsWall(false);
		Nodes[Goal].SetFlag("GOAL", 1.0f);
	}

	if (Start == -1 || Goal == -1)
		return INIT_CODE_NOT_READY_;


	Current = Start;
	Previous = Start - 1;
	return INIT_CODE_SUCCES_;
}

void Stay_left::Notify_node_core(int Node_ind)
{
	if (Start == -1)
		Start = Node_ind;
	else if (Goal == -1)
		Goal = Node_ind;
}

bool Stay_left::CanGo(int From, int To, std::vector<Node> &Nodes)
{
	if (From < 0 || From >= Nodes.size() || To < 0 || To >= Nodes.size())
		return false;

	if (Nodes[To].IsWall())
		return false;

	if(To == From - m_width && (Nodes[From].GetWall(TOP_WALL_) || Nodes[To].GetWall(BOTTOM_WALL_)))
		return false;

	if (To == From + m_width && (Nodes[From].GetWall(BOTTOM_WALL_) || Nodes[To].GetWall(TOP_WALL_)))
		return false;

	if (To == From - 1 && (Nodes[From].GetWall(LEFT_WALL_) || Nodes[To].GetWall(RIGHT_WALL_)))
		return false;

	if (To == From + 1 && (Nodes[From].GetWall(RIGHT_WALL_) || Nodes[To].GetWall(LEFT_WALL_)))
		return false;

	return true;
}

void Stay_left::StayLeft(std::vector<Node>& Nodes)
{
	// Previous is on the left, so ->
	if (Previous == Current - 1)
	{
		print("From left ");

		bool Wall_on_right = !this->CanGo(Current, Current + m_width, Nodes);
		bool Wall_on_front = !this->CanGo(Current, Current + 1, Nodes);
		bool Wall_on_left  = !this->CanGo(Current, Current - m_width, Nodes);

		if (Wall_on_right && !Wall_on_front)
		{
			print("going right\n");
			Previous = Current;
			Current += 1;
		}
		else if (!Wall_on_right)
		{
			print("going down\n");
			Previous = Current;
			Current += m_width;
		}
		else if(!Wall_on_left)
		{
			print("going up\n");
			Previous = Current;
			Current -= m_width;
		}
		else
		{
			print("turning around\n");
			Previous = Current + 1;
			Current = Current;
		}
	}
	// Previous is on the right, so <-
	else if (Previous == Current + 1)
	{
		print("From right ");

		bool Wall_on_right = !this->CanGo(Current, Current - m_width, Nodes);
		bool Wall_on_front = !this->CanGo(Current, Current - 1, Nodes);
		bool Wall_on_left  = !this->CanGo(Current, Current + m_width, Nodes);

		if (Wall_on_right && !Wall_on_front)
		{
			print("going left\n");
			Previous = Current;
			Current -= 1;
		}
		else if (!Wall_on_right)
		{
			print("going up\n");
			Previous = Current;
			Current -= m_width;
		}
		else if(!Wall_on_left)
		{
			print("going down\n");
			Previous = Current;
			Current += m_width;
		}
		else
		{
			print("turning around\n");
			Previous = Current - 1;
			Current = Current;
		}
	}
	//					      |
	// Previous is on top, so V
	else if (Previous == Current - m_width)
	{
		print("From up ");

		bool Wall_on_right = !this->CanGo(Current, Current - 1, Nodes);
		bool Wall_on_front = !this->CanGo(Current, Current + m_width, Nodes);
		bool Wall_on_left  = !this->CanGo(Current, Current + 1, Nodes);

		if (Wall_on_right && !Wall_on_front)
		{
			print("going down\n");
			Previous = Current;
			Current += m_width;
		}
		else if (!Wall_on_right)
		{
			print("going left\n");
			Previous = Current;
			Current -= 1;
		}
		else if(!Wall_on_left)
		{
			print("going right\n");
			Previous = Current;
			Current += 1;
		}
		else
		{
			print("turing around\n");
			Previous = Current + m_width;
			Current = Current;
		}
	}
	// Previous is on bottom, so ^
	//							 |
	else if (Previous == Current + m_width)
	{
		print("From down ");

		bool Wall_on_right = !this->CanGo(Current, Current + 1, Nodes);
		bool Wall_on_front = !this->CanGo(Current, Current - m_width, Nodes);
		bool Wall_on_left  = !this->CanGo(Current, Current - 1, Nodes);

		if (Wall_on_right && !Wall_on_front)
		{
			print("going up\n");
			Previous = Current;
			Current -= m_width;
		}
		else if (!Wall_on_right)
		{
			print("going right\n");
			Previous = Current;
			Current += 1;
		}
		else if(!Wall_on_left)
		{
			print("going left\n");
			Previous = Current;
			Current -= 1;
		}
		else
		{
			print("turing around\n");
			Previous = Current - m_width;
			Current = Current;
		}
	}
}
