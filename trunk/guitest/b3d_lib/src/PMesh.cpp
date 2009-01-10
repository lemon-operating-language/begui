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

#include "PMesh.h"

PMesh::PMesh() :
	m_nextSplit(-1),
	m_activeFaceCount(-1),
	m_firstSplit(-1),
	m_bIsPMesh(false),
	m_bUseVertexArrays(false)
{
}

PMesh::~PMesh()
{
}

void PMesh::create(const Mesh& mesh)
{
	m_vertices = mesh.m_vertices;
	m_normals = mesh.m_normals;
	m_faces = mesh.m_faces;
	m_edges = mesh.m_edges;
	m_vertexAdjEdges = mesh.m_vertexAdjEdges;
	m_vertexAdjFaces = mesh.m_vertexAdjFaces;
	m_bbox = mesh.m_bbox;
	m_lists = mesh.m_lists;
	m_indices.resize(0);

	m_activeFaceCount = (int)m_faces.size();
	m_firstSplit = -1;
	m_nextSplit = -1;
	m_bIsPMesh = false;

	computeBBox();
}

void PMesh::makeProgressive()
{
	m_vsplits.resize(0);
	m_indices.resize(0);

	// pmesh algorithm works only on triangular meshes. So, triangulate
	// the mesh and create connectivity information again (since it is
	// lost during triangulation)
	triangulate(true);
	buildConnectivityInfo();

	m_vsplits.resize(m_vertices.size());
	m_nextSplit = (int)m_vsplits.size();

	// collapse mesh.
	std::vector<bool> vRemoved(m_vertices.size(), false);
	std::vector<bool> fRemoved(m_faces.size(), false);
	std::vector<bool> eRemoved(m_edges.size(), false);
	std::vector<int> faceOrder(m_faces.size(), -1);
	int faceCount = (int)m_faces.size();
	Console::print("progress:   0%");
	size_t nSteps = m_vertices.size();
	for (size_t i=0; i<nSteps; ++i)
	{
		buildVCollapse(fRemoved, vRemoved, eRemoved, faceOrder, faceCount);
		Console::print("\b\b\b\b%3d%%", 100*i/nSteps);
	}
	Console::print("\ndone. Recorded %d vsplits\n", m_vsplits.size());
	Console::print("next split = %d\n", m_nextSplit);

	// rebuilt mesh.
	// first, build the index array by copying face indices in the order
	// imposed by faceOrder (creation order in vsplit direction)
	m_indices.resize(m_faces.size()*3);
	m_norm_indices.resize(m_faces.size()*3);
	int wrong = 0;
	for (size_t i=0; i<faceOrder.size(); ++i)
	{
		if (faceOrder[i] == -1)
			faceOrder[i] = wrong++;
	}
	for (size_t i=0; i<faceOrder.size(); ++i)
	{
		ASSERT(faceOrder[i] > -1);
		ASSERT(faceOrder[i] < m_faces.size());
		ASSERT(faceOrder[i]*3+2 < m_indices.size());

		m_indices[faceOrder[i]*3  ] = m_faces[i].ids[0];
		m_indices[faceOrder[i]*3+1] = m_faces[i].ids[1];
		m_indices[faceOrder[i]*3+2] = m_faces[i].ids[2];
		
		m_norm_indices[faceOrder[i]*3  ] = m_faces[i].nids[0];
		m_norm_indices[faceOrder[i]*3+1] = m_faces[i].nids[1];
		m_norm_indices[faceOrder[i]*3+2] = m_faces[i].nids[2];
	}

	// permute the ids in the vsplits
	for (size_t i=0; i<m_vsplits.size(); ++i)
	{
		VSplit& split = m_vsplits[i];
		for (size_t j=0; j<split.changedIds.size(); ++j)
		{
			int id = split.changedIds[j];
			split.changedIds[j] = faceOrder[id/3]*3 + id%3;
		}
	}

	// set the next vsplit and active face count
	m_activeFaceCount = faceCount;	// should ALWAYS be zero!
	//m_nextSplit = 0;
	m_firstSplit = m_nextSplit;

	// then apply all vsplits to resurrect the mesh (rise from thy ashes and the such.. :-P )
	for (size_t i=0; i<m_vsplits.size(); ++i)
		vsplit();

	m_bIsPMesh = true;

	// Duplicate some vertex data to be able to use vertex arrays
	// and multiple vertex normals (to support smoothing groups etc)
	// Quite inefficient, but necessary evil.
	if (m_bUseVertexArrays)
		prepareVertexArrays();	// NOT FUNCTIONING YET
}

void PMesh::render()
{
	if (!m_bIsPMesh) {
		Mesh::render();
		return;
	}

	if (m_bUseVertexArrays)
	{
		// render the mesh using vertex arrays.
		glEnable(GL_NORMAL_ARRAY);
		glEnable(GL_VERTEX_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, (void*)&m_vertices[0]);
		glNormalPointer(GL_FLOAT, 0, (void*)&m_normals[0]);
	}

	if (!m_bUseVertexArrays || glGetError() != GL_NO_ERROR)
	{
		// fall back
		glBegin(GL_TRIANGLES);
		for (int i=0; i<m_activeFaceCount*3; i++)
		{
				Vector3 &n = m_normals[m_norm_indices[i]];
				glNormal3f(n.x, n.y, n.z);
				
				Vector3& v = m_vertices[m_indices[i]];
				glVertex3f(v.x, v.y, v.z);
		}
		glEnd();
	}
	else
		glDrawElements(GL_TRIANGLES, m_activeFaceCount*3, GL_UNSIGNED_INT, (void*)&m_indices[0]);
}

void PMesh::buildVCollapse(std::vector<bool>& fRemoved, std::vector<bool>& vRemoved, std::vector<bool>& eRemoved,
						   std::vector<int>& faceOrder, int& faceCount)
{
	VSplit vsplit;	// the vsplit object we will create.
	vsplit.nFaces = 0;

	// select the edge to collapse.
	int v1 = -1, v2 = -1, edgeId = -1;
	float error = -1;
	for (size_t e=0; e<m_edges.size(); ++e) {
		if (!eRemoved[e]) {
			float err = (m_vertices[m_edges[e].vertex1] - m_vertices[m_edges[e].vertex2]).length2();
			err *= fabs(1.0f - m_normals[m_edges[e].vertex1].dot(m_normals[m_edges[e].vertex2]));
			if (err < error || edgeId == -1) {
				edgeId = (int)e;
				error = err;
			}
		}
	}
	if (edgeId == -1)
		return;
	Edge& edge = m_edges[edgeId];

	vsplit.error = error;
	
	// 0- remove edge
	eRemoved[edgeId] = true;

	v1 = edge.vertex1;
	v2 = edge.vertex2;
	ASSERT(!vRemoved[v1]);
	ASSERT(!vRemoved[v2]);

	// collapse this edge
	//	v1: this vertex is removed
	//	v2: this vertex remains. v1 is collapsed onto v2.
	
	// 1- remove all faces adjacent to this edge
	//	Remove those triangles from the face adjacency
	//	list of vertex v2 as well.
	// DO NOT USE edge.adjFaces SINCE WE DO NOT UPDATE THIS STRUCTURE
	// IN THE PROCESS OF BUILDING THE PMESH
	for (size_t i=0; i<m_vertexAdjFaces[v1].size(); ++i)
	{
		for (size_t j=0; j<m_vertexAdjFaces[v2].size(); ++j)
		{
			if (m_vertexAdjFaces[v1][i] == m_vertexAdjFaces[v2][j]) {
				// remove this face
				fRemoved[m_vertexAdjFaces[v1][i]] = true;

				// store the order of the removal of this face.
				ASSERT(faceOrder[m_vertexAdjFaces[v1][i]] == -1);
				ASSERT(m_vertexAdjFaces[v1][i] < faceOrder.size());
				faceOrder[m_vertexAdjFaces[v1][i]] = --faceCount;

				// remove this face from m_vertexAdjFaces[v2] as well
				m_vertexAdjFaces[v2].erase(m_vertexAdjFaces[v2].begin() + j);

				vsplit.nFaces++;	// count how many triangles were removed.

				break;
			}
		}
	}

	// 2- update indices of all triangles neighboring to v1
	//	and not removed, to point to v2
	for (size_t i=0; i<m_vertexAdjFaces[v1].size(); ++i)
	{
		int face = m_vertexAdjFaces[v1][i];
		if (fRemoved[face])
			continue;
		for (size_t j=0; j<m_faces[face].ids.size(); ++j)
		{
			if (m_faces[face].ids[j] == v1) {
				m_faces[face].ids[j] = v2;

				m_vertexAdjFaces[v2].push_back(face);

				// store the position of the changed id (pos = face*3 + j)
				int chpos = (int)(face*3 + j);
				vsplit.changedIds.push_back(chpos);
			}
		}
	}

	// 3- update adjacency info of v2. Move all adjacent triangles
	//	(not removed) and all adjacent edges (except v1-v2) of v1
	//	to v2.
	// SOME EDGES BECOME COMMON AND HAVE TO BE REMOVED
	// NOTE: m_vertexAdjEdge structure is not used while building
	// the pmesh. So do not update it - we'll fix it later in the
	// process.
	for (size_t i=0; i<m_vertexAdjEdges[v1].size(); ++i)
	{
		int eId = m_vertexAdjEdges[v1][i];
		if (eRemoved[eId])
			continue;

		Edge &e = m_edges[eId];
		if (e.vertex1 == v1)
			e.vertex1 = v2;
		else if (e.vertex2 == v1)
			e.vertex2 = v2;	// well, e.vertex2 SHOULD be equal to v1, or else something is terribly wrong
		else
			ASSERT(0);// panic.

		// if this edge was adjacent to a removed triangle, then it has
		// to be removed.
		bool bValid = true;
		for (size_t j=0; j<e.adjFaces.size(); ++j)
		{
			if (fRemoved[e.adjFaces[j]]) {
				bValid = false;
				eRemoved[eId] = true;
				break;
			}
		}
		if (bValid) {
			m_vertexAdjEdges[v2].push_back(eId);
		}
	}
	ASSERT(m_vertexAdjEdges[v2].size() < 100);

	// We skip updating the adjacent face list o f each edge, since
	// we do not use it.

	// 5- remove vertex v1
	vRemoved[v1] = true;

	// store the vsplit
	vsplit.v1 = v1;
	vsplit.v2 = v2;
	m_vsplits[--m_nextSplit] = vsplit;
	ASSERT(m_nextSplit >= 0);
}

void PMesh::vsplit()
{
	if (m_nextSplit >= (int)m_vsplits.size())
		return;

	VSplit& split = m_vsplits[m_nextSplit++];

	// Apply the next vertex split to the mesh.
	for (size_t i=0; i<split.changedIds.size(); ++i)
		m_indices[split.changedIds[i]] = split.v1;
	m_activeFaceCount += split.nFaces;
}

void PMesh::vcollapse()
{
	if (m_nextSplit <= m_firstSplit)
		return;

	VSplit& split = m_vsplits[--m_nextSplit];

	// Apply the next vertex split to the mesh.
	for (size_t i=0; i<split.changedIds.size(); ++i)
		m_indices[split.changedIds[i]] = split.v2;
	m_activeFaceCount -= split.nFaces;
}

void PMesh::setVertexNum(int nVertices)
{
	if (nVertices > m_nextSplit)
	{
		for (int i=0; i<nVertices - m_nextSplit; ++i)
			vsplit();
	}
	else if (nVertices < m_nextSplit)
	{
		for (int i=0; i<m_nextSplit - nVertices; ++i)
			vcollapse();
	}
}

void PMesh::flipFaces(bool bFlipVNormals)
{
	Mesh::flipFaces(bFlipVNormals);

	// flip the index array (just swap the two first vertices
	// of each triangle, that'll do the trick).
	// do the same with normal ids.
	for (size_t i=0; i<m_indices.size(); i+=3)
	{
		int tmp = m_indices[i];
		m_indices[i] = m_indices[i+1];
		m_indices[i+1] = tmp;
		
		tmp = m_norm_indices[i];
		m_norm_indices[i] = m_norm_indices[i+1];
		m_norm_indices[i+1] = tmp;
	}

	// change the data stored in the vsplit array...
	// (the pain never ends)
	for (size_t i=(size_t)m_firstSplit; i<m_vsplits.size(); ++i)
	{
		VSplit &split = m_vsplits[i];
		for (size_t j=0; j<split.changedIds.size(); ++j)
		{
			if (split.changedIds[j]%3 == 0)
				split.changedIds[j] += 1;
			else if (split.changedIds[j]%3 == 1)
				split.changedIds[j] -= 1;
		}
	}
}

void PMesh::prepareVertexArrays()
{
	std::vector<Vector3> vertices(m_normals.size());
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		Face &f = m_faces[i];
		ASSERT(f.ids.size() == 3);
		ASSERT(f.nids.size() == 3);

		m_indices[i*3  ] = f.nids[0];
		m_indices[i*3+1] = f.nids[1];
		m_indices[i*3+2] = f.nids[2];

		vertices[f.nids[0]] = m_vertices[f.ids[0]];
		vertices[f.nids[1]] = m_vertices[f.ids[1]];
		vertices[f.nids[2]] = m_vertices[f.ids[2]];
		
		f.ids = f.nids;
	}
	m_vertices = vertices;
}