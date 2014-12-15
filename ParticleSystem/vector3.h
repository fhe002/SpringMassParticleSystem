#ifndef __VECTOR3_H__
#define __VECTOR3_H__
#include <stdio.h>
// A simple wrapper for store 3D vectors
struct Vector3
{
	double x;
	double y;
	double z;

	Vector3();
	Vector3(double x, double y, double z);
	Vector3(const Vector3 & v);

	Vector3 operator+(const Vector3 & rhs) const;
	Vector3 operator-(const Vector3 & rhs) const;
	Vector3 operator*(double rhs) const;
	Vector3 operator/(double rhs) const;
	Vector3 operator+=(const Vector3 & rhs);
	Vector3 operator-=(const Vector3 & rhs);
	Vector3 operator*=(double rhs);
	Vector3 operator/=(double rhs);

	double magnitude() const;
	void normalize();
	Vector3 normalized() const;
	double dot(const Vector3 & rhs) const;
	Vector3 cross(const Vector3 & rhs) const;
	Vector3 rotate(const Vector3 & axis, double angle);
	void print() const { printf("%f, %f, %f\n", x,y,z); }
	    
};

#endif
