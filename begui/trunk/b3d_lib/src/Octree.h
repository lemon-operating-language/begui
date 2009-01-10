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
#include "BBox.h"

template <class T> class Octree;
template <class T> class OctreeNode;

/**
 * NOTE: type T must define a method BBox getBBox()
 * This is used to get the bounding box for the item represented by
 * an instance of type T.
 *
 */
template <class T>
class OctreeNode
{
	typedef bool (*tNodeCallback)(OctreeNode<T>& node);
	typedef void (*tItemCallback)(T& item);
public:
	BBox m_bbox;
	std::vector<T>	m_items;

protected:
	OctreeNode<T>	*m_children[8];
	Octree<T>		*m_owner;

public:
	OctreeNode(Octree<T> *owner) : m_owner(owner) { ASSERT(owner); for (int i=0; i<8; ++i) m_children[i] = 0; }
	~OctreeNode() { for (int i=0; i<8; ++i) delete m_children[i]; }
	void add(const T&);
	void remove(const T&);
	void split();
	void compact();
	void foreach(tNodeCallback cb);
	void foreach(tItemCallback cb);

	void query(tNodeCallback nodeCB, tItemCallback itemCB);	// nodeCB accepts/rejects nodes - itemCB is applied to 
										// all items of nodes accepted
};

/**
 * The octree implementation. See notes on OctreeNode class about type T.
 *
 */
template <class T>
class Octree
{
	typedef bool (*tNodeCallback)(OctreeNode<T>& node);
	typedef void (*tItemCallback)(T& item);

	int m_splitThres;
	int m_compactThres;
	OctreeNode<T>	*m_root;

public:
	Octree(const BBox& bbox, int splitThres = 10, int compactThres = 5);
	~Octree();

	void add(const T&);
	void remove(const T&);
	void foreach(tNodeCallback cb);
	void foreach(tItemCallback cb);

	int getSplitThres() const	{ return m_splitThres; }
	int getCompactThres() const	{ return m_compactThres; }
	
	void query(tNodeCallback nodeCB, tItemCallback itemCB);	// nodeCB accepts/rejects nodes - itemCB is applied to 
										// all items of nodes accepted
};

/**
 * Some auxiliary types
 */
template <class T>
class O8Container {
public:
	T value;
	BBox bbox;
public:
	O8Container(const T& val, const BBox& bbx) : value(val), bbox(bbx) { }
	const BBox& getBBox() const	{ return bbox; }
};

//--------------------------------------------------
// IMPLEMENTATION

template<class T> void OctreeNode<T>::split()
{
	// create the children
	for (int i=0; i<8; ++i) {
		m_children[i] = new OctreeNode(m_owner);
	}
	Vector3 minv(m_bbox.m_min), mid(m_bbox.getCenter()), maxv(m_bbox.m_max);
	m_children[0]->m_bbox = BBox(minv.x, minv.y, minv.z, mid.x, mid.y, mid.z);
	m_children[1]->m_bbox = BBox(mid.x, minv.y, minv.z, maxv.x, mid.y, mid.z);
	m_children[2]->m_bbox = BBox(minv.x, mid.y, minv.z, mid.x, maxv.y, mid.z);
	m_children[3]->m_bbox = BBox(mid.x, mid.y, minv.z, maxv.x, maxv.y, mid.z);
	m_children[4]->m_bbox = BBox(minv.x, minv.y, mid.z, mid.x, mid.y, maxv.z);
	m_children[5]->m_bbox = BBox(mid.x, minv.y, mid.z, maxv.x, mid.y, maxv.z);
	m_children[6]->m_bbox = BBox(minv.x, mid.y, mid.z, mid.x, maxv.y, maxv.z);
	m_children[7]->m_bbox = BBox(mid.x, mid.y, mid.z, maxv.x, maxv.y, maxv.z);

	// split contents in children
	for (size_t j=0; j<m_items.size(); ++j) {
		for (i=0; i<8; ++i) {
			if (m_children[i]->m_bbox.contains(m_items[j].getBBox()))
			{
				m_children[i]->m_items.push_back(m_items[j]);
				m_items.erase(m_items.begin()+j);
				--j;
				break;
			}
		}
	}
}

template<class T> void OctreeNode<T>::compact()
{
	for (int i=0; i<8; ++i)
	{
		if (m_children[i] == 0)
			continue;

		// let the children gather all its children
		m_children[i]->compact();	// inline recursion... hmmmmmm.... living on the edge as usual :P

		// rip the poor child out of all its possesions.. muhaha
		m_items.reserve(m_items.size() + m_children[i]->m_items.size());
		for (size_t j=0; j<m_children[i]->m_items.size(); ++j)
			m_items.push_back(m_children[i]->m_items[j]);

		// ..and finally, kill the child
		SAFE_DELETE(m_children[i]);
	}
}

template<class T> void OctreeNode<T>::add(const T& t)
{
	m_items.push_back(t);
	if ((int)m_items.size() > m_owner->getSplitThres())
		split();
}

template<class T> void OctreeNode<T>::remove(const T& t)
{
	for (size_t i=0; i<m_items.size(); ++i)
		if (m_items[i] == &t) {
			m_items.erase(m_items.begin()+i);
			break;
		}

	if (static_cast<int>(m_items.size()) < m_owner->getCompactThres())
		compact();
}

template <class T> void OctreeNode<T>::foreach(bool (*cb)(OctreeNode<T>&))
{
	for (size_t i=0; i<m_items.size(); ++i)
		cb(*this);
	for (i=0; i<8; ++i) {
		if (m_children[i])
			m_children[i]->foreach(cb);
	}
}

template <class T> void OctreeNode<T>::query(bool (*nodeCB)(OctreeNode<T>&), void (*itemCB)(T& item))
{
	// if node satisfies the node callback
	if (nodeCB(*this))
	{
		// apply the item callback
		for (size_t i=0; i<m_items.size(); ++i)
			itemCB(m_items[i]);

		// and query all children
		for (i=0; i<8; ++i) {
			if (m_children[i])
				m_children[i]->query(nodeCB, itemCB);
		}
	}
}

template<class T> Octree<T>::Octree(const BBox& bbox, int splitThres, int compactThres) : 
	m_splitThres(splitThres), m_compactThres(compactThres)
{
	// sanity check
	ASSERT(m_splitThres > 0);
	ASSERT(m_compactThres >= 0);
	if (m_splitThres < m_compactThres)
		m_splitThres = m_compactThres;

	m_root = new OctreeNode<T>(this);
	m_root->m_bbox = bbox;
}

template<class T> Octree<T>::~Octree()
{
	SAFE_DELETE(m_root);
}

template<class T> void Octree<T>::add(const T& t)
{
	ASSERT(m_root);
	m_root->add(t);
}

template<class T> void Octree<T>::remove(const T& t)
{
	ASSERT(m_root);
	m_root->remove(t);
}

template <class T> void Octree<T>::foreach(bool (*cb)(OctreeNode<T>&))
{
	ASSERT(m_root);
	m_root->foreach(cb);
}

template <class T> void Octree<T>::query(bool (*nodeCB)(OctreeNode<T>&), void (*itemCB)(T& item))
{
	ASSERT(m_root);
	m_root->query(nodeCB, itemCB);
}