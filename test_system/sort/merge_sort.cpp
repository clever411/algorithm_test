#include <cstring>
#include <iterator>
#include <utility>
#include "../structures/random_array.cpp"





// merge
template<typename Iter>
void merge(Iter fbeg, Iter fend, Iter sbeg, Iter send, Iter out)
{
loop:

	if(fbeg == fend)
	{
		while(sbeg != send)
		{
			*out = *sbeg;
			++sbeg;
			++out;
		}

		return;
	}
	else if(sbeg == send)
	{
		while(fbeg != fend)
		{
			*out = *fbeg;
			++fbeg;
			++out;
		}

		return;
	}


	if(*fbeg < *sbeg)
	{
		*out = *fbeg;
		++fbeg;
	}
	else
	{
		*out = *sbeg;
		++sbeg;
	}
	++out;

	goto loop;
}



// algorithm implement
// array representate by pointer
template<typename T>
void merge_sort(T *b, T *e, T *buf)
{
	using namespace std;
	// check distance
	int dis = distance(b, e);
	if(dis < 2)
		return;
	else if(dis == 2)
	{
		if(*next(b) < *b)
			swap(*next(b), *b);

		return;
	}

	// sorting halfs
	T *half = b + dis/2;
	merge_sort(b, half, buf);
	merge_sort(half, e, buf);

	merge(b, half, half, e, buf);
	memcpy( b, buf, sizeof(*buf) * dis );

	return;
}


// algorithm
void merge_sort(random_array_type &ar)
{
	std::cout << 'm' << std::endl;
	int *buf = new int[ar.n];
	merge_sort( ar.d, ar.d+ar.n, buf );
	delete[] buf;
	return;
}





// end
