#include <chrono>
#include <iostream>
#include <libconfig.h++>
#include <random>

#include <clever/Stopwatch.hpp>



#ifdef SELECTION_SORT
	#include "selection_sort.cpp"
	typedef random_array_type data_type;

#elif INSERTION_SORT
	#include "insertion_sort.cpp"
	typedef random_array_type data_type;

#else
	static_assert(false);

#endif





using namespace libconfig;
using namespace std;


Config config;
char const *DEFAULT_OUTPUT_FILE_NAME = "chart.txt";


/*
 * Algorithm:
 *         any_type operator()(Data &) - working;
 * 
 * Data:
 *         any_type update() - update data;
 *         numeric_type getN() - get N;
 *         any_type operator++() - next N;
 */
template<typename Algorithm, typename DataType>
void alghorithm_test(
	Setting &root, Algorithm alg,
	DataType data = DataType(),
	size_t maxn = 1000u, size_t repeatcount = 1000u
)
{
	using namespace chrono;

	// preparation
	Setting &cfgdata = root.add( "data", Setting::TypeArray );
	Setting *el;

	// loop
	clever::Stopwatch<chrono::high_resolution_clock> watch;
	for(size_t i = 0; i < maxn; ++i) {
		// algorithm testing
		for(size_t i = 0; i < repeatcount; ++i) {
			data.update();
			watch.start();
			alg(data);
			watch.stop();
		}

		// add to config
		el = &cfgdata.add(Setting::TypeFloat);
		*el = (float)data.getN();
		el = &cfgdata.add(Setting::TypeFloat);
		*el = (float)chrono::duration_cast<
			chrono::duration<double, ratio<1, 1000000>>
		>(watch.duration()).count();

		// reset and continue
		watch.reset();
		data.next();
#ifndef SILENCE
		if(i % 10 == 0)
			cout << "success " << i << " loop" << endl;
#endif
	}
#ifndef SILENCE
	cout << "success all loops" << endl;
#endif

}






// main
int main( int argc, char *argv[] )
{
	// give seed to random
	srand(
		chrono::system_clock::now().
		time_since_epoch().count()
	);
	
	// set output file name
	char const *outfilename;
	if(argc < 2) {
		outfilename = DEFAULT_OUTPUT_FILE_NAME;
	}
	else {
		outfilename = argv[1];
	}

	// test algorthim
	alghorithm_test(config.getRoot(), algorithm, data_type(), 1000);
	
	// write result in file
	config.writeFile(outfilename);

	return 0;
}





// end
