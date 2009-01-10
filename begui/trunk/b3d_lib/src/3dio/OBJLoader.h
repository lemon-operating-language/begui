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

#include "FileLoader.h"
#include "../BaseTextFile.h"

class OBJLoader : public FileLoader
{
private:
	std::string	m_MtlLibPath;
	BaseTextFile	m_file;

public:
	std::vector<Material>	m_materials;

public:
	OBJLoader() { };
	virtual ~OBJLoader() { };

	bool loadFile(const std::string &filename);
	
protected:
	void ReadFaceVertex(int *vid, int *tid, int *nid);
	//void LoadMaterial(const char *mat_name, Material* pMat);

	bool loadMatLib();
};