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

#include "Mesh.h"

void Mesh::genTexCoords(TexGenMode mode, TexGenPrimitive prim, Axis axis)
{
	Console::print("Generating texture coordinates for mesh...\n");

	// find primitive center
	Vector3 center = m_bbox.getCenter();
	Vector3 bboxSz = m_bbox.getSize();

	// create uv coords
	m_texCoords.resize(m_vertices.size());
	for (size_t i=0; i<m_vertices.size(); ++i)
	{
		Vector3 &v = m_vertices[i];
		Vector2 &uv = m_texCoords[i];

		if (mode == TG_ISNORMAL)
		{
			if (prim == TG_CYLINDER)
			{
				// suppose we have a unit-radius cylinder positioned at the
				// center of the object. Find the uv coords on the surface of
				// the cylinder, corresponding to this point.
				float dx = v.x-center.x;
				float dz = v.z-center.z;
				float cu = atan2(dx,dz)/PI;
				float cv = (v.y - center.y)/bboxSz.y;

				// map (cu,cv) to (u,v) depending on the mapping of the
				// texture onto the cylinder.
				uv.x = cu/2 + 0.5f;
				uv.y = cv;
			}
			else if (prim == TG_SPHERE)
			{
				float dx = v.x-center.x;
				float dy = v.y-center.y;
				float dz = v.z-center.z;
				float rad = sqrtf(dx*dx + dy*dy + dz*dz);
				uv.y = acosf(dy/rad)/PI;
				uv.x = atan2(dx,dz)/(2*PI) + 0.5f;
			}
			else if (prim == TG_PLANE)
			{
				float dx = v.x-center.x;
				float dy = v.y-center.y;
				float dz = v.z-center.z;
				switch (axis) {
					case X_AXIS:
						uv.x = -dz/bboxSz.z;
						uv.y = dy/bboxSz.y;
						break;
					case Y_AXIS:
						uv.x = dx/bboxSz.y;
						uv.y = dz/bboxSz.z;
						break;
					case Z_AXIS:
						uv.x = dx/bboxSz.x;
						uv.y = dy/bboxSz.y;
						break;
				}
			}
		}

		//m_texCoords[i] = mapTexCoords(m_vertices[i], mode, prim);
	}

	Console::print("\tdone\n");
}
