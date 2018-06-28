#ifndef CLEVER_LINE_HPP
#define CLEVER_LINE_HPP

#include <cmath>

#include <clever/SFML/HelpFunctions.hpp>

#ifdef DEBUG
	#include "debug.hpp"
#endif

namespace clever
{

class Line: public sf::Drawable
{
public:
	Line(
		sf::Vector2f const &begin = {0.0f, 0.0f},
		sf::Vector2f const &end = {0.0f, 0.0f},
		sf::Color const &color = sf::Color::Black,
		float thickness = 1.0f
	);

	void draw(
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const override;
	
	Line &setThickness(float thickness);
	float getThickness() const;

	Line &setPosition(
		sf::Vector2f const &begin, 
		sf::Vector2f const &end
	);
	std::pair<sf::Vector2f, sf::Vector2f> getPosition() const;

	Line &setColor(sf::Color const &newcolor);
	sf::Color const &getColor() const;

private:
	sf::RectangleShape rect_;

};


}


#endif
