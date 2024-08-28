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

	bool showMoving = false;
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

		bezier = Point( points[0][0].circle.getPosition(), 25, sf::Color( 50,184,251 ) );
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
		points[0][0] = Point( sf::Vector2f( 0, 450 ), 20, sf::Color( 17,253,169 ) );
		points[0][1] = Point( sf::Vector2f( 480, 828 ), 20, sf::Color( 17,253,169 ) );
		points[0][2] = Point( sf::Vector2f( 1584, 90 ), 20, sf::Color( 17,253,169 ) );
		points[0][3] = Point( sf::Vector2f( 1046, 748.8 ), 20, sf::Color( 17,253,169 ) );

		for( size_t i = 1; i < num_points - 1; i++ ) {
			points[i] = new Point[num_points - i];
			for( size_t j = 0; j < num_points - i; j++ ) {
				points[i][j] = Point( points[0][j].position, 15 - i * 2, sf::Color( 255,140,17 ) );
			}
		}
	}

	void draw() {
		// window.clear( sf::Color( color[0] * 255, color[1] * 255, color[2] * 255 ) );
		window.clear( sf::Color( 33, 22, 40 ) );

		for (size_t i = 0; i < num_points - 1; i++) {
			if( i == 0 || showMoving ) {
				for( size_t j = 0 ; j < num_points - i; j++ ) {
					window.draw( points[i][j].circle );
				}
			}
		}

		for( size_t i = 0; i < resolution; i++ ) {
			sf::RectangleShape texel( sf::Vector2f( 10, 10 ) );
			texel.setPosition( curve[i] );
			texel.setFillColor( sf::Color( 254,17,85 ) );
			window.draw( texel );
		}

		window.draw( bezier.circle );

		ImGui::SFML::Render(window);
		window.display();
	}

	void update( float dt ) {
		if( num_points != next_num_points ) {
			grow_points( next_num_points - num_points );
		}

		calculate_curve();

		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) && !move || move ) {
			move = true;

			update_points( lerp.get_t() );
			update_bezier();

			lerp.update_time( dt );
			if( lerp.isComplete() ) {
				move = false;
				lerp.reset();
				dir = dir == Direction::forward ? Direction::backward : Direction::forward;
			}
		}
	}

	void update_points( float t ) {
		for( size_t i = 1; i < num_points - 1; i++ ) {
			for( size_t j = 0; j < num_points - i; j++ ) {
				if( dir == Direction::forward ) {
					points[i][j].set_position(
						interpolate_points( i - 1, j, i - 1, j + 1, t )
					);
				}
				else if ( dir == Direction::backward ) {
					points[i][j].set_position(
						interpolate_points( i - 1, j + 1, i - 1, j, t )
					);
				}
			}
		}
	}

	void update_bezier() {
		if( dir == Direction::forward ) {
			bezier.set_position(
				interpolate_points( num_points - 2, 0, num_points - 2, 1, lerp.get_t() )
			);
		}
		else if ( dir == Direction::backward ) {
			bezier.set_position(
				interpolate_points( num_points - 2, 1, num_points - 2, 0, lerp.get_t() )
			);
		}
	}

	sf::Vector2f interpolate_points( size_t from_i, size_t from_j, size_t to_i, size_t to_j, float t ) {
		return Lerp<sf::Vector2f>::lerp(
			points[from_i][from_j].circle.getPosition(),
			points[to_i][to_j].circle.getPosition(),
			t
		);
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
			temp[0][i] = Point( sf::Vector2f( 0, 0 ), 20, sf::Color( 17,253,169 ) );
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

			curve[t] = interpolate_points( num_points - 2, 0, num_points - 2, 1, t / (float)resolution );
		}
	}
};

#endif