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

class DXFLoader : public FileLoader
{
private:
	BaseTextFile	m_file;

	int			m_gType;
	std::string m_sVal;
	int			m_iVal;
	double		m_fVal;
	bool		m_groupPutBack;

	int	m_meshVertexCount, m_meshFaceCount;

public:
	DXFLoader();
	virtual ~DXFLoader();

	bool loadFile(const std::string &filename);

private:
	bool isInt(int code);
	bool isFloat(int code);
	bool isString(int code);

	void skipSection();
	bool readHeader();
	bool readTables();
	bool readBlocks();
	bool readEntities();

	void readTable();
	void readBlock();

	bool read3dFace();
	bool readPolyLine();

	bool readGroup();
	void putGroupBack();
};