#include "Vec2.h"
#include <iostream>


// Constructors
Vec2::Vec2(){}
Vec2::Vec2(float n_x, float n_y) : x(n_x), y(n_y) {}


// Equality and Inequality Operators
bool Vec2::operator == (const Vec2& other) const
{
	return (x == other.x && y == other.y);
}

bool Vec2::operator != (const Vec2& other) const
{
	return (x != other.x || y != other.y);
}


// Arithmetic Operators
Vec2 Vec2::operator + (const Vec2& other) const
{
	return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator - (const Vec2& other) const
{
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator * (float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (float val) const
{
	if (val == 0) {
		throw std::invalid_argument("Division by zero is not allowed.");
	}
	return Vec2(x / val, y / val);
}


// Assignment Operators
Vec2& Vec2::operator += (const Vec2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vec2& Vec2::operator -= (const Vec2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vec2& Vec2::operator *= (float val)
{
	x *= val;
	y *= val;
	return *this;
}

Vec2& Vec2::operator /= (float val)
{
	if (val == 0) {
		throw std::invalid_argument("Division by zero is not allowed.");
	}
	x /= val;
	y /= val;
	return *this;
}

float Vec2::dist(const Vec2& other) const
{
	float dx = x - other.x;
	float dy = y - other.y;

	return std::sqrt(dx * dx + dy * dy);
}