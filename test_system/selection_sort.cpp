#include <random>

#include "random_array.cpp"





// find minimum element
template<typename Iter>
inline Iter min_element(Iter b, Iter e)
{
	if(b == e)
		return e;
	Iter min = b;
	while(++b != e) {
		if(*b < *min)
			min = b;
	}
	return min;
}


void algorithm(random_array_type &ar)
{
	int *min;
	for(auto *b = ar.d, *e = ar.d+ar.n; b < e; ++b) {
		min = min_element(b, e);
		std::swap(*min, *b);
	}
	return;
}





// end
