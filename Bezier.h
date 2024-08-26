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

#define num_points 4

enum Direction { forward, backward };

class Bezier {
private:
	sf::RenderWindow window;
	sf::Clock clock;
    sf::Time deltaTime;

	bool move;
	Direction dir;

	Point **points;	// points needed between control ones to lerp, points[0] stores the control points
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

		bezier = Point( points[0][0].circle.getPosition(), 25 );
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

				for( size_t i = 0; i < num_points; i++ ) {
					std::string point = "p" + std::to_string( i );
					ImGui::SliderFloat( ( point + "x" ).c_str(), &points[0][i].position.x, 0, SCREEN_WIDTH );
					ImGui::SliderFloat( ( point + "y" ).c_str(), &points[0][i].position.y, 0, SCREEN_HEIGHT );
					points[0][i].set_position();
				}
			ImGui::End();
			lerp.set_duration( duration );

			deltaTime = clock.restart();
			update( deltaTime.asSeconds() );
			draw();
		}
	}

	~Bezier() {
		ImGui::SFML::Shutdown();
		for( size_t i = 0; i < num_points - 1; i++ ) {
			delete[] points[i];
		}
		delete[] points;
		std::cout << "Freed memory, exiting" << std::endl;
	}

private:
	void init() {
		if( points ) {
			for( size_t i = 0; i < num_points - 1; i++ ) {
				delete[] points[i];
			}
			delete[] points;
		}

		points = new Point*[num_points - 1];

		points[0] = new Point[num_points];
		points[0][0] = Point( sf::Vector2f( 0, 450 ), 10 );
		points[0][1] = Point( sf::Vector2f( 480, 828 ), 10 );
		points[0][2] = Point( sf::Vector2f( 1584, 90 ), 10 );
		points[0][3] = Point( sf::Vector2f( 1046, 748.8 ), 10 );

		for( size_t i = 1; i < num_points - 1; i++ ) {
			points[i] = new Point[num_points - i];
			for( size_t j = 0; j < num_points - i; j++ ) {
				points[i][j] = Point( points[0][j].circle.getPosition(), 15 - i * 2 );
			}
		}
	}

	void update( float dt ) {
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) && !move || move ) {
			move = true;
			for( size_t i = 1; i < num_points - 1; i++ ) {
				for( size_t j = 0; j < num_points - i; j++ ) {
					if( dir == Direction::forward ) {
						points[i][j].set_position( lerp.get(
							points[i - 1][j].circle.getPosition(),
							points[i - 1][j + 1].circle.getPosition()
						) );
					}
					else if ( dir == Direction::backward ) {
						points[i][j].set_position( lerp.get(
							points[i - 1][j + 1].circle.getPosition(),
							points[i - 1][j].circle.getPosition()
						) );
					}
				}
			}

			if( dir == Direction::forward ) {
				bezier.set_position( lerp.get(
					points[num_points - 2][0].circle.getPosition(),
					points[num_points - 2][1].circle.getPosition()
				) );
			}
			else if ( dir == Direction::backward ) {
				bezier.set_position( lerp.get(
					points[num_points - 2][1].circle.getPosition(),
					points[num_points - 2][0].circle.getPosition()
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

		for (size_t i = 0; i < num_points - 1; i++) {
			if( i == 0 || showMoving ) {
				for( size_t j = 0 ; j < num_points - i; j++ ) {
					window.draw( points[i][j].circle );
				}
			}
		}
		window.draw( bezier.circle );

		ImGui::SFML::Render(window);
		window.display();
	}
};

#endif