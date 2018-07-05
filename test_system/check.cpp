#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

#include <clever/IostreamFunction.hpp>



#ifdef SELECTION_SORT
	#include "sort/selection_sort.cpp"
	typedef random_array_type data_type;
	constexpr void(*algorithm)(data_type &) = &selection_sort;

#elif INSERTION_SORT
	#include "sort/insertion_sort.cpp"
	typedef random_array_type data_type;
	constexpr void(*algorithm)(data_type &) = &insertion_sort; 

#elif BUBBLE_SORT
	#include "sort/bubble_sort.cpp"
	typedef random_array_type data_type;
	constexpr void(*algorithm)(data_type &) = &bubble_sort;

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

	sort(
		vec.begin(), vec.end(),
		[](int lhs, int rhs)->bool {
			return lhs > rhs;
		}
	);

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
