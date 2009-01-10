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

Mesh::Mesh() : m_lists(0)
{
}

Mesh::~Mesh()
{
}

void Mesh::render()
{
	// Call the display lists to render the mesh.
	if (m_lists)
	{
		glCallList(m_lists);
	}
	else {
		// fall back
		for (size_t face=0; face<m_faces.size(); ++face)
		{
			glBegin(GL_POLYGON);
			for (size_t i=0; i<m_faces[face].ids.size(); ++i)
			{
				
				if (m_normals.size() > 0) {
					Vector3 &n = m_normals[m_faces[face].nids[i]];
					glNormal3f(n.x, n.y, n.z);
				}
				
				if (m_texCoords.size() > 0) {
					Vector2 &t = m_texCoords[m_faces[face].tids[i]];
				}

				Vector3& v = m_vertices[m_faces[face].ids[i]]; 
					glTexCoord2f(v.x, v.y);
				glVertex3f(v.x, v.y, v.z);
			}
			glEnd();
		}
	}
}

void Mesh::renderVertexNormals()
{
	float normalLen = m_bbox.getMaxDim()/20;

	glBegin(GL_LINES);
	for (size_t face=0; face<m_faces.size(); ++face)
	{
		Face &f = m_faces[face];
		for (size_t i=0; i<f.ids.size(); ++i)
		{
			glVertex3f(m_vertices[f.ids[i]].x, m_vertices[f.ids[i]].y, m_vertices[f.ids[i]].z);
			glVertex3f(m_vertices[f.ids[i]].x + m_normals[f.nids[i]].x*normalLen,
						m_vertices[f.ids[i]].y + m_normals[f.nids[i]].y*normalLen,
						m_vertices[f.ids[i]].z + m_normals[f.nids[i]].z*normalLen);
		}
	}
	glEnd();
}

void Mesh::computeBBox()
{
	if (m_vertices.size() == 0) {
		m_bbox = BBox(Vector3(0,0,0), Vector3(0,0,0));
		return;
	}

	// Compute the bounding box for this mesh
	m_bbox = BBox(m_vertices[0], m_vertices[0]);
	for (size_t v=1; v<m_vertices.size(); ++v)
		m_bbox.add(m_vertices[v]);
}
/*
void Mesh::computeNormals()
{
	// init vertex normals to zero
	m_normals.resize(m_vertices.size());
	for (size_t i=0; i<m_normals.size(); ++i)
		m_normals[i] = Vector3(0,0,0);

	// compute face normals and add to vertex normal
	// Assume TRIANGULAR faces, oriented correctly etc. (ok, just a quick and dirty way to
	// do this - on assignement 2 everything will be much more elaborate.. :-)
	for (i=0; i<m_faces.size(); ++i)
	{
		Face &f = m_faces[i];
		if (f.ids.size() < 3)
			continue;
		Vector3 dv1 = m_vertices[f.ids[1]] - m_vertices[f.ids[0]];
		Vector3 dv2 = m_vertices[f.ids[2]] - m_vertices[f.ids[0]];
		Vector3 normal = dv1.cross(dv2);
		normal.normalize();

		for (size_t j=0; j<f.ids.size(); ++j)
			m_normals[f.ids[j]] += normal;

		f.normal = normal;
	}

	// normalize
	for (i=0; i<m_normals.size(); ++i)
		m_normals[i].normalize();
}*/

void Mesh::clear()
{
	glDeleteLists(m_lists, 1);
	m_vertices.clear();
	m_faces.clear();
	m_normals.clear();
	m_texCoords.clear();
	m_vertexAdjEdges.clear();
	m_vertexAdjFaces.clear();
	m_edges.clear();
	m_bbox = BBox();
}

void Mesh::mergeVertices()
{
	// Compute bbox if not computed
	if (m_bbox.getSize().length2() == 0)
		computeBBox();

	Console::print("Merging vertices...\n");

	// Create buckets
	std::vector<int> vmap(m_vertices.size());
	std::vector<int> vused(m_vertices.size());
	int nVertices = (int)m_vertices.size();

	int nbx = 1+powf(m_vertices.size(), 1.0f/3)/2;
	int nby = 1+powf(m_vertices.size(), 1.0f/3)/2;
	int nbz = 1+powf(m_vertices.size(), 1.0f/3)/2;
	int nBuckets = nbx*nby*nbz;
	std::vector<std::vector<int> > pVertexBuckets(nBuckets);

	Console::print("\tCreated %d buckets (%d vertices)\n", nBuckets, m_vertices.size());
	Console::print("\tDistributing vertices to buckets...\n");
	Vector3 bboxSz = m_bbox.getSize();
	for (size_t i=0; i<m_vertices.size(); ++i)
	{
		vmap[i] = (int)i;
		vused[i] = true;

		int bx = (int)((nbx-1)*(m_vertices[i].x - m_bbox.m_min.x)/bboxSz.x);
		int by = (int)((nby-1)*(m_vertices[i].y - m_bbox.m_min.y)/bboxSz.y);
		int bz = (int)((nbz-1)*(m_vertices[i].z - m_bbox.m_min.z)/bboxSz.z);
		int bucket = bz*nbx*nby + by*nbx + bx;

		pVertexBuckets[bucket].push_back((int)i);
	}
	Console::print("\tMerging buckets...\n");

	// merge buckets.
	for (size_t i=0; i<nBuckets; ++i)
	{
		for (size_t j=0; j<pVertexBuckets[i].size(); ++j)
		{
			int vid = pVertexBuckets[i][j];	
			for (size_t k=0; k<j; ++k)
			{
				int vid2 = pVertexBuckets[i][k];
				if (m_vertices[vid] == m_vertices[vid2])
				{
					vmap[vid2] = vid;
					vused[vid2] = false;
				}
			}
		}
	}
	
	Console::print("\tRearranging mesh data...\n");

	// Change the vertex permutation so that all removed
	// vertices are at the back (since they will be discarded)
	int nV = 0;
	for (size_t i=0; i<(int)vmap.size(); ++i)
	{
		if (vused[i])
			vmap[i] = nV++;
	}
	for (size_t i=0; i<(int)vmap.size(); ++i)
	{
		if (!vused[i])
			vmap[i] = vmap[vmap[i]];
	}

	// apply the vertex permutation
	applyVertexPermutation(vmap, nV);

	Console::print("\tDone.\n");
	Console::print("After merging: %d vertices, %d faces\n", m_vertices.size(), m_faces.size());
}

void Mesh::applyVertexPermutation(std::vector<int> perm_map, int nVertices)
{
	// build the new vertex array with vertices rearranged
	// according to the given permutation
	std::vector<Vector3> vertices(nVertices);
	for (size_t i=0; i<m_vertices.size(); ++i)
	{
		vertices[perm_map[i]] = m_vertices[i];
	}
	m_vertices = vertices;
	vertices.clear();
	
	std::vector<std::vector<int> > vAdjE;
	std::vector<std::vector<int> > vAdjF;
	if (m_vertexAdjEdges.size() > 0)
		vAdjE.resize(nVertices);
	if (m_vertexAdjFaces.size() > 0)
		vAdjF.resize(nVertices);
	for (size_t i=0; i<m_vertices.size(); ++i)
	{
		if (m_vertexAdjEdges.size() > 0)
			vAdjE[perm_map[i]] = m_vertexAdjEdges[i];
		if (m_vertexAdjFaces.size() > 0)
			vAdjF[perm_map[i]] = m_vertexAdjFaces[i];
	}
	m_vertexAdjEdges = vAdjE;
	vAdjE.clear();
	m_vertexAdjFaces = vAdjF;
	vAdjF.clear();

	// update edges
	for (size_t i=0; i<m_edges.size(); ++i)
	{
		Edge &e = m_edges[i];
		e.vertex1 = perm_map[e.vertex1];
		e.vertex2 = perm_map[e.vertex2];
	}

	// change vertex indices in faces.
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		for (size_t j=0; j<m_faces[i].ids.size(); ++j)
			m_faces[i].ids[j] = perm_map[m_faces[i].ids[j]];
	}
}

void Mesh::buildConnectivityInfo()
{
	Console::print("Generating connectivity data...\n");

	m_edges.clear();
	m_vertexAdjEdges.clear();
	m_vertexAdjEdges.resize(m_vertices.size());
	m_vertexAdjFaces.clear();
	m_vertexAdjFaces.resize(m_vertices.size());

	for (size_t i=0; i<m_faces.size(); ++i)
	{
		m_faces[i].adjEdges.clear();
		
		// build vertex-face adjacency array
		for (size_t j=0; j<m_faces[i].ids.size(); ++j)
		{
			m_vertexAdjFaces[m_faces[i].ids[j]].push_back((int)i);
		}

		// detect degenerate cases
		if (m_faces[i].ids.size() < 2)
			continue;

		int prevId = m_faces[i].ids.back();
		for (size_t j=0; j<m_faces[i].ids.size(); ++j)
		{
			int id = m_faces[i].ids[j];
			// edge (prevId, id)

			if (prevId == id)
				continue;

			// check if this edge already exists. If it has been
			// already created, then it will be present on the
			// edge adjacency lists for both vertices id and prevId.
			// Just check one of them.
			bool bExists = false;
			for (size_t k=0; k<m_vertexAdjEdges[id].size(); ++k)
			{
				Edge &e = m_edges[m_vertexAdjEdges[id][k]];
				if ((e.vertex1 == id && e.vertex2 == prevId) ||
					(e.vertex2 == id && e.vertex1 == prevId))
				{
					bExists = true;
					
					// add this face to the adjacent faces to this edge
					e.adjFaces.push_back((int)i);
					m_faces[i].adjEdges.push_back(/*k);/*/m_vertexAdjEdges[id][k]);

					prevId = id;

					break;
				}
			}
			if (bExists)
				continue;

			// if we got here, edge does not exists, so create it:
			Edge edge;
			edge.vertex1 = prevId;
			edge.vertex2 = id;
			edge.adjFaces.push_back(static_cast<int>(i));

			// add it to the edge list
			int edgeId = (int)m_edges.size();
			m_edges.push_back(edge);

			// add it to the face-edge adj list
			m_faces[i].adjEdges.push_back(edgeId);

			// add a reference to the vertex-edge adjacency lists
			m_vertexAdjEdges[prevId].push_back(edgeId);
			m_vertexAdjEdges[id].push_back(edgeId);

			prevId = id;
		}
	}

	Console::print("\tCreated %d edges\n", m_edges.size());
}

inline int firstId(int id1, int id2, std::vector<int>& ids)
{
	if (ids.front() == id2 && ids.back() == id1)
		return id1;
	else if (ids.front() == id1 && ids.back() == id2)
		return id2;
	else {
		for (size_t i=0; i<ids.size(); ++i)
		{
			if (ids[i] == id1)
				return id1;
			if (ids[i] == id2)
				return id2;
		}
	}
	return -1;
}

void Mesh::rearrangeFaces()
{
	Console::print("Rearranging face orientation...\n");

	// -works with polygons (not only triangles)
	// -fine with non-manifold meshes
	// -works ok with disconnected mesh parts
	// -does so fast :)

	if (m_edges.size() == 0)
		buildConnectivityInfo();

	// Start from a face and walk over the surface of the
	// mesh, reorienting normals
	std::list<int> stack;
	std::vector<bool> visited(m_faces.size(), false);

	int components = 0;
	for (int i=0; i<(int)m_faces.size(); ++i)
	{
		if (visited[i])
			continue;
		components++;

		stack.push_back(i);

		// start processing
		do {
			int face = stack.back();
			stack.pop_back();
			if (visited[face])
				continue;
			visited[face] = true;

			for (size_t j=0; j<m_faces[face].adjEdges.size(); ++j)
			{
				Edge &e = m_edges[m_faces[face].adjEdges[j]];

				int fV1 = firstId(e.vertex1, e.vertex2, m_faces[face].ids);

				for (size_t k=0; k<e.adjFaces.size(); ++k)
				{
					int adjF = e.adjFaces[k];
					if (adjF == face || visited[adjF])
						continue;

					bool bDuplicate = true;
					for (size_t i1=0; i1<m_faces[face].ids.size(); ++i1)
					{
						bool bFound = false;
						for (size_t i2=0; i2<m_faces[adjF].ids.size(); ++i2)
						{
							if (m_faces[face].ids[i1] == m_faces[adjF].ids[i2])
								bFound = true;
						}
						if (!bFound) {
							bDuplicate = false;
							break;
						}
					}
					if (bDuplicate)
						continue;

					// reorient triangle
					int fV2 = firstId(e.vertex1, e.vertex2, m_faces[adjF].ids);
					if (fV1 == fV2) {
						// flip.
						std::vector<int> fids(m_faces[adjF].ids.size());
						for (size_t m=0; m<m_faces[adjF].ids.size(); ++m)
							fids[fids.size()-m-1] = m_faces[adjF].ids[m];
						m_faces[adjF].ids = fids;
					}

					stack.push_back(adjF);
				}
			}
		}
		while (stack.size() > 0);
	}

	Console::print("Done.%d\n", components);
}

void Mesh::simpleNormals()
{
	Console::print("Computing normals... ");

	m_normals.resize(m_vertices.size(), Vector3(0,0,0));

	// Compute face normals (assuming PLANAR faces!).
	std::vector<Vector3> faceNormals(m_faces.size());
	for (size_t i=0; i<faceNormals.size(); ++i)
		faceNormals[i] = Vector3(0,0,0);
	for (size_t i=0; i<(int)m_faces.size(); ++i)
	{
		Face &f = m_faces[i];
		
		f.nids = f.ids;

		if (f.ids.size() < 3)
			continue;
		Vector3 dv1 = m_vertices[f.ids[1]] - m_vertices[f.ids[0]];
		Vector3 dv2 = m_vertices[f.ids[2]] - m_vertices[f.ids[0]];
		faceNormals[i] = dv1.cross(dv2);
		faceNormals[i].normalize();

		m_normals[f.ids[0]] += faceNormals[i];
		m_normals[f.ids[1]] += faceNormals[i];
		m_normals[f.ids[2]] += faceNormals[i];
	}

	for (size_t i=0; i<m_normals.size(); ++i)
		m_normals[i].normalize();

	Console::print("done\n");
}

void Mesh::computeNormals(float maxSmoothingAngle)
{
	Console::print("Creating smoothing groups... (smoothing angle = %f)\n", maxSmoothingAngle);

	maxSmoothingAngle *= 3.14159f/180.0f;

	// -works with polygons (not only triangles)
	// -fine with non-manifold meshes
	// -works ok with disconnected mesh parts
	// -does so fast :)

	if (m_edges.size() == 0)
		buildConnectivityInfo();

	// Compute face normals (assuming PLANAR faces!).
	std::vector<Vector3> faceNormals(m_faces.size());
	for (size_t i=0; i<faceNormals.size(); ++i)
		faceNormals[i] = Vector3(0,0,0);
	for (size_t i=0; i<(int)m_faces.size(); ++i)
	{
		Face &f = m_faces[i];
		if (f.ids.size() < 3)
			continue;
		Vector3 dv1 = m_vertices[f.ids[1]] - m_vertices[f.ids[0]];
		Vector3 dv2 = m_vertices[f.ids[2]] - m_vertices[f.ids[0]];
		faceNormals[i] = dv1.cross(dv2);
		faceNormals[i].normalize();
	}

	// Start from a face and walk over the surface of the
	// mesh, identifying the smoothing groups
	std::list<int> stack;
	std::vector<bool> visited(m_faces.size(), false);

	int nSmoothingGroups = 0;
	for (size_t i=0; i<(int)m_faces.size(); ++i)
	{
		if (visited[i])
			continue;

		stack.push_back(i);

		// start processing
		do {
			int face = stack.back();
			stack.pop_back();
			if (visited[face])
				continue;
			visited[face] = true;

			m_faces[face].smGroup = nSmoothingGroups;

			for (size_t j=0; j<m_faces[face].adjEdges.size(); ++j)
			{
				Edge &e = m_edges[m_faces[face].adjEdges[j]];

				for (size_t k=0; k<e.adjFaces.size(); ++k)
				{
					int adjF = e.adjFaces[k];
					if (adjF == face || visited[adjF])
						continue;
					
				/*	bool bDuplicate = true;
					for (int i1=0; i1<m_faces[face].ids.size(); ++i1)
					{
						bool bFound = false;
						for (int i2=0; i2<m_faces[adjF].ids.size(); ++i2)
						{
							if (m_faces[face].ids[i1] == m_faces[adjF].ids[i2])
								bFound = true;
						}
						if (!bFound) {
							bDuplicate = false;
							break;
						}
					}
					if (bDuplicate)
						continue;*/

					// find the angle between the two face normals
					// and compare it to the smoothing angle threshold
					Vector3 dbg1 = faceNormals[face];
					Vector3 dbg2 = faceNormals[adjF];
					float dot = dbg1.dot(dbg2);
					float angle = faceNormals[face].angle(faceNormals[adjF]);
					if (angle < maxSmoothingAngle)
					{
						// expand the smoothing group to this triangle
						stack.push_back(adjF);
					}
				}
			}
		}
		while (stack.size() > 0);

		nSmoothingGroups++;
	}

	Console::print("\tDone.\n");
	Console::print("\tCreated %d smoothing groups\n", nSmoothingGroups);
	Console::print("\tCreating face normal ids...\n");

	// set normal indices for faces.
	m_normals.resize(0);
	for (int s=0; s<nSmoothingGroups; ++s)
	{
		std::vector<int> nmap(m_vertices.size(), -1);
		for (size_t i=0; i<m_faces.size(); ++i)
		{
			Face &face = m_faces[i];
			if (face.smGroup != s)
				continue;
			face.nids.resize(face.ids.size());
			for (size_t j=0; j<face.ids.size(); ++j)
			{
				if (nmap[face.ids[j]] == -1)
				{
					m_normals.push_back(Vector3(0,0,0));
					face.nids[j] = (int)m_normals.size()-1;
					nmap[face.ids[j]] = (int)m_normals.size()-1;
				}
				else
					face.nids[j] = nmap[face.ids[j]];
			}
		}
	}

	// compute normals.
	Console::print("\tComputing face normals...\n");
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		Face &face = m_faces[i];
		for (size_t j=0; j<face.nids.size(); ++j)
		{
			m_normals[face.nids[j]] += faceNormals[i];
		}
	}

	// renormalize normals
	for (size_t i=0; i<m_normals.size(); ++i)
	{
		m_normals[i].normalize();
	}

	Console::print("\tDone\n");
}

void Mesh::flipFaces(bool bFlipVNormals)
{
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		std::vector<int> fids(m_faces[i].ids.size());
		for (size_t m=0; m<m_faces[i].ids.size(); ++m)
			fids[fids.size()-m-1] = m_faces[i].ids[m];
		m_faces[i].ids = fids;
		
		std::vector<int> nids(m_faces[i].nids.size());
		for (size_t m=0; m<m_faces[i].nids.size(); ++m)
			nids[nids.size()-m-1] = m_faces[i].nids[m];
		m_faces[i].nids = nids;
	}

	// flip normals too
	if (bFlipVNormals)
	{
		for (size_t i=0; i<m_normals.size(); ++i)
			m_normals[i] = -m_normals[i];
	}
}

void Mesh::triangulate(bool bStrict)
{
	// naive fan-like triangulation. Works only for CONVEX
	// triangles
	Console::print("Triangulating mesh..\n");

	std::vector<Face> faces;
	faces.reserve(m_faces.size());
	for (size_t i=0; i<m_faces.size(); ++i)
	{
		if (m_faces[i].ids.size() < 3) {
			if (!bStrict)
				faces.push_back(m_faces[i]);
			continue;
		}

		int orgId = m_faces[i].ids[0];
		int prevId = m_faces[i].ids[1];
		int orgNId = m_faces[i].nids[0];
		int prevNId = m_faces[i].nids[1];
		for (size_t j=2; j<m_faces[i].ids.size(); ++j)
		{
			int id = m_faces[i].ids[j];
			int nid = m_faces[i].nids[j];
			
			Face face;
			face.ids.resize(3);
			face.ids[0] = orgId;
			face.ids[1] = prevId;
			face.ids[2] = id;
			
			face.nids.resize(3);
			face.nids[0] = orgNId;
			face.nids[1] = prevNId;
			face.nids[2] = nid;

			faces.push_back(face);

			prevId = id;
			prevNId = nid;
		}
	}
	m_faces = faces;

	Console::print("\tFace count after triangulation: %d\n", m_faces.size());
}

/**
 * Pack data structures to avoid wasting memory unnecessarily.
 *
 */
template <typename T> void packVector(std::vector<T>& v)
{
	std::vector<T> newV(v.size());
	newV.assign(v.begin(), v.end());
	v.clear();
	v = newV;
}
void Mesh::pack()
{
	packVector(m_vertices);
	packVector(m_faces);
	packVector(m_normals);
	packVector(m_vertexAdjFaces);
	packVector(m_vertexAdjEdges);
}

void Mesh::genTangentSpace()
{
	ASSERT(m_normals.size() > 0);

	m_tangents.resize(m_vertices.size());
	for (int i=0; i<m_tangents.size(); ++i)
		m_tangents[i] = Vector3(0,0,0);

/*	for (i=0; i<m_faces.size(); ++i)
	{
		for (int j=0; j<m_faces[i].ids.size(); ++j)
		{
			int id = m_faces[i].ids[j];

			Vector3 v0 = m_vertices[m_faces[i].ids[0]];
			if (id == 0)
				v0 = m_vertices[m_faces[i].ids.back()];

			Vector3 t = m_vertices[id] - v0;

			if (m_tangents[id].length() > 0.000001)
			{
				// there's already some tangent info
				// check consistency
				if (t.dot(m_tangents[id]) < 0)
					m_tangents[id] += -t;
				else
					m_tangents[id] += t;
			}
			else
				m_tangents[id] = t;
		}
	}*/

	// reorthogonalize and normalize
	Vector3 base1(1,0,0);
	Vector3 base2(0,1,0);
	Vector3 base3(0,0,1);
	for (size_t i=0; i<m_tangents.size(); ++i)
	{
	/*	Vector3 reorth = m_normals[i]*(m_normals[i].dot(m_tangents[i]));
		m_tangents[i] -= reorth;
		m_tangents[i].normalize();*/
		Vector3 t;
		Vector3 n = m_normals[i];
		if (fabs(n.dot(base1)) > 0.01)
			t = n.cross(base1);
		else
			t = (n.cross(base2)).cross(base1);
		t.normalize();
		m_tangents[i] = t;
	}

	// generate binormals
	m_binormals.resize(m_vertices.size());
	for (size_t i=0; i<m_tangents.size(); ++i)
	{
		m_binormals[i] = m_normals[i].cross(m_tangents[i]);
		m_binormals[i].normalize();
	}
}

bool Mesh::load(const std::string &filename)
{
	clear();

	FILE *fp = fopen(filename.c_str(), "rb");
	if (!fp)
		return false;

	int nVertices;
	fread(&nVertices, 1, sizeof(int), fp);
	m_vertices.resize(nVertices);
	fread(&m_vertices[0], nVertices, sizeof(Vector3), fp);

	int nNormals;
	fread(&nNormals, 1, sizeof(int), fp);
	m_normals.resize(nNormals);
	fread(&m_normals[0], nNormals, sizeof(Vector3), fp);
	
	int nTexCoords;
	fread(&nTexCoords, 1, sizeof(int), fp);
	m_texCoords.resize(nTexCoords);
	fread(&m_texCoords[0], nTexCoords, sizeof(Vector2), fp);

	int nAmbOcclCoefs;
	fread(&nAmbOcclCoefs, 1, sizeof(int), fp);
	m_ambOcclCoef.resize(nAmbOcclCoefs);
	fread(&m_ambOcclCoef[0], nAmbOcclCoefs, sizeof(float), fp);

	int nFaces;
	fread(&nFaces, 1, sizeof(int), fp);
	m_faces.resize(nFaces);
	for (int i=0; i<nFaces; ++i)
	{
		// load face data
		int nIds, nNIds;
		fread(&nIds, 1, sizeof(int), fp);
		fread(&nNIds, 1, sizeof(int), fp);
		fread(&m_faces[i].smGroup, 1, sizeof(int), fp);
		fread(&m_faces[i].materialId, 1, sizeof(int), fp);
		for (int j=0; j<nIds; ++j)
		{
			int id;
			fread(&id, 1, sizeof(int), fp);
			m_faces[i].ids.push_back(id);
		}
		for (int j=0; j<nNIds; ++j)
		{
			int id;
			fread(&id, 1, sizeof(int), fp);
			m_faces[i].nids.push_back(id);
		}
	}

	fclose(fp);

	return true;
}

void Mesh::renderSection(int sec)
{
	ASSERT(sec >= 0 && sec < m_sections.size());
	glCallList(m_sections[sec].m_displayList);
}

void Mesh::store(const std::string& filename)
{
	FILE *fp = fopen(filename.c_str(), "wb+");
	if (!fp)
		return;

	int nVertices = m_vertices.size();
	fwrite(&nVertices, 1, sizeof(int), fp);
	fwrite(&m_vertices[0], nVertices, sizeof(Vector3), fp);

	int nNormals = m_normals.size();
	fwrite(&nNormals, 1, sizeof(int), fp);
	fwrite(&m_normals[0], nNormals, sizeof(Vector3), fp);
	
	int nTexCoords = m_texCoords.size();
	fwrite(&nTexCoords, 1, sizeof(int), fp);
	fwrite(&m_texCoords[0], nTexCoords, sizeof(Vector2), fp);

	int nAmbOcclCoefs = m_ambOcclCoef.size();
	fwrite(&nAmbOcclCoefs, 1, sizeof(int), fp);
	fwrite(&m_ambOcclCoef[0], nAmbOcclCoefs, sizeof(float), fp);

	int nFaces = m_faces.size();
	fwrite(&nFaces, 1, sizeof(int), fp);
	for (int i=0; i<nFaces; ++i)
	{
		// load face data
		int nIds = m_faces[i].ids.size(), nNIds = m_faces[i].nids.size();
		fwrite(&nIds, 1, sizeof(int), fp);
		fwrite(&nNIds, 1, sizeof(int), fp);
		fwrite(&m_faces[i].smGroup, 1, sizeof(int), fp);
		fwrite(&m_faces[i].materialId, 1, sizeof(int), fp);
		for (int j=0; j<nIds; ++j)
			fwrite(&m_faces[i].ids[j], 1, sizeof(int), fp);
		for (int j=0; j<nNIds; ++j)
			fwrite(&m_faces[i].nids[j], 1, sizeof(int), fp);
	}

	fclose(fp);
}
void Mesh::createDisplayLists()
{
	const int NMAX_FACES_PER_LIST = 32768;

	Console::print("Creating sections and display lists for mesh..\n");

	// seperate faces to sections, given their material
	std::vector<int> materials;
	std::vector< std::list<int> > facesPerMat;
	for (size_t face=0; face<m_faces.size(); ++face)
	{
		// check if we have already a list for this material. If so,
		// add this face to the existing list
		bool bExists = false;
		for (size_t i=0; i<materials.size(); ++i)
		{
			if (m_faces[face].materialId == materials[i])
			{
				bExists = true;
				facesPerMat[i].push_back(face);
				break;
			}
		}

		// if we first encounter this material, create a new list of faces
		// and add this face to the new list
		if (!bExists)
		{
			materials.push_back(m_faces[face].materialId);
			facesPerMat.push_back(std::list<int>());
			facesPerMat.back().push_back(face);
		}
	}
	Console::print("\t-%d materials used in mesh\n", materials.size());
	
	// count number of lists
	int nLists = 0;
	for (size_t i=0; i<facesPerMat.size(); ++i)
	{
		nLists++;

		// if this list contains more than 32k faces, break it up
		nLists += facesPerMat[i].size()/NMAX_FACES_PER_LIST;

		Console::print("\t\t-%d faces with material %d\n", facesPerMat[i].size(), i);
	}

	// Create sections and display lists
	m_sections.resize(nLists);
	for (int i=0; i<nLists; ++i)
	{
		// Create display lists for this mesh
		m_sections[i].m_displayList = glGenLists(1);
		if (m_sections[i].m_displayList == 0) {
			Console::print("ERROR: Failed to create display lists\n");
			return;
		}
	}
	Console::print("\t-created %d mesh sections\n", nLists);

	// Fill in the display lists
	int curSection = 0;
	for (int s=0; s<facesPerMat.size(); ++s)
	{
		glNewList(m_sections[curSection].m_displayList, GL_COMPILE);
		
		// set the material for this section
		if (m_materials.size() > materials[s])
			m_sections[curSection].m_material = m_materials[materials[s]];
		else
			Console::print("WARNING: unreferenced materials\n");
		
		// Assume that all faces are triangles
		glBegin(GL_TRIANGLES);

		int nFacesAdded = 0;
		for (std::list<int>::iterator it=facesPerMat[s].begin(); it!=facesPerMat[s].end(); ++it)
		{
			int face = (*it);
			
			// check if it is time to go to the next display list/section
			if (nFacesAdded>0 && (nFacesAdded % NMAX_FACES_PER_LIST == 0))
			{
				glEnd();
				glEndList();
				curSection++;
				glNewList(m_sections[curSection].m_displayList, GL_COMPILE);
				if (m_materials.size() > materials[s])
					m_sections[curSection].m_material = m_materials[materials[s]];
				else
					Console::print("WARNING: unreferenced materials\n");
				glBegin(GL_TRIANGLES);
			}
			nFacesAdded++;

			// Check if the mesh contains only triangles
			if (m_faces[face].ids.size() != 3)
			{
				Console::print("ERROR: mesh not triangular!\n");
				return;
			}

			for (size_t i=0; i<m_faces[face].ids.size(); ++i)
			{
				if (m_normals.size() > 0) {
					Vector3 &n = m_normals[m_faces[face].nids[i]];
					glNormal3f(n.x, n.y, n.z);
				}
			
				if (m_texCoords.size() > 0 && m_faces[face].tids.size() > 0) {
					Vector2 &t = m_texCoords[m_faces[face].tids[i]];
					glTexCoord2f(t.x, t.y);
				}

			/*	if (m_tangents.size() > 0) {
					Vector3 &t = m_tangents[m_faces[face].nids[i]];
					glColor3f(t.x, t.y, t.z);
				}*/
				if (m_ambOcclCoef.size() > 0) {
					glColor3f(m_ambOcclCoef[m_faces[face].ids[i]], 0, 0);
				}

				Vector3 &v = m_vertices[m_faces[face].ids[i]]; 
				glVertex3f(v.x, v.y, v.z);
			}
		}

		curSection++;

		glEnd();
		glEndList();
	}
}