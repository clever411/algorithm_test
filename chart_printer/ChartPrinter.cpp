#include "ChartPrinter.hpp"

#include <algorithm>
#include <cmath>
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
		20.0f, 3.0f, sf::Color::Black
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

void ChartPrinter::calculate_tags_bycount(size_t xcount, size_t ycount)
{
	if(ischanged_) {
		calculate_size_();
		ischanged_ = false;
	}

	calculate_tags_byinterval(
		xl_/(float)xcount, yl_/(float)ycount
	);
	return;
}

void ChartPrinter::calculate_tags_byinterval(float xinter, float yinter)
{
	tags_.abscissa.clear();
	tags_.ordinate.clear();

	
	if(ischanged_)
		calculate_size_();

	if(xl_ == 0 || yl_ == 0 || xinter == 0 || yinter == 0)
		return;

	float start = 0.0f;
	while(start > xmin_) {
		start -= xinter;
	}
	while(start <= xmin_+xl_) {
		tags_.abscissa.push_back(start);
		start += xinter;
	}

	start = 0.0f;
	while(start > ymin_) {
		start -= yinter;
	}
	while(start <= ymin_+xl_) {
		tags_.ordinate.push_back(start);
		start += yinter;
	}

	ischanged_ = true;

	return;
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
	draw_();

	// it's all
	ischanged_ = false;
	return;
}


void ChartPrinter::calculate_size_()
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

	// calculate axispoint
	if(xl_ == 0.0f || yl_ == 0.0f) {
		axispoint_.x = padding_;
		axispoint_.y = height_ - padding_;
		xk_ = yk_ = 0.0f;
	}
	else {
		xk_ = (width_-2.0f*padding_) / xl_;
		yk_ = (height_-2.0f*padding_) / yl_;

		axispoint_.x = -xmin_ / xl_ * (width_-2.0f*padding_) + padding_;
		axispoint_.y = height_ -
			(-ymin_ / yl_ * (height_-2.0f*padding_) + padding_);
	}

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
	sprite_.setTexture(rtexture_.getTexture());
	sprite_.setTextureRect(
		sf::IntRect(0, 0, int(width_), int(height_))
	);

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
		buf = descartes_to_pixels({0.0f, *b});
		line.setPosition(
			{ 0.0f, buf.y },
			{ width_, buf.y }
		);
		rtexture_.draw(line);
	}

	// vertical lines
	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		buf = descartes_to_pixels({*b, 0.0f});
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
	rect.setFillColor(tagset_.color);


	// for abscissa
	rect.setSize({tagset_.thickness, tagset_.length});
	rect.setOrigin(rect.getSize()/2.0f);

	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		rect.setPosition(
			descartes_to_pixels({*b, 0})
		);
		rtexture_.draw(rect);
	}


	// for ordinate
	rect.setSize({tagset_.length, tagset_.thickness});
	rect.setOrigin(rect.getSize()/2.0f);

	for(auto b = tags_.ordinate.cbegin(), e = tags_.ordinate.cend(); b != e; ++b) {

		rect.setPosition(
			descartes_to_pixels({0, *b})
		);
		rtexture_.draw(rect);
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
				descartes_to_pixels({
					std::prev(b)->first, std::prev(b)->second
				}),
				descartes_to_pixels({b->first, b->second})
			);

			rtexture_.draw(line);
		}
	}

	return;
}


sf::Vector2f ChartPrinter::descartes_to_pixels(sf::Vector2f const &point)
{
	return sf::Vector2f{
		point.x*xk_ + axispoint_.x,
		- point.y*yk_ + axispoint_.y
	};
}





// end
