#include <iostream>
#include <SFML/Graphics.hpp>

#include "Point.h"
#include "Lerp.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

int main( int argc, char **argv ) {
	sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), "Bezier Curves" );
	sf::Clock clock;
    sf::Time deltaTime;

	Point a( sf::Vector2f( 400, 450 ), 10 );
	Point b( sf::Vector2f( 1200, 450 ), 10 );
	Point moveCircle( a.circle.getPosition(), 25 );

	bool move = false;

	Lerp<sf::Vector2f> lerp( a.circle.getPosition(), b.circle.getPosition() );

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
			moveCircle.circle.setPosition( lerp.update( deltaTime.asSeconds() ) );
			if( moveCircle.circle.getPosition() == b.circle.getPosition() ) {
				move = false;
			}
		}

		window.clear( sf::Color( 32, 32, 32, 255 ) );

		window.draw( a.circle );
		window.draw( b.circle );
		window.draw( moveCircle.circle );

		window.display();
	}
}