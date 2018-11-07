#include "Dummy.hpp"
#include "guicon.hpp"


Dummy::Dummy(int priority, int It_per_Step):
	Algorithm(priority, It_per_Step)
{
	m_ID = 1;
}

bool Dummy::Update_core(std::vector<Node>& Nodes)
{
	//print("blob\n");
	static int i = 0;
	i++;
	if (i > 100)
		return true;
	return false;
}
