
#ifndef NODE_ARRAY_HPP
#define NODE_ARRAY_HPP

#include <vector>
#include "Node.hpp"

template<class T>
struct _2d_array
{
	_2d_array(unsigned int Width_, unsigned int Height_, std::vector<T> &Objects_)
	{
		Width = Width_;
		Height = Height_;

		Objects = Objects_;
		NULL_Object = T();
		Created = true;
	}
	_2d_array()
	{
		Width = 0;
		Height = 0;

		Objects = std::vector<T>(0);
		NULL_Object = T();
		Created = false;
	}
	int GetInd(int x, int y)
	{
		int ind = y * Width + x;
		if (ind < this->size())
			return ind;
		return 0;
	}
	int GetX(int ind)
	{
		return ind / Width;
	}
	int GetY(int ind)
	{
		return ind % Height;
	}
	T &operator()(int x, int y)
	{
		return this->operator()(y * Width + x);
	}
	T &operator()(unsigned int ind)
	{
		if(ind < Objects.size() && Created)
			return Objects[ind];

		return NULL_Object;
	}
	int size()
	{
		return Objects.size();
	}
	std::vector<T> Objects;
	T NULL_Object;
	int Width, Height;
	bool Created;
};

#endif