#include <cmath>
#include <iostream>

#include <libconfig.h++>

using namespace libconfig;
using namespace std;



Config config;
char const *DEFAULT_OUTPUT_FILE_NAME = "chart.txt";



float foo(float x)
{
	return cos(x)+1.0f;
}





// main
int main( int argc, char *argv[] )
{
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
	for(float b = 0.0f, e = 10.0f; b < e; b += 0.1f) {
		el = &data.add(Setting::TypeFloat);
		*el = b;
		el = &data.add(Setting::TypeFloat);
		*el = foo(b);
	}
	
	config.writeFile(outfilename);

	return 0;
}
