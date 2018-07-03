#include <cmath>
#include <iostream>

#include <libconfig.h++>

using namespace libconfig;
using namespace std;



Config config;
char const *DEFAULT_OUTPUT_FILE_NAME = "chart.txt";



float foo(float x)
{
	return x/1920.0f*1080.0f;
}





// main
int main( int argc, char *argv[] )
{
	constexpr static float const
		BEGIN = 0.0f, END = 1920.0f, STEP = 1.0f
	;

	char const *outfilename;
	if(argc < 2) {
		outfilename = DEFAULT_OUTPUT_FILE_NAME;
	}
	else {
		outfilename = argv[1];
	}

	Setting &root = config.getRoot();
	Setting &data = root.add( "data", Setting::TypeArray );

	Setting *el = nullptr;
	for(float b = BEGIN, e = END; b < e; b += STEP) {
		el = &data.add(Setting::TypeFloat);
		*el = b;
		el = &data.add(Setting::TypeFloat);
		*el = foo(b);
	}
	
	config.writeFile(outfilename);

	return 0;
}
