#include <random>

#include "Data.hpp"





// struct
struct RandomArrayStruct
{
	int *d;
	unsigned int n;

protected:
	void fill_()
	{
		for(auto *b = d, *e = d+n; b != e; ++b) {
			*b = std::rand()%1000 - 500;
		}
		return;
	}

};





// Data constructor, destructor
template<>
Data<RandomArrayStruct>::Data(): RandomArrayStruct{new int[1u], 1u} {}

template<>
Data<RandomArrayStruct>::~Data()
{
	if(d)
		delete[] d;
	return;
}


// Data interface method
template<>
Data<RandomArrayStruct> &Data<RandomArrayStruct>::update()
{
	fill_();
	return *this;
}

template<>
unsigned int Data<RandomArrayStruct>::getN() const
{
	return n;
}

template<>
Data<RandomArrayStruct> &Data<RandomArrayStruct>::next()
{
	delete[] d;
	++n;
	d = new int[n];
	fill_();

	return *this;
}


typedef Data<RandomArrayStruct> random_array_type;
