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

#include "RayQueryTree.h"
#include "RayTriIntersect.h"

RayQueryTree::RayQueryTree()// : m_octree(BBox(-100,-100,-100,100,100,100), 200, 100)
	: m_root(BBox(-100,-100,-100,100,100,100)), m_lastT(-1)
{
/*	FaceEntry entry;
	entry.v[0] = Vector3(0,0,0);
	entry.v[1] = Vector3(0,0,0);
	entry.v[2] = Vector3(0,0,0);
	m_octree.add(tEntry(entry, BBox(-100,-100,-100, 100,100,100)));*/
}

void RayQueryTree::addTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
//	FaceEntry entry;
	Face entry;
	entry.v[0] = v1;
	entry.v[1] = v2;
	entry.v[2] = v3;
	BBox bbox(v1,v1);
	if (v2.x < bbox.m_min.x) bbox.m_min.x = v2.x;
	if (v2.y < bbox.m_min.y) bbox.m_min.y = v2.y;
	if (v2.z < bbox.m_min.z) bbox.m_min.z = v2.z;
	if (v2.x > bbox.m_max.x) bbox.m_max.x = v2.x;
	if (v2.y > bbox.m_max.y) bbox.m_max.y = v2.y;
	if (v2.z > bbox.m_max.z) bbox.m_max.z = v2.z;
	if (v3.x < bbox.m_min.x) bbox.m_min.x = v3.x;
	if (v3.y < bbox.m_min.y) bbox.m_min.y = v3.y;
	if (v3.z < bbox.m_min.z) bbox.m_min.z = v3.z;
	if (v3.x > bbox.m_max.x) bbox.m_max.x = v3.x;
	if (v3.y > bbox.m_max.y) bbox.m_max.y = v3.y;
	if (v3.z > bbox.m_max.z) bbox.m_max.z = v3.z;

	entry.bbox = bbox;
	m_root.add(entry);
//	m_octree.add(tEntry(entry, bbox));
}
/*
Vector3 g_rayOrig, g_rayDir;
bool g_bRes;

bool nodeCB(OctreeNode<RayQueryTree::tEntry>& node)
{
	return true;
	// check the node's bbox for intersection
	float tMin, tMax;
	if (rayAABInt(g_rayOrig, g_rayDir, node.m_bbox.m_min, node.m_bbox.m_max, &tMin, &tMax))
		return true;
	return false;
}

void itemCB(RayQueryTree::tEntry& item)
{
	// intersect with this triangle
	static float t, u, v;
	if (intersect_triangle(&g_rayOrig.x, &g_rayDir.x, &item.value.v[0].x, &item.value.v[1].x, &item.value.v[2].x,
		&t, &u, &v))
	{
		if (t > 0.0001f)
			g_bRes = true;
	}
}*/

/*bool RayQueryTree::intersectsRay(const Vector3& orig, const Vector3& dir)
{
	// query the octree to check for intersection
//	g_bRes = false;
//	g_rayOrig = orig;
//	g_rayDir = dir;

//	m_octree.query(nodeCB, itemCB);
	m_lastT = -1;
	return m_root.ray_query(orig, dir, m_lastT);

//	return g_bRes;
}*/