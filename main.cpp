#include <iostream>
#include <SFML/Graphics.hpp>

#include "Point.h"
#include "Lerp.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define num_points 5

enum Direction { right, left };

int main( int argc, char **argv ) {
	sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), "Bezier Curves" );
	sf::Clock clock;
    sf::Time deltaTime;

	bool move = false;
	Direction dir = Direction::right;

	Point *points = new Point[num_points];
	points[0] = Point( sf::Vector2f( 400, 450 ), 10 );
	points[1] = Point( sf::Vector2f( 600, 350 ), 10 );
	points[2] = Point( sf::Vector2f( 800, 250 ), 10 );
	points[3] = Point( sf::Vector2f( 1000, 350 ), 10 );
	points[4] = Point( sf::Vector2f( 1200, 450 ), 10 );

	Point *moving = new Point[num_points - 1];
	for( size_t i = 0; i < num_points - 1; i++ ) {
		moving[i] = Point( points[i].circle.getPosition(), 25 );
	}

	Lerp<sf::Vector2f> *forwards = new Lerp<sf::Vector2f>[num_points - 1];
	Lerp<sf::Vector2f> *backwards = new Lerp<sf::Vector2f>[num_points - 1];
	for( size_t i = 0; i < num_points - 1; i++ ) {
		forwards[i]  = Lerp<sf::Vector2f>( points[i].circle.getPosition(), points[i + 1].circle.getPosition() );
		backwards[i] = Lerp<sf::Vector2f>( points[i + 1].circle.getPosition(), points[i].circle.getPosition() );
	}

	while( window.isOpen() ) {
		sf::Event event;
        while ( window.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                window.close();
            }
    	}

		deltaTime = clock.restart();

		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) && !move || move ) {
			move = true;

			if( dir == Direction::right ) {
				for (size_t i = 0; i < num_points - 1; i++) {
					moving[i].circle.setPosition( forwards[i].update( deltaTime.asSeconds() ) );
					if( forwards[i].isComplete() ) {
						move = false;
						forwards[i].reset();
						dir = Direction::left;
					}
				}
			}
			else if( dir == Direction::left ) {
				for (size_t i = 0; i < num_points - 1; i++) {
					moving[i].circle.setPosition( backwards[i].update( deltaTime.asSeconds() ) );
					if( backwards[i].isComplete() ) {
						move = false;
						backwards[i].reset();
						dir = Direction::right;
					}
				}
			}
		}

		window.clear( sf::Color( 32, 32, 32, 255 ) );

		for (size_t i = 0; i < num_points; i++) {
			window.draw( points[i].circle );
			if( i != num_points - 1 ) window.draw( moving[i].circle );
		}

		window.display();
	}
}