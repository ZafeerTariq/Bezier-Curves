#ifndef POINT_H
#define POINT_H

#include <SFML/Graphics.hpp>

class Point {
public:
	sf::CircleShape circle;
	sf::Vector2f position;

	Point() {}

	Point( sf::Vector2f position, float radius, sf::Color color = sf::Color::White ) {
		this->position = position;
		circle = sf::CircleShape( radius );
		circle.setPosition( position );
		circle.setOrigin( radius, radius );
		circle.setFillColor( color );
	}

	void set_position( sf::Vector2f position ) {
		this->position = position;
		circle.setPosition( position );
	}

	void set_position() {
		circle.setPosition( position );
	}
};

#endif