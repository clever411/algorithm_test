#ifndef SFML_HELP_FUNCTIONS_HPP
#define SFML_HELP_FUNCTIONS_HPP

#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>


namespace clever
{


template<typename T, typename U>
inline sf::Vector2<T> conversion(sf::Vector2<U> const &vec)
{
	return sf::Vector2<T>(T(vec.x), T(vec.y));
}

inline double angle(double x1, double y1, double x2, double y2)
{
	return std::acos(
		(x1*x2 + y1*y2) /
		(std::hypot(x1, y1) * std::hypot(x2, y2))
	);
}

template<typename T, typename U>
inline double angle(sf::Vector2<T> const &first, sf::Vector2<U> const &second)
{
	return angle(first.x, first.y, second.x, second.y);
}

constexpr static double const PI = 3.141592653589;
inline double togradus(double angle)
{
	return 180.0f*angle/PI;
}

inline double toradian(double angle)
{
	return angle*PI/180.0f;
}

}

template<typename T>
inline std::ostream &operator<<(std::ostream &os, sf::Vector2<T> const &vec)
{
	return os << "(" << vec.x << ", " << vec.y << ")";
}
template<typename T>
inline std::istream &operator>>(std::ostream &is, sf::Vector2<T> &vec)
{
	return is >> vec.x >> vec.y;
}


template<typename T>
inline std::ostream &operator<<(std::ostream &os, sf::Rect<T> const &rect)
{
	return os << "(" << rect.left << ", " << rect.top <<
		")&(" << rect.width << ", " << rect.height << ")";
}
template<typename T>
inline std::istream &operator<<(std::istream &is, sf::Rect<T> &rect)
{
	return is >> rect.left >> rect.top >> rect.width >> rect.height;
}



template<typename T, typename U>
inline sf::Vector2<decltype(T()+U())> operator+(
	sf::Vector2<T> const &lhs,
	sf::Vector2<U> const &rhs
)
{
	return {lhs.x+rhs.x, lhs.y+rhs.y};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()-U())> operator-(
	sf::Vector2<T> const &lhs,
	sf::Vector2<U> const &rhs
)
{
	return {lhs.x-rhs.x, lhs.y-rhs.y};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()*U())> operator*(
	sf::Vector2<T> const &lhs,
	sf::Vector2<U> const &rhs
)
{
	return {lhs.x*rhs.x, lhs.y*rhs.y};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()/U())> operator/(
	sf::Vector2<T> const &lhs,
	sf::Vector2<U> const &rhs
)
{
	return {lhs.x/rhs.x, lhs.y/rhs.y};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()/U())> operator*(
	sf::Vector2<T> const &lhs,
	U const &number
)
{
	return {lhs.x*number, lhs.y*number};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()/U())> operator*(
	U const &number,
	sf::Vector2<T> const &lhs
)
{
	return {lhs.x*number, lhs.y*number};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()/U())> operator/(
	sf::Vector2<T> const &lhs,
	U const &number
)
{
	return {lhs.x/number, lhs.y/number};
}

template<typename T, typename U>
inline sf::Vector2<decltype(T()/U())> operator/(
	U const &number,
	sf::Vector2<T> const &lhs
)
{
	return {lhs.x/number, lhs.y/number};
}

#endif // SFML_HELP_FUNCTIONS_HPP

