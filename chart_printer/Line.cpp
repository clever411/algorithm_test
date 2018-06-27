#include "Line.hpp"

using namespace clever;



Line::Line(
	sf::Vector2f const &begin,
	sf::Vector2f const &end,
	sf::Color const &color,
	float thickness
)
{
	setPosition(begin, end);
	setColor(color);
	setThickness(thickness);
	return;
}



void Line::draw(
	sf::RenderTarget &target,
	sf::RenderStates states
) const
{
	target.draw(rect_, states);
	return;
}



Line &Line::setThickness(float thickness)
{
	rect_.setSize({rect_.getSize().x, thickness});
	rect_.setOrigin({0.0f, thickness/2.0f});
	return *this;
}
float Line::getThickness() const
{
	return rect_.getSize().y;
}



Line &Line::setPosition(
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
std::pair<sf::Vector2f, sf::Vector2f> Line::getPosition() const
{
	std::pair<sf::Vector2f, sf::Vector2f> res;
	res.first = rect_.getPosition();
	float l = rect_.getSize().x;
	float angle = clever::toradian(rect_.getRotation());
	res.second = {
		res.first.x + l * std::sin( angle ),
		res.first.y + l * std::cos( angle )
	};
	return res;
}



Line &Line::setColor(sf::Color const &newcolor)
{
	rect_.setFillColor(newcolor);
	return *this;
}
sf::Color const &Line::getColor() const
{
	return rect_.getFillColor();
}



// end
