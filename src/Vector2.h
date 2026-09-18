#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include "Engine.h"

struct Vector2 {
	Tepeyo::int12 x;
	Tepeyo::int12 y;

	Vector2() : x(Tepeyo::fp12(0)), y(Tepeyo::fp12(0)) {}
	Vector2(Tepeyo::int12 x, Tepeyo::int12 y) : x(x), y(y) {}

	const Vector2 operator+(const Vector2 & add) const {
		return Vector2(Tepeyo::add12(x, add.x), Tepeyo::add12(y, add.y));
	}
	const Vector2 operator-(const Vector2 & sub) const {
		return Vector2(Tepeyo::sub12(x, sub.x), Tepeyo::sub12(y, sub.y));
	}
	const Vector2 operator*(const Tepeyo::int12 & mul) const {
		return Vector2(Tepeyo::mul12(x, mul), Tepeyo::mul12(y, mul));
	}
	const Vector2 operator/(const Tepeyo::int12 & div) const {
		return Vector2(Tepeyo::div12(x, div), Tepeyo::div12(y, div));
	}
	void normalize();
	Tepeyo::int12 norm();
	Tepeyo::int12 squaredNorm();
};

#endif // _VECTOR2_H_
