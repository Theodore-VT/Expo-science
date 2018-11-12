#include "Algorithm.hpp"

bool operator==(Algorithm & algo_1, Algorithm & algo_2)
{
	if (algo_1.GetID() == algo_2.GetID() && algo_1.GetPriority() == algo_2.GetPriority() && algo_1.GetSpeed() == algo_2.GetSpeed())
		return true;
	else
		return false;
}


Algorithm::Algorithm(int priority, int It_per_Step) :
	m_priority(priority),
	m_speed(It_per_Step),
	m_initialSpeed(It_per_Step),
	Finished(false),
	Initialized(false),
	m_ID(0)					// Base class doesn't need ID
{};

void Algorithm::Update(std::vector<Node> &Nodes)
{
	if (!Initialized)
	{
		int InitCode = this->Init(Nodes);
		if (InitCode == INIT_CODE_FAILED_) // Failed to initialize
		{
			print("Algorithm failed to initialize!\n");
			Finished = true;
			return;
		}
		else if(InitCode == INIT_CODE_SUCCES_)
			Initialized = true;
	}

	if (!Finished && Initialized)
	{
		if (m_speed == INSTANTANEOUS)
		{
			std::chrono::time_point<std::chrono::system_clock> start, end; // Watch dog
			start = std::chrono::system_clock::now();
			bool succes = false;
			while (1)
			{
				if (this->Update_core(Nodes))
				{
					succes = true;
					break;
				}

				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() > MAX_TIME_INSTANTANEOUS_PROCESS)
				{
					m_speed = 1;
					print("Speed = 1\n");
					break;
				}
			}
			Finished = succes;
		}
		else if (1)
		{
			Finished = this->Update_core(Nodes);
		}

		if (Finished)
			print("Finished algorithm " + std::to_string(m_ID) + "\n");
		m_it++;
	}

	//print(std::string("Finished : " + std::to_string(Finished) + "\n"));
}
void Algorithm::Notify_node(int Node_ind)
{
	this->Notify_node_core(Node_ind);
}
int Algorithm::GetPriority()
{
	return m_priority;
}
int Algorithm::GetID()
{
	return m_ID;
}
int Algorithm::GetSpeed()
{
	return m_initialSpeed;
}
bool Algorithm::Is_Finished()
{
	return Finished;
}

int Algorithm::GetIt()
{
	return m_it;
}
