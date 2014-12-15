#ifndef __COLOR_H__
#define __COLOR_H__

// A simple wrapper to store colors
// red-green-blue-alpha
struct Color4
{
	float r;
	float g;
	float b;
	float a;

	Color4()
		: r(0.0), g(0.0), b(0.0), a(1.0)
	{}
	Color4(float r, float g, float b, float a = 1.0)
		: r(r), g(g), b(b), a(a)
	{}
};

#endif
