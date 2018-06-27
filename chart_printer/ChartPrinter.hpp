#ifndef CHART_PRINTER_HPP
#define CHART_PRINTER_HPP

#include <algorithm>
#include <cmath>
#include <list>
#include <memory>
#include <vector>

#include <clever/SFML/HelpFunctions.hpp>
#include <clever/SFML/Line.hpp>

#include <SFML/Graphics.hpp>




template<typename T = float>
struct ChartSettings
{
	typedef T value_type;

	sf::Color color;
	value_type thickness;
	value_type overlayprior;

	static ChartSettings const DEFAULT;
};
template<typename T>
ChartSettings<T> const ChartSettings<T>::DEFAULT =
	ChartSettings<T>{sf::Color::Black, 2.0f, 0.0f};




template<typename T = float>
struct AxisSettings
{
	typedef T value_type;

	sf::Color color;
	value_type thickness;

	static AxisSettings const DEFAULT;
};
template<typename T>
AxisSettings<T> const AxisSettings<T>::DEFAULT =
	AxisSettings<T>{sf::Color::Black, 4.0f};




template<typename T = float>
struct TagSettings
{
	typedef T value_type;

	value_type length;
	value_type thickness;
	sf::Color color;
	sf::Text text;

	static TagSettings const DEFAULT;
};
template<typename T>
TagSettings<T> const TagSettings<T>::DEFAULT =
	TagSettings<T>{31.0f, 4.0f, sf::Color::Black};



template<typename T = float>
struct Tags
{
	typedef T value_type;

	std::vector<value_type> abscissa;
	std::vector<value_type> ordinate;
};





template<typename T = float>
struct GridSettings
{
	typedef T value_type;

	bool enable;
	value_type thickness;
	sf::Color color;

	static GridSettings const DEFAULT;
};
template<typename T>
GridSettings<T> const GridSettings<T>::DEFAULT =
	GridSettings<T>{true, 1.0f, sf::Color(0x22, 0x22, 0x22)};





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
		ChartSettings<value_type> const &settings =
			ChartSettings<value_type>::DEFAULT
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


	ChartPrinter &setAxisSettings(
		AxisSettings<value_type> const &newaxis
	)
	{
		axis_ = newaxis;
		ischanged_ = true;
		return *this;
	}
	AxisSettings<value_type> const &getAxisSettings() const
	{
		return axis_;
	}


	ChartPrinter &setTagSettings(
		TagSettings<value_type> const &newtagset
	)
	{
		tagset_ = newtagset;
		ischanged_ = true;
		return *this;
	}
	TagSettings<value_type> const &getTagSettings() const
	{
		return tagset_;
	}

	ChartPrinter &setGridSettings(
		GridSettings<value_type> const &gridset
	)
	{
		gridset_ = gridset;
		ischanged_ = true;
		return *this;
	}
	GridSettings<value_type const &> getGridSettings() const
	{
		return gridset_;
	}

	void calculate_tags_bycount(size_t xcount = 10, size_t ycount = 10)
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

	void calculate_tags_byinterval(float xinter, float yinter)
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

	ChartPrinter &setPadding(float padding)
	{
		padding_ = padding;
		ischanged_ = true;
		return *this;
	}
	float getPadding() const
	{
		return padding_;
	}
private:

	void adjust_()
	{
		// for overlay
		std::sort(
			charts_.begin(), charts_.end(),
			[](
				std::pair<
					chart_type,
					ChartSettings<value_type>
				> const &lhs,
				std::pair<
					chart_type,
					ChartSettings<value_type>
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

	void calculate_size_()
	{
		xmin_ = 0;
		value_type xmax = 0;
		ymin_ = 0;
		value_type ymax = 0;

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

	void draw_()
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
	void draw_axis_()
	{
		clever::Line<> line;
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
	void draw_tags_()
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
	void draw_grid_()
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
	void draw_charts_()
	{
		clever::Line<> line;

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


	std::vector<
		std::pair<chart_type, ChartSettings<value_type>>
	> charts_;

	AxisSettings<value_type> axis_ = AxisSettings<value_type>::DEFAULT;
	sf::Vector2f axispoint_ = {0.0f, 0.0f};
	float width_ = 300.0f, height_ = 300.0f;
	float padding_ = 10.0f;

	TagSettings<value_type> tagset_ = TagSettings<value_type>::DEFAULT;
	Tags<value_type> tags_;
	GridSettings<value_type> gridset_ = GridSettings<value_type>::DEFAULT;

	float xl_ = 0.0f, yl_ = 0.0f;
	float xmin_ = 0.0f, ymin_ = 0.0f;


	bool ischanged_ = true;

	sf::RenderTexture rtexture_;
	sf::Sprite sprite_;

};





#endif
