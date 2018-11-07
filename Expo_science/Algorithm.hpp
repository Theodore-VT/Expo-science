#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <vector>
#include <chrono>
#include "Node.hpp"
#include "guicon.hpp"

#define INSTANTANEOUS -1
#define MAX_TIME_INSTANTANEOUS_PROCESS 3

class Algorithm
{
public:
	Algorithm(int priority, int It_per_Step);
	virtual ~Algorithm() {};

	void Update(std::vector<Node> &Nodes);
	int GetPriority();
	int GetID();
	int GetSpeed();
	bool Is_Finished();

protected:
	virtual bool Update_core(std::vector<Node> &Nodes) = 0;
	virtual void Init(std::vector<Node> &Nodes) = 0;

	int GetIt();

	int m_ID;

private:
	
	int m_it;
	int m_priority;
	
	int m_speed;
	int m_initialSpeed;
	bool Finished;
	bool Initialized;
};

#endif