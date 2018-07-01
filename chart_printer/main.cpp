#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>
#include <libconfig.h++>

#ifdef DEBUG
	#include "debug.hpp"
#endif

#include "ChartPrinter.hpp"


using namespace clever;
using namespace libconfig;
using namespace sf;
using namespace std;





// variables and constants
RenderWindow window;
VideoMode const dsmode = VideoMode::getDesktopMode();
string const TITLE = "Chart Printer";
Color backgroundcolor;
Event event;


typedef std::vector< std::pair<float, float> > chart_type;
typedef std::shared_ptr<chart_type> chartptr_type;

ChartPrinter chart;


Config config;
string const CONFIG_FILE_NAME = "config";





// init functions
void init_window()
{
	ContextSettings sets;
	sets.antialiasingLevel = 8;
	window.create(dsmode, TITLE, Style::None, sets);
	window.setPosition({0, 0});
	window.setVerticalSyncEnabled(true);
	return;
}



template<typename T>
inline void lookup(
	Setting const &set, std::string const &name,
	T &value, T const &defvalue
)
{
	try {
		value = set.lookup(name);
	}
	catch(SettingNotFoundException const &e) {
		value = defvalue;
	}
	return;
}

template<>
inline void lookup<string>(
	Setting const &set, std::string const &name,
	string &value, string const &defvalue
)
{
	try {
		value = set.lookup(name).c_str();
	}
	catch(SettingNotFoundException const &e) {
		value = defvalue;
	}
	return;
}

void strtolower( std::string &s )
{
	for(auto b = s.begin(), e = s.end(); b != e; ++b) {
		*b = tolower(*b);
	}
	return;
}

char hextodec(char ch)
{
	if(isdigit(ch)) {
		char charr[2] = { ch, 0 };
		return atoi(charr);
	}
	switch(ch) {
	case 'a':
		return 10;
	case 'b':
		return 11;
	case 'c':
		return 12;
	case 'd':
		return 13;
	case 'e':
		return 14;
	case 'f':
		return 15;
	default:
		return 0;
	}
}

Color read_color( std::string str )
{
	strtolower(str);

	if(str.empty() || str == "black")
		return Color::Black;
	if(str == "blue")
		return Color::Blue;
	if(str == "green")
		return Color::Green;
	if(str == "cyan")
		return Color::Cyan;
	if(str == "magenta")
		return Color::Magenta;
	if(str == "red")
		return Color::Red;
	if(str == "yellow")
		return Color::Yellow;
	if(str == "transparent")
		return Color::Transparent;
	if(str == "white")
		return Color::White;
	
	Color color = Color::Black;
	unsigned char *refs[] = {
		&color.r, &color.g, &color.b, &color.a
	};

	for(
		size_t strit = 0, rit = 0;
		strit < str.size() && rit < 4;
		++strit, ++rit
	) {
		*refs[rit] = hextodec(str[strit])*16;
		if(++strit == str.size())
			break;
		*refs[rit] += hextodec(str[strit]);
	}

	return color;
}


void init_chart()
{
	config.readFile(CONFIG_FILE_NAME.c_str());
	config.setAutoConvert(true);

	chart.setSize(conversion<float>(window.getSize()));
	Setting const &root = config.getRoot();
	
	float fbuf;
	string sbuf;

	// padding and color
	{
		lookup(root, "padding", fbuf, 100.0f);
		chart.setPadding(fbuf);

		lookup(root, "backgroundcolor", sbuf, string("white"));
		backgroundcolor = read_color(sbuf);
	}

	// axis
	{
		AxisSettings sets;


		lookup(root, "axis.color", sbuf, string("black"));
		sets.color = read_color(sbuf);
		lookup(root, "axis.thickness", sets.thickness, 5.0f);

		chart.setAxisSettings(sets);
	}

	// tags
	{
		TagSettings sets;

		lookup(root, "tags.length", sets.length, 30.0f);
		lookup(root, "tags.thickness", sets.thickness, 3.0f);
		lookup(root, "tags.xinter", sets.xinter, 10.0f);
		lookup(root, "tags.yinter", sets.yinter, 10.0f);
		lookup(root, "tags.xyratio", sets.xyratio, 1.0f);

		lookup(root, "tags.tcolor", sbuf, string("black"));
		sets.tcolor = read_color(sbuf);
		lookup(root, "tags.lcolor", sbuf, string("black"));
		sets.lcolor = read_color(sbuf);

		chart.setTagSettings(sets);
	}

	// grid
	{
		GridSettings sets;

		lookup(root, "grid.enable", sets.enable, true);
		lookup(root, "grid.thickness", sets.thickness, 2.0f);
		lookup(root, "grid.color", sbuf, string("black"));
		sets.color = read_color(sbuf);

		chart.setGridSettings(sets);
	}

	// charts
	{
		Setting const *chset;
		try {
			chset = &root.lookup("charts");
			if(!chset->isList())
				throw logic_error("name 'charts' not a list");
		}
		catch(SettingNotFoundException const &e) {
			throw logic_error("charts not found");
		}


		ChartSettings sets;
		chartptr_type achart(new chart_type());
		std::pair<float, float> point;
		Setting const *data;

		for(auto b = chset->begin(), e = chset->end(); b != e; ++b) {
			lookup(*b, "thickness", sets.thickness, 2.0f);
			lookup(*b, "overlayprior", sets.overlayprior, 0.0f);
			lookup(*b, "color", sbuf, string("black"));
			sets.color = read_color(sbuf);

			try {
				data = &b->lookup("data");
			}
			catch(SettingNotFoundException const &e) {
				continue;
			}
			if(!data->isArray())
				continue;

			try {
				achart->reserve( data->getLength()/2 );
				for(auto b = data->begin(), e = data->end(); b != e; ++b) {
					point.first = *b;
					if(++b == e)
						break;
					point.second = *b;
					achart->push_back(point);
				}
			}
			catch(SettingTypeException const &e) {
				continue;
			}

			chart.addChart( achart, sets );
			achart.reset(new chart_type());

		}

	}


}




// main
int main( int argc, char *argv[] )
{
	// init
	init_window();
	try {
		init_chart();
	}
	catch(FileIOException const &e) {
		std::cerr << "error: can't read config file" << std::endl;
		return EXIT_FAILURE;
	}
	catch(ParseException const &e) {
		std::cerr << "error: parse error at line " << e.getLine() << std::endl;
		return EXIT_FAILURE;
	}
	

	// main loop
	while(window.isOpen()) {
		if(window.pollEvent(event)) {
			if(event.type == Event::KeyPressed) {
				switch(event.key.code) {
				case Keyboard::I:
					cout << chart.pixelsToDescartes(
						conversion<float>(Mouse::getPosition(window))
					) << endl;
					break;
				case Keyboard::C: case Keyboard::Escape:
					window.close();
					break;
				default:
					break;
				}
			}
		}
		// update
		chart.update();

		// draw
		window.clear(backgroundcolor);
		window.draw(chart);
		window.display();
	}

	return 0;
}





// end
