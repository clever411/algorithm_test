#include <utility>
#include "../structures/random_array.cpp"





void insertion_sort(random_array_type &ar)
{
	int buf;
	int *j;
	for(int *i = ar.d+1, *ie = ar.d+ar.n; i < ie; ++i)
	{
		buf = *i;
		j = i-1;
		while(j >= ar.d && buf < *j)
		{
			*(j+1) = *j;
			--j;
		}
		*(j+1) = buf;
	}

	return;
}





// end
