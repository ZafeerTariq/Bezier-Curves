#include <iostream>
#include <SFML/Graphics.hpp>

#include "Point.h"
#include "Lerp.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

enum Direction { right, left };

int main( int argc, char **argv ) {
	sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), "Bezier Curves" );
	sf::Clock clock;
    sf::Time deltaTime;

	Point a( sf::Vector2f( 400, 450 ), 10 );
	Point b( sf::Vector2f( 1200, 450 ), 10 );
	Point moveCircle( a.circle.getPosition(), 25 );

	bool move = false;
	Direction dir = Direction::right;

	Lerp<sf::Vector2f> lerpForward( a.circle.getPosition(), b.circle.getPosition() );
	Lerp<sf::Vector2f> lerpBackward( b.circle.getPosition(), a.circle.getPosition() );

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
				moveCircle.circle.setPosition( lerpForward.update( deltaTime.asSeconds() ) );
				if( moveCircle.circle.getPosition() == b.circle.getPosition() ) {
					move = false;
					lerpForward.reset();
					dir = Direction::left;
				}
			}
			else if( dir == Direction::left ) {
				moveCircle.circle.setPosition( lerpBackward.update( deltaTime.asSeconds() ) );
				if( moveCircle.circle.getPosition() == a.circle.getPosition() ) {
					move = false;
					lerpBackward.reset();
					dir = Direction::right;
				}
			}
		}

		window.clear( sf::Color( 32, 32, 32, 255 ) );

		window.draw( a.circle );
		window.draw( b.circle );
		window.draw( moveCircle.circle );

		window.display();
	}
}