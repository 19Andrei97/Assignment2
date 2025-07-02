#pragma once

class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float n_x, float n_y);

	bool operator == (const Vec2& other) const;
	bool operator != (const Vec2& other) const;

	Vec2 operator + (const Vec2& other) const;
	Vec2 operator - (const Vec2& other) const;
	Vec2 operator * (const float val) const;
	Vec2 operator / (const float val) const;

	Vec2& operator += (const Vec2& other);
	Vec2& operator -= (const Vec2& other);
	Vec2& operator *= (const float val);
	Vec2& operator /= (const float val);

		
	float dist(const Vec2& other) const;
};

