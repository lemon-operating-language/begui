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

void Mesh::MeshCallback::onMeshBegin(int nVertices, int nFaces)
{
	m_pMesh->clear();
	if (nVertices > 0)
		m_pMesh->m_vertices.reserve(nVertices);
	if (nFaces > 0)
		m_pMesh->m_faces.reserve(nFaces);
}

void Mesh::MeshCallback::onMeshEnd()
{
	// Mesh is fully loaded. Compute a bounding box, quick and dirty normals
	// and create display lists to show the thing onscreen.

	m_pMesh->computeBBox();

/*	m_pMesh->mergeVertices();
	m_pMesh->buildConnectivityInfo();
	m_pMesh->rearrangeFaces();

	m_pMesh->computeNormals(180);*/
//	m_pMesh->genTangentSpace();
	m_pMesh->simpleNormals();

//	m_pMesh->pack();

	m_pMesh->createDisplayLists();
}

void Mesh::MeshCallback::onNewVertex(const Vector3 *pos, const Vector3 *norm, const Vector2 *uv)
{
	m_pMesh->m_vertices.push_back(*pos);
}

void Mesh::MeshCallback::onNewFace(const std::vector<int>& ids, int material, const std::vector<int>* texids)
{
	Mesh::Face face;
	face.ids = ids;
	if (texids)
		face.tids = *texids;
	face.materialId = material;
	m_pMesh->m_faces.push_back(face);
}

void Mesh::MeshCallback::onNewMaterial(const Material& mat)
{
	m_pMesh->m_materials.push_back(mat);
}

void Mesh::MeshCallback::onNewUV2(const Vector2 *uv)
{
	ASSERT(uv);
	m_pMesh->m_texCoords.push_back(*uv);
}