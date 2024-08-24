#ifndef LERP_H
#define LERP_H

template <typename T>
class Lerp {
private:
	T a;
	T b;
	float t;
	float duration;
public:
	Lerp() {
		t = 0;
		duration = 1.0f;
	}

	Lerp( T a, T b, float duration = 1.0f ) {
		this->a = a;
		this->b = b;
		this->duration = duration;
		t = 0;
	}

	T update( float dt ) {
		if( t < 1.0f ) {
			t += dt / duration;
			return a + ( b - a ) * t;
		}
		return b;
	}

	T update( T a, T b, float dt ) {
		if( t < 1.0f ) {
			t += dt / duration;
			return a + ( b - a ) * t;
		}
		return b;
	}

	void reset() {
		t = 0;
	}

	bool isComplete() {
		return t >= 1.0f;
	}
};

#endif