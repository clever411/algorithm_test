#include "ChartPrinter.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#include <clever/SFML/HelpFunctions.hpp>

#ifdef DEBUG
	#include "debug.hpp"
#endif

#include "Line.hpp"





// structures
ChartSettings const &ChartSettings::getDefault()
{
	static ChartSettings const singleton {
		sf::Color::Black, 2.0f, 0.0f
	};
	return singleton;
}

AxisSettings const &AxisSettings::getDefault()
{
	static AxisSettings const singleton {
		sf::Color::Black, 5.0f
	};
	return singleton;
}

TagSettings const &TagSettings::getDefault()
{
	static TagSettings const singleton {
		20.0f, 3.0f, // size
		10.0f, 10.0f, 1.0f, // intervals, ratio
		sf::Color::Black, // tag color
		50.0f, 50.0f, // intervals in pixels
		1, 1 // label frequence
	};
	return singleton;
}

GridSettings const &GridSettings::getDefault()
{
	static GridSettings const singleton {
		true, 1.0f, sf::Color::Black
	};
	return singleton;
}

AimSettings const &AimSettings::getDefault()
{
	static AimSettings const singleton {
		1.0f, sf::Color::Red
	};
	return singleton;
}

TableSettings const &TableSettings::getDefault()
{
	static TableSettings const singleton {
		{30.0f, 30.0f}
	};
	return singleton;
}





// using methods
void ChartPrinter::update()
{
	if(ischanged_)
		adjust_();
	return;
}

void ChartPrinter::draw(
	sf::RenderTarget &target,
	sf::RenderStates states
) const
{
	states.transform *= sf::Transformable::getTransform();
	target.draw(sprite_, states);
	return;
}

void ChartPrinter::drawAim(
	sf::Vector2f const &pixpoint,
	sf::RenderTarget &target,
	sf::RenderStates states
) const
{
	states.transform *= sf::Transformable::getTransform();

	clever::Line line;
	line.setColor(crset_.color);
	line.setThickness(crset_.thickness);

	// horizontal
	line.setPosition(
		{0.0f, pixpoint.y},
		{width_, pixpoint.y}
	);
	target.draw(line, states);

	// vertical
	line.setPosition(
		{pixpoint.x, 0.0f},
		{pixpoint.x, height_}
	);
	target.draw(line, states);

	return;
}

void ChartPrinter::drawTable(
	sf::Vector2f pixpoint,
	sf::RenderTarget &target,
	sf::RenderStates states
) const
{
	states.transform *= sf::Transformable::getTransform();
	char buf[32];
	pixpoint = pixelsToDescartes(pixpoint);
	std::snprintf(buf, sizeof buf, "%6g, %6g",
		float(int(100 * pixpoint.x))/100.0f,
		float(int(100 * pixpoint.y))/100.0f
	);
	tabset_.text.setString(buf);
	tabset_.text.setPosition({
		width_ - ( tabset_.text.getLocalBounds().width + tabset_.padding.x ),
		height_ - ( tabset_.text.getLocalBounds().height + tabset_.padding.y )
	});
	target.draw(tabset_.text, states);
	return;
}





// size
ChartPrinter &ChartPrinter::setSize(float width, float height)
{
	if(width_ == width && height_ == height)
		return *this;
	width_ = width;
	height_ = height;
	ischanged_ = true;
	return *this;
}
ChartPrinter &ChartPrinter::setSize(sf::Vector2f const &size)
{
	if(width_ == size.x && height_ == size.y)
		return *this;
	width_ = size.x;
	height_ = size.y;
	ischanged_ = true;
	return *this;
}

sf::Vector2f ChartPrinter::getSize() const
{
	return {width_, height_};
}



// padding
ChartPrinter &ChartPrinter::setPadding(float padding)
{
	if(padding_ == padding)
		return *this;
	padding_ = padding;
	ischanged_ = true;
	return *this;
}
float ChartPrinter::getPadding() const
{
	return padding_;
}





// charts
ChartPrinter &ChartPrinter::addChart(
	chartptr_type newchart,
	ChartSettings const &settings
)
{
	charts_.push_back({newchart, settings});
	ischanged_ = true;
	return *this;
}
ChartPrinter &ChartPrinter::removeChart(chartptr_type const &chart)
{
	for(auto b = charts_.begin(), e = charts_.end(); b != e; ++b) {
		if(b->first == chart) {
			charts_.erase(b);
			ischanged_ = true;
			break;
		}
	}
	return *this;
}
ChartPrinter &ChartPrinter::clearCharts()
{
	if(charts_.empty())
		return *this;
	charts_.clear();
	ischanged_ = true;
	return *this;
}



// axis settings
ChartPrinter &ChartPrinter::setAxisSettings(
	AxisSettings const &newaxis
)
{
	if(
		axis_.color == newaxis.color &&
		axis_.thickness == newaxis.thickness
	)
		return *this;
	axis_ = newaxis;
	ischanged_ = true;
	return *this;
}
AxisSettings const &ChartPrinter::getAxisSettings() const
{
	return axis_;
}



// tag settings
ChartPrinter &ChartPrinter::setTagSettings(
	TagSettings const &newtagset
)
{
	tagset_ = newtagset;
	ischanged_ = true;
	return *this;
}

TagSettings const &ChartPrinter::getTagSettings() const
{
	return tagset_;
}



// grid settings
ChartPrinter &ChartPrinter::setGridSettings(
	GridSettings const &gridset
)
{
	gridset_ = gridset;
	ischanged_ = true;
	return *this;
}
GridSettings const &ChartPrinter::getGridSettings() const
{
	return gridset_;
}



// cross settings
ChartPrinter &ChartPrinter::setAimSettings(
	AimSettings const &crset
)
{
	crset_ = crset;
	return *this;
}
AimSettings const &ChartPrinter::getAimSettings() const
{
	return crset_;
}



// table settings
ChartPrinter &ChartPrinter::setTableSettings(
	TableSettings const &tabset
)
{
	tabset_ = tabset;
	return *this;
}
TableSettings const &ChartPrinter::getTableSettings() const
{
	return tabset_;
}





// other 
sf::Vector2f ChartPrinter::descartesToPixels(sf::Vector2f const &point) const
{
	return sf::Vector2f {
		point.x*xk_ + axispoint_.x,
		- point.y*yk_ + axispoint_.y
	};
}

sf::Vector2f ChartPrinter::pixelsToDescartes(sf::Vector2f const &point) const
{
	return sf::Vector2f {
		(point.x - axispoint_.x)/xk_,
		-(point.y - axispoint_.y)/yk_
	};
}







// implement
void ChartPrinter::adjust_()
{
	// sorting for overlay
	std::sort(
		charts_.begin(), charts_.end(),
		[](
			std::pair<
				chartptr_type,
				ChartSettings
			> const &lhs,
			std::pair<
				chartptr_type,
				ChartSettings
			> const &rhs
		)->bool {
			return lhs.second.overlayprior > rhs.second.overlayprior;
		}
	);


	// calc&draw
	calculate_charts_characts_();
	calculate_xkyk_();
	calculate_font_size_and_frequency_();
	calculate_axis_point_();
	generate_tags_();

	draw_();


	// it's all
	ischanged_ = false;
	return;
}



// calculating
void ChartPrinter::calculate_charts_characts_()
{
	// null always appears
	xmin_ = 0.0f;
	float xmax = 0.0f;
	ymin_ = 0.0f;
	float ymax = 0.0f;

	// buffer for elements
	typename std::vector<
		std::pair<float, float>
	>::const_iterator
		buf
	;

	// search minimum, maximum x, y
	for(auto b = charts_.begin(), e = charts_.end(); b != e; ++b) {
		// minimum x
		buf = std::min_element(
			b->first->cbegin(),
			b->first->cend(),
			[](
				std::pair<float, float> const &lhs,
				std::pair<float, float> const &rhs
			)->bool {
				return lhs.first < rhs.first;
			}
		);

		if(buf != b->first->cend() && buf->first < xmin_)
			xmin_ = buf->first;

		// maximum x
		buf = std::max_element(
			b->first->cbegin(),
			b->first->cend(),
			[](
				std::pair<value_type, value_type> const &lhs,
				std::pair<value_type, value_type> const &rhs
			)->bool {
				return lhs.first < rhs.first;
			}
		);

		if(buf != b->first->cend() && buf->first > xmax)
			xmax = buf->first;

		// minimum y
		buf = std::min_element(
			b->first->cbegin(),
			b->first->cend(),
			[](
				std::pair<value_type, value_type> const &lhs,
				std::pair<value_type, value_type> const &rhs
			)->bool {
				return lhs.second < rhs.second;
			}
		);
		if(buf != b->first->cend() && buf->second < ymin_)
			ymin_ = buf->second;

		// maximum y
		buf = std::max_element(
			b->first->cbegin(),
			b->first->cend(),
			[](
				std::pair<value_type, value_type> const &lhs,
				std::pair<value_type, value_type> const &rhs
			)->bool {
				return lhs.second < rhs.second;
			}
		);
		if(buf != b->first->cend() && buf->second > ymax)
			ymax = buf->second;
	}

	// calculate length
	xl_ = xmax-xmin_;
	yl_ = ymax-ymin_;

	return;
}

void ChartPrinter::calculate_xkyk_()
{
	if(
		xl_ <= 0.0f || yl_ <= 0.0f ||
		width_ <= 0.0f || height_ <= 0.0f
	) {
		tagset_.xinter = 0.0f;
		tagset_.yinter = 0.0f;
		xk_ = 0.0f;
		yk_ = 0.0f;

		return;
	}
		
	// smart calculate
	float w = width_-2*padding_;
	float h = height_-2*padding_;
	if(tagset_.xinter <= 0.0f || tagset_.yinter <= 0.0f) {
		// rough calculate interval
		tagset_.xinter = xl_ * tagset_.pxinter / w;
		tagset_.yinter = yl_ * tagset_.pyinter / h;

		// unrough
		tagset_.xinter = make_beauty_(
			tagset_.xinter
		);
		tagset_.yinter = make_beauty_(
			tagset_.yinter
		);
		


		// xk, yk calculate
		xk_ = tagset_.pxinter / tagset_.xinter;
		yk_ = tagset_.pyinter / tagset_.yinter;

		if(xl_*xk_ > w) {
			yk_ *= (w/xl_) / xk_;
			xk_ = w/xl_;
		}
		if(yl_*yk_ > h) {
			xk_ *= (h/yl_) / yk_;
			yk_ = h/yl_;
		}
			

	}
	// accurate calculate
	else {
		xk_ = (width_-2.0f*padding_) / xl_;
		yk_ = (height_-2.0f*padding_) / yl_;

		if(tagset_.xyratio != 0.0f) {
			if(yk_ > xk_/tagset_.xyratio) {
				yk_ = xk_/tagset_.xyratio;
			}
			else {
				xk_ = yk_*tagset_.xyratio;
			}
		}
	}

	return;
}

void ChartPrinter::calculate_font_size_and_frequency_()
{
	// accurate font size
	if(tagset_.fontsize > 0) {
		tagset_.text.setCharacterSize( tagset_.fontsize );
		return;
	}


	// look for maximum size string
	{
		char max[32];
		int maxlength = 0;
		char buf[32];
		int buflength = 0;
		
		std::vector<float> longnum;
		longnum.push_back( make_beauty_(xmin_) );
		longnum.push_back( make_beauty_(xmin_+xl_) );
		longnum.push_back( make_beauty_(tagset_.xinter) );

		for(auto b = longnum.cbegin(), e = longnum.cend(); b != e; ++b) {
			buflength = std::snprintf(buf, (sizeof buf)-1, "%.8g", *b);
			if(buflength > maxlength) {
				std::memcpy(max, buf, buflength+1);
				maxlength = buflength;
			}
		}
		tagset_.text.setString(max);
	}


	// prepration
	tagset_.fontsize = 40;
	tagset_.text.setCharacterSize(tagset_.fontsize);
	float dis = tagset_.xinter * xk_;
	bool canreducefreq = tagset_.xlabelfreq <= 0;
	if(canreducefreq) {
		tagset_.xlabelfreq = 1;
	}

	// selection 
	while(
		(
			2.0f *
			(float)tagset_.text.getLocalBounds().width /
			(float)tagset_.xlabelfreq
		) > dis
	) {
		if(tagset_.fontsize <= 20 && canreducefreq) {
			if(tagset_.xlabelfreq == 1) {
				++tagset_.xlabelfreq;
				continue;
			}
			else if(tagset_.fontsize <= 15) {
				++tagset_.xlabelfreq;
				continue;
			}
		}
		else if(tagset_.fontsize <= 15) {
			break;
		}
		--tagset_.fontsize;
		tagset_.text.setCharacterSize(tagset_.fontsize);
	}

	return;
}

void ChartPrinter::calculate_axis_point_()
{
	if(xl_ == 0.0f || yl_ == 0.0f) {
		axispoint_.x = padding_;
		axispoint_.y = height_ - padding_;
	}
	else {
		axispoint_.x = -xmin_ * xk_ + padding_;
		axispoint_.y = height_ - (-ymin_ * yk_ + padding_);
	}
	
	return;
}

void ChartPrinter::generate_tags_()
{
	// clear old tags
	tags_.abscissa.clear();
	tags_.ordinate.clear();

	
	// preparation
	if(
		xl_ == 0 || yl_ == 0 ||
		tagset_.xinter == 0 || tagset_.yinter == 0
	) {
		return;
	}
	double start;


	// generate abscissa tags
	start = int(
		pixelsToDescartes({0.0f, 0.0f}).x /
		tagset_.xinter
	) * double(tagset_.xinter);
	while(start < pixelsToDescartes({width_, 0.0f}).x) {
		tags_.abscissa.push_back(start);
		start += tagset_.xinter;
	}


	// generate ordinate tags
	start = int(
		pixelsToDescartes({0.0f, height_}).y /
		tagset_.yinter
	) * double(tagset_.yinter);
	while(start < pixelsToDescartes({0.0f, 0.0f}).y) {
		tags_.ordinate.push_back(start);
		start += tagset_.yinter;
	}


	// it's all
	return;
}



// drawing
void ChartPrinter::draw_()
{
	// preparation render texture
	rtexture_.create(width_, height_);
	rtexture_.clear(sf::Color::Transparent);

	// draw
	if(gridset_.enable)
		draw_grid_();
	draw_axis_();
	draw_tags_();
	draw_charts_();

	rtexture_.display();

	// adjust sprite
	sprite_.setTexture(rtexture_.getTexture(), true);

	return;
}

void ChartPrinter::draw_grid_()
{
	// create line and buffer for calculation
	clever::Line line;
	line.setColor(gridset_.color);
	line.setThickness(gridset_.thickness);
	sf::Vector2f buf;

	// horizontal lines
	for(auto b = tags_.ordinate.cbegin(), e = tags_.ordinate.cend(); b != e; ++b) {
		buf = descartesToPixels({0.0f, *b});
		line.setPosition(
			{ 0.0f, buf.y },
			{ width_, buf.y }
		);
		rtexture_.draw(line);
	}

	// vertical lines
	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		buf = descartesToPixels({*b, 0.0f});
		line.setPosition(
			{ buf.x, 0.0f },
			{ buf.x, height_ }
		);
		rtexture_.draw(line);
	}


	return;
}

void ChartPrinter::draw_axis_()
{
	// create line
	clever::Line line;
	line.setThickness(axis_.thickness);
	line.setColor(axis_.color);


	// abscissa
	line.setPosition(
		{0.0f, axispoint_.y},
		{width_, axispoint_.y}
	);
	rtexture_.draw(line);

	// ordinate
	line.setPosition(
		{axispoint_.x, 0.0f},
		{axispoint_.x, height_}
	);
	rtexture_.draw(line);

	return;
}

void ChartPrinter::draw_tags_()
{
	// create Rectangle
	sf::RectangleShape rect;
	rect.setFillColor(tagset_.tcolor);


	// for abscissa
	rect.setSize({tagset_.thickness, tagset_.length});
	rect.setOrigin(rect.getSize()/2.0f);

	if(tagset_.xlabelfreq == 0u)
		tagset_.xlabelfreq = 1u;
	if(tagset_.ylabelfreq == 0u)
		tagset_.ylabelfreq = 1u;
	auto zero = std::min_element(
		tags_.abscissa.cbegin(), tags_.abscissa.cend(), 
		[](float lhs, float rhs)->bool {
			return std::fabs(lhs) < std::fabs(rhs);
		}
	);
	if(zero == tags_.abscissa.cend())
		throw "zero not found(abscissa)";

	int n;
	int dis = zero-tags_.abscissa.cbegin();
	n = dis % tagset_.xlabelfreq;

	char buf[24];
	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		// tag
		rect.setPosition(
			descartesToPixels({*b, 0})
		);
		rtexture_.draw(rect);

		// label
		if(b == zero) {
			n = tagset_.xlabelfreq-1;
			continue;
		}
		if(n != 0) {
			--n;
			continue;
		}

		std::snprintf(buf, 24u, "%.8g", *b);
		tagset_.text.setString(buf);
		tagset_.text.setPosition(
			descartesToPixels({*b, 0}) +
			sf::Vector2f{
				-0.5f*tagset_.text.getGlobalBounds().width,
				axis_.thickness + tagset_.length*0.5f
			}
		);
		rtexture_.draw(tagset_.text);
		n = tagset_.xlabelfreq-1;
	}


	// for ordinate
	rect.setSize({tagset_.length, tagset_.thickness});
	rect.setOrigin(rect.getSize()/2.0f);

	zero = std::min_element(
		tags_.ordinate.cbegin(), tags_.ordinate.cend(),
		[](float lhs, float rhs)->bool {
			return std::fabs(lhs) < std::fabs(rhs);
		}
	);
	if(zero == tags_.ordinate.cend())
		throw "zero not found(ordinate)";
	dis = zero - tags_.ordinate.cbegin();
	n = dis % tagset_.ylabelfreq;

	for(auto b = tags_.ordinate.cbegin(), e = tags_.ordinate.cend(); b != e; ++b) {
		// tag
		rect.setPosition(
			descartesToPixels({0, *b})
		);
		rtexture_.draw(rect);

		// label
		if(b == zero) {
			n = tagset_.ylabelfreq-1;
			continue;
		}
		if(n != 0) {
			--n;
			continue;
		}
		std::snprintf(buf, 24u, "%.8g", *b);
		tagset_.text.setString(buf);
		tagset_.text.setPosition(
			descartesToPixels({0, *b}) +
			sf::Vector2f{
				-(
					1.0f*tagset_.text.getLocalBounds().width +
					tagset_.length*0.5f +
					axis_.thickness
				),
				-0.5f*tagset_.text.getLocalBounds().height
			}
		);
		rtexture_.draw(tagset_.text);
		n = tagset_.ylabelfreq-1;
	}

	return;
}

void ChartPrinter::draw_charts_()
{
	clever::Line line;

	for(auto const &i : charts_) {
		if(i.first->size() < 2)
			continue;

		line.setColor(i.second.color);
		line.setThickness(i.second.thickness);

		sf::Vector2f begin, end;
		for(auto b = i.first->begin()+1, e = i.first->end(); b != e; ++b) {

			line.setPosition(
				descartesToPixels({
					std::prev(b)->first, std::prev(b)->second
				}),
				descartesToPixels({b->first, b->second})
			);

			rtexture_.draw(line);
		}
	}

	return;
}


float ChartPrinter::make_beauty_(float n)
{
	constexpr static float const BEAUTY_NUMS[] = {
		0.0f, 0.05, 0.1f, 0.25f, 0.5f,

		1.0f, 1.5f, 2.0f, 2.5f,
		3.0f, 4.0f, 5.0f, 6.0f, 8.0f,

		10.0f, 15.0f, 20.0f, 25.0f,
		30.0f, 40.0f, 50.0f, 60.0f, 80.0f,

		100.0f, 150.0f, 200.0f, 250.0f,
		300.0f, 400.0f, 500.0f, 600.0f, 800.0f,

		1000.0f, 1500.0f, 2000.0f, 2500.0f,
		3000.0f, 4000.0f, 5000.0f, 6000.0f, 8000.0f,

		10000.0f, 15000.0f, 20000.0f, 25000.0f,
		30000.0f, 40000.0f, 50000.0f, 60000.0f, 80000.0f,

		100000.0f, 150000.0f, 200000.0f, 250000.0f,
		300000.0f, 400000.0f, 500000.0f, 600000.0f, 800000.0f,

		1000000.0f
	};
	constexpr static unsigned int const BEAUTY_NUMS_SIZE = 60;

	bool isnegative = n < 0.0f;
	if(isnegative)
		n = -n;

	auto bigger = std::find_if(
		BEAUTY_NUMS, BEAUTY_NUMS+BEAUTY_NUMS_SIZE,
		[&n](float arg)->bool {
			return n < arg;
		}
	);

	if(bigger == BEAUTY_NUMS+BEAUTY_NUMS_SIZE)
		return isnegative ?
			-BEAUTY_NUMS[BEAUTY_NUMS_SIZE-1] :
			BEAUTY_NUMS[BEAUTY_NUMS_SIZE-1]
		;
	if(*bigger - n < n - *(bigger-1)) {
		return isnegative ? 
			-*bigger :
			*bigger
		;
	}
	else {
		return isnegative ?
			-*(bigger-1) : 
			*(bigger-1)
		;
	}

}







// end
