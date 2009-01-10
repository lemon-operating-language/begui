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

class Light
{
public:
	enum Type {
		POINT,
		DIRECTIONAL
	};
	Type	type;
	Vector3 pos;
	Vector3 dir;
	float	intensity;
	Color	diffuse;
	Color	ambient;
	Color	specular;

public:
	// ATTENTION!!!!!!!!!!!!!!!!! : light arg should be = GL_LIGHT1 + i
	void set(int light)
	{
		Vector4 ambient(0.2f, 0.2f, 0.2f, 0);
		Vector4 diffuse(1.0f, 0.9f, 0.8f, 0);
		Vector4 position;
		if (type == POINT)
			position = Vector4(pos.x, pos.y, pos.z, 1);
		else {
			position = Vector4(dir.x, dir.y, dir.z, 0);
			position.normalize();
		}
		glLightfv(light, GL_AMBIENT, (float*)&ambient);
		glLightfv(light, GL_DIFFUSE, (float*)&diffuse);
		glLightfv(light, GL_SPECULAR, (float*)&diffuse);
		glLightfv(light, GL_POSITION, (float*)&position);
		glEnable(light);
	}
};