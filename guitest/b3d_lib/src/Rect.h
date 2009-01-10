#pragma once

#include "misc.h"

template <class T>
class Rect
{
public:
	T left, right, top, bottom;

	Rect() { };
	Rect(const T& _left, const T& _right, const T& _top, const T& _bottom) : left(_left), 
							right(_right), top(_top), bottom(_bottom) { }
	inline T getWidth() const	{ return right - left; }
	inline T getHeight() const	{ return bottom - top; }

	inline bool contains(T x, T y) const {
		if (x<left || x>right || y<top || y>bottom)
			return false;
		return true;
	}
};