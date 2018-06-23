#ifndef CHART_PRINTER_HPP
#define CHART_PRINTER_HPP

#include <algorithm>
#include <cmath>
#include <list>
#include <memory>
#include <vector>

#include <clever/SFML/HelpFunctions.hpp>

#include <SFML/Graphics.hpp>





template<typename T = void>
class Line: public sf::Drawable
{
public:
	Line(
		sf::Vector2f const &begin = {0.0f, 0.0f},
		sf::Vector2f const &end = {0.0f, 0.0f},
		sf::Color const &color = sf::Color::Black,
		float thickness = 1.0f
	)
	{
		setPosition(begin, end);
		setColor(color);
		setThickness(thickness);
		return;
	}

	void draw(
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const override
	{
		target.draw(rect_, states);
		return;
	}
	
	Line &setThickness(float thickness)
	{
		rect_.setSize({rect_.getSize().x, thickness});
		return *this;
	}
	float getThickness() const
	{
		return rect_.getSize().y;
	}

	Line &setPosition(
		sf::Vector2f const &begin, 
		sf::Vector2f const &end
	) 
	{
		rect_.setPosition(begin);
		rect_.setSize({
			std::hypot(end.x-begin.x, end.y-begin.y),
			rect_.getSize().y
		});
		rect_.setOrigin(0.0f, rect_.getSize().y/2);
		rect_.setRotation(
			clever::togradus(clever::angle(
				end.x-begin.x, end.y-begin.y, 1.0f, 0.0f
			))
		);
		return *this;
	}

	Line &setColor(sf::Color const &newcolor)
	{
		rect_.setFillColor(newcolor);
		return *this;
	}
	sf::Color const &getColor() const
	{
		return rect_.getFillColor();
	}
private:
	sf::RectangleShape rect_;

};





struct ChartSettings
{
	sf::Color color;
	float thickness;
	float overlayprior;

	static ChartSettings const Default;
};
ChartSettings const ChartSettings::Default =
	ChartSettings{sf::Color::Black, 2.0f, 0.0f};




struct AxisSettings
{
	sf::Color color;
	float thickness;

	static AxisSettings const Default;
};
AxisSettings const AxisSettings::Default =
	AxisSettings{sf::Color::Black, 1};




template<typename T = float>
class ChartPrinter: public sf::Drawable, public sf::Transformable
{
public:
	typedef T value_type;
	typedef std::shared_ptr<
		std::vector<std::pair<value_type, value_type>>
	> chart_type;




	void update()
	{
		if(ischanged_)
			adjust_();
		return;
	}

	void draw(
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const override
	{
		states.transform *= sf::Transformable::getTransform();
		target.draw(sprite_, states);
		return;
	}


	ChartPrinter &addChart(
		chart_type newchart,
		ChartSettings const &settings = ChartSettings::Default
	)
	{
		charts_.push_back({newchart, settings});
		ischanged_ = true;
		return *this;
	}
	ChartPrinter &removeChart(chart_type const &chart)
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
	ChartPrinter &clearCharts()
	{
		charts_.clear();
		ischanged_ = true;
		return *this;
	}


	AxisSettings const &getAxisSettings() const
	{
		return axis_;
	}
	ChartPrinter &setAxisSettings(AxisSettings const &newaxis)
	{
		axis_ = newaxis;
		ischanged_ = true;
		return *this;
	}


	ChartPrinter &setSize(float width, float height)
	{
		width_ = width;
		height_ = height;
		ischanged_ = true;
		return *this;
	}
	ChartPrinter &setSize(sf::Vector2f size)
	{
		width_ = size.x;
		height_ = size.y;
		ischanged_ = true;
		return *this;
	}

	sf::Vector2f getSize() const
	{
		return {width_, height_};
	}

private:

	void adjust_()
	{
		// for overlay
		std::sort(
			charts_.begin(), charts_.end(),
			[](
				std::pair<chart_type, ChartSettings> const &lhs,
				std::pair<chart_type, ChartSettings> const &rhs
			)->bool {
				return lhs.second.overlayprior > rhs.second.overlayprior;
			}
		);
		
		calculate_size_();

		draw_();

		ischanged_ = false;
		return;
	}

	void calculate_size_()
	{
		value_type xmin = 0, xmax = 0;
		value_type ymin = 0, ymax = 0;

		typename std::vector<
			std::pair<value_type, value_type>
		>::const_iterator
			buf
		;

		for(auto b = charts_.begin(), e = charts_.end(); b != e; ++b) {
			buf = std::min_element(
				b->first->cbegin(),
				b->first->cend(),
				[](
					std::pair<value_type, value_type> const &lhs,
					std::pair<value_type, value_type> const &rhs
				)->bool {
					return lhs.first < rhs.first;
				}
			);

			if(buf != b->first->cend() && buf->first < xmin)
				xmin = buf->first;

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
			if(buf != b->first->cend() && buf->second < ymin)
				ymin = buf->second;

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
		
		xl_ = xmax-xmin;
		yl_ = ymax-ymin;

		if(xl_ == 0)
			axispoint_.x = 0.0f;
		else
			axispoint_.x = float(-xmin) / xl_ * width_;
		if(yl_ == 0)
			axispoint_.y = 0.0f;
		else
			axispoint_.y = float(-ymin) / yl_ * height_;

		return;
	}

	void draw_()
	{
		rtexture_.create(width_, height_);
		rtexture_.clear(sf::Color::Transparent);

		Line<> line;
		line.setThickness(axis_.thickness);
		line.setColor(axis_.color);

		line.setPosition(
			{0.0f, axispoint_.y+1},
			{width_, axispoint_.y+1}
		);
		rtexture_.draw(line);

		line.setPosition(
			{axispoint_.x+1, 0.0f},
			{axispoint_.x+1, height_}
		);
		rtexture_.draw(line);

		for(auto const &i : charts_) {
			if(i.first->size() < 2)
				continue;

			line.setColor(i.second.color);
			line.setThickness(i.second.thickness);

			for(auto b = i.first->begin()+1, e = i.first->end(); b != e; ++b) {
				line.setPosition(
					{
						(b-1)->first/xl_*width_ + axispoint_.x,
						((b-1)->second/yl_*height_ + axispoint_.y)
					},
					{
						b->first/xl_*width_ + axispoint_.x,
						(b->second/yl_*height_ + axispoint_.y)
					}
				);
				rtexture_.draw(line);
			}

		}

		sprite_.setTexture(rtexture_.getTexture());
		sprite_.setTextureRect(
			sf::IntRect(0, 0, int(width_), int(height_))
		);
	}

	std::vector<
		std::pair<chart_type, ChartSettings>
	> charts_;

	AxisSettings axis_ = AxisSettings::Default;
	sf::Vector2f axispoint_ = {0.0f, 0.0f};
	float xl_ = 0.0f, yl_ = 0.0f;

	float width_ = 300.0f, height_ = 300.0f;


	bool ischanged_ = true;

	sf::RenderTexture rtexture_;
	sf::Sprite sprite_;

};





#endif
