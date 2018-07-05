#include <chrono>
#include <iostream>
#include <fstream>
#include <libconfig.h++>

#include <clever/Stopwatch.hpp>



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





using namespace libconfig;
using namespace std;



Config config;
char const *DEFAULT_OUTPUT_FILE_NAME = "chart.chart";


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
	// using, types
	using namespace chrono;

	typedef duration<double, ratio<1, 1000000>> duration_type;



	// preparation
	clever::Stopwatch<chrono::high_resolution_clock> watch;
	auto durs = new duration_type[repeatcount];
	duration_type result;
	float fbuf;


	// loop
	for(size_t i = 0; i < maxn; ++i) {

		// algorithm testing
		for(size_t i = 0; i < repeatcount; ++i) {
			watch.reset();
			data.update();

			// execute algorithm
			watch.start();
			alg(data);
			watch.stop();

			// writing
			durs[i] = duration_cast<
				duration_type
			>( watch.duration() );
		}


		// exclude minimum and maximum case
		auto maxdur = max_element( durs, durs+repeatcount );
		auto mindur = min_element( durs, durs+repeatcount );

		result = duration_type::zero();
		for(auto *b = durs, *e = durs+repeatcount; b != e; ++b) {
			if(b != maxdur && b != mindur) {
				result += *b;
			}
		}

		
		// write point to file
		fbuf = (float)data.getN();
		os.write( (char const *)&fbuf, sizeof fbuf );

		fbuf = (float)result.count();
		os.write( (char const *)&fbuf, sizeof fbuf );
		

		// to be continue...
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
			500u, 100u
		);
	}


	// it's all
	return 0;
}





// end
