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

#include "Mesh.h"

class PMesh : public Mesh
{
	class VSplit
	{
	public:
		int	v1, v2;		// v1: the new vertex (collapsed) - v2: the old one
		int nFaces;		// number of faces removed during the edge collapse
		std::vector<int> changedIds;	// the ids of the vertex ids that have to change
		double error;	// the error introduced by removing this vertex
	};
	std::vector<VSplit>	m_vsplits;
	int					m_nextSplit;
	int					m_activeFaceCount;
	int					m_firstSplit;
	bool				m_bIsPMesh;
	bool				m_bUseVertexArrays;

	std::vector<unsigned int>	m_indices;
	std::vector<unsigned int>	m_norm_indices;

public:
	PMesh();
	virtual ~PMesh();

	void create(const Mesh& mesh);
	void makeProgressive();
	virtual void render();

	void vsplit();
	void vcollapse();
	void setVertexNum(int nVertices);

	int getActiveVertexCount() const	{ return m_nextSplit; }
	int getBaseVertexCount() const		{ return m_firstSplit; }
	int getMaxVertexCount() const		{ return (int)m_vertices.size(); }
	bool isPMesh() const				{ return m_bIsPMesh; }
	double	getCurError() const			{ if (m_nextSplit >= (int)m_vsplits.size()) return 0; if (m_nextSplit < m_firstSplit) return 1e30; return m_vsplits[m_nextSplit].error; }
	
	virtual void flipFaces(bool bFlipVNormals = true);

private:
	void buildVCollapse(std::vector<bool>& fRemoved, std::vector<bool>& vRemoved, std::vector<bool>& eRemoved, 
						std::vector<int>& faceOrder, int& faceCount);
	void prepareVertexArrays();
};
