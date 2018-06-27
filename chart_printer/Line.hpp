#ifndef CLEVER_LINE_HPP
#define CLEVER_LINE_HPP

#include <cmath>

#include <clever/SFML/HelpFunctions.hpp>

namespace clever
{

template<typename T = void>
class Line: public sf::Drawable
{
public:
	typedef float value_type;

	Line(
		sf::Vector2f const &begin = {0.0f, 0.0f},
		sf::Vector2f const &end = {0.0f, 0.0f},
		sf::Color const &color = sf::Color::Black,
		value_type thickness = 1.0f
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
	
	Line &setThickness(value_type thickness)
	{
		rect_.setSize({rect_.getSize().x, thickness});
		rect_.setOrigin({0.0f, thickness/2.0f});
		return *this;
	}
	value_type getThickness() const
	{
		return rect_.getSize().y;
	}

	Line &setPosition(
		sf::Vector2f const &begin, 
		sf::Vector2f const &end
	) 
	{
		rect_.setSize({
			std::hypot(end.x-begin.x, end.y-begin.y),
			rect_.getSize().y
		});
		rect_.setOrigin({0.0f, rect_.getSize().y/2});
		rect_.setPosition(begin);
		rect_.setRotation(
			clever::togradus(clever::angle(
				end.x-begin.x, end.y-begin.y, 1.0f, 0.0f
			))
		);
		return *this;
	}
	std::pair<sf::Vector2f, sf::Vector2f> getPosition() const
	{
		std::pair<sf::Vector2f, sf::Vector2f> res;
		res.first = rect_.getPosition();
		auto l = rect_.getSize().x;
		auto angle = clever::toradian(rect_.getRotation());
		res.second = {
			res.first.x + l * std::sin( angle ),
			res.first.y + l * std::cos( angle )
		};
		return res;
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


}


#endif
