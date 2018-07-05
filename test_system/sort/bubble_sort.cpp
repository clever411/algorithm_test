#include <utility>
#include "../structures/random_array.cpp"




void bubble_sort( random_array_type &ar )
{
	if(ar.n == 0)
		return;

	for(int *end = ar.d+ar.n; end != ar.d; --end) {
		for(int *b = ar.d+1; b != end; ++b) {
			// if position invalid
			if( *b < *(b-1) ) {
				std::swap(*b, *(b-1));
			}
		}
	}

	return;
}
