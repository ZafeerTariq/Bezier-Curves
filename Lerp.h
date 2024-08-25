#ifndef LERP_H
#define LERP_H

template <typename T>
class LerpGeneric {
protected:
	float t;
	float duration;

public:
	LerpGeneric( float t = 0, float duration = 1.0f ) {
		this->t = t;
		this->duration = duration;
	}

	T update( T a, T b, float dt ) {
		if( t < 1.0f ) {
			t += dt / duration;
			return a + ( b - a ) * t;
		}
		return b;
	}

	T get( T a, T b ) {
		if( t < 1.0f ) {
			return a + ( b - a ) * t;
		}
		return b;
	}

	void update_time( float dt ) {
		if( t < 1.0f ) {
			t += dt / duration;
		}
	}

	void reset() {
		t = 0;
	}

	bool isComplete() {
		return t >= 1.0f;
	}
};

template <typename T>
class Lerp : public LerpGeneric<T> {
private:
	T a;
	T b;

public:
	Lerp( T a, T b, float duration = 1.0f ) {
		this->a = a;
		this->b = b;
		this->duration = duration;
		this->t = 0;
	}

	T update( float dt ) {
		if( this->t < 1.0f ) {
			this->t += dt / this->duration;
			return a + ( b - a ) * this->t;
		}
		return b;
	}
};

#endif