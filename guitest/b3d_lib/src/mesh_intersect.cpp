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
#include "RayTriIntersect.h"
#include "RayQueryTree.h"

#define NUM_RAYS 60

bool Mesh::intersectRay(const Vector3 &orig, const Vector3 &dir, double *t_out)
{
	ASSERT(t_out);

	// check for intersection with the mesh bbox
	// TODO

	float t,u,v;
	float min_t=-1;

	// check for intersection with each triangle
	for (int i=0; i<m_faces.size(); ++i)
	{
		Vector3 v1 = m_vertices[m_faces[i].ids[0]];
		Vector3 v2 = m_vertices[m_faces[i].ids[1]];
		Vector3 v3 = m_vertices[m_faces[i].ids[2]];

		// tri-ray intersection
		if (intersect_triangle(&orig.x, &dir.x, &v1.x, &v2.x, &v3.x, &t, &u, &v))
		{
			if ((t < min_t || min_t==-1) && t>0.0001)
				min_t = t;
		}
	}

	*t_out = min_t;
	if (min_t > -1)
		return true;
	return false;
}

void Mesh::precalcAmbientOcclusion()
{
	ASSERT(m_normals.size() >= m_vertices.size());

	m_ambOcclCoef.resize(m_vertices.size(), 0);

	Console::print("building octree:   0%");
	RayQueryTree tree;
	for (int i=0; i<m_faces.size(); ++i)
	{
		tree.addTriangle(m_vertices[m_faces[i].ids[0]],
						m_vertices[m_faces[i].ids[1]],
						m_vertices[m_faces[i].ids[2]]);
		Console::print("\b\b\b\b%3d%%", i*100/m_faces.size());
	}

	Console::print("Computing ambient occlusion:   0%%");

	// for each vertex, shoot rays to random directions
	// in the visible hemisphere and get average
	for (int i=0; i<m_vertices.size(); ++i)
	{
		Vector3 v_pos = m_vertices[i];
		Vector3 v_norm = m_normals[i];

		for (int j=0; j<NUM_RAYS; ++j)
		{
			// compute random ray direction
			Vector3 ray_dir = v_norm;
			
			ray_dir.x = (float)rand()/RAND_MAX;
			ray_dir.y = (float)rand()/RAND_MAX;
			ray_dir.z = (float)rand()/RAND_MAX;
			if (ray_dir.dot(v_norm) < 0)
				ray_dir *= -1;

			ray_dir.normalize();

			// check intersection
			double t;
	//		if (!intersectRay(v_pos, ray_dir, &t))
	//			m_ambOcclCoef[i] += 1.0f;	// no intersection: ambient light hits the vertex
			if (!tree.intersectsRay(v_pos, ray_dir))
				m_ambOcclCoef[i] += 1.0f/NUM_RAYS;
		}

		Console::print("\b\b\b\b%3d%%", i*100/m_vertices.size());
	}
	Console::print("\n");
}