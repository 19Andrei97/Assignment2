#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform
{
public:
	Vec2 pos		= { 0.f, 0.f };
	Vec2 velocity	= { 0.f, 0.f };
	float angle		= 0;

	CTransform(const Vec2 & p, const Vec2 & v, float a)
		: pos(p), velocity(v), angle(a) {}
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin({ radius, radius });
	}
};

class CCollision
{
public:
	float radius = 0.0f;
	CCollision(float r) 
		: radius(r) {}
};

class CScore
{
public:
	int score = 0;
	CScore(int initialScore = 0)
		: score(initialScore) {}
};

class CLifespan
{
public:
	int remaining	= 0;
	int total		= 0;
	CLifespan(int total)
		: remaining(total), total(total) {}
};	

class CInput
{
public:
	bool up		= false;
	bool down	= false;
	bool left	= false;
	bool right	= false;
	bool shoot	= false;

	CInput() {}
};