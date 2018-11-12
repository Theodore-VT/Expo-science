#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <vector>
#include <chrono>
#include "Node.hpp"
#include "guicon.hpp"

#define INSTANTANEOUS -1
#define MAX_TIME_INSTANTANEOUS_PROCESS 3

#define INIT_CODE_FAILED_		0
#define INIT_CODE_NOT_READY_	1
#define INIT_CODE_SUCCES_		2

class Algorithm
{
public:
	Algorithm(int priority, int It_per_Step);
	virtual ~Algorithm() {};

	void Update(std::vector<Node> &Nodes);
	void Notify_node(int Node_ind);

	int GetPriority();
	int GetID();
	int GetSpeed();
	bool Is_Finished();

protected:
	virtual bool Update_core(std::vector<Node> &Nodes) = 0;
	virtual int  Init(std::vector<Node> &Nodes) = 0;
	virtual void Notify_node_core(int Node_ind) = 0;

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