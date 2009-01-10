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

// OBJLoader.cpp: implementation of the OBJLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "OBJLoader.h"
#include "../Material.h"
#include "../Shader.h"
#include "../CgManager.h"

struct OBJ_face_desc
{
	int v1, v2, v3;
	int n1, n2, n3;
	int t1, t2, t3;

	int mat_id;

	int id11, id21, id31;
	int id12, id22, id32;
};

struct OBJ_vtn_triplet
{
	int v, t, n;
	int id;

	bool operator == (const OBJ_vtn_triplet& tr)
	{
		if (v != tr.v)
			return false;
		if (t != tr.t)
			return false;
		if (n != tr.n)
			return false;
		return true;
	};
};

std::string GetDirectoryFromPath(std::string path)
{
	std::string dir;
	bool copying = false;
	for (int i=path.length(); i>=0; i--)
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			copying = true;
		}
		if (copying)
		{
			dir = path[i] + dir;
		}
	}
	return dir;
}

bool OBJLoader::loadFile(const std::string& filename)
{
	ASSERT(filename.length() > 0);
	ASSERT(m_pMeshCallback);

	if (!m_file.loadFile(filename.c_str()))
	{
		Console::print("OBJLoader: Error opening file %s\n", filename.c_str());
		return false;
	}

	Console::print("Loading OBJ file...\n");

	// Get working directory.
	std::string work_dir = GetDirectoryFromPath(filename);

	// Find the materials library.
	std::string token;
	while (token != "mtllib" && !m_file.eof())
	{
		m_file >> token;

		if (token == "mtllib")
		{
			std::string mtllib_str;
			mtllib_str = m_file.getline();
			m_MtlLibPath = mtllib_str;
			if (GetDirectoryFromPath(m_MtlLibPath).length() == 0)
				m_MtlLibPath = work_dir + m_MtlLibPath;
			Console::print("\tmaterial library in: %s\n", m_MtlLibPath.c_str());

			// load all materials
			if (!loadMatLib())
				Console::print("ERROR: failed to load materials\n");
		}
	}

	// Now start reading the data from the file.
	std::vector<Vector3> vpos;
	std::vector<Vector3> vnorm;
	std::vector<Vector2> vtex;
	std::vector<OBJ_face_desc> faces;
	
	int cur_material = 0;
	
	m_file.goToStart();
	while (!m_file.eof())
	{
		m_file >> token;

		if (token == "v")
		{
			double x,y,z;
			m_file >> x;
			m_file >> y;
			m_file >> z;
			vpos.push_back(Vector3(x,y,z));
		}
		else
		if (token == "vn")
		{
			double x,y,z;
			m_file >> x;
			m_file >> y;
			m_file >> z;
			vnorm.push_back(Vector3(x,y,z));
		}
		else
		if (token == "vt")
		{
			double x,y;
			m_file >> x;
			m_file >> y;
			vtex.push_back(Vector2(x,y));
		}
		else if (token == "f")
		{
			//face_count = faces.size();
			//faces.push_back(OBJ_face_desc());

		/*	ReadFaceVertex(&faces[face_count].v1, &faces[face_count].t1, &faces[face_count].n1);
			ReadFaceVertex(&faces[face_count].v2, &faces[face_count].t2, &faces[face_count].n2);
			ReadFaceVertex(&faces[face_count].v3, &faces[face_count].t3, &faces[face_count].n3);

			faces[face_count].id11 = faces[face_count].id21 = faces[face_count].id31 = -1;
			faces[face_count].id12 = faces[face_count].id22 = faces[face_count].id32 = 0;

			faces[face_count].mat_id = cur_material;*/

			OBJ_face_desc face;
//			m_file >> face.v1;
//			m_file >> face.v2;
//			m_file >> face.v3;

			ReadFaceVertex(&face.v1, &face.t1, &face.n1);
			ReadFaceVertex(&face.v2, &face.t2, &face.n2);
			ReadFaceVertex(&face.v3, &face.t3, &face.n3);

			face.mat_id = cur_material;
			faces.push_back(face);

			//Console::print("f %d %d %d (%d)\n", face.v1, face.v2, face.v3, vpos.size());

//			if (file.just_changed_line())
//				continue;
		}
		else if (token == "usemtl")
		{
			// Change cur_material.

			// Get material name
			m_file >> token;	// material name.

			// check if the material with the given name already exists
			cur_material = 0;
			for (size_t i=0; i<m_materials.size(); ++i)
			{
				if (m_materials[i].m_name == token)
				{
					break;
				}
				cur_material++;
			}

			// if it is a new material
			if (cur_material == m_materials.size())
			{
				Console::print("WARNING: unknown material: %s\n", token.c_str());
				Material mat;
				mat.m_name = token;
				m_materials.push_back(mat);
				cur_material = m_materials.size()-1;
			}
			//Material mat;
			//LoadMaterial(token, &mat);
			//materials.push_back(mat);
			
//			if (file.just_changed_line())
//				continue;
		}
//		else
//			m_file.getline();
	}

	m_file.close();
	
	Console::print("\t-read %d vertices, %d normals, %d tex coords, %d faces\n", vpos.size(), vnorm.size(), vtex.size(), faces.size());

	m_pMeshCallback->onMeshBegin(vpos.size(), faces.size());
	for (size_t i=0; i<faces.size(); ++i)
	{
		std::vector<int> ids;
		if (faces[i].v1 < 1 || faces[i].v1>vpos.size())
		{ MessageBox(0,"invalid vertex id","Error",MB_OK); exit(-1); }
		if (faces[i].v2 < 1 || faces[i].v2>vpos.size())
		{ MessageBox(0,"invalid vertex id","Error",MB_OK); exit(-1); }
		if (faces[i].v3 < 1 || faces[i].v3>vpos.size())
		{ MessageBox(0,"invalid vertex id","Error",MB_OK); exit(-1); }
		ids.push_back(faces[i].v1-1);
		ids.push_back(faces[i].v2-1);
		ids.push_back(faces[i].v3-1);

		std::vector<int> tids;
		if (faces[i].t1 > 0 && faces[i].t1<=vtex.size() && 
			faces[i].t2 > 0 && faces[i].t2<=vtex.size() && 
			faces[i].t3 > 0 && faces[i].t3<=vtex.size())
		{
			tids.push_back(faces[i].t1-1);
			tids.push_back(faces[i].t2-1);
			tids.push_back(faces[i].t3-1);
		}
		m_pMeshCallback->onNewFace(ids, faces[i].mat_id, (tids.size() > 0)?&tids : 0);
	}
	for (size_t i=0; i<vpos.size(); ++i)
	{
		m_pMeshCallback->onNewVertex(&vpos[i], (vnorm.size()>0)?&vnorm[i]:0, 0);//(vtex.size()>0)?&vtex[i]:0);
	}
	for (size_t i=0; i<vtex.size(); ++i)
	{
		m_pMeshCallback->onNewUV2(&vtex[i]);
	}
	m_pMeshCallback->onMeshEnd();

	return true;
}

void OBJLoader::ReadFaceVertex(int *vid, int *tid, int *nid)
{
	ASSERT(vid);
	ASSERT(tid);
	ASSERT(nid);

	std::string tmp_str;
	m_file >> tmp_str;
	char val_str[64];
	int l = 0;
	while(l<tmp_str.length() && tmp_str[l] != '/' && !isspace(tmp_str[l]))
	{
		val_str[l] = tmp_str[l];
		l++;
	}
	val_str[l] = 0;
	ASSERT(l > 0);
	*vid = atoi(val_str);
	
	l++;
	int offs = l;
	while(l<tmp_str.length() && tmp_str[l] != '/' && !isspace(tmp_str[l]))
	{
		val_str[l-offs] = tmp_str[l];
		l++;
	}
	val_str[l-offs] = 0;
	if (l-offs != 0)
		*tid = atoi(val_str);
	
	l++;
	offs = l;
	for (;l<tmp_str.size(); ++l)
	{
		val_str[l-offs] = tmp_str[l];
	}
	val_str[l-offs] = 0;
	if (l-offs != 0)
		*nid = atoi(val_str);
}

bool OBJLoader::loadMatLib()
{
	BaseTextFile file;
	if (!file.loadFile(m_MtlLibPath))
		return false;

	std::string token;
	while (!file.eof())
	{
		file >> token;
		if (token == "newmtl")
		{
			file >> token;
			// Create a new material with this name
			Material mat;
			mat.m_name = token;
			m_materials.push_back(mat);
		}
		else if (token == "Kd")
		{
			Material &mat = m_materials.back();
			file >> mat.m_diffuse.r;
			file >> mat.m_diffuse.g;
			file >> mat.m_diffuse.b;
		}
		else if (token == "Ks")
		{
			Material &mat = m_materials.back();
			file >> mat.m_specular.r;
			file >> mat.m_specular.g;
			file >> mat.m_specular.b;
		}
		else if (token == "Ns")
		{
			Material &mat = m_materials.back();
			file >> mat.m_specPower;
		}
		else if (token == "Ka")
		{
			Material &mat = m_materials.back();
			file >> mat.m_ambient.r;
			file >> mat.m_ambient.g;
			file >> mat.m_ambient.b;
		}
		else if (token == "Nt")	// transparency
		{
			Material &mat = m_materials.back();
			file >> mat.m_transparency;
		}
		else if (token == "Fp")	// fresnel params
		{
			Material &mat = m_materials.back();
			file >> mat.m_fresnelScale;
			file >> mat.m_fresnelPower;
			file >> mat.m_fresnelOffs;
		}
		else if (token == "Nr")	// reflectivity
		{
			Material &mat = m_materials.back();
			file >> mat.m_reflectivity;
		}
		else if (token == "Ke")	// emissive
		{
			Material &mat = m_materials.back();
			file >> mat.m_emissive.r;
			file >> mat.m_emissive.g;
			file >> mat.m_emissive.b;
		}
		else if (token == "shader")
		{
			// Load the shader and its parameters
			Shader *pShader = ShaderManager::newShader();
			if (!pShader->parseParamsAndLoad(file))
			{
				Console::print("ERROR: failed to load shader from .mtl file\n");
				break;
			}

			Material &mat = m_materials.back();
			if (pShader->getType() == Shader::VERTEX)
			{
				mat.m_pVertShader = pShader;
			}
			else if (pShader->getType() == Shader::FRAGMENT)
			{
				mat.m_pFragShader = pShader;
			}
		}
	}

	file.close();

	for (size_t i=0; i<m_materials.size(); ++i)
		m_pMeshCallback->onNewMaterial(m_materials[i]);

	return true;
}