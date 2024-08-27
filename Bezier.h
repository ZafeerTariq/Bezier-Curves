#ifndef BEZIER_H
#define BEZIER_H

#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "Point.h"
#include "Lerp.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

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

	sf::Vector2f *curve;
	int resolution = 1000;

	bool showMoving = true;
	float duration = 1.0f;
	int num_points = 4;
	int next_num_points = 4;

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
		curve = new sf::Vector2f[resolution];

		calculate_curve();
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

			if( num_points != next_num_points ) {
				grow_points( next_num_points - num_points );
			}

			calculate_curve();

			ImGui::SFML::Update( window, deltaTime );
			ImGui::Begin( "Menu" );
				ImGui::Checkbox( "show moving points", &showMoving );
				ImGui::SliderInt( "curve resolution", &resolution, 10, 1000 );
				ImGui::SliderFloat( "duration", &duration, 0.0f, 20.0f );
				ImGui::SliderInt( "points", &next_num_points, 2, 9 );

				ImGui::Text( "points positions" );
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
		delete[] curve;
		std::cout << "Freed memory, exiting" << std::endl;
	}

private:
	void init() {
		points = new Point*[num_points - 1];

		points[0] = new Point[num_points];
		points[0][0] = Point( sf::Vector2f( 0, 450 ), 10 );
		points[0][1] = Point( sf::Vector2f( 480, 828 ), 10 );
		points[0][2] = Point( sf::Vector2f( 1584, 90 ), 10 );
		points[0][3] = Point( sf::Vector2f( 1046, 748.8 ), 10 );

		for( size_t i = 1; i < num_points - 1; i++ ) {
			points[i] = new Point[num_points - i];
			for( size_t j = 0; j < num_points - i; j++ ) {
				points[i][j] = Point( points[0][j].position, 15 - i * 2 );
			}
		}
	}

	void grow_points( int size ) {
		Point **temp = new Point*[num_points + size - 1];
		temp[0] = new Point[num_points + size];

		int size_to_copy = size > 0 ? num_points : num_points + size;
		// copy original points control points
		for( size_t i = 0; i < size_to_copy; i++ ) {
			temp[0][i] = points[0][i];
		}
		// set the newly added control points to ( 0, 0 )
		for( size_t i = num_points; i < num_points + size; i++ ) {
			temp[0][i] = Point( sf::Vector2f( 0, 0 ), 10 );
		}

		for( size_t i = 1; i < num_points + size - 1; i++ ) {
			temp[i] = new Point[num_points + size - 1];
			for( size_t j = 0; j < num_points + size - i; j++ ) {
				temp[i][j] = Point( temp[0][j].position, 15 - i * 2 );
			}
		}

		for( size_t i = 0; i < num_points - 1; i++ ) {
			delete[] points[i];
		}
		delete[] points;

		points = temp;
		num_points += size;
	}

	void calculate_curve() {
		for( size_t t = 0; t < resolution; t++ ) {
			for( size_t i = 1; i < num_points - 1; i++ ) {
				for( size_t j = 0; j < num_points - i; j++ ) {
					points[i][j].set_position( Lerp<sf::Vector2f>::lerp(
						points[i - 1][j].circle.getPosition(),
						points[i - 1][j + 1].circle.getPosition(),
						t / (float)resolution
					) );
				}
			}

			curve[t] = ( Lerp<sf::Vector2f>::lerp(
				points[num_points - 2][0].circle.getPosition(),
				points[num_points - 2][1].circle.getPosition(),
				t / (float)resolution
			) );
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

		for( size_t i = 0; i < resolution; i++ ) {
			sf::RectangleShape texel( sf::Vector2f( 10, 10 ) );
			texel.setPosition( curve[i] );
			window.draw( texel );
		}

		ImGui::SFML::Render(window);
		window.display();
	}
};

#endif