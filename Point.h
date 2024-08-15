#ifndef POINT_H
#define POINT_H

#include <SFML/Graphics.hpp>

class Point {
public:
	sf::CircleShape circle;

	Point( sf::Vector2f position, float radius ) {
		circle = sf::CircleShape( radius );
		circle.setPosition( position );
		circle.setOrigin( radius, radius );
	}
};

#endif