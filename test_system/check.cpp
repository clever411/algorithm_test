#include <chrono>
#include <iostream>
#include <random>

#include <clever/IostreamFunction.hpp>



#ifdef SELECTION_SORT
	#include "selection_sort.cpp"
	typedef random_array_type data_type;

#elif INSERTION_SORT
	#include "insertion_sort.cpp"
	typedef random_array_type data_type;

#else
	static_assert(false);

#endif





using namespace std;



int main( int argc, char *argv[] )
{
	srand(chrono::system_clock::now().time_since_epoch().count());
	std::vector<int> vec;

	vec.reserve(30);
	for(size_t i = 0; i < 30; ++i) {
		vec.push_back( rand()%1000 - 500 );
	}

	std::cout << "before: " << vec << std::endl;

	data_type data;
	data.d = vec.data();
	data.n = vec.size();
	algorithm(data);
	data.d = nullptr;
	std::cout << "after: " << vec << std::endl;

	return 0;
}
