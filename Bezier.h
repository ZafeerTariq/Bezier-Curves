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

	Point *points;		// the static points which control the curve
	Point **auxillary;	// points needed between the static ones to lerp, aux[0] stores the original points
	Point bezier;

	LerpGeneric<sf::Vector2f> lerp;

	bool showMoving = true;
	float duration = 1.0f;

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

		bezier = Point( points[0].circle.getPosition(), 25 );
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
				ImGui::SliderFloat( "duration", &duration, 0.0f, 20.0f );
			ImGui::End();

			lerp.set_duration( duration );

			deltaTime = clock.restart();
			update( deltaTime.asSeconds() );
			draw();
		}
	}

	~Bezier() {
		ImGui::SFML::Shutdown();
		delete[] points;
		for( size_t i = 0; i < num_points - 1; i++ ) {
			delete[] auxillary[i];
		}
		delete[] auxillary;
		std::cout << "Freed memory, exiting" << std::endl;
	}

private:
	void init() {
		points = new Point[num_points];

		points[0] = Point( sf::Vector2f( 400, 450 ), 10 );
		points[1] = Point( sf::Vector2f( 600, 350 ), 10 );
		points[2] = Point( sf::Vector2f( 800, 250 ), 10 );
		points[3] = Point( sf::Vector2f( 1000, 550 ), 10 );
		points[4] = Point( sf::Vector2f( 1200, 350 ), 10 );

		auxillary = new Point*[num_points - 1];
		for( size_t i = 0; i < num_points - 1; i++ ) {
			auxillary[i] = new Point[num_points - i];
			for( size_t j = 0; j < num_points - i; j++ ) {
				auxillary[i][j] = Point( points[j].circle.getPosition(), 15 - i * 2 );
			}
		}
	}

	void update( float dt ) {
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) && !move || move ) {
			move = true;
			for( size_t i = 1; i < num_points - 1; i++ ) {
				for( size_t j = 0; j < num_points - i; j++ ) {
					if( dir == Direction::forward ) {
						auxillary[i][j].circle.setPosition( lerp.get(
							auxillary[i - 1][j].circle.getPosition(),
							auxillary[i - 1][j + 1].circle.getPosition()
						) );
					}
					else if ( dir == Direction::backward ) {
						auxillary[i][j].circle.setPosition( lerp.get(
							auxillary[i - 1][j + 1].circle.getPosition(),
							auxillary[i - 1][j].circle.getPosition()
						) );
					}
				}
			}

			if( dir == Direction::forward ) {
				bezier.circle.setPosition( lerp.get(
					auxillary[num_points - 2][0].circle.getPosition(),
					auxillary[num_points - 2][1].circle.getPosition()
				) );
			}
			else if ( dir == Direction::backward ) {
				bezier.circle.setPosition( lerp.get(
					auxillary[num_points - 2][1].circle.getPosition(),
					auxillary[num_points - 2][0].circle.getPosition()
				) );
			}

			lerp.update_time( dt );
			if( lerp.isComplete() ) {
				move = false;
				lerp.reset();
				dir = dir == Direction::forward ? Direction::backward : Direction::forward;
			}
		}
	}

	void draw() {
		window.clear( sf::Color( 32, 32, 32, 255 ) );

		for (size_t i = 0; i < num_points; i++) {
			window.draw( points[i].circle );
			if( showMoving && i != num_points - 1 && i != 0 ) {
				for( size_t j = 0 ; j < num_points - i; j++ ) {
					window.draw( auxillary[i][j].circle );
				}
			}
		}
		window.draw( bezier.circle );

	ImGui::SFML::Render(window);
		window.display();
	}
};

#endif