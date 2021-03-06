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
unsigned int const FRAMERATE_LIMIT = 60u;
Color backgroundcolor;
Event event;


typedef std::vector< std::pair<float, float> > chart_type;
typedef std::shared_ptr<chart_type> chartptr_type;

ChartPrinter chart;
Font font;


Config config;





// init functions
void init_window()
{
	ContextSettings sets;
	//sets.antialiasingLevel = 8;
	window.create(dsmode, TITLE, Style::None, sets);
	window.setPosition({0, 0});
	window.setFramerateLimit(FRAMERATE_LIMIT);
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


void init_chart(char const *filename)
{
	config.readFile(filename);
	config.setAutoConvert(true);

	chart.setSize(conversion<float>(window.getSize()));
	Setting const &root = config.getRoot();
	
	float fbuf;
	unsigned int uibuf;
	string sbuf;



	// font
	lookup(root, "fontfilename", sbuf, string("font.ttf"));
	font.loadFromFile(sbuf);

	// padding and color
	{
		lookup(root, "padding", fbuf, 200.0f);
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

		// tags settings
		lookup(root, "tags.length", sets.length, 30.0f);
		lookup(root, "tags.thickness", sets.thickness, 5.0f);
		lookup(root, "tags.xinter", sets.xinter, -1.0f);
		lookup(root, "tags.yinter", sets.yinter, -1.0f);
		lookup(root, "tags.xyratio", sets.xyratio, -1.0f);
		lookup(root, "tags.tcolor", sbuf, string("black"));
		sets.tcolor = read_color(sbuf);

		lookup(root, "tags.pxinter", sets.pxinter, 80.0f);
		lookup(root, "tags.pyinter", sets.pyinter, 80.0f);

		// text for label settings
		sets.text.setFont(font);
		lookup(root, "tags.fontsize", sets.fontsize, -1);
		lookup(root, "tags.lcolor", sbuf, string("black"));
		sets.text.setFillColor(read_color(sbuf));
		lookup(root, "tags.xlabelfreq", sets.xlabelfreq, -1);
		lookup(root, "tags.ylabelfreq", sets.ylabelfreq, -1);

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
		// get charts node
		Setting const *chset;
		try {
			chset = &root.lookup("charts");
			if(!chset->isList())
				throw logic_error("name 'charts' not a list");
		}
		catch(SettingNotFoundException const &e) {
			throw logic_error("charts not found");
		}



		// read charts
		ChartSettings sets;
		chartptr_type achart(new chart_type());
		std::pair<float, float> point;
		float overlayprior;

		for(auto b = chset->begin(), e = chset->end(); b != e; ++b) {
			// read base characts
			lookup(*b, "thickness", sets.thickness, 3.0f);
			lookup(*b, "overlayprior", sets.overlayprior, overlayprior);
			lookup(*b, "color", sbuf, string("black"));
			sets.color = read_color(sbuf);


			// read chart's data
			lookup(*b, "datafilename", sbuf, string(""));
			if(sbuf.empty())
				continue;

			{
				ifstream fin(sbuf, ifstream::binary);
				if(!fin) {
					cerr << "error: file not found '" << sbuf << "'" << endl;
					continue;
				}
				while(
					fin.read( (char *)&point.first, sizeof point.first ) &&
					fin.read( (char *)&point.second, sizeof point.second )
				) {
					achart->push_back(point);
				}
			}


			// add chart
			chart.addChart( achart, sets );
			achart.reset(new chart_type());
			overlayprior += 1.0f;
		}

	}

	// aim
	{
		AimSettings sets;

		lookup(root, "aim.thickness", sets.thickness, 1.0f);
		lookup(root, "aim.color", sbuf, string("black"));
		sets.color = read_color(sbuf);

		chart.setAimSettings(sets);
	}

	// table
	{
		TableSettings sets;

		sets.text.setFont(font);
		lookup(root, "table.xpadding", sets.padding.x, 50.0f);
		lookup(root, "table.ypadding", sets.padding.y, 50.0f);
		lookup(root, "table.fontsize", uibuf, 20u);
		sets.text.setCharacterSize(uibuf);
		lookup(root, "table.color", sbuf, string("black"));
		sets.text.setFillColor(read_color(sbuf));

		chart.setTableSettings(sets);
	}


}





// main
int main( int argc, char *argv[] )
{
	// init
	init_window();

	if(argc < 2) {
		std::cerr << "error: empty argument list" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		init_chart(argv[1]);
	}
	catch(FileIOException const &e) {
		std::cerr << "error: can't read config file" << std::endl;
		return EXIT_FAILURE;
	}
	catch(ParseException const &e) {
		std::cerr << "error: parse error at line " << e.getLine() << std::endl;
		return EXIT_FAILURE;
	}
	catch(std::exception const &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	

	// main loop
	bool drawcross = false;
	bool drawtable = false;
	while(window.isOpen()) {
		// handle event
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
				case Keyboard::A:
					drawcross = !drawcross;
					break;
				case Keyboard::T:
					drawtable = !drawtable;
				default:
					break;
				}
			}
		}


		// update
		chart.update();

		// draw
		window.clear(backgroundcolor);
		if(drawcross)
			chart.drawAim(
				clever::conversion<float>(Mouse::getPosition(window)),
				window
			);
		window.draw(chart);
		if(drawtable)
			chart.drawTable(
				clever::conversion<float>(Mouse::getPosition(window)),
				window
			);
		window.display();
	}

	return 0;
}





// end
