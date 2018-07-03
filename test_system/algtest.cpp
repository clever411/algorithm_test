#include <chrono>
#include <iostream>
#include <libconfig.h++>

#include <clever/Stopwatch.hpp>

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
template<typename Algorithm, typename Data>
void alghorithm_test(Setting &root, Algorithm alg, Data data, size_t count = 100)
{
	using namespace chrono;

	// preparation
	Setting &cfgdata = root.add( "data", Setting::TypeArray );
	Setting *el;

	// loop
	clever::Stopwatch<chrono::high_resolution_clock> watch;
	for(size_t i = 0; i < count; ++i) {
		// algorithm testing
		for(size_t i = 0; i < 1000; ++i) {
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
		++data;
	}

}



class Data
{
public:
	Data &update()
	{
		return *this;
	}
	size_t getN() const
	{
		return n_;
	}
	Data &operator++()
	{
		++n_;
		return *this;
	}

private:
	size_t n_ = 0u;

};



int something(Data const &data)
{
	int result = 0;
	for(size_t i = 0; i < data.getN(); ++i) {
		for(size_t j = 0; j < data.getN(); ++j) {
			result += j;
		}
		result += i;
	}
	return result;
}





int main( int argc, char *argv[] )
{
	char const *outfilename;
	if(argc < 2) {
		outfilename = DEFAULT_OUTPUT_FILE_NAME;
	}
	else {
		outfilename = argv[1];
	}

	alghorithm_test(config.getRoot(), something, Data(), 150);
	config.writeFile(outfilename);

	return 0;
}





// end
