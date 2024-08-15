#include <iostream>
#include <SFML/Graphics.hpp>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

int main( int argc, char **argv ) {
	sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), "Bezier Curves" );

	while( window.isOpen() ) {
		sf::Event event;
        while ( window.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                window.close();
            }
    	}

		window.clear( sf::Color( 32, 32, 32, 255 ) );
		window.display();
	}
}