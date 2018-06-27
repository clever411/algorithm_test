#include "ChartPrinter.hpp"

#include <algorithm>
#include <cmath>

#include <clever/SFML/HelpFunctions.hpp>
#include <clever/IostreamFunction.hpp>

#include "Line.hpp"


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



ChartPrinter &ChartPrinter::setAxisSettings(
	AxisSettings const &newaxis
)
{
	axis_ = newaxis;
	ischanged_ = true;
	return *this;
}
AxisSettings const &ChartPrinter::getAxisSettings() const
{
	return axis_;
}


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

ChartPrinter &ChartPrinter::setGridSettings(
	GridSettings const &gridset
)
{
	gridset_ = gridset;
	ischanged_ = true;
	return *this;
}
GridSettings ChartPrinter::getGridSettings() const
{
	return gridset_;
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



ChartPrinter &ChartPrinter::setSize(float width, float height)
{
	width_ = width;
	height_ = height;
	ischanged_ = true;
	return *this;
}
ChartPrinter &ChartPrinter::setSize(sf::Vector2f size)
{
	width_ = size.x;
	height_ = size.y;
	ischanged_ = true;
	return *this;
}

sf::Vector2f ChartPrinter::getSize() const
{
	return {width_, height_};
}

ChartPrinter &ChartPrinter::setPadding(float padding)
{
	padding_ = padding;
	ischanged_ = true;
	return *this;
}
float ChartPrinter::getPadding() const
{
	return padding_;
}


void ChartPrinter::adjust_()
{
	// for overlay
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

	calculate_size_();

	draw_();

	ischanged_ = false;
	return;
}

void ChartPrinter::calculate_size_()
{
	xmin_ = 0;
	float xmax = 0;
	ymin_ = 0;
	float ymax = 0;

	typename std::vector<
		std::pair<float, float>
	>::const_iterator
		buf
	;

	for(auto b = charts_.begin(), e = charts_.end(); b != e; ++b) {
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

	xl_ = xmax-xmin_;
	yl_ = ymax-ymin_;

	if(xl_ == 0)
		axispoint_.x = padding_;
	else
		axispoint_.x = float(-xmin_) / xl_ * (width_-2*padding_) + padding_;
	if(yl_ == 0)
		axispoint_.y = 0.0f;
	else
		axispoint_.y = float(-ymin_) / yl_ * (height_-2*padding_) + padding_;

	return;
}

void ChartPrinter::draw_()
{
	rtexture_.create(width_, height_);
	rtexture_.clear(sf::Color::Transparent);

	draw_axis_();
	if(gridset_.enable)
		draw_grid_();
	draw_tags_();
	draw_charts_();

	sprite_.setTexture(rtexture_.getTexture());
	sprite_.setTextureRect(
		sf::IntRect(0, 0, int(width_), int(height_))
	);
	return;
}
void ChartPrinter::draw_axis_()
{
	clever::Line line;
	line.setThickness(axis_.thickness);
	line.setColor(axis_.color);

	line.setPosition(
		{0.0f, axispoint_.y},
		{width_, axispoint_.y}
	);
	rtexture_.draw(line);

	line.setPosition(
		{axispoint_.x, 0.0f},
		{axispoint_.x, height_}
	);
	rtexture_.draw(line);

	return;
}
void ChartPrinter::draw_tags_()
{
	sf::RectangleShape rect;
	rect.setFillColor(tagset_.color);

	rect.setSize({tagset_.thickness, tagset_.length});
	rect.setOrigin(rect.getSize()/2.0f);

	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		rect.setPosition({
			*b/xl_*width_ + axispoint_.x,
			axispoint_.y
		});
		rtexture_.draw(rect);
	}

	rect.setSize({tagset_.length, tagset_.thickness});
	rect.setOrigin(rect.getSize()/2.0f);

	for(auto b = tags_.ordinate.cbegin(), e = tags_.ordinate.cend(); b != e; ++b) {
		rect.setPosition({
			axispoint_.x,
			*b/yl_*height_ + axispoint_.y
		});
		rtexture_.draw(rect);
	}

	return;
}
void ChartPrinter::draw_grid_()
{
	sf::RectangleShape rect;
	rect.setFillColor(gridset_.color);

	rect.setSize({width_, gridset_.thickness});
	rect.setOrigin({0.0f, rect.getSize().y/2.0f});

	for(auto b = tags_.ordinate.cbegin(), e = tags_.ordinate.cend(); b != e; ++b) {
		rect.setPosition({
			0.0f, *b/yl_*height_ + axispoint_.y
		});
		rtexture_.draw(rect);
	}

	rect.setSize({gridset_.thickness, height_});
	rect.setOrigin({rect.getSize().x/2.0f, 0.0f});

	for(auto b = tags_.abscissa.cbegin(), e = tags_.abscissa.cend(); b != e; ++b) {
		rect.setPosition({
			*b/xl_*width_ + axispoint_.x, 0.0f
		});
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

		for(auto b = i.first->begin()+1, e = i.first->end(); b != e; ++b) {
			line.setPosition(
				{
					(b-1)->first/xl_*width_ + axispoint_.x,
					(b-1)->second/yl_*height_ - axispoint_.y
				},
				{
					b->first/xl_*width_ + axispoint_.x,
					b->second/yl_*height_ - axispoint_.y
				}
			);
			rtexture_.draw(line);
		}
	}

	return;
}






// end
