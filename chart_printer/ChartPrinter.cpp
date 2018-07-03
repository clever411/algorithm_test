#include "ChartPrinter.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

#include <clever/SFML/HelpFunctions.hpp>

#ifdef DEBUG
	#include "debug.hpp"
#endif

#include "Line.hpp"





// structures
ChartSettings const &ChartSettings::getDefault()
{
	static ChartSettings const singelton {
		sf::Color::Black, 2.0f, 0.0f
	};
	return singelton;
}

AxisSettings const &AxisSettings::getDefault()
{
	static AxisSettings const singelton {
		sf::Color::Black, 5.0f
	};
	return singelton;
}

TagSettings const &TagSettings::getDefault()
{
	static TagSettings const singelton {
		20.0f, 3.0f, 10.0f, 10.0f, 1.0f,
		sf::Color::Black,
		1, 1
	};
	return singelton;
}

GridSettings const &GridSettings::getDefault()
{
	static GridSettings const singelton {
		true, 1.0f, sf::Color::Black
	};
	return singelton;
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
ChartPrinter &ChartPrinter::setSize(sf::Vector2f size)
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
	calculate_size_();
	generate_tags_();
	draw_();

	// it's all
	ischanged_ = false;
	return;
}


void ChartPrinter::calculate_size_()
{
	calculate_charts_characts_();
	calculate_xkyk_();

	// calculate axispoint
	if(xl_ == 0.0f || yl_ == 0.0f) {
		axispoint_.x = padding_;
		axispoint_.y = height_ - padding_;
	}
	else {
		axispoint_.x = -xmin_ / xl_ * (width_-2.0f*padding_) + padding_;
		axispoint_.y = height_ -
			(-ymin_ / yl_ * (height_-2.0f*padding_) + padding_);
	}

	return;
}

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
	if(xl_ == 0.0f || yl_ == 0.0f) {
		xk_ = yk_ = 0.0f;
		return;
	}

	xk_ = (width_-2.0f*padding_) / xl_;
	yk_ = (height_-2.0f*padding_) / yl_;

	if(tagset_.xyratio != 0.0f) {
		if(tagset_.xyratio >= 1.0f)
			yk_ = xk_/tagset_.xyratio;
		else
			xk_ = yk_*tagset_.xyratio;
	}

	return;
}

void ChartPrinter::generate_tags_()
{
	// clear old tags
	tags_.abscissa.clear();
	tags_.ordinate.clear();

	
	// preparation
	if(xl_ == 0 || yl_ == 0 || tagset_.xinter == 0 || tagset_.yinter == 0)
		return;

	float start;


	// generate abscissa tags
	start = 0.0f;
	while(start > pixelsToDescartes({0.0f, 0.0f}).x) {
		start -= tagset_.xinter;
	}
	while(start < pixelsToDescartes({width_, 0.0f}).x) {
		tags_.abscissa.push_back(start);
		start += tagset_.xinter;
	}


	// generate ordinate tags
	start = 0.0f;
	while(start > pixelsToDescartes({0.0f, height_}).y) {
		start -= tagset_.yinter;
	}
	while(start < pixelsToDescartes({0.0f, 0.0f}).y) {
		tags_.ordinate.push_back(start);
		start += tagset_.yinter;
	}

	// it's all

	return;
}


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
	auto zero = std::find_if(
		tags_.abscissa.cbegin(), tags_.abscissa.cend(), 
		[](float point)->bool {
			return point == 0.0f;
		}
	);
	if(zero == tags_.abscissa.cend())
		throw "zero not found(abscissa)";

	int n;
	int dis = zero-tags_.abscissa.cbegin();
	n = dis % tagset_.xlabelfreq;

	char buf[16];
	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		// tag
		rect.setPosition(
			descartesToPixels({*b, 0})
		);
		rtexture_.draw(rect);

		// label
		if(*b == 0.0f) {
			n = tagset_.xlabelfreq-1;
			continue;
		}
		if(n != 0) {
			--n;
			continue;
		}

		std::snprintf(buf, 16u, "%.8g", *b);
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

	zero = std::find_if(
		tags_.ordinate.cbegin(), tags_.ordinate.cend(),
		[](float point)->bool {
			return point == 0.0f;
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
		if(*b == 0.0f) {
			n = tagset_.ylabelfreq-1;
			continue;
		}
		if(n != 0) {
			--n;
			continue;
		}
		std::snprintf(buf, 16u, "%.8g", *b);
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




ChartPrinter &ChartPrinter::correctSize(float xy)
{
	return *this;
}

// end
