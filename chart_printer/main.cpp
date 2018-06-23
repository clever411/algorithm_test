#include <cmath>
#include <iostream>
#include <functional>

#include <clever/SFML/HelpFunctions.hpp>

#include "ChartPrinter.hpp"

using namespace clever;

using namespace sf;
using namespace std;


RenderWindow window;
VideoMode const dsmode = VideoMode::getDesktopMode();;
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


typedef shared_ptr<
	vector<pair<float, float>>
> chart_type;

chart_type generate_chart(
	float start, float finish, float step,
	function<float(float)> const &fun
)
{
	chart_type chart(new typename chart_type::element_type());
	
	for(; start < finish; start += step) {
		chart->push_back({start, fun(start)});
	}

	return chart;
}

int main( int argc, char *argv[] )
{
	init_window();
	
	ChartPrinter<float> chart;
	chart.setSize(
		conversion<float>(window.getSize()-Vector2u{100, 100})
	);
	chart.setAxisSettings( AxisSettings{Color::Black, 10.0f} );
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return sqrt(x);
			}
		),
		{ Color::Black, 2.0f, 10.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return pow(x, 0.75);
			}
		),
		{ Color::Blue, 4.0f, 11.0f }
	);
	chart.addChart(
		generate_chart(
			0.0f, 1920.0f, 1.0f, 
			[](float x)->float {
				return pow(x, 0.80);
			}
		),
		{ Color::Red, 1.0f, 0.0f }
	);

	chart.setPosition({50.0f, 50.0f});

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
