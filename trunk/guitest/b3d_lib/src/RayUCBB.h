/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of Be3D library.
//
//    Be3D is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Be3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Be3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "common.h"

/**
 * Calculate the intersection of an arbitrary ray with a CENTERED Axis-Aligned Box
 *
 * Should be slightly slower than Pierre Terdiman's code - but it calculates both
 * intersections of the ray with the box, not only first intersection
 * For intersection case, the number of cycles should be almost the same
 * For no intersection, this code is slower cause there's no early exit (TODO :-))
 *
 * ARGUMENTS: ray origin, ray direction, aabox half-size in all 3 axises, min and max t
 *
 * NOTE: Converting this code for not-centered AABs is trivial - I just saved two
 * additions here
 *
 * NOTE 2: ok, there is still room for optimization...
 *
 */
__forceinline bool rayCAABInt(const Vector3& pos, const Vector3& dir, float sx, float sy, float sz, 
					   float *tMin, float *tMax)
{
	/*
	Ray equation: x = pos.x + dir.x*t etc
	intersection with plane x: x=sx -> t = (sx-posx)/dir.x  and t = (-sx-pos.x)/dir.x
	*/

	// t1[x] : the first intersection (closest to ray origin) with x-plane
	// t2[x] : the second (farthest from ray origin)
	float t1[3] = { 0,0,0 }, t2[3] = {1e10,1e10,1e10};

	// find the intersection with x-planes
	if (dir.x != 0)
	{
		if (dir.x > 0) {
			t2[0] = (sx-pos.x)/dir.x;
			t1[0] = (-sx-pos.x)/dir.x;
		}
		else
		{
			t2[0] = (-sx-pos.x)/dir.x;
			t1[0] = (sx-pos.x)/dir.x;
		}
	}
	
	// y-planes
	if (dir.y != 0)
	{
		if (dir.y > 0) {
			t2[1] = (sy-pos.y)/dir.y;
			t1[1] = (-sy-pos.y)/dir.y;
		}
		else
		{
			t2[1] = (-sy-pos.y)/dir.y;
			t1[1] = (sy-pos.y)/dir.y;
		}
	}
	
	// z-planes
	if (dir.z != 0)
	{
		if (dir.z > 0) {
			t2[2] = (sz-pos.z)/dir.z;
			t1[2] = (-sz-pos.z)/dir.z;
		}
		else
		{
			t2[2] = (-sz-pos.z)/dir.z;
			t1[2] = (sz-pos.z)/dir.z;
		}
	}

	// select the biggest of t's
	// entrance plane:
	*tMin = t1[0];
	if (t1[1] > *tMin) *tMin = t1[1];
	if (t1[2] > *tMin) *tMin = t1[2];
	// exit plane:
	*tMax = t2[0];
	if (t2[1] < *tMax) *tMax = t2[1];
	if (t2[2] < *tMax) *tMax = t2[2];

	// check if it is actually inside the box
	// equality is a degenerate case... anyway, consider it as intersection
	return (*tMin <= *tMax);
};

__forceinline bool rayAABInt(const Vector3& pos, const Vector3& dir, const Vector3& minV, const Vector3& maxV,
					   float *tMin, float *tMax)
{
	Vector3 d = maxV-minV;
	return rayCAABInt(pos-(minV+maxV)/2, dir, d.x/2, d.y/2, d.z/2, tMin, tMax);
}