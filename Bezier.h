#ifndef BEZIER_H
#define BEZIER_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "Point.h"
#include "Lerp.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define num_points 5

enum Direction { forward, backward };

class Bezier {
private:
	sf::RenderWindow window;
	sf::Clock clock;
    sf::Time deltaTime;

	bool move;
	Direction dir;

	Point *points;
	Point *moving;
	Lerp<sf::Vector2f> *forwards;
	Lerp<sf::Vector2f> *backwards;

	bool showMoving = true;

public:
	Bezier() {
		init();
		window.create( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), "Bezier Curves" );

		if ( !ImGui::SFML::Init(window) ) {
			std::cout << "Could not init imgui\n";
			exit( EXIT_FAILURE );
		}

		move = false;
		dir = Direction::forward;

		points[0] = Point( sf::Vector2f( 400, 450 ), 10 );
		points[1] = Point( sf::Vector2f( 600, 350 ), 10 );
		points[2] = Point( sf::Vector2f( 800, 250 ), 10 );
		points[3] = Point( sf::Vector2f( 1000, 350 ), 10 );
		points[4] = Point( sf::Vector2f( 1200, 450 ), 10 );

		for( size_t i = 0; i < num_points - 1; i++ ) {
			moving[i] = Point( points[i].circle.getPosition(), 25 );
			forwards[i]  = Lerp<sf::Vector2f>( points[i].circle.getPosition(), points[i + 1].circle.getPosition() );
			backwards[i] = Lerp<sf::Vector2f>( points[i + 1].circle.getPosition(), points[i].circle.getPosition() );
		}
	}

	void run() {
		while( window.isOpen() ) {
			sf::Event event;
			while ( window.pollEvent( event ) ) {
				ImGui::SFML::ProcessEvent( window, event );
				if ( event.type == sf::Event::Closed ) {
					window.close();
				}
			}

			ImGui::SFML::Update( window, deltaTime );
			ImGui::Begin( "Menu" );
				ImGui::Checkbox( "show moving points", &showMoving );
			ImGui::End();

			deltaTime = clock.restart();
			update( deltaTime.asSeconds() );
			draw();
		}

		ImGui::SFML::Shutdown();

		delete[] points;
		delete[] moving;
		delete[] forwards;
		delete[] backwards;
	}

private:
	void init() {
		points = new Point[num_points];
		moving = new Point[num_points - 1];
		forwards = new Lerp<sf::Vector2f>[num_points - 1];
		backwards = new Lerp<sf::Vector2f>[num_points - 1];
	}

	void update( float dt ) {
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) && !move || move ) {
			move = true;

			if( dir == Direction::forward ) {
				for (size_t i = 0; i < num_points - 1; i++) {
					moving[i].circle.setPosition( forwards[i].update( deltaTime.asSeconds() ) );
					if( forwards[i].isComplete() ) {
						move = false;
						forwards[i].reset();
						dir = Direction::backward;
					}
				}
			}
			else if( dir == Direction::backward ) {
				for (size_t i = 0; i < num_points - 1; i++) {
					moving[i].circle.setPosition( backwards[i].update( deltaTime.asSeconds() ) );
					if( backwards[i].isComplete() ) {
						move = false;
						backwards[i].reset();
						dir = Direction::forward;
					}
				}
			}
		}
	}

	void draw() {
		window.clear( sf::Color( 32, 32, 32, 255 ) );

		for (size_t i = 0; i < num_points; i++) {
			window.draw( points[i].circle );
			if( showMoving && i != num_points - 1 ) window.draw( moving[i].circle );
		}

	ImGui::SFML::Render(window);
		window.display();
	}
};

#endif