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
#include "3dio/FileLoader.h"
#include "BBox.h"
#include "Edge.h"
#include "Material.h"

class Mesh
{
public:
	class MeshCallback : public FileLoader::IMeshCallback
	{
		Mesh *m_pMesh;
	public:
		MeshCallback(Mesh *pMesh) : m_pMesh(pMesh) { };
		virtual void onMeshBegin(int nVertices=0, int nFaces=0);
		virtual void onMeshEnd();
		virtual void onNewVertex(const Vector3* pos, const Vector3* norm = 0, const Vector2 *uv = 0);
		virtual void onNewFace(const std::vector<int>& ids, int material=0, const std::vector<int>* texids=0);
		virtual void onNewMaterial(const Material& mat);
		virtual void onNewUV2(const Vector2* uv);
	};

	class Face {
	public:
		std::vector<int> ids;
		std::vector<int> adjEdges;
		int				smGroup;	//smoothing group id
		std::vector<int> nids;	// normal ids.
		std::vector<int> tids;	// texture coord ids
		int				materialId;
	};

	/* This mesh is polygonal rather than triangular - this is only for the purposes of this
	 * exercise - a simple triangulation method will be added to convert quads and other
	 * polygons to triangles for the next assignments...
	 */
	std::vector<Vector3> m_vertices;
	std::vector<Vector3> m_normals;
	std::vector<Face>	m_faces;
	std::vector<Edge>	m_edges;
	std::vector<std::vector<int> > m_vertexAdjEdges;
	std::vector<std::vector<int> > m_vertexAdjFaces;

	std::vector<Vector2>	m_texCoords;

	std::vector<Vector3>	m_tangents;
	std::vector<Vector3>	m_binormals;

	std::vector<float>	m_ambOcclCoef;	// per-vertex ambient occlusion coefficients

	BBox	m_bbox;
	GLuint	m_lists;

private:
	class Section
	{
	public:
		GLuint		m_displayList;
		Material	m_material;
	};
	std::vector<Section> m_sections;
	std::vector<Material>	m_materials;

public:
	Mesh();
	virtual ~Mesh();

	virtual void render();
			void renderVertexNormals();

	virtual void clear();
	inline	BBox getBBox() const	{ return m_bbox; }
			void computeBBox();
	virtual void computeNormals(float maxSmoothingAngle = 89);
	virtual void createDisplayLists();

	// per-section ops
			int	getSectionsNum() const			{ return (int)m_sections.size(); }
	Material&	material(int sec)				{ return m_sections[sec].m_material; }
	void	renderSection(int sec);

	virtual void rearrangeFaces();
	virtual void mergeVertices();
	virtual void buildConnectivityInfo();
	virtual void applyVertexPermutation(std::vector<int> perm_map, int nVertices);

	virtual void flipFaces(bool bFlipVNormals = true);
			void triangulate(bool bStrict = false);
			void pack();

	virtual void genTexCoords(TexGenMode mode, TexGenPrimitive prim, Axis axis);

	virtual	void genTangentSpace();
	std::vector<Vector3>& getBinormals();
	std::vector<Vector3>& getTangents();

	void simpleNormals();
	void precalcAmbientOcclusion();
	bool intersectRay(const Vector3& rayOrig, const Vector3& rayDir, double* t);

	virtual bool load(const std::string &filename);
	virtual void store(const std::string &filename);
};