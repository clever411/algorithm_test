#include <chrono>
#include <iostream>
#include <fstream>
#include <libconfig.h++>

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
template<typename Ostream, typename Algorithm, typename DataType>
void alghorithm_test(
	Ostream &os, Algorithm alg,
	DataType data = DataType(),
	size_t maxn = 1000u, size_t repeatcount = 1000u
)
{
	using namespace chrono;

	// loop
	clever::Stopwatch<chrono::high_resolution_clock> watch;
	float fbuf;
	for(size_t i = 0; i < maxn; ++i) {
		// algorithm testing
		for(size_t i = 0; i < repeatcount; ++i) {
			data.update();
			watch.start();
			alg(data);
			watch.stop();
		}

		// add to config
		fbuf = (float)data.getN();
		os.write( (char const *)&fbuf, sizeof fbuf );

		fbuf = (float)chrono::duration_cast<
			chrono::duration<double, ratio<1, 1000000>>
		>(watch.duration()).count();
		os.write( (char const *)&fbuf, sizeof fbuf );
		

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

	return;
}






// main
int main( int argc, char *argv[] )
{
	char const *outfilename;


	// set output file name
	if(argc < 2)
		outfilename = DEFAULT_OUTPUT_FILE_NAME;
	else
		outfilename = argv[1];


	// test algorthim
	{
		ofstream fout(outfilename, ofstream::binary);
		if(!fout) {
			cerr << "can't open file '" << outfilename << "'" << endl;
			return EXIT_FAILURE;
		}

		alghorithm_test(
			fout, algorithm, data_type(),
			1000u, 100u
		);
	}


	// it's all
	return 0;
}





// end
