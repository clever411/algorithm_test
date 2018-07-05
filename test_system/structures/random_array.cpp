#include <algorithm>
#include <chrono>
#include <random>

#include "Data.hpp"





// struct
struct RandomArrayStruct
{
	int *d;
	unsigned int n;

	std::default_random_engine dre;
};





// Data constructor, destructor
template<>
Data<RandomArrayStruct>::Data():
	RandomArrayStruct{
		new int[1], 1,
		std::default_random_engine(
			std::chrono::system_clock::now().
			time_since_epoch().count()
		)
	}
{
	*d = 0;
	return;
}

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
	std::shuffle(d, d+n, dre);
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
	// resize
	if(d)
		delete[] d;
	++n;
	d = new int[n];

	// fill
	for(size_t i = 0; i < n; ++i) {
		d[i] = i;
	}

	// randomize
	update();

	return *this;
}





typedef Data<RandomArrayStruct> random_array_type;





// end
