#include "vector3.h"

#include <cmath>

static const Vector3 ZERO = Vector3(0.0, 0.0, 0.0);

Vector3::Vector3()
	: x(0.0), y(0.0), z(0.0)
{
}

Vector3::Vector3(double x, double y, double z)
	: x(x), y(y), z(z)
{}

Vector3::Vector3(const Vector3 & v)
	: x(v.x), y(v.y), z(v.z)
{}

Vector3 Vector3::operator+(const Vector3 & rhs) const
{ 
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); 
}

Vector3 Vector3::operator-(const Vector3 & rhs) const
{ 
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); 
}

Vector3 Vector3::operator*(double rhs) const
{ 
	return Vector3(x * rhs, y * rhs, z * rhs); 
}

Vector3 Vector3::operator/(double rhs) const
{ 
	return Vector3(x / rhs, y / rhs, z / rhs); 
}

Vector3 Vector3::operator+=(const Vector3 & rhs)
{ 
	x += rhs.x; y += rhs.y; z += rhs.z; 
	return *this; 
}

Vector3 Vector3::operator-=(const Vector3 & rhs)
{ 
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; 
}

Vector3 Vector3::operator*=(double rhs)
{ 
	x *= rhs; y *= rhs; z *= rhs; return *this; 
}

Vector3 Vector3::operator/=(double rhs)
{ 
	x /= rhs; y /= rhs; z /= rhs; return *this; 
}

double Vector3::magnitude() const
{ 
	return std::sqrt(x * x + y * y + z * z); 
}

void Vector3::normalize()
{ 
	*this /= magnitude(); 
}

Vector3 Vector3::normalized() const
{ 
	return *this / magnitude(); 
}

double Vector3::dot(const Vector3 & rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::cross(const Vector3 & rhs) const
{
	return Vector3(y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x);
}

Vector3 Vector3::rotate(const Vector3 & axis, double angle)
{
	double cosTheta = std::cos(angle);
	double sinTheta = std::sin(angle);
	double aXX = axis.x * axis.x;
	double aXY = axis.x * axis.y;
	double aXZ = axis.x * axis.z;
	double aYY = axis.y * axis.y;
	double aYZ = axis.y * axis.z;
	double aZZ = axis.z * axis.z;

	double nx = x * (cosTheta + aXX * (1 - cosTheta)) + 
				y * (aXY * (1 - cosTheta) - axis.z * sinTheta) +
				z * (aXZ * (1 - cosTheta) + axis.y * sinTheta);
	double ny = x * (aXY * (1 - cosTheta) + axis.z * sinTheta) +
				y * (cosTheta + aYY * (1 - cosTheta)) +
				z * (aYZ * (1 - cosTheta) - axis.x * sinTheta);
	double nz = x * (aXZ * (1 - cosTheta) - axis.y * sinTheta) +
				y * (aYZ * (1 - cosTheta) + axis.x * sinTheta) +
				z * (cosTheta + aZZ * (1 - cosTheta));

	return Vector3(nx, ny, nz);
}

