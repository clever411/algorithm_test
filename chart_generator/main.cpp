#include <cmath>
#include <iostream>
#include <fstream>


using namespace std;



char const *DEFAULT_OUTPUT_FILE_NAME = "chart.chart";





float foo(float x)
{
	return cos(x);
}





// main
int main( int argc, char *argv[] )
{
	char const *outfilename;

	// characteristics
	constexpr static float const
		BEGIN = 0.0f, END = 15.0f, STEP = 0.1f
	;

	// set output file name
	if(argc < 2)
		outfilename = DEFAULT_OUTPUT_FILE_NAME;
	else
		outfilename = argv[1];

	// write chart to file
	{
		ofstream fout(outfilename);
		if(!fout) {
			cerr << "can't open file" << endl;
			return EXIT_FAILURE;
		}

		float fbuf;

		for(float b = BEGIN, e = END; b <= e; b += STEP) {
			fout.write( (char const *)&b, sizeof b );
			fbuf = foo(b);
			fout.write( (char const *)&fbuf, sizeof fbuf );
		}
	}
	

	return 0;
}





// 3nd
