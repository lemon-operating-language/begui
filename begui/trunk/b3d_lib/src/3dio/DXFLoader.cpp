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

#include "DXFLoader.h"

DXFLoader::DXFLoader() : m_meshVertexCount(0), m_meshFaceCount(0), m_groupPutBack(false)
{
	m_file.setReadWholeLines(true);
}

DXFLoader::~DXFLoader()
{
}

bool DXFLoader::loadFile(const std::string &filename)
{
	// open the file.
	if (!m_file.loadFile(filename))
		return false;

	m_pMeshCallback->onMeshBegin(0, 0);

	// read contents
	while (readGroup())
	{
		if (isString(m_gType))
		{
			// check for known groups.
			if (m_gType == 999)
			{
				// comment - discard
			}
			else if (m_sVal == "SECTION")
			{
				// read next group.
				readGroup();

				if (isString(m_gType) && m_sVal == "HEADER")
				{
					readHeader();
				}
				else if (isString(m_gType) && m_sVal == "TABLES")
				{
					readTables();
				}
				else if (isString(m_gType) && m_sVal == "BLOCKS")
				{
					readBlocks();
				}
				else if (isString(m_gType) && m_sVal == "ENTITIES")
				{
					readEntities();
				}
				else if (isString(m_gType) && m_sVal == "EOF")
				{
					Console::print("Finished reading the file\n");
					break;
				}
				else
				{
					skipSection();
				}
			}
		}
	}

	//TEMP
	m_pMeshCallback->onMeshEnd();

	// close the file.
	m_file.close();

	return true;
}

bool DXFLoader::readGroup()
{
	if (m_groupPutBack)
	{
		// return the previous group.
		m_groupPutBack = false;
		return true;
	}

	if (m_file.eof())
		return false;

	m_file >> m_gType;
	if (isString(m_gType))
		m_file >> m_sVal;
	else if (isInt(m_gType))
		m_file >> m_iVal;
	else if (isFloat(m_gType))
		m_file >> m_fVal;
	else
	{
		// unknown group type - disregard.
		m_file.getline();
	}

	return true;
}

void DXFLoader::putGroupBack()
{
	ASSERT(m_groupPutBack == false);
	m_groupPutBack = true;
}

void DXFLoader::skipSection()
{
	while (readGroup())
	{
		if (isString(m_gType) && m_sVal == "ENDSEC")
			break;
	}
}

bool DXFLoader::readHeader()
{
	Console::print("skipping HEADER section\n");
	while (readGroup())
	{
		if (isString(m_gType) && m_sVal == "ENDSEC")
			break;
	}
	return true;
}

bool DXFLoader::readTables()
{
	Console::print("reading TABLES section\n");
	while (readGroup())
	{
		if (isString(m_gType) && m_sVal == "TABLE")
		{
			readTable();
		}
		else if (isString(m_gType) && m_sVal == "ENDSEC")
			break;
	}
	return true;
}

bool DXFLoader::readBlocks()
{
	Console::print("reading BLOCKS section\n");
	while (readGroup())
	{
		if (isString(m_gType) && m_sVal == "BLOCK")
		{
			readBlock();
		}

		if (isString(m_gType) && m_sVal == "ENDSEC")
			break;
	}
	return true;
}

void DXFLoader::readBlock()
{
	while (readGroup())
	{
		// skip block.
		if (m_gType == 2 || m_gType == 3) {
			Console::print("\tskipping block: %s\n", m_sVal.c_str());
		}
		else if (m_gType == 0 && m_sVal == "ENDBLK")
			break;
	}
}

bool DXFLoader::readEntities()
{
	Console::print("reading ENTITIES section\n");
	while (readGroup())
	{
		if (m_gType == 0)
		{
			if (m_sVal == "3DFACE")
			{
				read3dFace();
			}
			else if (m_sVal == "POLYLINE")
			{
				readPolyLine();
			}
			else if (m_sVal == "INSERT")
			{
				// Read an INSERT group.
				/*while (readGroup())
				{
					if (m_gType == 2) {
						Console::print("\tread INSERT: %s\n", m_sVal.c_str());

						// insert this block (?)
					}
					//else if (m_gType == 102) break;
					else if (m_gType == 0) {
						putGroupBack();
						break;
					}
				}*/
			}
			else if (m_sVal == "BLOCK")
			{
				readBlock();
			}
			else if (m_sVal == "LINE") {
				Console::print("\tskipped LINE\n");
			}
			else if (m_sVal == "POINT") {
				Console::print("\tskipped POINT\n");
			}
			else if (m_sVal == "CIRCLE") {
				Console::print("\tskipped CIRCLE\n");
			}
			else if (m_sVal == "ARC") {
				Console::print("\tskipped ARC\n");
			}
			else if (m_sVal == "TRACE") {
				Console::print("\tskipped TRACE\n");
			}
			else if (m_sVal == "SOLID") {
				Console::print("\tskipped SOLID\n");
			}
			else if (m_sVal == "TEXT") {
				Console::print("\tskipped TEXT\n");
			}
			else if (m_sVal == "SHAPE") {
				Console::print("\tskipped SHAPE\n");
			}
			else if (m_sVal == "VIEWPORT") {
				Console::print("\tskipped VIEWPORT\n");
			}
			else if (m_sVal == "ENDSEC") {
				Console::print("End of ENTITIES\n");
				break;
			}
			else
			{
				/*static int c = 0;
				if (c < 1200)
				{
					if (atof(m_sVal.c_str()) == 0.0) {
						Console::print("skipped entity: %s\n", m_sVal.c_str());
						c++;
					}
				}*/
			}
		}
	}
	return true;
}

void DXFLoader::readTable()
{
	std::string tableType;
	readGroup();
	if (m_gType == 2) {
		tableType = m_sVal;
		Console::print("reading TABLE: %s\n", m_sVal.c_str());

		if (tableType == "VPORT")
		{
			// read viewport
		}
		// read other TABLE groups...
	}

	// skip data to the end of the table.
	do
	{
		if (isString(m_gType) && m_sVal == "ENDTAB")
			break;
	}
	while (readGroup());
}

bool DXFLoader::read3dFace()
{
	//Console::print("\treading 3DFACE\n");

	static Vector3 v[4];
	static std::vector<int> ids;

	while (readGroup())
	{
		if (m_gType == 8)
		{
		}
		else if (m_gType == 10 || m_gType == 11 || m_gType == 12 || m_gType == 13) {
			v[m_gType%10].x = m_fVal;
		}
		else if (m_gType == 20 || m_gType == 21 || m_gType == 22 || m_gType == 23) {
			v[m_gType%10].y = m_fVal;
		}
		else if (m_gType == 30 || m_gType == 31 || m_gType == 32 || m_gType == 33) {
			v[m_gType%10].z = m_fVal;
		}
		else if (m_gType == 0)
		{
			// gone too far.
			putGroupBack();
			break;
		}
		else {
			// ignore
		}
	}

	// Check if it is a triangle or a rectangle.
	bool bTri = (v[2] == v[3]);

	// Store the vertices and face.
	if (m_pMeshCallback)
	{
		// Store vertex data.
		m_pMeshCallback->onNewVertex(&v[0]);
		m_pMeshCallback->onNewVertex(&v[1]);
		m_pMeshCallback->onNewVertex(&v[2]);
		if (!bTri)
			m_pMeshCallback->onNewVertex(&v[3]);

		// Store the face data.
		ids.resize((bTri)?3:4);
		ids[0] = m_meshVertexCount++;
		ids[1] = m_meshVertexCount++;
		ids[2] = m_meshVertexCount++;
		if (!bTri)
			ids[3] = m_meshVertexCount++;
		m_pMeshCallback->onNewFace(ids);
		m_meshFaceCount++;
	}

	return true;
}

bool DXFLoader::readPolyLine()
{
	Console::print("\treading POLYLINE\n");

	int type = 0;
	bool bPolyFace = false;
	int nVertices = 0;
	int nFaces = 0;

	std::vector<int> ids;
	std::vector<int> vertexMap;

	std::string sVal;
	while (readGroup())
	{
		if (m_gType == 70)
		{
			// POLYLINE type
			type = m_iVal;
			if (m_iVal & 64u)
				bPolyFace = true;
		}
		else if (m_gType == 71) {
			nVertices = m_iVal;
			vertexMap.reserve(nVertices);	// nVertices might be wrong, just reserve
		}
		else if (m_gType == 72) {
			nFaces = m_iVal;
		}
		else if (m_gType == 0) {
			// the end of polyline group.
			// readout the vertices and return.
			do {
				if (m_gType == 0 && m_sVal == "VERTEX")
				{
					// read next vertex.
					Vector3 pos;
					int flags = 0;
					int id1 = 0, id2 = 0, id3 = 0, id4 = 0;

					while (readGroup())
					{
						if (m_gType == 10)
							pos.x = m_fVal;
						else if (m_gType == 20)
							pos.y = m_fVal;
						else if (m_gType == 30)
							pos.z = m_fVal;
						else if (m_gType == 70)
							flags = m_iVal;
						else if (m_gType == 71)
							id1 = m_iVal;
						else if (m_gType == 72)
							id2 = m_iVal;
						else if (m_gType == 73)
							id3 = m_iVal;
						else if (m_gType == 74)
							id4 = m_iVal;
						else if (m_gType == 0)
						{
							// finished reading vertex. Time to find out what we have read so far...
							if (flags & 64u)
							{
								// mesh vertex
								// add this vertex to the mesh
								m_pMeshCallback->onNewVertex(&pos);
								if (!bPolyFace)
									ids.push_back(m_meshVertexCount);
								vertexMap.push_back(m_meshVertexCount++);
							}
							else if (flags & 128u)
							{
								// mesh face
								ids.clear();
								if (id1 != 0)
									ids.push_back(id1);
								if (id2 != 0)
									ids.push_back(id2);
								if (id3 != 0)
									ids.push_back(id3);
								if (id4 != 0)
									ids.push_back(id4);

								// correct and remap face ids
								for (size_t i=0; i<ids.size(); ++i) {
									//ids[i] = abs(ids[1]) - 1;
									ids[i] = vertexMap[ids[i]];
								}

								// add this face to the mesh.
								m_pMeshCallback->onNewFace(ids);
								m_meshFaceCount++;
							}

							putGroupBack();
							break;
						}
						else {
							// skip unknown group.
						}
					}
				}
				else if (m_gType == 0) {
					// If not polyface, create the face here.
					if (!bPolyFace) {
						m_pMeshCallback->onNewFace(ids);
						m_meshFaceCount++;
					}

					// Gone too far. Put last group back and
					// return.
					putGroupBack();
					return true;
				}
				else {
					//...
				}
			}
			while (readGroup());
		}
	}

	return true;
}

bool DXFLoader::isString(int type)
{
	if ((type >= 0 && type <= 9) ||
		type == 999 ||
		(type >= 1000 && type <= 1009))
		return true;
	return false;
}

bool DXFLoader::isInt(int type)
{
	if ((type >= 60 && type <= 79) ||
		(type >= 170 && type <= 175) ||
		(type >= 1060 && type <= 1079))
		return true;
	return false;
}

bool DXFLoader::isFloat(int type)
{
	if ((type >= 10 && type <= 59) ||
		(type >= 140 && type <= 147) ||
		(type >= 210 && type <= 239) ||
		(type >= 1010 && type <= 1059))
		return true;
	return false;
}