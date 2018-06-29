#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>

#ifdef DEBUG
	#include "debug.hpp"
#endif

#include "ChartPrinter.hpp"


using namespace clever;
using namespace sf;
using namespace std;





// window and other about SFML
RenderWindow window;
VideoMode const dsmode = VideoMode::getDesktopMode();
string const TITLE = "Chart Printer";
Event event;


void init_window()
{
	ContextSettings sets;
	sets.antialiasingLevel = 8;
	window.create(dsmode, TITLE, Style::None, sets);
	window.setPosition({0, 0});
	window.setVerticalSyncEnabled(true);
	return;
}





// charts and other about it
typedef std::vector< std::pair<float, float> > chart_type;
typedef std::shared_ptr<chart_type> chartptr_type;

ChartPrinter chart;

chartptr_type generate_chart(
	float start, float finish, float step,
	function<float(float)> const &fun
)
{
	chartptr_type achart(new chart_type);
	achart->reserve( int( (finish-start)/step ) + 2 );
	for(; start < finish; start += step) {
		achart->push_back({start, fun(start)});
	}

	return achart;
}

void init_chart() 
{
	constexpr static float const MAX_X = 500.0f;
	constexpr static float const STEP = 0.2f;
	 
	chart.setSize(
		conversion<float>(window.getSize())
	);
	chart.setPadding(100.0f);
	chart.setAxisSettings(AxisSettings{Color::Blue, 10.0f});
	chart.addChart(
		generate_chart(
			0.0f, MAX_X, STEP, 
			[](float x)->float {
				return pow(x, 0.75);
			}
		),
		{ Color::Blue, 3.0f, 3.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, MAX_X, STEP, 
			[](float x)->float {
				return sqrt(x);
			}
		),
		{ Color::Red, 3.0f, 2.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, MAX_X, STEP, 
			[](float x)->float {
				return pow(x, 0.4);
			}
		),
		{ Color::Black, 3.0f, 1.0f }
	);

	chart.calculate_tags_byinterval(10.0f, 10.0f);

	return;
}

void init_chart2(string filename)
{
	chart.setAxisSettings({Color::Blue, 10.0f});
	chartptr_type achart(new chart_type());
	{
		fstream fout(filename);
		if(!fout.is_open()) {
			std::cerr << "can't open file" << std::endl;
			return;
		}

		achart->reserve(128);
		float fbuf, sbuf;
		while(true) {
			fout >> fbuf >> sbuf;
			if(!fout)
				break;
			achart->push_back({fbuf, sbuf});
		}
		achart->shrink_to_fit();
	}
	cout << *achart << endl;
	chart.addChart(
		achart,
		{ Color::Black, 5.0f, 1.0f }
	);

	chart.setSize(conversion<float>(window.getSize()));
	chart.calculate_tags_byinterval(1.0f, 1.0f);


	return;
}




// main
int main( int argc, char *argv[] )
{
	init_window();
	init_chart();
	

	while(window.isOpen()) {
		if(window.pollEvent(event)) {
			if(event.type == Event::KeyPressed) {
				window.close();
			}
		}
		// update
		chart.update();

		// draw
		window.clear(Color::White);
		window.draw(chart);
		window.display();
	}

	return 0;
}
