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
#include "Octree.h"
#include "RayUCBB.h"
#include "RayTriIntersect.h"

#define SPLIT_THRES 20

class RayQueryTree
{
public:
	class Face {
	public:
		Vector3 v[3];
		BBox bbox;
	};
private:
	class FaceEntry {
	public:
		Vector3 v[3];
	};
	class TreeNode
	{
		TreeNode* m_children[8];
		std::list<Face> m_faces;
		BBox m_bbox;
	public:
		TreeNode(const BBox& bbox) : m_bbox(bbox) { for (int i=0; i<8; ++i) m_children[i] = 0; }
		~TreeNode() { for (int i=0; i<8; ++i) delete m_children[i]; }

		void add(const Face& face)
		{
			if (m_children[0])
			{
				// send down to the children
				for (int i=0; i<8; ++i)
					if (m_children[i]->m_bbox.contains(face.bbox)) {
						m_children[i]->add(face);
						return;
					}
				// add here
				m_faces.push_back(face);
			}
			else
			{
				m_faces.push_back(face);

				// if too many faces, split
				if (m_faces.size() > SPLIT_THRES)
					split();
			}
		}
		__forceinline bool ray_query(const Vector3& orig, const Vector3& dir, float& t, Face **face)
		{
			// check if we are in the bbox
			float tMin, tMax;
			if (!rayAABInt(orig, dir, m_bbox.m_min, m_bbox.m_max, &tMin, &tMax))
				return false;

			for (std::list<Face>::iterator it=m_faces.begin(); it!=m_faces.end(); ++it)
			{
				static float u, v;
				if (intersect_triangle(&orig.x, &dir.x, &(*it).v[0].x, &(*it).v[1].x, &(*it).v[2].x,
					&t, &u, &v))
				{
					if (t > 0.001f) {
						*face = &(*it);
						return true;
					}
				}
			}
			for (int i=0; i<8; ++i)
			{
				if (m_children[i])
					if (m_children[i]->ray_query(orig, dir, t, face))
						return true;
			}
			return false;
		}
		void split()
		{
			if (m_children[0])
				return;

			// split the children
			Vector3 minv(m_bbox.m_min), mid(m_bbox.getCenter()), maxv(m_bbox.m_max);
			m_children[0] = new TreeNode(BBox(minv.x, minv.y, minv.z, mid.x, mid.y, mid.z));
			m_children[1] = new TreeNode(BBox(mid.x, minv.y, minv.z, maxv.x, mid.y, mid.z));
			m_children[2] = new TreeNode(BBox(minv.x, mid.y, minv.z, mid.x, maxv.y, mid.z));
			m_children[3] = new TreeNode(BBox(mid.x, mid.y, minv.z, maxv.x, maxv.y, mid.z));
			m_children[4] = new TreeNode(BBox(minv.x, minv.y, mid.z, mid.x, mid.y, maxv.z));
			m_children[5] = new TreeNode(BBox(mid.x, minv.y, mid.z, maxv.x, mid.y, maxv.z));
			m_children[6] = new TreeNode(BBox(minv.x, mid.y, mid.z, mid.x, maxv.y, maxv.z));
			m_children[7] = new TreeNode(BBox(mid.x, mid.y, mid.z, maxv.x, maxv.y, maxv.z));

			// send triangles down
			for (std::list<Face>::iterator it=m_faces.begin(); it!=m_faces.end(); ++it)
				for (int i=0; i<8; ++i)
					if (m_children[i]->m_bbox.contains((*it).bbox)) {
						m_children[i]->add((*it));
						it = m_faces.erase(it);
						if (it != m_faces.begin())
							--it;
						break;
					}
		}
		void clear()
		{
			m_bbox = BBox(-100,-100,-100,100,100,100);
			for (int i=0; i<8; ++i) { delete m_children[i]; m_children[i] = 0; };
			m_faces.clear();
		}
	};
	TreeNode	m_root;
public:
	typedef O8Container<FaceEntry> tEntry;

private:
//	Octree<tEntry>	m_octree;
	float m_lastT;
	Face *m_hitFace;

public:
	RayQueryTree();
	~RayQueryTree() {};

	void addTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
	__forceinline bool intersectsRay(const Vector3& orig, const Vector3& dir);
	void clear()				{ m_root.clear(); }

	float last_t() const		{ return m_lastT; }
	Face* getFaceHit() const	{ return m_hitFace; }
};

//----------------------------------------------------------------------
bool RayQueryTree::intersectsRay(const Vector3& orig, const Vector3& dir)
{
	m_lastT = -1;
	m_hitFace = 0;
	return m_root.ray_query(orig, dir, m_lastT, &m_hitFace);
}