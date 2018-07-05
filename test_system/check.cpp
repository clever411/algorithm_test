#include <algorithm>
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
	constexpr unsigned int const VECTOR_SIZE = 20u;


	// fill
	default_random_engine dre(
		chrono::system_clock::now().
		time_since_epoch().count()
	);
	std::vector<int> vec;

	vec.reserve(VECTOR_SIZE);
	for(size_t i = 0; i < VECTOR_SIZE; ++i) {
		vec.push_back(i);
	}
	shuffle( vec.begin(), vec.end(), dre );


	// print&test
	std::cout << "before: " << vec << std::endl;

		// testing
		data_type data;
		data.d = vec.data();
		data.n = vec.size();
		algorithm(data);
		data.d = nullptr;

	std::cout << "after: " << vec << std::endl;


	return 0;
}
