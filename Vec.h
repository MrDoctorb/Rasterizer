#pragma once

template<typename T>

class Vec2
{
public:
	T x;
	T y;
};

template<typename T>
class Vec3
{
public:
	T x;
	T y;
	T z;

	friend Vec3<T> operator-(const Vec3<T>& a)
	{
		return { -a.x, -a.y, -a.z };
	}
	
	friend Vec3<T> operator-(const Vec3<T>& a, const Vec3<T>& b)
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z };
	}

	friend Vec3<T> operator+(const Vec3<T>& a, const Vec3<T>& b)
	{
		return { a.x + b.x, a.y + b.y, a.z + b.z };
	}

	friend Vec3<T> operator*(T a, const Vec3<T>& b)
	{
		return { a * b.x, a * b.y, a * b.z };
	}


};

template<typename T>
class Vec4
{
public:
	T x;
	T y;
	T z;
	T w;
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec4i = Vec4<int>;
using Vec4f = Vec4<float>;


