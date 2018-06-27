#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>

#include <clever/SFML/HelpFunctions.hpp>
#include <clever/IostreamFunction.hpp>

#include "ChartPrinter.hpp"

using namespace clever;

using namespace sf;
using namespace std;


RenderWindow window;
VideoMode const dsmode = VideoMode::getDesktopMode();
string const TITLE = "Chart Printer";
Event event;

ChartPrinter<float> chart;

void init_window()
{
	ContextSettings sets;
	sets.antialiasingLevel = 8;
	window.create(dsmode, TITLE, Style::None, sets);
	window.setPosition({0, 0});
	window.setVerticalSyncEnabled(true);
	return;
}


typedef std::vector< std::pair<float, float> > chart_type;
typedef std::shared_ptr<chart_type> chartptr_type;

chartptr_type generate_chart(
	float start, float finish, float step,
	function<float(float)> const &fun
)
{
	chartptr_type achart(new chart_type);
	
	for(; start < finish; start += step) {
		achart->push_back({start, fun(start)});
	}

	return achart;
}


void init_chart() 
{
	chart.setSize(
		conversion<float>(window.getSize())
	);
	chart.setAxisSettings({Color::Black, 10.0f});
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return sqrt(x);
			}
		),
		{ Color::Magenta, 2.0f, 10.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return pow(x, 0.75);
			}
		),
		{ Color::Blue, 3.0f, 11.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return pow(x, 0.4);
			}
		),
		{ Color::Red, 1.0f, 0.0f }
	);

	chart.calculate_tags_byinterval(100.0f, 100.0f);

	return;
}

void init_chart2(string filename)
{
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
		chart.update();

		window.clear(Color::White);
		window.draw(chart);
		window.display();
	}

	return 0;
}
